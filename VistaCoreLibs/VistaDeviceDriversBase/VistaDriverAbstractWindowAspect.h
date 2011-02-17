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

#ifndef _VISTADRIVERABSTRACTWINDOWASPECT_H
#define _VISTADRIVERABSTRACTWINDOWASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"
#include "VistaDeviceDriver.h"

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/**
 *  little wrapper to allow structured access of a platform specific window
 *  handle using the m_nHandle member and the id() method.
 *  See the typedefs in this file.
 */
template<class T>
class _windowHandle
{
public:
	/**
	 * Copy nHandle to m_nHandle.
	 */
	_windowHandle(T nHandle)
		: m_nHandle(nHandle) {}

	/**
	 * plain accessor, return the private member (read protection)
	 */
	T id() const { return m_nHandle; }

private:
	T m_nHandle;
};


#if defined(WIN32)
#include <Windows.h>
typedef _windowHandle<HWND> CWindowHandle; /*<< wrap windows handles HWND */
#else // this goes for all the others as void*
typedef _windowHandle<void*> CWindowHandle; /*<< wrap generic handles */
#endif


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Some drivers may have the need to communicate with the system by using
 * a window handle (e.g., in order to use the process message port for system
 * communication. The mapping from and to windows (VistaWindow) can be done
 * with the WindowAspect. Windows may be attached and detached, as such attach
 * and detach sequences can be defined on a per-aspect basis. This aspect has
 * the name "WINDOW".
 */
class VISTADEVICEDRIVERSAPI VistaDriverAbstractWindowAspect
				   : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	/**
	 * Registers an aspect class with name "WINDOW" iff no such class was
	 * registered before.
	 */
	VistaDriverAbstractWindowAspect();
	virtual ~VistaDriverAbstractWindowAspect();

	/**
	 * This method is called by the application / environment <b>every time</b>
	 * a window is to be attached to this aspect. The CWindowHandle is assumed to
	 * be valid as long as this aspect lives (it is copied to the handle list).
	 * This API is non-virtual on purpose, it calls
	 * IVistaDriverAbstractWindowTouchSequence::AttachSequence()
	 * passing the CWindowHandle. So be sure to register this touch sequence by the driver
	 * before using the AttachToWindow API. The construction allows to <b>give</b> window
	 * implementations to drivers without the need for sub-classing (driver code does not
	 * subclass the aspect, it specializes the TouchSequence!)
	 * @param a non NULL CWindowHandle wrapper to read the platform specific window handle from
	 * @return true if this call went through good, false else
	 * @see IVistaDriverAbstractWindowTouchSequence()
	 */
	bool AttachToWindow(CWindowHandle *);
	bool DetachFromWindow(CWindowHandle *);

	/**
	 * return a list of window handles to which the aspect is attached to.
	 */
	std::list<CWindowHandle*> GetWindowList() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

	class VISTADEVICEDRIVERSAPI IVistaDriverAbstractWindowTouchSequence
	{
	public :
		virtual bool AttachSequence(CWindowHandle *) = 0;
		virtual bool DetachSequence(CWindowHandle *) = 0;
		virtual std::list<CWindowHandle*> GetWindowList() const = 0;
	};

	/**
	 * sets the callback to call every time AttachToWindow() is called by a toolkit shell.
	 * The touch sequence is expected to live as long as this aspect lives. Passing NULL
	 * erases the current setting.
	 * @param pAtSeq the sequence to hold onto
	 */
	bool SetTouchSequence(IVistaDriverAbstractWindowTouchSequence *pAtSeq);

	/**
	 * returns the current touch sequence for this aspect. NULL iff none was set
	 * by driver code.
	 */
	IVistaDriverAbstractWindowTouchSequence *GetTouchSequence() const;

protected:
private:
	IVistaDriverAbstractWindowTouchSequence *m_pTouchSeq;

	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERWINDOWASPECT_H