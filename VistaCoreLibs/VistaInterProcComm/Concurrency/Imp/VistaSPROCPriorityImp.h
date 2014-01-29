/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#if defined(VISTA_THREADING_SPROC)

#ifndef _VISTASPROCPRIORITYIMP_H
#define _VISTASPROCPRIORITYIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaPriorityImp.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaSPROCPriorityImp : public IVistaPriorityImp
{
public:
	VistaSPROCPriorityImp();
	virtual ~VistaSPROCPriorityImp();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAWIN32PRIORITY_H

#endif // VISTA_THREADING_POSIX


