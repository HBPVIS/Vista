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

#ifndef _VISTATHREADTASK_H
#define _VISTATHREADTASK_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaThread.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaThreadedTask;
class VistaThreadTask;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaThreadTask : public VistaThread
{
public:
	VistaThreadTask();
	virtual ~VistaThreadTask();
	virtual void ThreadBody   ();

	bool SetThreadedTask(IVistaThreadedTask *);
	IVistaThreadedTask *GetThreadedTask() const;

	virtual void PostRun();
protected:
private:

	IVistaThreadedTask *m_pTask;
};

class VISTAINTERPROCCOMMAPI IVistaThreadedTask
{
	friend class VistaThreadTask;
public:
	virtual ~IVistaThreadedTask();

	bool GetIsDone() const;


	/**
	 * indicates whether this work instance is CURRENTLY processed.
	 * a DONE work will return false here!
	 */
	bool GetIsProcessed() const;

	void ThreadWork();

protected:
	IVistaThreadedTask();

	virtual void PreWork();
	virtual void PostWork();
	virtual void DefinedThreadWork() = 0;
private:
	void StartWork();
	void StopWork();

	bool m_bIsProcessed;
	bool m_bIsDone;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

