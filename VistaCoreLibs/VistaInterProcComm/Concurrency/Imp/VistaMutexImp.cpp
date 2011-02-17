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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#include "VistaMutexImp.h"


#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32MutexImp.h"
#elif defined(VISTA_THREADING_SPROC)
#include "VistaSPROCMutexImp.h"
#elif defined(VISTA_THREADING_POSIX)
#include "VistaPthreadsMutexImp.h"
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaMutexImp::IVistaMutexImp ()
{
}

IVistaMutexImp::~IVistaMutexImp ()
{
}

IVistaMutexImp *IVistaMutexImp::CreateMutexImp(const std::string &sName, eScope nScope)
{
#if defined(VISTA_THREADING_WIN32)
	return new VistaWin32MutexImp(sName, nScope);
#elif defined(VISTA_THREADING_SPROC)
	return new VistaSPROCMutexImp(sName, nScope);
#elif defined(VISTA_THREADING_POSIX)
	return new VistaPthreadsMutexImp(sName, nScope);
#else
#error NO THREAD MODEL?
	return 0;
#endif
}
	
