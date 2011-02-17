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

#include "VistaProcessImp.h"

#if defined(VISTA_THREADING_WIN32)

#include "VistaWin32ProcessImp.h"
#elif defined(VISTA_THREADING_POSIX) || defined(VISTA_THREADING_SPROC)
#include "VistaPosixProcessImp.h"
#else
#error "PROCESSIMPLEMENTATION UNSUPPORTED OR UNDEFINED FOR THIS PLATFORM!"
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaProcessImp::IVistaProcessImp()
{
}

IVistaProcessImp::~IVistaProcessImp()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVistaProcessImp *IVistaProcessImp::CreateProcessImp()
{
	// factory method
#if defined(WIN32)
	return new VistaWin32ProcessImp;
#elif defined(LINUX) || defined(HPUX) || defined(SUNOS) || defined(IRIX)
	return new VistaPosixProcessImp;
#else
	return 0;
#endif
}

void IVistaProcessImp::PreRun()
{
}

void IVistaProcessImp::PostRun()
{
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

