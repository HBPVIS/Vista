/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2011 RWTH Aachen University               */
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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#if defined(WIN32)
#pragma warning(disable: 4996)
#pragma warning(disable: 4786)
#include <Windows.h>
#endif

#if defined(SUNOS)
#include <strings.h>
#endif

#include <string>
#include <cstring>
#include "CommTimer.h"

// prototyping
double r_ctime__ (void);
double r_ctime_  (void);
double r_ctime   (void);


/*============================================================================*/
/*                                                                            */
/*  CLASS      :   CCommTimerTiming                                           */
/*                                                                            */
/*============================================================================*/

//////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////

CCommTimerTiming::CCommTimerTiming()
{
	m_nEndTiming = 0;
	m_nResultTiming = 0;
	m_nBeginTiming = 0;
	m_bUseCommTime = false;
	m_nNumberOfTimings = 0;
}

CCommTimerTiming::~CCommTimerTiming()
{
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   StartTiming /EndTiming                                      */
/*                                                                            */
/*============================================================================*/

inline void CCommTimerTiming::StartTiming (double nTime)
{
	m_nEndTiming = 0;
	m_nBeginTiming = nTime;
}

inline void CCommTimerTiming::EndTiming (double nTime)
{
	m_nEndTiming = nTime;
	m_nResultTiming = m_nEndTiming - m_nBeginTiming;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  EndAdditiveTiming                                            */
/*                                                                            */
/*============================================================================*/

inline void CCommTimerTiming::EndAdditiveTiming (double nTime)
{
	m_nEndTiming = nTime;
	m_nResultTiming += (m_nEndTiming - m_nBeginTiming);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  EndAverageTiming			                                  */
/*                                                                            */
/*============================================================================*/

inline void CCommTimerTiming::EndAverageTiming (double nTime)
{
	m_nEndTiming = nTime;
	if (m_nNumberOfTimings==0)
	{
		m_nNumberOfTimings++;
		m_nResultTiming = (m_nEndTiming - m_nBeginTiming);
	}
	else
	{
		m_nNumberOfTimings++;
		m_nResultTiming = (
							(m_nEndTiming - m_nBeginTiming)+
							(m_nResultTiming*(m_nNumberOfTimings-1))
						  )/m_nNumberOfTimings;
	}
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetTiming                                                   */
/*                                                                            */
/*============================================================================*/


double CCommTimerTiming::GetTiming (double nTime)
{
	if (m_nEndTiming == 0)
	{
		return (nTime - m_nBeginTiming);
	}
	else
	{
		return m_nResultTiming;
	}
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetTimingName                                           */
/*                                                                            */
/*============================================================================*/


inline void CCommTimerTiming::SetTimingName (std::string sTimingName)
{ 
	m_sTimingName = sTimingName; 
}

inline std::string CCommTimerTiming::GetTimingName ()
{ 
	return m_sTimingName; 
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetUseCommTime                                          */
/*                                                                            */
/*============================================================================*/


inline void CCommTimerTiming::SetUseCommTime (bool bUseCommTime)
{ 
	m_bUseCommTime = bUseCommTime; 
}

inline bool CCommTimerTiming::GetUseCommTime ()
{ 
	return m_bUseCommTime; 
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsRunning                                                   */
/*                                                                            */
/*============================================================================*/

inline bool CCommTimerTiming::IsRunning ()
{ 
	return (m_nBeginTiming != 0); 
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsEnded                                                     */
/*                                                                            */
/*============================================================================*/

inline bool CCommTimerTiming::IsEnded   ()
{ 
	return (m_nEndTiming != 0); 
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ResetTiming                                                 */
/*                                                                            */
/*============================================================================*/

void CCommTimerTiming::ResetTiming   ()
{ 
	m_nEndTiming = 0;
	m_nResultTiming = 0;
	m_nBeginTiming = 0;
	m_nNumberOfTimings = 0;
}

/******************************************************************************/
/*                                                                            */
/*  CLASS      :   CCommTimer                                                 */
/*                                                                            */
/******************************************************************************/

//////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////

CCommTimer::CCommTimer()
{
	m_nLastCommTimeInSeconds   = 0;
	
	this->m_pTimeName = new char[100];

	m_listTimings.clear ();
}
CCommTimer::~CCommTimer()
{
	m_listTimings.clear ();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetCommTime                                                     */
/*                                                                            */
/*============================================================================*/

double CCommTimer::GetCommTime ()
{
	double   commSeconds = 0;

#ifdef WIN32
	clock_t     curClock;
	time_t      curTime;
	curClock = clock();
	time (&curTime);
	commSeconds =  (double)(curClock) / CLOCKS_PER_SEC;
	//return curTime;
#else
		//commSeconds = r_ctime ();
		commSeconds = GetLifeTime ();
#endif
	// store current stamp as last stamp for later queries
	m_nLastCommTimeInSeconds = commSeconds;
	
	return commSeconds;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetLastCommTime                                             */
/*                                                                            */
/*============================================================================*/
double CCommTimer::GetLastCommTime ()
{
	return m_nLastCommTimeInSeconds;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetSystemTime                                               */
/*                                                                            */
/*============================================================================*/
/*
double CCommTimer::GetSystemTime()
{
	double  sysSeconds;

#ifdef WIN32
	time_t   timeSince1970;
	//localtime (&timeSince1970);
	time (&timeSince1970);
	sysSeconds = (double)timeSince1970;
#else

	struct timeval tv;
	//tv.tv_sec = 0;
	//tv.tv_usec = 0;
	gettimeofday(&tv, (struct timezone*)0);

	// seconds since 1.1.1970
	sysSeconds = ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0 );
#endif
	// store current stamp as last stamp for later queries
	m_nLastSystemTimeInSeconds = sysSeconds;

	return sysSeconds;
}
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetLastSystemTime                                           */
/*                                                                            */
/*============================================================================*/
/*double CCommTimer::GetLastSystemTime ()
{
	return m_nLastSystemTimeInSeconds;
}
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetTime                                                     */
/*                                                                            */
/*============================================================================*/
/*
double CCommTimer::GetTime()
{
	return GetSystemTime ();
}
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetLastTime                                                 */
/*                                                                            */
/*============================================================================*/
/*double CCommTimer::GetLastTime ()
{
	return m_nLastSystemTimeInSeconds;
}
*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ConvertSystemToDayTime                                      */
/*                                                                            */
/*============================================================================*/
/*
double CCommTimer::ConvertSystemToDayTime (double  totalSecs)
{
	double  secondsToday = 0;

	// a day has 86400 seconds
	secondsToday = totalSecs - (((int)(totalSecs / 86400.0)) * 86400.0);

	return secondsToday;
}
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   Sleep					                                      */
/*                                                                            */
/*============================================================================*/

inline void CCommTimer::Sleep (float msec)
		{
#ifdef WIN32
			::Sleep((int)msec);
//#elif defined(IRIX) || defined(LINUX) || defined(SUNOS)
//          // must be less than 1.000.000 !!! (<1sec)
//			usleep(static_cast<int>(msec*1000.0f));
#else

			// clock() returns current time in secs divided by CLOCK_PER_SEC
			// POSIX requires that CLOCKS_PER_SEC equals 1000000 independent
			// of the actual resolution.
			long counter = clock() + (long)(msec * CLOCKS_PER_SEC) / 1000;
			// perform own sleep
			while ( clock() < counter);                                          
			//WTmsleep(msec)
			//float sec = msec/1000;
			//if ((sec < 1)&&(msec > 0))
			//    sec = 1;
			//sleep (sec);
#endif
		}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   r_ctime                                                     */
/*                                                                            */
/*============================================================================*/
/*double r_ctime (void)
{
#ifndef WIN32
	double  user, sys, factor;
	int rc;
#ifdef UXPV
	struct  tmsu t1;
	if ( (rc=timesu(&t1)) == -1 ) perror("r_lib:r_ctime_c:timesu");
	factor = 0.000001;
#else
	//struct//  tms t1;
	//if ( (rc=times(&t1)) == -1 ) perror("r_lib:r_ctime_c:times");
	// aus limits.h: factor = 1.0 / CLK_TCK; //
	factor = 1.0 / (double) sysconf(_SC_CLK_TCK);
#endif
	user    = (double) (t1.tms_cutime + t1.tms_utime );
		sys     = (double) (t1.tms_cstime + t1.tms_stime );

	return ((user + sys) * factor);
#endif
	return 0;
}

// ============================================================================
// ============================================================================
#ifdef UNICOS
double R_CTIME (void)
#else
double r_ctime_ (void)
#endif
{
#ifndef WIN32
	double  user, sys, factor;
	int rc;
#ifdef UXPV
	struct  tmsu t1;
	if ( (rc=timesu(&t1)) == -1 ) perror("r_lib:r_ctime_c:timesu");
	factor = 0.000001;
#else
	struct  tms t1;
	//if ( (rc=times(&t1)) == -1 ) perror("r_lib:r_ctime_c:times");
	// aus limits.h: factor = 1.0 / CLK_TCK; //
	factor = 1.0 / (double) sysconf(_SC_CLK_TCK);
#endif
	user    = (double) (t1.tms_cutime + t1.tms_utime );
		sys     = (double) (t1.tms_cstime + t1.tms_stime );

	return ((user + sys) * factor);
#endif
	return 0;
}
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   r_ctime__                                                   */
/*                                                                            */
/*============================================================================*/
/*
double r_ctime__ (void)
{
#ifndef WIN32
		double  user, sys, factor;
		int rc;
#ifdef UXPV
		struct  tmsu t1;
		if ( (rc=timesu(&t1)) == -1 ) perror("r_lib:r_ctime_c:timesu");
		factor = 0.000001;
#else
		struct  tms t1;
		//if ( (rc=times(&t1)) == -1 ) perror("r_lib:r_ctime_c:times");
		// aus limits.h: factor = 1.0 / CLK_TCK; //
		factor = 1.0 / (double) sysconf(_SC_CLK_TCK);
#endif
		user    = (double) (t1.tms_cutime + t1.tms_utime );
		sys     = (double) (t1.tms_cstime + t1.tms_stime );

		return ((user + sys) * factor);
#endif
		return 0;
}
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetTimeName  / SetOutputPrefix                              */
/*                                                                            */
/*============================================================================*/

void CCommTimer::SetTimeName(const char * pOutputName)
{
	strcpy(this->m_pTimeName, pOutputName); 
}

void CCommTimer::SetOutputPrefix (std::string sOutputPrefix) 
{ 
	m_sOutputPrefix = sOutputPrefix; 
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   AddTime                                                     */
/*                                                                            */
/*============================================================================*/

void CCommTimer::AddTime(std::string sHostName, 
						 std::string sCheckName, 
						 int nNumWorkers, 
						 double nNeededTime)
{
	this->m_pTimerFile << sHostName << " timing: " << sCheckName 
					   << " with " << nNumWorkers << " workers, needed time: " 
					   << nNeededTime << std::endl;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   WriteTime                                                   */
/*                                                                            */
/*============================================================================*/

bool CCommTimer::WriteTime()
{
	m_sFilestr.open (m_pTimeName,std::ios::app );
   
	m_sFilestr << m_pTimerFile.str().c_str();

	// insert timings from m_listTimings with Prefix m_sOutputPrefix
	std::list<CCommTimerTiming>::iterator i;
	i = m_listTimings.begin();
	while (i != m_listTimings.end())
	{
		m_sFilestr << m_sOutputPrefix << " timing: " << i->GetTimingName () 
					   <<" needed time: " 
					   << i->GetTiming(this->GetCommTime())<< std::endl;
		++i;
	}
	m_sFilestr << std::endl;

	m_sFilestr.close();

	return 1;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   NewTiming                                                   */
/*                                                                            */
/*============================================================================*/

void CCommTimer::NewTiming (std::string sTimingName, bool bUseCommTime)
{
	CCommTimerTiming newTiming;
	newTiming.SetTimingName (sTimingName);
	newTiming.SetUseCommTime (bUseCommTime);

	m_listTimings.insert (m_listTimings.end(),newTiming);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetTimingByName                                             */
/*                                                                            */
/*============================================================================*/

CCommTimerTiming* CCommTimer::GetTimingByName (std::string sTimingName)
{
	std::list<CCommTimerTiming>::iterator i;
	i = m_listTimings.begin();
	while (i != m_listTimings.end())
	{
		if (sTimingName == i->GetTimingName ())
			return &(*i);

		++i;
	}

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   StartTiming, EndTiming, GetTiming                           */
/*                                                                            */
/*============================================================================*/

void CCommTimer::StartTiming (std::string sTimingName)
{
	CCommTimerTiming* Timing = GetTimingByName (sTimingName);
	if (Timing != NULL)
		if (Timing->GetUseCommTime())
			Timing->StartTiming (this->GetCommTime());
		else
			Timing->StartTiming (this->GetSystemTime());

}

void CCommTimer::EndTiming (std::string sTimingName)
{
	CCommTimerTiming* Timing;
	Timing = GetTimingByName (sTimingName);
	if (Timing != NULL)
		if (Timing->GetUseCommTime())
			Timing->EndTiming (this->GetCommTime());
		else
			Timing->EndTiming (this->GetSystemTime());

}

void CCommTimer::EndAdditiveTiming (std::string sTimingName)
{
	CCommTimerTiming* Timing;
	Timing = GetTimingByName (sTimingName);
	if (Timing != NULL)
		if (Timing->GetUseCommTime())
			Timing->EndAdditiveTiming (this->GetCommTime());
		else
			Timing->EndAdditiveTiming (this->GetSystemTime());

}
void CCommTimer::EndAverageTiming (std::string sTimingName)
{
	CCommTimerTiming* Timing;
	Timing = GetTimingByName (sTimingName);
	if (Timing != NULL)
		if (Timing->GetUseCommTime())
			Timing->EndAverageTiming (this->GetCommTime());
		else
			Timing->EndAverageTiming (this->GetSystemTime());

}

double CCommTimer::GetTiming (std::string sTimingName)
{
	CCommTimerTiming* Timing = GetTimingByName (sTimingName);
	if (Timing != NULL)
		if (Timing->GetUseCommTime())
			return Timing->GetTiming (this->GetCommTime());
		else
			return Timing->GetTiming (this->GetSystemTime());
	else
	   return 0;
}

void CCommTimer::ResetTimings ()
{
	std::list<CCommTimerTiming>::iterator i = m_listTimings.begin();
	while (i != m_listTimings.end())
	{
		i->ResetTiming();
		++i;
	}
}

/*============================================================================*/
/*                                                                            */
/*  END OF FILE                                                               */
/*                                                                            */
/*============================================================================*/
