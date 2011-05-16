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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>


#if defined(VISTA_THREADING_SPROC)

#include "VistaSPROCMutexImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

usptr_t * VistaSPROCMutexImp::irixArena = 0;
static int iArenaRefCount = 0;

VistaSPROCMutexImp::VistaSPROCMutexImp (const std::string &sName, IVistaMutexImp::eScope nScope)
{
		// acquire single-process mutex
		if(iArenaRefCount == 0 && !irixArena)
			irixArena = usinit (sName.empty() ? "/dev/zero" : sName.c_str());
		
		++iArenaRefCount;
		

	irixLock = usnewlock ( irixArena );
}

VistaSPROCMutexImp::~VistaSPROCMutexImp ()
{
	usfreelock            ( irixLock, irixArena );
		if(--iArenaRefCount == 0)
			usdetach(irixArena);
}


void VistaSPROCMutexImp::Lock    ()
{
	ussetlock            ( irixLock );
}

bool VistaSPROCMutexImp::TryLock ()
{
	return ( uscsetlock            ( irixLock, 5 )    == 1 );
}

void VistaSPROCMutexImp::Unlock  ()
{
	usunsetlock          ( irixLock );
}


#endif // VISTA_THREADING_SPROC

