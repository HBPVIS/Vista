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

#include "VistaTimer.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <iostream>
#if defined(WIN32)
	#include <Time.h>
	#include <Windows.h>
#pragma warning(disable: 4996)
#else
	#include <unistd.h>
	#include <sys/times.h>
	#include <sys/time.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace DeprecatedTimer
{

#if !defined WIN32

double VistaTimer::m_a = initM_a();

#endif

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaTimer::VistaTimer()
{
	m_dObjektStartTime = GetSystemTime();
	m_nLastTimeInSeconds = 0;
	m_nLastSystemTimeInSeconds = 0;
}


VistaTimer::~VistaTimer()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#if !defined WIN32

double VistaTimer::initM_a()
{
	struct tms start;
	return times(&start);
}

#endif

/*NAME : GetSystemTime */
// the system time is the time in seconds since midnight (00:00:00), January 1, 1970

double VistaTimer::GetSystemTime()
{
	double systime;
	struct timeb x;

	ftime(&x);
	//The _ftime function gets the current local time and stores it
	//in the structure pointed to by x.
	systime =  (double)x.time + ((double)x.millitm)/1000;
	m_nLastSystemTimeInSeconds = systime;

	return systime;
}

bool VistaTimer::GetSystemTime( long &tv_secs, long &tv_usecs) const
{
#if !defined(WIN32)
	struct timeval tv;
	if(gettimeofday(&tv, NULL) == -1)
		return false;
	tv_secs = long(tv.tv_sec);
	tv_usecs = long(tv.tv_usec);
	return true;
#else
	return false;
#endif

}

/*NAME  :GetLastSystemTime */

double VistaTimer::GetLastSystemTime () const
{
	return m_nLastSystemTimeInSeconds;
}

/* NAME : GetTimeOfDay */
// convert system time in time of day in seconds
double VistaTimer::ConvertSystemToDayTime(double dSysTime)
{
	double today=0;
	struct tm * z;
	double mlsec = dSysTime - (long)dSysTime;
	//struct timeb x;
	time_t x = (time_t)dSysTime;
	//ftime(&x);
	z = localtime(&x);//.time
	//The localtime function converts a time stored as a time_t value
	//and stores the result in a structure of type tm (with local correction).
	today = ((double)z->tm_sec) + (((double)z->tm_min)*60) + (((double)z->tm_hour))*3600 + mlsec;//(((double)x.millitm)/1000);

	return today;
}

void VistaTimer::ConvertToSimpleValues( double dSysTime, int &nSec, int &nMin, int &nHour )
{
	double today=0;
	struct tm * z;
	double mlsec = dSysTime - (long)dSysTime;
	//struct timeb x;
	time_t x = (time_t)dSysTime;
	//ftime(&x);
	z = localtime(&x);//.time
	//The localtime function converts a time stored as a time_t value
	//and stores the result in a structure of type tm (with local correction).
    nSec = (int)z->tm_sec;
    nMin = (int)z->tm_min;
    nHour = (int)z->tm_hour;
}

/*NAME : GetProcessTimeLife  */
//Calculate the processor time used by the calling process.
double VistaTimer::GetTime()
{
	double processtime;

#if defined(WIN32)
	clock_t y;
	y=clock();
	//The clock function tells how much processor time the calling process has used.
	processtime = ((double)y)/CLOCKS_PER_SEC;
	//The time in seconds is approximated by dividing the clock return value
	//by the value of the CLOCKS_PER_SEC constant.

#else
	clock_t s;
	struct tms end;
	long clktck;
	s=times(&end);
	clktck = sysconf(_SC_CLK_TCK);
	processtime = (s - m_a)/(double)clktck;

#endif
	m_nLastTimeInSeconds = processtime;
	return processtime;

}


/*NAME  : GetLastTime */

double VistaTimer::GetLastTime () const
{
	return m_nLastTimeInSeconds;
}

/*NAME : GetTimerLifeTime */
//Calculate the object lifetime
double VistaTimer::GetLifeTime()
{
	return (this->GetSystemTime() - m_dObjektStartTime);
}


/*NAME :  Sleep */
//The Sleep function suspends the execution of the current thread for a specified interval.
void VistaTimer::Sleep(int mlsec)

{
#ifdef WIN32
		::Sleep((int)mlsec);
#else
	if(mlsec != 0)
	{
		if( mlsec >= 1000)
		{
			mlsec = mlsec * 1000;
			struct timeval tv;
			tv.tv_sec  = (mlsec != 0) ? (mlsec / 1000000) : 0;
			tv.tv_usec = (mlsec != 0) ? (mlsec % 1000000) : 0;
				// simply poll, but in microwait!
			select(0, NULL, NULL, NULL, ((tv.tv_sec!=0 || tv.tv_usec!=0) ? &tv : NULL));
		}
		else
			usleep(mlsec*1000);
	}
#endif
}

/*NAME  : GnetleSecondSleep*/

void VistaTimer::GentleSecondSleep(int iSecs)
{
	VistaTimer::Sleep(iSecs*1000);
}

/*NAME : GetDate */

std::string VistaTimer::GetDateString()
{
	time_t ms = (long)this->GetSystemTime();
	return (ctime(&ms));
}

}
