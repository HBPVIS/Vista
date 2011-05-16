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

#if !defined(_VISTATOOLSTIMER_H)
#define _VISTATOOLSTIMER_H

/**
 * @todo mode into VistaAspects
 */
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"
#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
namespace DeprecatedTimer
{
	class VISTATOOLSAPI VistaTimer
	{
		public:
			// CONSTRUCTOR / DESTRUCTOR
			// Initializing class variables
			VistaTimer();
			virtual ~VistaTimer();

			//IMPLEMENTATION

			//convert system time in time of day in seconds static
			double ConvertSystemToDayTime(double dSysTime);

			static void ConvertToSimpleValues( double dSysTime, int &nSec, int &nMin, int &nHour );

			//Calculate the time used by the calling process
			double GetTime();

			//Calculate the object lifetime
			double GetLifeTime();

			//The Sleep function suspends the execution of the current thread for a specified interval
			static void Sleep(int mlsec);

			static void GentleSecondSleep(int iSecs);

			double GetLastTime() const;

			double GetLastSystemTime() const;

			//the time in seconds since midnight (00:00:00), January 1, 1970
			double GetSystemTime();

			// implemented only for non-win systems (returns false on windows)!!!
			bool GetSystemTime( long &tv_secs, long &tv_usecs) const;

			//get actual date  as string
			std::string GetDateString();

	#if !defined WIN32

			static double initM_a();

	#endif

		// MEMBERS

		private:

			double m_nLastTimeInSeconds;

			double m_nLastSystemTimeInSeconds;

			//variable for lifetime of objekt
			double m_dObjektStartTime;

	#if !defined WIN32

			//variable for a starttime of process
			static double m_a;
	#endif

	};
}
#endif //_VISTATOOLSTIMER_H



