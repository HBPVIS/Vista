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

#include "VistaDriverWindowAspect.h"

#include <cassert>

#if defined(WIN32)
#include <Windows.h>
#endif

#include <VistaKernel/DisplayManager/VistaWindow.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverWindowAspect::VistaDriverWindowAspect()
: VistaDriverAbstractWindowAspect()
{
}

VistaDriverWindowAspect::~VistaDriverWindowAspect()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaDriverWindowAspect::AttachToWindow(VistaWindow *pWindow)
{
	if(!GetTouchSequence())
		return false;

	WindowHandle *c = NULL;
#if defined(WIN32)
	// unfortunately, there seems to be now way to
	// retrieve the window id in a reliable way from ViSTA
	// as GLUT, for example, does not allow to give out
	// the window handle "as-raw" handle.
	// so we have to lookup the "global-active-top-level" window
	// from windows itself.
	HWND wnd = FindWindow( NULL, pWindow->GetWindowProperties()->GetTitle().c_str() );
	c = new WindowHandle( pWindow->GetWindowId(), wnd );
#else
	c = new WindowHandle( pWindow->GetWindowId(), (void*)((long)pWindow->GetWindowId()) );
#endif
	if(c == NULL)
		return false;

	// ok, no check for the generic case: does attaching on the WindowHandle work?
	if(GetTouchSequence()->AttachSequence(c))
	{
		// yes, add a mapping from the VistaWindow to the WindowHandle
		// for cleanup later on
		m_liWindows.push_back( std::pair<VistaWindow*, WindowHandle*>(pWindow,c) );
		return true;
	}
	else
	{
		// no, it did not work... this is a slight hack... it can happen to fail
		// for devices in the kernel that define their own touch sequence (e.g.,
		// mouse and keyboard and joystick (glut)
		IVistaDriverWindowTouchSequence *pTS
			= dynamic_cast<IVistaDriverWindowTouchSequence*>(GetTouchSequence());
		if(pTS)
		{
			// yes, that is the case... call the attach sequence using the VistaWindow
			// said again: a slight hack...
			if( pTS->AttachSequence( pWindow ) )
			{
				// this worked, create a mapping for cleanup
				m_liWindows.push_back( std::pair<VistaWindow*, WindowHandle*>(pWindow, c) );
				return true;
			}
		}
		// all in vain: get rid of the WindowHandle, it's only a carrier anyways...
		delete c;
		return false;
	}
}

bool VistaDriverWindowAspect::DetachFromWindow(VistaWindow *pWindow)
{
	// no touch sequence... no fun
	if(!GetTouchSequence())
		return false;

	// retrieve window handle from mapping
	std::list<
		std::pair<VistaWindow*, WindowHandle*> >::iterator it;
	for( it = m_liWindows.begin(); it != m_liWindows.end(); ++it )
	{
		// found it...
		if( (*it).first == pWindow )
		{
			// call detach on the native handle
			bool bRet = GetTouchSequence()->DetachSequence( (*it).second );
			if(bRet)
			{
				// worked... kill iterator in map
				m_liWindows.erase( it );
				return true;
			}
			else
			{
				// same as above... a slight hack... cast to kernel-level window touch sequence
				IVistaDriverWindowTouchSequence *pTS
					= dynamic_cast<IVistaDriverWindowTouchSequence*>(GetTouchSequence());
				if(pTS)
				{
					// ok, call detach on the VistaWindow pointer
					if( pTS->DetachSequence( (*it).first ) )
					{
						// kill carrier
						delete (*it).second;

						// remove from map
						m_liWindows.erase(it);
						return true;
					}
				}
			}
			return false;
		}
	}
	return false;
}

bool VistaDriverWindowAspect::GetWindowList(std::list<VistaWindow*> &liWindow) const
{
	if(GetTouchSequence())
	{
		for(std::list<std::pair<VistaWindow*, WindowHandle*> >::const_iterator cit = m_liWindows.begin();
			cit != m_liWindows.end(); ++cit)
		{
			liWindow.push_back( (*cit).first );
		}
		return true;
	}

	return false;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


