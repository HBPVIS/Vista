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
// $Id: VistaThreadEventImp.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

// project includes

#include "VistaThreadEventImp.h"


#if !defined(NULL)
#define NULL 0
#endif

#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32ThreadEventImp.h"
#else
#include "VistaPosixThreadEventImp.h"
#include "VistaPthreadThreadEventImp.h"
#endif


/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
//using namespace std;

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
IVistaThreadEventImp::IVistaThreadEventImp()
{
}

IVistaThreadEventImp::~IVistaThreadEventImp()
{}

IVistaThreadEventImp *IVistaThreadEventImp::CreateThreadEventImp(bool bCreatePosix)
{
#if defined(VISTA_THREADING_WIN32)
	return new VistaWin32ThreadEventImp;
	
#elif defined(VISTA_THREADING_POSIX)
	if(bCreatePosix)
		return new VistaPosixThreadEventImp;
	else
		return new VistaPthreadThreadEventImp;
#elif defined(VISTA_THREADING_SPROC)
	return new VistaPosixThreadEventImp;
#else
	printf("WARNING: NO THREAD-EVENT-IMP!\n");
	return 0;
#endif
}


// ============================================================================
// ============================================================================

