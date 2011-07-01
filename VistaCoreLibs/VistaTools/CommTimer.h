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


/*************************
 *  THIS IS LEGACY CODE  *
 * USE AT YOUR OWN RISK  *
 *************************/

#if !defined(_COMMTIMER_H)
#define _COMMTIMER_H

/*============================================================================*/
// INCLUDES
/*============================================================================*/
#include "VistaToolsConfig.h"

#include <sys/types.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>

#include "VistaTimer.h"

/*============================================================================*/
// MAKROS / DEFINES
/*============================================================================*/

#ifdef UXPV
	#include <sys/timesu.h>
#else
#ifdef WIN32
	#include <limits.h>
	#include <time.h>
	#include <iostream>
#else
	#include <unistd.h>
	#include <sys/times.h>
	#include <sys/time.h>
#endif
#endif

/***************************************************************************************************
* Class:   CCommTimerTiming
*
* Purpose: one time measurement
*****************************************************************************************************/

class VISTATOOLSAPI CCommTimerTiming
{
public:
	//////////////////////////////////////////////////////////////////////
	// Constructor / Destructor
	//////////////////////////////////////////////////////////////////////
	CCommTimerTiming();
	virtual ~CCommTimerTiming ();

	/** StartTiming, EndTiming, GetTiming
	* Purpose: Begin/Stop/Get time measurement
	*
	* @param actual time (may be VistaType::systemtime, Commtime or your own time system)
	*/
	void StartTiming (double nTime);

	void EndTiming (double nTime);
	void EndAdditiveTiming (double nTime);
	void EndAverageTiming (double nTime);

	double GetTiming (double nTime = 0);


	/** Set/Get TimingName
	*
	*/
	void SetTimingName (std::string sTimingName);
	std::string GetTimingName ();

	/** Set/Get UseCommTime
	*
	*/
	void SetUseCommTime (bool bUseCommTime);
	bool GetUseCommTime ();


	/** IsRunning
	* @return TRUE = time measurement started, but not stopped
	*/
	bool IsRunning ();

	/** IsEnded
	* @return TRUE = time measurement stopped
	*/
	bool IsEnded   ();

	void ResetTiming ();
protected:

	/** UseCommTime
	* Variable unused by CCommTimerTiming, but may be set for extern use
	* used by CCommTimer to decide wether timing is done by Systemtime or CommTime
	*/
	bool   m_bUseCommTime;


	/** Timing variables
	* Result = End - Begin
	*/
	double m_nBeginTiming;
	double m_nEndTiming;
	double m_nResultTiming;

	int		m_nNumberOfTimings;

	/** TimingName
	* Name of time measurement for identification and output
	*/
	std::string m_sTimingName;

};

/***************************************************************************************************
* Class:  CCommTimer
*
* Purpose: Timer with System- and CommTime, variable list of time measurements
*
*****************************************************************************************************/

class VISTATOOLSAPI CCommTimer : public DeprecatedTimer::VistaTimer
{
public:
	//////////////////////////////////////////////////////////////////////
	// Constructor / Destructor
	//////////////////////////////////////////////////////////////////////
	CCommTimer();
	virtual ~CCommTimer();


	//////////////////////////////////////////////////////////////////////
	// Timing functions
	//////////////////////////////////////////////////////////////////////

	/** NewTiming
	* Purpose: creates new time measurement named sTimingName in timing list
	*
	* @param sTimingName  = name of time measurement
	* @param bUseCommTime = TRUE if use GetCommTime for measurement / FALSE if use Systemtime for measurement
	*/
	void NewTiming (std::string sTimingName, bool bUseCommTime);

	/** GetTimingByName
	* Purpose: get a timing from the timing list by its name
	*
	* @param  sTimingName = name of time measurement
	* @param sTimingName = name of time measurement
	*/
	CCommTimerTiming* GetTimingByName (std::string sTimingName);

	/** Start/End/GetTiming
	* Purpose: start/stop/get time measurement
	*
	* @param  sTimingName = name of time measurement
	* @param [only GetTiming] if timing is stopped the time between starttiming and endtiming
	*                          if timing is running the actual runtime
	*/
	void StartTiming (std::string sTimingName);
	void EndTiming (std::string sTimingName);
	void EndAdditiveTiming (std::string sTimingName);
	void EndAverageTiming (std::string sTimingName);
	double GetTiming (std::string sTimingName);


	//////////////////////////////////////////////////////////////////////
	// Output file functions
	//////////////////////////////////////////////////////////////////////

	/** AddTime
	* Purpose: adds an output string to the output file
	*
	* @param sHostName, sCheckName, nNumWorkers, nNeededTime
	*/
	void AddTime(std::string sHostName,
				 std::string sCheckName,
				 int nNumWorkers,
				 double nNeededTime);

	/** WriteTime
	* Purpose: writes output file
	*
	* @return TRUE = success / FALSE = failed
	*/
	bool WriteTime();

	/** SetTimeName
	* Purpose: set output filename
	*
	* @param pOutputName : filename of timer-output file
	*/
	void SetTimeName(const char * pOutputName);

	/** SetOutputPrefix
	* Purpose: set string-prefix for output of timings from timinglist
	*
	* @param sOutputPrefix
	*/
	void SetOutputPrefix (std::string sOutputPrefix);

	/**
	*	Purpose: Add a timing specific comment to timefile (e.g. what has been measured, specific parameters...)
	*
	*	@param string sComment		Comment to be added
	*/
	void AddComment(std::string sComment){
		m_pTimerFile << sComment << std::endl;
	}


	//////////////////////////////////////////////////////////////////////
	// Timer functions
	//////////////////////////////////////////////////////////////////////

	// IMPLEMENTATION
	// ================
	// GetTime() sets the internal time stamps which can be inquired later
	// with GetLastTime()
	// ================
	// CommTime is the optimized time stamp provided by the underlying
	// communication library
	virtual double GetCommTime ();
	virtual double GetLastCommTime ();
/*    // the system time is usually the time since 1.1.1970
	virtual double GetSystemTime ();
	virtual double GetLastSystemTime ();
	// convert system time into seconds go by today
	virtual double ConvertSystemToDayTime (double sysTime);

	// same as GetSystemTime/GetLastSystemTime
	virtual double GetTime ();
	virtual double GetLastTime ();
*/
	void Sleep (float msec);

	void ResetTimings ();
protected:
	// time stamps used by "GetLast..."
	double      m_nLastCommTimeInSeconds;
//    double      m_nLastSystemTimeInSeconds;

	// list of timings
	std::list<CCommTimerTiming> m_listTimings;

	// line-prefix for timings-output with WriteTime
	std::string                 m_sOutputPrefix;

	// output file variables
	std::ofstream      m_sFilestr;
	char *             m_pTimeName;
	std::ostringstream m_pTimerFile;

	//comments for time measurement
	std::list<std::string> m_liComments;
};

#endif // !defined(_COMMTIMER_H)

