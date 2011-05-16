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
// $Id$

#ifndef _VISTADRIVERWINDOWASPECT_H
#define _VISTADRIVERWINDOWASPECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverAbstractWindowAspect.h>

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaWindow;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Some drivers may have the need to communicate with the system by using
 * a window handle (e.g., in order to use the process message port for system
 * communication. The mapping from and to windows (VistaWindow) can be done
 * with the WindowAspect. Windows may be attached and detached, as such attach
 * and detach sequences can be defined on a per-aspect basis.
 */
class VISTAKERNELAPI VistaDriverWindowAspect : public VistaDriverAbstractWindowAspect
{
public:
	VistaDriverWindowAspect();
	virtual ~VistaDriverWindowAspect();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// KERNEL SPECIFIC
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * this is no redefinition: it works on VistaWindows instead of WindowHandles
	 * and this is on purpose. The system configurator will work on this API,
	 * and not on the generic one from the DeviceDriver lib. This implementation
	 * does recover the HWND on windows systems.
	 */
	bool AttachToWindow(VistaWindow *);
	bool DetachFromWindow(VistaWindow *);
	bool GetWindowList(std::list<VistaWindow*> &) const;

	/**
	 * This is a special attach sequence to work with VistaWindow pointers
	 * instead of CWindowHandles and is used by kernel level devices, for
	 * example the glut devices in the OpenSG/glut implementation.
	 */
	class VISTAKERNELAPI IVistaDriverWindowTouchSequence
		: public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
	{
	public :
		virtual bool AttachSequence(VistaWindow *) = 0;
		virtual bool DetachSequence(VistaWindow *) = 0;
		virtual bool GetWindowList(std::list<VistaWindow*> &) const = 0;
	};


protected:
private:
	std::list<
		std::pair<VistaWindow*, CWindowHandle*>
	> m_liWindows;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTADRIVERWINDOWASPECT_H
