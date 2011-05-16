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


#ifndef _VISTADEFAULTTIMERIMP_H
#define _VISTADEFAULTTIMERIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#include <windows.h>
#endif

#if defined(DARWIN)
#include <mach/mach_time.h>
#endif

#include "VistaTimerImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTABASEAPI VistaDefaultTimerImp : public IVistaTimerImp
{
public:
	VistaDefaultTimerImp();	

	virtual microtime  GetMicroTime()   const;
	virtual microstamp GetMicroStamp()  const;
	virtual microtime  GetSystemTime()  const;
	virtual systemtime ConvertToSystemTime( const microtime mtTime ) const;

protected:
	~VistaDefaultTimerImp();
private:
#ifdef WIN32
	mutable microstamp		m_nInitialStamp;
	mutable microstamp		m_nLastStamp;	
	mutable DWORD			m_nLastTickCount;
	microtime				m_nFrequencyDenom;
	microtime				m_nInitialSystemTime;
#elif defined DARWIN
	mach_timebase_info_data_t       m_sTimebaseInfo;
#else
	long					m_nInitialSecs;
#endif
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif /* _VISTADEFAULTTIMERIMP_H */
