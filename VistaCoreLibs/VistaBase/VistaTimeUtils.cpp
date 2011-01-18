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

#include "VistaTimeUtils.h"

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

#include "VistaTimer.h"
//
//namespace
//{
//	IVistaTimerImp *SSingleton = NULL;
//}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

microtime VistaTimeUtils::ConvertToDayTime( microtime dTime )
{	
	microtime dMilliseconds = dTime - (long)dTime;

	time_t tCurrentTime = (time_t)dTime;
	struct tm* tLocalTime = localtime(&tCurrentTime);

	microtime dDayTime = ( (microtime)tLocalTime->tm_sec) 
						+ ( ((microtime)tLocalTime->tm_min)*60)
						+ ( ((microtime)tLocalTime->tm_hour) )*3600
						+ dMilliseconds;//(((microtime)x.millitm)/1000);

	return dDayTime;
}

void VistaTimeUtils::ConvertToDate( const microtime dTime, 
								int& iMillisecond,
								int& iSecond,
								int& iMinute,
								int& iHour,
								int& iDay,
								int& iMonth,
								int& iYear )
{
	iMillisecond = (int)( ( dTime - (long)dTime ) * 1000 );

	time_t tCurrentTime = (time_t)dTime;
	struct tm* tLocalTime = localtime(&tCurrentTime);

	iSecond = tLocalTime->tm_sec;
	iMinute = tLocalTime->tm_min;
	iHour = tLocalTime->tm_hour;
	iDay = tLocalTime->tm_mday;
	iMonth = 1 + tLocalTime->tm_mon;
	iYear = 1900 + tLocalTime->tm_year;
}

std::string VistaTimeUtils::ConvertToLexicographicDateString( const systemtime dTime )
{
	std::string sResult;
	sResult.resize( 13 );

	time_t tCurrentTime = (time_t)dTime;
	struct tm* tLocalTime = localtime(&tCurrentTime);

	strftime( &sResult[0], 14 , "%y%m%d_%H%M%S", tLocalTime );
	return sResult;
}

void VistaTimeUtils::Sleep( int iMilliseconds )
{
#ifdef WIN32
	::Sleep( iMilliseconds );
#else
	if( iMilliseconds != 0 )
	{
		if( iMilliseconds >= 1000)
		{
			int iMicroseconds = iMilliseconds * 1000;
			struct timeval tv;
			tv.tv_sec  = iMicroseconds / 1000000;
			tv.tv_usec = iMicroseconds % 1000000;
				// simply poll, but in microwait!
			select(0, NULL, NULL, NULL, &tv );
		}
		else
			usleep( iMilliseconds*1000 );
	}
#endif
}

const VistaTimer& VistaTimeUtils::GetStandardTimer()
{
	return VistaTimer::GetStandardTimer();
}

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

