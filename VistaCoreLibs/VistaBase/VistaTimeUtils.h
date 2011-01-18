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

#ifndef _VISTATIMEUTILS_H
#define _VISTATIMEUTILS_H

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"
#include "VistaTimer.h"

#include <vector>
#include <string>

class VISTABASEAPI VistaTimeUtils
{
public:
	/**
	 * @param system time
	 * @return time since start of current day
	 */
	static microtime ConvertToDayTime( systemtime mtTime );
	/**
	 * Converts a system time value to several integer values
	 * describing the current time and date
	 */
	static void ConvertToDate( const systemtime dTime, 
								int& iMillisecond,
								int& iSecond,
								int& iMinute,
								int& iHour,
								int& iDay,
								int& iMonth,
								int& iYear );

	/**
	 * Converts the system time value to a string of the form
	 * YYMMDD_HHMMSS, which lexicographically sorts by age
	 * This is especially suitable as postfix for filenames
	 */
	static std::string ConvertToLexicographicDateString( const systemtime dTime );

	static void Sleep( int iMilliseconds );

	/**
	 * @return The standard timer singleton (same as from VistaTimer).
	 *         If no timer instance is available yet, a new one will
	 *         be created using the current TimerImp singleton.
	 *         NOTE: If no default timer imp is set, an invalid
	 *         Timer will be created!
	 */
	static const VistaTimer& GetStandardTimer();

};

#endif /* _VISTATIMERINTEUTILS_H */
