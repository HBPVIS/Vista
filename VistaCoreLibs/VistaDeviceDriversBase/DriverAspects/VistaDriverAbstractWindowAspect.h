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

#ifndef _VISTADRIVERABSTRACTWINDOWASPECT_H
#define _VISTADRIVERABSTRACTWINDOWASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <list>

#if defined(WIN32)
#include <Windows.h>
#endif
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
 * and detach sequences can be defined on a per-aspect basis. This aspect has
 * the name "WINDOW".
 */
class VISTADEVICEDRIVERSAPI VistaDriverAbstractWindowAspect
				   : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	class VISTADEVICEDRIVERSAPI WindowHandle
	{
	public:
#if defined(WIN32)
		typedef HWND OSHANDLE; /*<< wrap windows handles HWND */
#else // this goes for all the others as void*
		typedef void* OSHANDLE; /*<< wrap generic handles */
#endif

		WindowHandle( int iWindowID, OSHANDLE pHandle, VistaWindow* pWindow );

		OSHANDLE GetOSHandle() const;
		VistaWindow* GetWindow() const; //@OSGTODO: think about this...
		int GetID() const;

		bool operator< ( const WindowHandle& oOther ) const
		{
			if( m_iID == oOther.m_iID )
				return ( m_pHandle < oOther.m_pHandle );
			else
				return ( m_iID < oOther.m_iID );
		}
		bool operator== ( const WindowHandle& oOther ) const
		{
			return( m_iID == oOther.m_iID && m_pHandle == oOther.m_pHandle );
		}
	private:
		int				m_iID;
		OSHANDLE		m_pHandle;
		VistaWindow*	m_pWindow;
	};

	/**
	 * Registers an aspect class with name "WINDOW" iff no such class was
	 * registered before.
	 */
	VistaDriverAbstractWindowAspect();
	virtual ~VistaDriverAbstractWindowAspect();

	/**
	 * This method is called by the application / environment <b>every time</b>
	 * a window is to be attached to this aspect. The WindowHandle is assumed to
	 * be valid as long as this aspect lives (it is copied to the handle list).
	 * This API is non-virtual on purpose, it calls
	 * IVistaDriverAbstractWindowTouchSequence::AttachSequence()
	 * passing the WindowHandle. So be sure to register this touch sequence by the driver
	 * before using the AttachToWindow API. The construction allows to <b>give</b> window
	 * implementations to drivers without the need for sub-classing (driver code does not
	 * subclass the aspect, it specializes the TouchSequence!)
	 * @param a non NULL WindowHandle wrapper to read the platform specific window handle from
	 * @return true if this call went through good, false else
	 * @see IVistaDriverAbstractWindowTouchSequence()
	 */
	bool AttachToWindow( const WindowHandle& oWindow );
	bool DetachFromWindow( const WindowHandle& oWindow );

	/**
	 * return a list of window handles to which the aspect is attached to.
	 */
	std::list<WindowHandle> GetWindowList() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

	class VISTADEVICEDRIVERSAPI IVistaDriverAbstractWindowTouchSequence
	{
	public :
		virtual ~IVistaDriverAbstractWindowTouchSequence() {}
		virtual bool AttachSequence( const WindowHandle& oWindow ) = 0;
		virtual bool DetachSequence( const WindowHandle& oWindow ) = 0;
		virtual std::list<WindowHandle> GetWindowList() const = 0;
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
