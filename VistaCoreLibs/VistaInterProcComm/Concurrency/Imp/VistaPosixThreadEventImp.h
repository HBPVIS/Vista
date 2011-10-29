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
// $Id: VistaPosixThreadEventImp.h 22867 2011-08-07 15:29:00Z dr165799 $

#if !defined(VISTA_THREADING_WIN32)

#ifndef _VISTAPOSIXTHREADEVENTIMPL_H
#define _VISTAPOSIXTHREADEVENTIMPL_H


#include "VistaThreadEventImp.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaThreadEventImp;

class VISTAINTERPROCCOMMAPI VistaPosixThreadEventImp : public IVistaThreadEventImp
{
private:
	int m_fd[2];
protected:
public:

	VistaPosixThreadEventImp( );
	virtual ~VistaPosixThreadEventImp();


	void SignalEvent();
	long WaitForEvent(bool bBlock);
	long WaitForEvent(int iBlockTime);

	virtual HANDLE GetEventSignalHandle() const;
	virtual HANDLE GetEventWaitHandle() const;

	bool ResetThisEvent();
};


/*============================================================================*/

#endif // _VISTAITERATIONTHREAD_H

#endif // VISTA_THREADING_WIN32

