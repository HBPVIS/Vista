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
// $Id: VistaSPROCMutexImp.h 22867 2011-08-07 15:29:00Z dr165799 $

#if defined(VISTA_THREADING_SPROC)

#ifndef _VISTASPROCMUTEXIMP_H
#define _VISTASPROCMUTEXIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaMutexImp.h"


#include <unistd.h>
#include <signal.h>
#include <ulocks.h>
#include <sys/wait.h>
#include <sys/prctl.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaSPROCMutexImp : public IVistaMutexImp
{
public:
	VistaSPROCMutexImp(const std::string &sName, IVistaMutexImp::eScope nScope);
	virtual ~VistaSPROCMutexImp ();

	virtual void Lock();
	virtual bool TryLock();
	virtual void Unlock();

private:
	static usptr_t *     irixArena;
	ulock_t              irixLock;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTASYSTEM_H

#endif // VISTA_THREADING_SPROC


