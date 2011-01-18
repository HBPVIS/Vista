/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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

#include "VistaTimer.h"

#include "VistaTimerImp.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

#if defined(WIN32)
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include <iostream>
#include <cassert>

// bleh... windows define screws resolution of term std::min
#ifdef min
#undef min
#endif


namespace
{
	VistaTimer *SSingleton = NULL;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* VistaTimer                                                                */
/*============================================================================*/
VistaTimer::VistaTimer( IVistaTimerImp *pImp )
{
	if(pImp == NULL)
		m_pImp = IVistaTimerImp::GetSingleton();
	else
		m_pImp = pImp;

	assert( m_pImp != NULL);

	m_pImp->IncReferenceCount();

	// we assume pImp to be non-NULL and the singleton to be set
	m_nBirthTime = m_pImp->GetMicroTime();
}

VistaTimer::~VistaTimer()
{
	m_pImp->DecReferenceCount();
}

microtime  VistaTimer::GetMicroTime() const
{
	return m_pImp->GetMicroTime();
}

microstamp VistaTimer::GetMicroStamp() const
{
	return m_pImp->GetMicroStamp();
}

double    VistaTimer::GetSystemTime() const
{
	return m_pImp->GetSystemTime();
}

systemtime VistaTimer::ConvertToSystemTime( const microtime mtTime ) const
{
	return m_pImp->ConvertToSystemTime( mtTime );
}

microtime VistaTimer::GetLifeTime() const
{
	// simple: return now - birth time
	return m_pImp->GetMicroTime() - m_nBirthTime;
}

void VistaTimer::ResetLifeTime()
{
	m_nBirthTime = m_pImp->GetMicroTime();
}

const VistaTimer& VistaTimer::GetStandardTimer()
{
	if( SSingleton == NULL )
		SSingleton = new VistaTimer;

	return *SSingleton;
}


/*============================================================================*/
/* VistaAverageTimer                                                         */
/*============================================================================*/

VistaAverageTimer::VistaAverageTimer( IVistaTimerImp *pImp )
: VistaTimer( pImp )
, m_nRecordStartTime( m_nBirthTime )
, m_nAccumulatedTime( 0.0 )
, m_iCount( 0 )
, m_nAverage( 0 )
{		
}
VistaAverageTimer::~VistaAverageTimer()
{
}

void VistaAverageTimer::StartRecording()
{
	m_nRecordStartTime = GetMicroTime();
}

void VistaAverageTimer::RecordTime()
{
	microtime nCurrentTime = GetMicroTime();
	m_nAccumulatedTime += nCurrentTime - m_nRecordStartTime;
	++m_iCount;
	m_nRecordStartTime = nCurrentTime;
	m_nAverage = m_nAccumulatedTime / (float)m_iCount;
}

microtime VistaAverageTimer::GetCurrentRecordingTime() const
{
	return ( GetMicroTime() - m_nRecordStartTime );
}

microtime VistaAverageTimer::GetAccumulatedTime() const
{
	return m_nAccumulatedTime;
}

int VistaAverageTimer::GetRecordCount() const
{
	return m_iCount;
}

microtime VistaAverageTimer::GetAverageTime() const
{
	return m_nAverage;
}			

void VistaAverageTimer::ResetAveraging()
{
	m_nRecordStartTime = GetMicroTime();
	m_iCount = 0;
	m_nAccumulatedTime = 0.0;
}

/*============================================================================*/
/* VistaWeightedAverageTimer                                                 */
/*============================================================================*/

VistaWeightedAverageTimer::VistaWeightedAverageTimer( const float fWeight,
													IVistaTimerImp *pImp )
: VistaAverageTimer( pImp )
, m_fWeight( fWeight )
, m_fNorm( 1.0f / ( fWeight + 1.0f ) )
{		
}
VistaWeightedAverageTimer::~VistaWeightedAverageTimer()
{
}

void VistaWeightedAverageTimer::RecordTime()
{
	microtime nCurrentTime = GetMicroTime();
	microtime nDelta = nCurrentTime - m_nRecordStartTime;
	m_nAccumulatedTime += nDelta;
	++m_iCount;
	m_nRecordStartTime = nCurrentTime;

	if( m_iCount == 1 )
		m_nAverage = nDelta;
	else
		m_nAverage = ( m_fWeight * nDelta + m_nAverage ) * m_fNorm;
}

void VistaWeightedAverageTimer::ResetAveraging()
{
	m_nRecordStartTime = GetMicroTime();
	m_iCount = 0;
	m_nAccumulatedTime = 0.0;
}

float VistaWeightedAverageTimer::GetWeight() const
{
	return m_fWeight;
}
void VistaWeightedAverageTimer::SetWeight( const float fWeight )
{
	m_fWeight = fWeight;
	m_fNorm = 1.0f / ( fWeight + 1.0f );
}

/*============================================================================*/
/* VistaWindowAverageTimer                                                 */
/*============================================================================*/

VistaWindowAverageTimer::VistaWindowAverageTimer( const int iWindowSize,
													IVistaTimerImp *pImp )
: VistaAverageTimer( pImp )
, m_vecRecords( (size_t)iWindowSize, 0.0 )
{
	m_itCurrentSlot = m_vecRecords.begin();
}
VistaWindowAverageTimer::~VistaWindowAverageTimer()
{
}

void VistaWindowAverageTimer::RecordTime()
{
	microtime nCurrentTime = GetMicroTime();
	microtime nDelta = nCurrentTime - m_nRecordStartTime;

	m_iCount++;
	m_iCount = std::min( m_iCount, (int)m_vecRecords.size() );
	
	m_nAccumulatedTime += nDelta;
	m_nAccumulatedTime -= (*m_itCurrentSlot);
	(*m_itCurrentSlot) = nDelta;

	++m_itCurrentSlot;
	if( m_itCurrentSlot == m_vecRecords.end() )
		m_itCurrentSlot = m_vecRecords.begin();
	
	m_nRecordStartTime = nCurrentTime;
	m_nAverage = m_nAccumulatedTime / m_iCount;
}

void VistaWindowAverageTimer::ResetAveraging()
{
	m_nRecordStartTime = GetMicroTime();
	m_iCount = 0;
	m_nAccumulatedTime = 0.0;
	m_vecRecords.assign( m_vecRecords.size(), 0.0 );
	m_itCurrentSlot = m_vecRecords.begin();
}

int VistaWindowAverageTimer::GetWindowSize() const
{
	return (int)m_vecRecords.size();
}

//
///*============================================================================*/
///* VistaTickStop                                                             */
///*============================================================================*/
//	VistaTickStop::VistaTickStop( index_type nWindowLength,
//									IVistaTimerImp *pImp )
//	: VistaTimer( pImp )
//	, m_vTicks(nWindowLength, 0)
//	, m_nTickIdx(0)
//	, m_nTickCount(0)
//	, m_nAccumMt(0)
//	, m_nLastMt(0)
//	{
//	}
//
//	VistaTickStop::~VistaTickStop()
//	{
//	}
//
//	void VistaTickStop::Tick()
//	{
//		microtime mt = GetMicroTime();
//		m_nAccumMt += mt - m_nLastMt;
//
//		m_vTicks[m_nTickIdx++] = m_nLastMt = mt;
//		++m_nTickCount;
//
//		if( m_nTickIdx == m_vTicks.size() )
//			m_nTickIdx = 0; // start over
//	}
//
//	double VistaTickStop::GetFrequency() const
//	{
//		microtime dt = GetAverageDelta();
//		if(dt)
//			return 1.0 / dt;
//		else
//			return 0;
//	}
//
//	microtime VistaTickStop::GetAverageDelta() const
//	{
//		if(m_nTickCount == 0)
//			return 0;
//
//		return m_nAccumMt / microtime(m_nTickCount);
//	}
//
//	double VistaTickStop::GetWindowFrequency() const
//	{
//		return 0;
//	//	if( m_vTicks.empty() )
//	//		return 0;
//	//
//	//	microtime d = 0;
//	//	for( index_type n = 0; n < m_vTicks.size(); ++n)
//	//		d += m_vTicks[n];
//	//
//	//	return double(m_vTicks.size()) / double( d );
//	}
//
//	microtime VistaTickStop::GetAgeFromLast() const
//	{
//		if(m_nLastMt)
//		{
//			return GetMicroTime() - m_nLastMt;
//		}
//		return 0;
//	}
//
//	void VistaTickStop::ResetTickStop()
//	{
//		m_nTickIdx = 0;
//		m_nTickCount = 0;
//		m_nAccumMt = 0;
//		m_nLastMt = 0;
//		m_vTicks = std::vector<microtime>(m_vTicks.size(),0);
//	}
//
///*============================================================================*/
///* VistaWeightedAverageTimer                                                           */
///*============================================================================*/
//	VistaWeightedAverageTimer::VistaWeightedAverageTimer()
//	: m_nAvgTime(0)
//	{
//	}
//
//	microtime VistaWeightedAverageTimer::GetAverage() const
//	{
//		return m_nAvgTime;
//	}

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

