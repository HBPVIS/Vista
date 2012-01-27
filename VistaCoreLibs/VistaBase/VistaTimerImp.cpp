/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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
// $Id: VistaTimerImp.cpp 21499 2011-05-25 08:17:39Z dr165799 $

#include "VistaTimerImp.h"

#include "VistaTimer.h"
#include "VistaDefaultTimerImp.h"

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

namespace
{
	IVistaTimerImp* STimerImp = NULL;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaTimerImp::IVistaTimerImp()
: m_iReferenceCount( 0 )
{
}

IVistaTimerImp::~IVistaTimerImp()
{
	//std::cout << "Deleting TimerImp!" << std::endl;
}

IVistaTimerImp *IVistaTimerImp::GetSingleton( bool bCreateDefaultIfNull )
{
	if( ( STimerImp == NULL ) && bCreateDefaultIfNull )
		SetSingleton( new VistaDefaultTimerImp );
	return STimerImp;
}

void IVistaTimerImp::SetSingleton( IVistaTimerImp *pInterface )
{
	// note that WE DO NOT LOCK, so make sure, this
	// call is not done in a multi-threaded environment!	
	if( STimerImp != NULL )
	{
		STimerImp->DecReferenceCount();
	}
	STimerImp = pInterface;
	if( STimerImp != NULL )
	{
		STimerImp->IncReferenceCount();
	}
}

void IVistaTimerImp::IncReferenceCount()
{
	++m_iReferenceCount;
}

void IVistaTimerImp::DecReferenceCount()
{
	--m_iReferenceCount;
	if( m_iReferenceCount == 0 )
		delete this;
}


/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

