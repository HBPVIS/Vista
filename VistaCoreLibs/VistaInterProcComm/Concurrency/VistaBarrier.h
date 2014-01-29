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

#ifndef _VISTABARRIER_H
#define _VISTABARRIER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaInterProcCommConfig.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSemaphore;
class VistaMutex;
class VistaThreadCondition;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Construct a barrier that threads can wait on.
 * Think of barriers as an atomic counter that threads wait for until it reaches
 * the nNumber threshold.
 * VistaBarrier b(5); will stop five threads and let them continue only when
 * there are 5 threads in total inside the Barrier.
 */
class VISTAINTERPROCCOMMAPI VistaBarrier
{
public:
	VistaBarrier(unsigned int nNumber);
	virtual ~VistaBarrier();

	/**
	 * This method will try to enter the calling thread inside the Barrier.
	 * If the current number of threads waiting for the barrier is
	 * smaller than GetNumWaitFor(), the calling thread is put to sleep
	 * and awakened once the queue is full up to GetNumWaitFor().
	 * In rare cases it might happen that a larger number of threads is trying
	 * to access the barrier than it was created for (e.g., 10 threads trying
	 * to access a barrier made for 5). The caller can now select an alternative:
	 * - wait for the barrier to have room again
	 * - try to get inside, if it is full, leave and return false in this method.
	 *
	 * The choice is up to you, but in general, you should not try to access a barrier
	 * with more threads than it can hold at the same time. Think of you programm design
	 * here.
	 *
	 * @param bBlockOnQueueFull the calling thread is blocked when
			  the current equals GetNumWaitFor(). If set to false,
			  and the current queue is still full, this method
			  returns false.
	 * @return true when the thread left the barrier.
	 */
	bool Enter(bool bBlockOnQueueFull = true);

	/**
	 * Returns the numbe given during creation of this barrier as the argument
	 * to the constructor.
	 * @return the number that defines the maximum capability for this barrier.
	 */
	unsigned int GetNumWaitFor() const;

	/**
	 * Returns the number of currently waiting threads.
	 * Note that the method tries to lock the counter in order to return
	 * its value safely, so be sure not to use this too often, as this could
	 * "confuse" the callers to Enter().
	 * @return the number of the currently waiting threads.
	 */
	unsigned int GetNumWaiting() const;

	/**
	 * use this method to cancel a wait for all participants of the barrier.
	 * This might be necessary when trying to leave a programm, but not all threads
	 * are in the barrier.
	 * Enter() will return false (!!)
	 *
	 */
	void CancelWait();

	void ResetCancelFlag();
protected:
private:
	VistaSemaphore *m_pQueueFullBlock;
	VistaMutex     *m_pWaitCondMutex;
	VistaThreadCondition *m_pWaitCondition;

	unsigned int m_nNumWait;
	const unsigned int m_nMaxWait;
	unsigned int m_nWaitLeaveState;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTABARRIER_H

