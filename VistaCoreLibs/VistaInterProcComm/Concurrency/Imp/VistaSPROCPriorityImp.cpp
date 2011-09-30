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
// $Id: VistaSPROCPriorityImp.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>
#if defined(VISTA_THREADING_SPROC)

#include "VistaSPROCPriorityImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSPROCPriorityImp::VistaSPROCPriorityImp()
{
	// s ist halt so :B)
	SetSystemPriorityMax( -20 );
	SetSystemPriorityMin( 20 );

	InitInterval();
}


VistaSPROCPriorityImp::~VistaSPROCPriorityImp()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif // VISTA_THREADING_POSIX


