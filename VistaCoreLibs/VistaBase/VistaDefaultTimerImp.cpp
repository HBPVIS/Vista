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

#include <iostream>

#include "VistaDefaultTimerImp.h"
#include <sys/timeb.h>

#ifdef WIN32
#elif defined DARWIN
  #include <CoreServices/CoreServices.h>
  #include <mach/mach.h>
  #include <mach/mach_time.h>
  #include <sys/time.h>
#else // unix-like os
  #include <unistd.h>
  #include <sys/times.h>
  #include <sys/time.h>
  #include <time.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// specifies wether or not checks for unexpected jumps in PermormanceCounter
// should be checked and compensated
// see http://support.microsoft.com/kb/274323/en-us/
// This should hardly ever occur, however if it does and if the compensation is
// active, the implementation is not thread-safe! So best to leave it turned off,
// unless you run into the jumping time problem
//#define CHECK_FOR_JUMPS_UNDER_WIN32

#define JUMP_CHECK_MAX_MILLISECS 200
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

VistaDefaultTimerImp::VistaDefaultTimerImp()
: IVistaTimerImp()
{
#ifdef WIN32
	LARGE_INTEGER nTimestamp;
	QueryPerformanceCounter( &nTimestamp );
	m_nInitialStamp = (microstamp)nTimestamp.QuadPart;
	m_nLastStamp = 0;
	m_nLastTickCount = GetTickCount();
	LARGE_INTEGER nFrequency;
	QueryPerformanceFrequency( &nFrequency );
	m_nFrequencyDenom = 1.0 / (microtime)nFrequency.QuadPart;
	
	struct timeb tSysTime;
	ftime(&tSysTime);
	m_nInitialSystemTime = (microtime)tSysTime.time 
						+ (microtime)tSysTime.millitm / 1000.0;
#elif defined DARWIN
	(void) mach_timebase_info(&m_sTimebaseInfo);
#else // unix-like os
	struct timespec tv;
    clock_gettime( CLOCK_REALTIME, &tv );
	m_nInitialSecs = tv.tv_sec;
#endif
}

VistaDefaultTimerImp::~VistaDefaultTimerImp()
{
#ifdef WIN32

#elif defined DARWIN

#else // unix-like os

#endif
}

microtime  VistaDefaultTimerImp::GetMicroTime()   const
{
#ifdef WIN32
	microstamp nNewStamp = GetMicroStamp();
	microtime nTime = nNewStamp * m_nFrequencyDenom;
	return nTime;
#elif defined DARWIN
	// returns elapsed seconds (with high, hopefully nanosecond, precision)
	return double(mach_absolute_time()) * m_sTimebaseInfo.numer / m_sTimebaseInfo.denom / 1000000000.0;
#else // unix-like os
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv );
	
    return  ( (double)( tv.tv_sec - m_nInitialSecs )
			  + ((double)tv.tv_nsec / 1000000000.0) );
#endif
}

microstamp VistaDefaultTimerImp::GetMicroStamp()  const
{
#ifdef WIN32
	LARGE_INTEGER nTimestamp;
	QueryPerformanceCounter( &nTimestamp );	
	microstamp nStamp = (microstamp)( nTimestamp.QuadPart ) - m_nInitialStamp;
#ifdef CHECK_FOR_JUMPS_UNDER_WIN32
	// we have to convert ticks to time, and then compare to system clock
	microtime nTimeDelta = ( nStamp - m_nLastStamp ) * m_nFrequencyDenom;
	DWORD nNewTick = GetTickCount();
	if( nTimeDelta > JUMP_CHECK_MAX_MILLISECS 
		|| nTimeDelta < -JUMP_CHECK_MAX_MILLISECS )
	{
		microtime nTickDelta = (microtime)( nNewTick - m_nLastTickCount ) / 1000;
		microtime nDiscrepancy = nTickDelta - nTimeDelta;
		if( nDiscrepancy > JUMP_CHECK_MAX_MILLISECS
			|| nDiscrepancy < -JUMP_CHECK_MAX_MILLISECS )
		{
			microstamp nMissedTicks = (microstamp)( nDiscrepancy / m_nFrequencyDenom );
			//Safety check: new stamp really larger than previous one?
			if( nStamp + nMissedTicks <= m_nLastStamp )
			{
				// no nice solution, but oh well, shouldn't happen anyways ;)
				nMissedTicks = m_nLastStamp + 1000 - nStamp;
			}
			nStamp += nMissedTicks;
			// compensate for future readings
			m_nInitialStamp -= nMissedTicks;
		}
	}
	m_nLastStamp = nStamp;
	m_nLastTickCount = nNewTick;	
#endif
	return nStamp;

#elif defined DARWIN
  return mach_absolute_time();
#else // unix-like os

  #if defined(__GNUC__) && defined(LINUX)
  	__asm __volatile ( "rdtsc\n" );
  #else
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME,&ts);
	return (ts.tv_sec * 100000000) + (long long)ts.tv_nsec;
  #endif

#endif
}

microtime VistaDefaultTimerImp::GetSystemTime()  const
{
#ifdef WIN32
	//QueryPerformanceCounter returns a relative value, so
	//we have to convert it
	return ( m_nInitialSystemTime + GetMicroTime() );
#elif defined DARWIN
	// static since the struct is read each time, this way the memory has to 
	// be allocated only once (during global init) which saves time for measurements.
	static struct timeval tv;
	gettimeofday(&tv, NULL);
	
	return double(tv.tv_sec) + tv.tv_usec / 1000000.0;
#else // unix-like os
	return ( (double)m_nInitialSecs + GetMicroTime() );
#endif
}

systemtime VistaDefaultTimerImp::ConvertToSystemTime( const microtime mtTime ) const
{
#ifdef WIN32
	return ( m_nInitialSystemTime + mtTime );
#elif defined DARWIN
	// @todo fix this as in the linux imp, or get rid of the method
	// PS: I don't think it actually makes sense to have this routine,
	//     since microtimes are by definition intervals or times with arbitrary
	//     reference point, so the conversion is not meaningful IMO.
	return 0;
#else // unix-like os
	return ( (double)m_nInitialSecs + mtTime );
#endif
}