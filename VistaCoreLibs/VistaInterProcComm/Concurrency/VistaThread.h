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

#ifndef _VISTATHREAD_H
#define _VISTATHREAD_H


/*============================================================================*/
/* DEFINES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaInterProcComm/Concurrency/VistaFork.h"

#include <string>


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPriority;
class IVistaThreadImp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 *
 */
class VISTAINTERPROCCOMMAPI VistaThread : public VistaFork
{
public:


	/**
	 *
	 */
	VistaThread (IVistaThreadImp *pImp = NULL);


	/**
	 *
	 */
	virtual  ~VistaThread ();


	/**
	 *
	 */
	bool      Run        ();

	/**
	 *
	 */
	bool      Suspend         ();


	/**
	 *
	 */
	bool      IsRunning() const;


	/**
	 *
	 */
	bool      Resume     ();

	/**
	 *
	 */
	bool      Join       ();


	/**
	 *
	 */
	bool      Abort       ();


	/**
	 *
	 */
	void      SetCancelAbility(const bool bOkToCancel);

	/**
	 *
	 */
	bool      CanBeCancelled() const;

	/**
	 *
	 */
	bool      SetPriority  ( const VistaPriority &inPrio );


	/**
	 *
	 */
	void GetPriority  (VistaPriority &) const;


	/**
	 * derive your own class from VistaThread and override this method
	 *
	 * ThreadBody is called when a VistaThread is Start()ed and when
	 * this method returns, the thread is finished.
	 */
	virtual void ThreadBody   () = 0;


	/**
	 * give the processor away temporarily
	 */
	void            YieldThread   ();

	/**
	 * Method that is to be performed BEFORE departed fork starts execution
	 */
	virtual void PreRun();

	/**
	 * Method that is to be performed AFTER forked work is done
	 */
	virtual void PostRun();

	bool Equals(const VistaThread &oOther) const;
	bool operator==(const VistaThread &oOther);

	virtual bool GetIsFinished() const;
	long GetThreadIdentity() const;

	/**
	 * Sets the affinity (i.e. stickiness) of the thread to a
	 * given CPU id. Note that it is implementation specific, when
	 * to call this. On windows(c), it is only working when calling
	 * this before a call to Run(). Using pthreads, this call can
	 * in theory be called at any point in the program.
	 * Note that the iProcessorNum means: CPU, not necessarily CORE!
	 * @return true when the affinity mask was set, false else
	 * @param iProcessorNum the number of the CPU (not CORE!) to
	          run this process on exclusively
	 */
	bool SetProcessorAffinity(int iProcessorNum);
	virtual bool SetThreadName(const std::string &sName);
	virtual std::string GetThreadName() const;

	/**
	 * returns the CPU on which the thread is running on at the
	 * time of call. This refers typically to the CPU the thread
	 * is running on NOT the core, the thread is scheduled to.
	 * @return -1 when something really weird happened (i.e., not implemented)
	 */
	int GetCpu() const;

protected:
	IVistaThreadImp *GetThreadImp() const;
private:

	/**
	 *
	 */
	bool m_bIsRunning, m_bIsFinished;

	/**
	 *
	 */
	IVistaThreadImp *m_pImp;
};


/*============================================================================*/
/* INLINED IMPLEMENTATION                                                     */
/*============================================================================*/


#endif // _VISTATHREAD_H