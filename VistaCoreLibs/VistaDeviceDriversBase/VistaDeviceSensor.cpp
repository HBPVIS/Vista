/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id$

#include "VistaDeviceSensor.h"
#include <VistaAspects/VistaPropertyFunctorRegistry.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaTools/VistaCPUInfo.h>

#include "VistaDeviceDriversOut.h"

//#define _USE_ATOMICS

#if !defined(_USE_ATOMICS) || defined(SUNOS)
#include <VistaInterProcComm/Concurrency/VistaSemaphore.h>
#else
	#if defined(WIN32)
	#include <Windows.h>
	#elif defined(LINUX) && defined(__GNUC__) //&& defined(__i386__)
	// up to now, we are using hand-crafted assembler...
    // up-coming unix kernels have atomics in user-land, so we might switch one day
	// note: the assembler only works for 32bit
	#endif
#endif // _USE_ATOMICS || SUNOS

/**
 * @todo move atomics to IPC (IAR: would not move this code! go and copy/modify)
 * @todo check atomics for 64bit platforms, this code will only work on 32bit
 */
class VistaAtomics
{
public:

	//typedef unsigned int uint32;

	typedef void* SWAPADDRESS;
	//typedef uint32  CMPVAL;

	/**
	 * @return the prior-value of v1
	 */
	uint32 AtomicSwap(SWAPADDRESS v1, uint32 v2)
	{
#if !defined(_USE_ATOMICS)
		VistaSemaphoreLock l(m_lock);
		uint32 vOld = *((uint32*)v1); // assign old value
		*((uint32*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
#else
	#if defined(WIN32)
			/** @todo check on win64: is LONG 32bit? */
			return InterlockedExchange((LONG*)v1, (LONG)v2);
	#elif defined(LINUX) //&& defined(__i386__)
			return intel_gnuc_atomicswap((uint32*)v1, (uint32)v2); //atomic_xchg(v1, v2);
	#else
			VistaSemaphoreLock l(m_lock);
			*((uint32*)v1) = v2;
			return *((uint32*)v1);
	#endif
#endif // _USE_ATOMICS
	}

	/**
	 * Return the prior-value of v1
	 */
	uint32 AtomicCompareAndSwap(SWAPADDRESS v1, SWAPADDRESS v2, uint32 cmpval)
	{
#if !defined(_USE_ATOMICS)
		VistaSemaphoreLock l(m_lock);
		if( *((uint32*)v1) != cmpval )
		{
			uint32 vOld = *((uint32*)v1);
			*((uint32*)v1) = *((uint32*)v2);
			return vOld;
		}
		return *((uint32*)v1);
#else
	#if defined(WIN32)
			/** @todo check on win64: is LONG 32bit? */
			return InterlockedCompareExchange((PLONG)v1, (LONG)v2, (LONG)cmpval);
	#elif defined(LINUX) && defined(__GNUC__) //&& defined(__i386__)
			return intel_gnuc_atomiccas((uint32*)v1, (uint32)v2, (uint32)cmpval);
	#else
			VistaSemaphoreLock l(m_lock);
			if( *((uint32*)v1) != cmpval )
				*((uint32*)v1) = *((uint32*)v2);
			return *((uint32*)v1);
	#endif
#endif
	}

	#if defined(LINUX) && defined(__GNUC__) //&& defined(__i386__)
	// only valid for 32bit platforms
	static uint32 intel_gnuc_atomiccas(volatile uint32 *mem,
			uint32 with,
			uint32 cmp)
							{
							uint32 prev;
							asm volatile ("lock; cmpxchgl %1, %2"
							: "=a" (prev)
							: "r" (with), "m" (*(mem)), "0"(cmp)
							: "memory", "cc");
							return prev;
							}
	#endif

	#if defined(LINUX) && defined(__GNUC__) //&& defined(__i386__)
	// only valid for 32bit platforms
	static uint32 intel_gnuc_atomicswap(volatile uint32 *mem, uint32 val)
						{
						uint32 prev = val;
						asm volatile ("lock; xchgl %0, %1"
						: "=r" (prev)
						: "m" (*(mem)), "0"(prev)
						: "memory");
						return prev;
						}
	#endif

	inline uint32 AtomicRead(const uint32 &ref)
	{
#if !defined(_USE_ATOMICS) || defined(SUNOS)
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}
#if !defined(_USE_ATOMICS) || defined(SUNOS)
	VistaSemaphore m_lock;
#endif


	VistaAtomics()
#if !defined(_USE_ATOMICS) || defined(SUNOS)
	: m_lock(1, VistaSemaphore::SEM_TYPE_FASTEST)
#endif
	{

	}

	static std::string GetAtomicState()
	{
#if defined(_USE_ATOMICS)
		return "_USE_ATOMICS SET!";
#else
		return "SEMAPHORE LOCKED ACCESS";
#endif
	}
};


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


VistaMeasureHistory::VistaMeasureHistory(unsigned int nHistorySize,
										   VistaAtomics *pAtomics)
: m_rbHistory(1024),
  m_nUpdateTs(0),
  m_nMeasureCount(0),
  m_nDriverWriteSize(0),
  m_nClientReadSize(0),
  m_nSwapCount(0),
  m_nSnapshotWriteHead(~0),
  m_pAtomics(pAtomics)
{
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const VistaSensorMeasure *VistaMeasureHistory::GetCurrentRead() const
{
	// nothing read, yet?
	uint32 nCurHd = m_pAtomics->AtomicRead(m_nSnapshotWriteHead);
	if( nCurHd == ~0)
		return NULL;

	// return the most current when swap was called
	TVistaRingBuffer<VistaSensorMeasure>::size_type st = m_rbHistory.pred(nCurHd);
	if(st == ~0)
		return NULL;
	return &(*m_rbHistory.index(st));
}

const VistaSensorMeasure *VistaMeasureHistory::GetMostCurrent() const
{
	// the latest data resided directly behind (pred) the current write head
	// position, so this is what we look at
	TVistaRingBuffer<VistaSensorMeasure>::const_iterator cit(
			m_rbHistory.index(
					m_rbHistory.pred(
							m_pAtomics->AtomicRead((uint32)m_rbHistory.GetCurrentVal()))));
	return &(*(cit));

}

uint32 VistaMeasureHistory::GetCurrentReadPosition() const
{
	return m_pAtomics->AtomicRead(m_nSnapshotWriteHead);
}

const VistaSensorMeasure *VistaMeasureHistory::GetPast(unsigned int nNum, uint32 nCurRdPos ) const
{
	if(nCurRdPos == ~0)
		nCurRdPos = m_pAtomics->AtomicRead(m_nSnapshotWriteHead);

	if( nCurRdPos == ~0)
		return NULL;

	TVistaRingBuffer<VistaSensorMeasure>::const_iterator cit
			 = m_rbHistory.index(m_rbHistory.pred(nCurRdPos));

//	for(unsigned int n=0; n < nNum; ++n)
//		--cit;
//	return &(*cit);
	return &(cit - nNum);
}


const VistaSensorMeasure *VistaMeasureHistory::GetCurrentPast(unsigned int nNum) const
{
	TVistaRingBuffer<VistaSensorMeasure>::const_iterator cit(
			m_rbHistory.index(
					m_rbHistory.pred(
							m_pAtomics->AtomicRead((uint32)m_rbHistory.GetCurrentVal()))));
//	for(unsigned int n=0; n < nNum; ++n)
//		--cit;
	return &(cit - nNum);
}

const TVistaRingBuffer<VistaSensorMeasure> &VistaMeasureHistory::GetReadBuffer() const
{
	return m_rbHistory;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IVistaMeasureTranscode::IVistaMeasureTranscode()
: m_pMeasure(NULL)
, m_nNumberOfScalars(0)
{
}


IVistaMeasureTranscode::~IVistaMeasureTranscode()
{
}


bool IVistaMeasureTranscode::SetSensorMeasure(const VistaSensorMeasure *pMeasure)
{
	m_pMeasure = pMeasure;
	return true;
}

const VistaSensorMeasure *IVistaMeasureTranscode::GetSensorMeasure() const
{
	return m_pMeasure;
}

unsigned int IVistaMeasureTranscode::GetNumberOfScalars() const
{
	return m_nNumberOfScalars;
}

void IVistaMeasureTranscode::SetNumberOfScalars(unsigned int uiNumScalars)
{
	m_nNumberOfScalars = uiNumScalars;
}

std::string IVistaMeasureTranscode::GetTypeString()
{
	return "IVistaMeasureTranscode";
}

IVistaMeasureTranscode::ITranscodeGet *IVistaMeasureTranscode::GetMeasureProperty(const std::string &strPropName) const
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();
	// ok, ok... it is a const-getter, but we might need to cache the
	// base-class list, as this is unlikely to change, so we check
	// whether we have to build it
	if(m_BtList.empty())
	{
		// ... ah, yes
		// cache it
		m_BtList = GetBaseTypeList();
	}
	// use the cached version for the get-functor
	return dynamic_cast<ITranscodeGet*>(pReg->GetGetFunctor(strPropName, GetReflectionableType(), m_BtList));
}

std::set<std::string> IVistaMeasureTranscode::GetMeasureProperties() const
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();
		// ok, ok... it is a const-getter, but we might need to cache the
		// base-class list, as this is unlikely to change, so we check
		// whether we have to build it
		if(m_BtList.empty())
		{
			// ... ah, yes
			// cache it
			m_BtList = GetBaseTypeList();
		}
		std::set<std::string> oSet;
		pReg->GetGetterSymbolSet(oSet, m_BtList );
		return oSet;
}

class VistaMeasureIndexTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<uint32>
{
public:
	VistaMeasureIndexTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<unsigned int>("MEASUREINDEX",
		"IVistaMeasureTranscode", "absolute index of measurement")
	{
	}

	virtual uint32 GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		return pMeasure->m_nMeasureIdx;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure,
							uint32 &dIndex ) const
	{
		dIndex = pMeasure->m_nMeasureIdx;
		return true;
	}
};

class VistaMeasureTimeStampTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<microtime>
{
public:
	VistaMeasureTimeStampTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<microtime>("TIMESTAMP",
		"IVistaMeasureTranscode", "timestamp of raw measurement")
	{
	}

	virtual microtime GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		return pMeasure->m_nMeasureTs;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure,
			microtime &dStamp ) const
	{
		dStamp = pMeasure->m_nMeasureTs;
		return true;
	}
};

class VistaMeasureTimeStampSecTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
{
public:
	VistaMeasureTimeStampSecTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<double>("TIMESTAMP",
		"IVistaMeasureTranscode", "timestamp of raw measurement"),
		m_nSpeed(1)
	{
		VistaCPUInfo info;
		m_nSpeed = info.GetSpeed();
	}

	virtual double GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		return double(pMeasure->m_nMeasureTs) / double(m_nSpeed);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure,
								 double &dStamp ) const
	{
		dStamp = GetValue(pMeasure);
		return true;
	}
private:
	uint64 m_nSpeed;
};

class VistaMeasureDeliveryTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
{
public:
	VistaMeasureDeliveryTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<double>("DELIVERY_TIMESTAMP",
		"IVistaMeasureTranscode", "the timestamp of delivery to the history")
	{
	}
	virtual double GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		return double(pMeasure->m_nDeliverTs);
	}
	virtual bool GetValue(const VistaSensorMeasure *pMeasure,
								 double &dStamp ) const
	{
		dStamp = GetValue(pMeasure);
		return true;
	}
private:
};

class VistaMeasureSwapTimeTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
{
public:
	VistaMeasureSwapTimeTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<double>("SWAPTIME",
		"IVistaMeasureTranscode", "time in between locks (lock latency)")
	{
	}

	virtual double GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		return pMeasure->m_nSwapTime;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure,
			double &dStamp ) const
	{
		dStamp = pMeasure->m_nSwapTime;
		return true;
	}
};

namespace
{
	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaMeasureIndexTranscode,
		new VistaMeasureTimeStampTranscode,
		new VistaMeasureSwapTimeTranscode,
		new VistaMeasureTimeStampSecTranscode,
		NULL
	};
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDeviceSensor::VistaDeviceSensor()
: m_pAtomics(new VistaAtomics),
  m_rbSensorHistory(1024, m_pAtomics),
  m_pMeasureTranscode(NULL),
  m_nSwap1FailCount(0),
  m_nSwap2FailCount(0),
  m_nTypeHint(~0),
  m_nLastUpdateTs(~0),
  m_nLastMeasureCount(0),
  m_pTickStop( new VistaWindowAverageTimer ),
  m_pParent(NULL)
{
#if defined(DEBUG)
	vddout << "VistaDeviceSensor::VistaDeviceSensor() -- "
		 << VistaAtomics::GetAtomicState() << std::endl;
#endif
}

VistaDeviceSensor::~VistaDeviceSensor()
{
	delete m_pAtomics;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaDeviceDriver *VistaDeviceSensor::GetParent() const
{
	return m_pParent;
}

void VistaDeviceSensor::SetParent( IVistaDeviceDriver *pParent )
{
	m_pParent = pParent;
}

microtime VistaDeviceSensor::GetUpdateTimeStamp() const
{
	// yes, this is a race to read the time-stamp unprotected,
	// as it is written by the driver concurrently.
	// in our design, the timestamp is accessed to test for
	// difference, and is usually not interpreted. In that logic:
	// a new corrupted data is, well, still new...
	return m_rbSensorHistory.m_nUpdateTs;
}

bool VistaDeviceSensor::SetUpdateTimeStamp(microtime nTs)
{
	// !! see comment above: it's a race and we know that.
	m_rbSensorHistory.m_nUpdateTs = nTs;
	return (m_rbSensorHistory.m_nUpdateTs == nTs);
}

unsigned int VistaDeviceSensor::GetMeasureIndex() const
{
	// protected read of measure count
	return m_pAtomics->AtomicRead(m_rbSensorHistory.m_nMeasureCount);
}

unsigned int VistaDeviceSensor::GetDataCount() const
{
	// unprotected access of swap count (the number of times that
	// the swap-api was called, is <= MeasureIndex()
	return 	m_rbSensorHistory.m_nSwapCount;
}

void VistaDeviceSensor::SetMeasureHistorySize(unsigned int nSamples)
{

	m_rbSensorHistory.m_rbHistory.SetBufferSize(nSamples);
}

unsigned int VistaDeviceSensor::GetMeasureHistorySize() const
{
	return (unsigned int)m_rbSensorHistory.m_rbHistory.GetBufferSize();
}


bool VistaDeviceSensor::AddMeasure(const VistaSensorMeasure &oMeasure)
{
	m_rbSensorHistory.m_rbHistory.Add(oMeasure);
	return true;
}

bool VistaDeviceSensor::SwapMeasures()
{
	if( m_rbSensorHistory.m_nUpdateTs == 0 )
		return false;

	// nWriteHead marks the current physical location of the buffer on the
	// ring, it is determined directly from the history index using
	// an atomic read on the respective reference, as the history is
	// probably under access right now
	unsigned int nWriteHead =
		m_pAtomics->AtomicRead( (uint32)m_rbSensorHistory.m_rbHistory.GetCurrentRef() );

	unsigned int nMeasureCount =
		m_pAtomics->AtomicRead( (uint32)m_rbSensorHistory.m_nMeasureCount );

	// m_nSnapshotWriteHead is compared against that, note that
	// m_nSnapshotWriteHead is NOT written by a concurrent thread,
	// so reading it here is safe.
	// 1. check current write head against snapshot write head
	if((nWriteHead != m_rbSensorHistory.m_nSnapshotWriteHead)
	// can be the same in case of overturn
	|| (m_nLastUpdateTs != m_rbSensorHistory.m_nUpdateTs)
	// for small buffers, fast writers and non-strong-monotonic clocks,
	// the above can lead to the same timestamp written all over the history.
	// so finally take the absolute measure into account.
	/** @todo should be changed, once a good clock is present */
	// that resolves a fine enough granularity, or reflects a proper
	// system clock (strong monotony)
	|| (nMeasureCount != m_nLastMeasureCount ) )
	{
		// they do not match, so point the snapshot of the write head
		// to the current slot
		m_rbSensorHistory.m_nSnapshotWriteHead = nWriteHead;

		// try to count the number of swaps
		// this indicates a change
		++m_rbSensorHistory.m_nSwapCount;

		// in order to detect overturns, we check the timestamp
		// for the next read, so in case we have the unlikely event
		// of accessing the same index (slow reader), we check the
		// timestamp accordingly
		m_nLastUpdateTs     = m_rbSensorHistory.m_nUpdateTs;
		m_nLastMeasureCount = nMeasureCount;

		return true;
	}
	return false;
}


microtime VistaDeviceSensor::GetLastUpdateTs() const
{
	return m_nLastUpdateTs;
}

bool VistaDeviceSensor::AdvanceMeasure()
{
	// get current slot, but make sure, it really is a volatile and not
	// an compiler alias

	// get a reference to the memory where the current ref (write head) is
	// stored.
	volatile TVistaRingBuffer<VistaSensorMeasure>::size_type &cur
		= m_rbSensorHistory.m_rbHistory.GetCurrentRef();

	// get a reference to the memory where the current measure count is
	// stored
	volatile unsigned int &nCnt = m_rbSensorHistory.m_nMeasureCount;

	// get next slot index and save in next (using RingBuffer.succ())
	// succ() does not change the value, it merely reads it and increases it
	uint32 next = (uint32)m_rbSensorHistory.m_rbHistory.succ(cur);
	uint32 cnt  = m_rbSensorHistory.m_nMeasureCount+1; // increase measure count


	// the next two steps should be performed atomically in one step...
	// and not as two disjunct steps, but let's give it a try.
	// atomically swap the new index with the old index

	// first... some statistics...
	microtime nPre = VistaTimeUtils::GetStandardTimer().GetMicroTime();

	// this while loop may seem awkward to you, but AtomicSwap returns the OLD
	// value of cur. That means: when cur is set to next the return value
	// does not equal the value of cur anymore.
	// if the set failed, we may try again (should not happen during semaphore
	// lock and InterlockExchange(), but may happen during hand-crafted assembler)
	// so this is while we use a while() here... loop until it changes.

	// first set the write head to a new offset
	// we should do the two following things in a one-time-step!
	// but due to the order of access, this should not be a problem
	// repeat: "should"
	while(m_pAtomics->AtomicSwap((VistaAtomics::SWAPADDRESS)&cur, next) == cur)
		++m_nSwap1FailCount;

	while(m_pAtomics->AtomicSwap((VistaAtomics::SWAPADDRESS)&nCnt, cnt) == nCnt)
		++m_nSwap2FailCount;

	// finally... more statistics...
	microtime nPost = VistaTimeUtils::GetStandardTimer().GetMicroTime();

	// write the swap-time to the "last" written value!
	// this is, of course, a race, as the "last" value is possibly under
	// access... but currently, the swap-time is for debugging and likely to
	// be removed in the future.

	// so, fetch the current slot
	TVistaRingBuffer<VistaSensorMeasure>::iterator it(
			m_rbSensorHistory.m_rbHistory.index( // get the index
					m_rbSensorHistory.m_rbHistory.pred( // if the predecessor
							m_rbSensorHistory.m_rbHistory.GetCurrentVal()))); // of the current slot being written


	// write swap-time as the difference to post / pre
	(*it).m_nSwapTime = nPost - nPre;

	// return true, iff this swap really worked
	return (m_rbSensorHistory.m_rbHistory.GetCurrentRef() == next);
}


const VistaMeasureHistory &VistaDeviceSensor::GetMeasures() const
{
	return m_rbSensorHistory;
}

VistaMeasureHistory &VistaDeviceSensor::GetDriverMeasures()
{
	return m_rbSensorHistory;
}

IVistaMeasureTranscode *VistaDeviceSensor::GetMeasureTranscode() const
{
	return m_pMeasureTranscode;
}

void VistaDeviceSensor::SetMeasureTranscode(IVistaMeasureTranscode *pTranscode)
{
	m_pMeasureTranscode = pTranscode;
}


unsigned int VistaDeviceSensor::GetSwap1CountFail() const
{
	return m_nSwap1FailCount;
}

unsigned int VistaDeviceSensor::GetSwap2CountFail() const
{
	return m_nSwap2FailCount;
}

unsigned int VistaDeviceSensor::GetTypeHint() const
{
	return m_nTypeHint;
}

void         VistaDeviceSensor::SetTypeHint(unsigned int nType)
{
	m_nTypeHint = nType;
}

std::string VistaDeviceSensor::GetSensorName() const
{
	return m_nSensorName;
}

void VistaDeviceSensor::SetSensorName( const std::string &strName )
{
	m_nSensorName = strName;
}

VistaWindowAverageTimer &VistaDeviceSensor::GetWindowTimer() const
{
	return *m_pTickStop;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


