/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#ifndef _VISTADRIVERTHREADASPECT_H
#define _VISTADRIVERTHREADASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThread;
class VistaDriverPollThread;
class VistaPriority;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Some devices may define their own thread of execution, instead of per-frame
 * updates or connection driven (data driven) approaches.
 * These drivers may export and configure the ThreadAspect, with which the
 * VistaSystem'' can work and configure the driver (start/stop etc.).
 * So in case you need a custom, asynchronous update thread for your driver,
 * consider using this aspect for it. It will simplify existing interfaces.
 * - the IDriverUpdatePrepare() which is user defined is called PreUpdate()
 * - the driver's Update() method is called
 * - the IDriverUpdatePrepare() which is user defined is called PostUpdate()
 *
 *
 * @see VistaDriverThreadAspect::IDriverUpdatePrepare()
 */
class VISTADEVICEDRIVERSAPI VistaDriverThreadAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	/**
	 * The thread aspect will setup a thread that will continuously call
	 * a user defined update method, but that will need the driver to
	 * work on. That's why we pass the parent driver here.
	 * @param the driver to activate.
	 */
	VistaDriverThreadAspect(IVistaDeviceDriver *pDriver);

	/**
	 * is the update thread is still running, the destructor will try to
	 * gently stop the update thread. Note that this might deadlock, when
	 * the update loop (which is user defined) refuses to get to the
	 * cancellation point.
	 */
	virtual ~VistaDriverThreadAspect();


	/**
	 * calls the update thread's StopGently() method blocking. This method
	 * might deadlock when the user definable thread refuses to come to
	 * the cancellation point.
	 * @return true if the update thread could be stopped.
	 */
	bool StopProcessing();

	/**
	 * if the update thread is not running, it will be started using this API.
	 * @return the return value of Run() or true (started thread's are 'started')
	 */
	bool StartProcessing();

	bool PauseProcessing();

	bool UnpauseProcessing();

	/**
	 * returns whether the update thread is running or not
	 * @return true when the update thread is processing, false else.
	 */
	bool GetIsProcessing() const;

	/**
	 * interface for user defined update code.
	 * @see SetDriverUpdatePrepare()
	 */
	class VISTADEVICEDRIVERSAPI IDriverUpdatePrepare
	{
	public:
		virtual ~IDriverUpdatePrepare();

		/**
		 * is called //before// the call to update
		 * @return return value is ignored currently
		 */
		virtual bool PrePoll() = 0;

		/**
		 * is called //after// the call to update
		 * @return return value is ignored currently
		 */
		virtual bool PostPoll() = 0;

	protected:
		IDriverUpdatePrepare();
	};

	/**
	 * retrieve the update prepare
	 * @return the update prepare that was set by the user or NULL
	 */
	IDriverUpdatePrepare *GetDriverUpdatePrepare() const;

	/**
	 * set the driver update prepare on behalf of the user.
	 * the method can be called even when the thread is running.
	 * The method tries to pause the update thread, set the prepare and
	 * then continue. This method can deadlock when the driver update
	 * decides to deadlock. During the call, the old update prepare is
	 * still working, will effectively be called with pre/post and
	 * after that, the new update prepare is set.
	 * Memory for the old update prepare is not released.
	 * @param a pointer to the update prepare to set or NULL
	 */
	void           SetDriverUpdatePrepare(IDriverUpdatePrepare *pUpdate);

	/**
	 * get the update thread priority.
	 */
	void GetPriority(VistaPriority &oPrio) const;

	/**
	 * set the update priority
	 */
	void SetPriority( const VistaPriority &oPrio );

	/**
	 * call this thread //before// a call to StartProcessing()
	 * to nail the thread to a specific processor. This call might
	 * or might not work, depending on the implementation of the
	 * InterProcComm.
	 */
	void SetProcessorAffinity(int nProcessorNum);

	/**
	 * return the processor affinity that was set
	 * @return the processor affinity effectively set or -1 for none
	 */
	int GetProcessorAffinity() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:


private:
	static int m_nAspectId;

	friend class VistaDriverPollThread;
	VistaDriverPollThread *m_pThread;
	int m_nProcAffinity;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERTHREADASPECT_H
