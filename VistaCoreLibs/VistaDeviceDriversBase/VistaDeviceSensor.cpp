/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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
#include <VistaBase/VistaStreamUtils.h>

#include <VistaTools/VistaCPUInfo.h>

#include <cassert>

unsigned int IVistaMeasureTranscode::ITranscodeIndexedGet::UNKNOWN_NUMBER_OF_INDICES = ~0;

#if defined( VISTA_USE_ATOMICS ) && ( defined( WIN32 ) || defined( LINUX ) )
	#define USE_32BIT_ATOMICS
	#if !defined( WIN32 ) || !defined( VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE )
		#define USE_64BIT_ATOMICS
	#endif
#endif


#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
#include <VistaInterProcComm/Concurrency/VistaSemaphore.h>
#else
	#if defined(WIN32)
	#include <Windows.h>
	#elif defined(LINUX) && defined(__GNUC__) //&& defined(__i386__)
	// up to now, we are using hand-crafted assembler...
    // up-coming unix kernels have atomics in user-land, so we might switch one day
	#endif
#endif // !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )

/**
 * @todo move atomics to IPC (IAR: would not move this code! go and copy/modify)
 * @todo check atomics for 64bit platforms, this code will only work on 32bit
 */
class VistaAtomics
{
public:
	typedef void* SWAPADDRESS;

	/**
	 * @return the prior-value of v1
	 */
	VistaType::uint32 AtomicSet(SWAPADDRESS v1, VistaType::uint32 v2)
	{
#if !defined( USE_32BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		VistaType::uint32 vOld = *((VistaType::uint32*)v1); // assign old value
		*((VistaType::uint32*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
#else
	#if defined(WIN32)
			/** LONG is always 32bit, even on win64 */
			return InterlockedExchange((LONG*)v1, (LONG)v2);
	#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) || defined( __LP32__) || defined(__LP64__) )
			return intel_gnuc_atomicswap((VistaType::uint32*)v1, (VistaType::uint32)v2); //atomic_xchg(v1, v2);
	#else
		VistaSemaphoreLock l(m_lock);
		VistaType::uint32 vOld = *((VistaType::uint32*)v1); // assign old value
		*((VistaType::uint32*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
	#endif
#endif // USE_32BIT_ATOMICS
	}

	VistaType::uint64 AtomicSet64(SWAPADDRESS v1, VistaType::uint64 v2)
	{
#if !defined( USE_64BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		VistaType::uint64 vOld = *((VistaType::uint64*)v1); // assign old value
		*((VistaType::uint64*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
#else
	#if defined(WIN32)
		#if _MSC_VER < 1900 // old versions have a different prototype
				return InterlockedExchange64((LONGLONG*)v1, (LONGLONG)v2);
		#else
				return InterlockedExchange64((unsigned __int64*)v1, (unsigned __int64)v2);
		#endif // _MSC_VER
	#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) || defined( __LP32__) || defined(__LP64__) )
			return intel_gnuc_atomicswap64((VistaType::uint64*)v1, (VistaType::uint64)v2); //atomic_xchg(v1, v2);
	#else
		VistaSemaphoreLock l(m_lock);
		VistaType::uint64 vOld = *((VistaType::uint64*)v1); // assign old value
		*((VistaType::uint64*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
	#endif
#endif // USE_64BIT_ATOMICS
	}

	#if defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) || defined( __LP32__) || defined(__LP64__) )
	// only swaps 32bit values
	static VistaType::uint32 intel_gnuc_atomicswap(volatile VistaType::uint32 *mem, VistaType::uint32 val)
						{
						VistaType::uint32 prev = val;
						asm volatile ("lock; xchgl %0, %1"
						: "=r" (prev)
						: "m" (*(mem)), "0"(prev)
						: "memory");
						return prev;
						}

	static VistaType::uint64 intel_gnuc_atomicswap64(volatile VistaType::uint64 *mem, VistaType::uint64 val)
						{
						VistaType::uint64 prev = val;
						asm volatile ("lock; xchgq %0, %1"
						: "=r" (prev)
						: "m" (*(mem)), "0"(prev)
						: "memory");
						return prev;
						}
	#endif

	inline VistaType::uint32 AtomicRead(const VistaType::uint32 &ref)
	{
#if !defined( USE_32BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}

	inline VistaType::uint64 AtomicRead64(const VistaType::uint64 &ref)
	{
#if !defined( USE_64BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}

	template<class T>
	T AtomicReadT( const T &ref )
	{
#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}

#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
	VistaSemaphore m_lock;
#endif


	VistaAtomics()
#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
	: m_lock(1, VistaSemaphore::SEM_TYPE_FASTEST)
#endif
	{

	}

	static std::string GetAtomicState()
	{
#if defined( USE_32BIT_ATOMICS ) && defined( USE_64BIT_ATOMICS )
		return "VISTA_USE_ATOMICS SET!";
#elif defined( USE_32BIT_ATOMICS )
		return "VISTA_USE_ATOMICS SET (32BIT) PLUS SEMAPHORE (64BIT)!";
#else
		return "SEMAPHORE LOCKED ACCESS";
#endif
	}
};


// *****************************************************************************
// helper classes
// *****************************************************************************

IVistaTranscoderFactoryFactory::~IVistaTranscoderFactoryFactory() {}

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
	VistaType::uint32 nCurHd = m_pAtomics->AtomicRead(m_nSnapshotWriteHead);
	if( nCurHd == ~0)
		return NULL;

	// return the most current when swap was called
	TVistaRingBuffer<VistaSensorMeasure>::size_type st = m_rbHistory.pred(nCurHd);
	if(st == (unsigned int)~0)
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
							m_pAtomics->AtomicRead((VistaType::uint32)m_rbHistory.GetCurrentVal()))));
	return &(*(cit));

}

VistaType::uint32 VistaMeasureHistory::GetCurrentReadPosition() const
{
	return m_pAtomics->AtomicRead(m_nSnapshotWriteHead);
}

const VistaSensorMeasure *VistaMeasureHistory::GetPast(unsigned int nNum, VistaType::uint32 nCurRdPos ) const
{
	if(nCurRdPos == (unsigned int)~0)
		nCurRdPos = m_pAtomics->AtomicRead(m_nSnapshotWriteHead);

	if( nCurRdPos == (unsigned int)~0)
		return NULL;

	TVistaRingBuffer<VistaSensorMeasure>::const_iterator cit
			 = m_rbHistory.index(m_rbHistory.pred(nCurRdPos));

	return &(cit - nNum);
}


const VistaSensorMeasure *VistaMeasureHistory::GetCurrentPast(unsigned int nNum) const
{
	TVistaRingBuffer<VistaSensorMeasure>::const_iterator cit(
			m_rbHistory.index(
					m_rbHistory.pred(
							m_pAtomics->AtomicRead((VistaType::uint32)m_rbHistory.GetCurrentVal()))));
	return &(cit - nNum);
}

const TVistaRingBuffer<VistaSensorMeasure> &VistaMeasureHistory::GetReadBuffer() const
{
	return m_rbHistory;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool IVistaMeasureTranscoderFactory::OnUnload()
{
#if defined(DEBUG)
	vstr::outi() << "IVistaMeasureTranscoderFactory::OnUnload("
			<< typeid( *this ).name()
			<< ")" << std::endl;
#endif
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();

	// get all getter symbols from prop fac with the co-responding type tag.
	std::list<IVistaPropertyGetFunctor*> liGetterByClass = pReg->GetGetterByClass( GetTranscoderName() );
	std::list<IVistaPropertySetFunctor*> liSetterByClass = pReg->GetSetterByClass( GetTranscoderName() );

	// unregister all getters first
	for( std::list<IVistaPropertyGetFunctor*>::iterator git = liGetterByClass.begin(); git != liGetterByClass.end(); ++git )
		delete *git;

	for( std::list<IVistaPropertySetFunctor*>::iterator sit = liSetterByClass.begin(); sit != liSetterByClass.end(); ++sit )
		delete *sit;

	return true;
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

	if( m_MpList.empty() )
	{
		// we have a m_BtList here,
		// we need to have all symbols starting at IVistaMeasureTranscode 

		// first: copy the base-class list
		m_MpList = m_BtList;
		
		// remove any type class sting belonging to out base class (leave in the rest)
		m_MpList.remove_if( VistaAspectsComparisonStuff::ObjEqualsCompare<std::string>( SuperClass::GetReflectionableType() ) );
	}


	// use the cached version for the get-functor
	return dynamic_cast<ITranscodeGet*>(pReg->GetGetFunctor(strPropName, GetReflectionableType(), m_MpList));
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

		if( m_MpList.empty() )
		{
			// we have a m_BtList here,
			// we need to have all symbols starting at IVistaMeasureTranscode 

			// first: copy the base-class list
			m_MpList = m_BtList;
			
			// remove any type class sting belonging to out base class (leave in the rest)
			m_MpList.remove_if( VistaAspectsComparisonStuff::ObjEqualsCompare<std::string>( SuperClass::GetReflectionableType() ) );
		}

		// get all symbols belonging to these base-classes
		std::set<std::string> oSet;
		pReg->GetGetterSymbolSet( oSet, m_MpList );
		return oSet;
}

namespace
{
	const std::string sSReflectionType("IVistaMeasureTranscode");

	class VistaMeasureIndexTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<VistaType::uint32>
	{
	public:
		VistaMeasureIndexTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<unsigned int>("MEASUREINDEX",
			sSReflectionType, "absolute index of measurement")
		{
		}

		virtual VistaType::uint32 GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return pMeasure->m_nMeasureIdx;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
								VistaType::uint32 &dIndex ) const
		{
			dIndex = pMeasure->m_nMeasureIdx;
			return true;
		}
	};

	class VistaMeasureTimeStampTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<VistaType::microtime>
	{
	public:
		VistaMeasureTimeStampTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaType::microtime>("TIMESTAMP",
			sSReflectionType, "timestamp of raw measurement")
		{
		}

		virtual VistaType::microtime GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return pMeasure->m_nMeasureTs;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
				VistaType::microtime &dStamp ) const
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
			sSReflectionType, "timestamp of raw measurement"),
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
		VistaType::uint64 m_nSpeed;
	};

	class VistaMeasureDeliveryTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
	{
	public:
		VistaMeasureDeliveryTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<double>("DELIVERY_TIMESTAMP",
			sSReflectionType, "the timestamp of delivery to the history")
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
			sSReflectionType, "time in between locks (lock latency)")
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
}

VistaDeviceSensor::~VistaDeviceSensor()
{
	delete m_pAtomics;
	delete m_pTickStop;
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

VistaType::microtime VistaDeviceSensor::GetUpdateTimeStamp() const
{
	// @todo: atomically setting a double leads to problems under special circumstances
	//return m_pAtomics->AtomicReadT<VistaType::microtime>(m_rbSensorHistory.m_nUpdateTs);
	return m_rbSensorHistory.m_nUpdateTs;
}

bool VistaDeviceSensor::SetUpdateTimeStamp(VistaType::microtime nTs)
{
	// @todo: atomically setting a double leads to problems under special circumstances
	// (more precisely, gcc+linux with -03 seems to screw up the modification when using
	// semaphore-locked access - the update timestamp is sometimes undefined (mostly 0) afterwards

	//assert( sizeof(VistaType::microtime) == sizeof(VistaType::uint64) );
	// assumption: sizeof(microtime) == sizeof(double) == sizeof(uint64)
	// copy value of nTs bytewise
	//m_pAtomics->AtomicSet64( &m_rbSensorHistory.m_nUpdateTs, *((VistaType::uint64*)&nTs) );
	m_rbSensorHistory.m_nUpdateTs = nTs;
	return true;
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
	if( m_rbSensorHistory.m_nUpdateTs == 0 ) // check for (first) new data
		return false;

	// nWriteHead marks the current physical location of the currently written (probably unfinished!) buffer on the
	// ring, it is determined directly from the history index using
	// an atomic read on the respective reference, as the history is
	// probably under access right now
	VistaType::uint32 nWriteHead =
		m_pAtomics->AtomicRead( (VistaType::uint32)m_rbSensorHistory.m_rbHistory.GetCurrentRef() );

	VistaType::uint32 nMeasureCount =
		m_pAtomics->AtomicRead( m_rbSensorHistory.m_nMeasureCount );

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


VistaType::microtime VistaDeviceSensor::GetLastUpdateTs() const
{
	return m_nLastUpdateTs;
}

bool VistaDeviceSensor::AdvanceMeasure()
{
	// get a reference to the memory where the current ref (write head) is
	// stored (mark it as volatile). size_type is typically sizeof(long) (bus-width)
	volatile TVistaRingBuffer<VistaSensorMeasure>::size_type &cur
		= m_rbSensorHistory.m_rbHistory.GetCurrentRef();

	// get a reference to the memory where the current measure count is
	// stored
	volatile unsigned int &nCnt = m_rbSensorHistory.m_nMeasureCount;

	// get next slot index and save in next (using RingBuffer.succ())
	// succ() does not change the value, it merely reads it and increases it in the return value
	// while it aboyes the array bounds
	TVistaRingBuffer<VistaSensorMeasure>::size_type next = m_rbSensorHistory.m_rbHistory.succ(cur);
	VistaType::uint32 cnt  = nCnt+1; // increase measure count as this is a new sampling try


	// first... some statistics...
	VistaType::microtime nPre = VistaTimeUtils::GetStandardTimer().GetMicroTime();


	// the next two steps should be performed atomically in one step...
	// and not as two disjunct steps, but let's give it a try.
	// atomically swap the new index with the old index

	// this while loop may seem awkward to you, but AtomicSwap returns the OLD
	// value of cur. That means: when cur is set to next the return value
	// does not equal the value of cur anymore.
	// if the set failed, we may try again (should not happen during semaphore
	// lock and InterlockExchange(), but may happen during hand-crafted assembler)
	// so this is while we use a while() here... loop until it changes.


	// we should do the two following things in a one-time-step!
	// but due to the order of access, this should not be a problem
	// repeat: "should"

	// first set the write head to a new offset
	// note: the following comparison could be resolved during compile time... 
	if( sizeof(TVistaRingBuffer<VistaSensorMeasure>::size_type) == sizeof( VistaType::uint64 ) )
	{
		while(m_pAtomics->AtomicSet64((VistaAtomics::SWAPADDRESS)&cur, next) == cur)
			++m_nSwap1FailCount;
	}
	else
	{
		while(m_pAtomics->AtomicSet((VistaAtomics::SWAPADDRESS)&cur, (VistaType::uint32)next) == (VistaType::uint32)cur )
			++m_nSwap1FailCount;
	}


	while(m_pAtomics->AtomicSet((VistaAtomics::SWAPADDRESS)&nCnt, cnt) == nCnt)
		++m_nSwap2FailCount;

	// finally... more statistics...
	VistaType::microtime nPost = VistaTimeUtils::GetStandardTimer().GetMicroTime();

	// write the swap-time to the "last" written value!
	// this is, of course, a race, as the "last" value is possibly under
	// access... but currently, the swap-time is for debugging and likely to
	// be removed in the future.

	// so, fetch the current slot
	TVistaRingBuffer<VistaSensorMeasure>::iterator it(
			m_rbSensorHistory.m_rbHistory.index( // get the index
					m_rbSensorHistory.m_rbHistory.pred( // of the predecessor
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

unsigned int VistaDeviceSensor::GetNewMeasureCount( unsigned int &lastRead ) const
{
	unsigned int nMIndex = GetMeasureIndex();

	// skim: the user can only read what he decided to read, even if there are more
	// entries in the current history.
	// Example: user said, n slots are interesting, n+m (m>0) are there, the new measure count
	// is limited to n.
	unsigned int nNewMeasureCount = std::min<unsigned int>( nMIndex - lastRead,
			m_rbSensorHistory.m_nClientReadSize);

	// write back to user given reference value
	lastRead = nMIndex;

	// return the number of new measures available for read
	return nNewMeasureCount;
}




/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


