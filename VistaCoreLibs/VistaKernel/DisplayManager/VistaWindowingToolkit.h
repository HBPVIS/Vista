/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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
// $Id: VistaWindowingToolkit.h 21936 2011-06-16 15:05:48Z dr165799 $

#ifndef _VISTAWINDOWINGTOOLKIT_H
#define _VISTAWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaAspects/VistaObserveable.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSimpleTextOverlay;
class IVistaExplicitCallbackInterface;
class Vista3DTextOverlay;
class IVistaTextEntity;
class VistaWindow;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * The VistaWindowingToolkit class provides an interface to bind ViSTA
 * to an specific windowingtoolkit. Every class in the ViSTA CoreLibs should
 * *not* use a concret Implementation of IVistaWindowingToolkit so you
 * keep the capability to change the WindowingToolkit without doing changes
 * in your code. This class acts as factory for TextEntities and 2D Overlay
 * texts and as well as "normal" class with methods to handle windowing stuff.
 */
class VISTAKERNELAPI IVistaWindowingToolkit : public IVistaObserveable
{
public:
	/**
	 * Starts the main loop
	 */
	virtual void Run() = 0;
	/**
	 * Stops all activities for the given toolkit
	 */
	virtual void Quit() = 0;

	/**
	 * Display Windows - this creates a buffer swap
	 */
	virtual void DisplayWindow( VistaWindow* pWindow ) = 0;
	virtual void DisplayAllWindows() = 0;

	/**
	 * RegisterWindow registers the window, but does not initialize it
	 * yet. Use InitWindow to actually set it up after configuring it
	 * using the normal Window API (e.g. to set size or stereo mode).
	 */
	virtual bool RegisterWindow( VistaWindow* pWindow ) = 0;
	/**
	 * UnregisterWindow unregisters the window, but does not destroy
	 * it. The creator is responsible for freeing the VistaWindow's
	 * memory afterwards.
	 */
	virtual bool UnregisterWindow( VistaWindow* pWindow ) = 0;

	/**
	 * Opens the window and creates a new OpenGL context. This
	 * requires the window to be registered already, and the desired
	 * initial parameters that can't be changed later on (e.g. STEREO,
	 * USE_ACCUM_BUFFER, USE_STENCIL_BUFFER).
	 */
	virtual bool InitWindow( VistaWindow* pWindow ) = 0;

	/**
	 * Register a callback which is called on each frame.
	 */
	virtual bool SetWindowUpdateCallback( IVistaExplicitCallbackInterface* pCallback ) = 0;

	virtual bool GetWindowPosition( const VistaWindow* pWindow, int &iX, int& iY  ) = 0;
	virtual bool SetWindowPosition( VistaWindow* pWindow, const int iX , const int iY ) = 0;

	virtual bool GetWindowSize( const VistaWindow* pWindow, int& iWidth, int& iHeight  ) = 0;
	virtual bool SetWindowSize( VistaWindow* pWindow, const int iWidth, const int iHeight ) = 0;
	
	virtual bool GetFullscreen( const VistaWindow* pWindow  )const = 0;
	virtual bool SetFullscreen( VistaWindow* pWindow, const bool bSet ) = 0;

	virtual std::string GetWindowTitle( const VistaWindow* pWindow  ) = 0;
	virtual bool SetWindowTitle( VistaWindow* pWindow, const std::string& sTitle ) = 0;

	virtual bool GetVSyncCanBeModified( const VistaWindow* pWindow  ) = 0;
	virtual int GetVSyncMode( const VistaWindow* pWindow  ) = 0;
	virtual bool SetVSyncMode( VistaWindow* pWindow, const bool bEnabled ) = 0;

	virtual bool GetUseStereo( const VistaWindow* pWindow  ) const = 0;
	virtual bool SetUseStereo( VistaWindow* pWindow, const bool bSet ) = 0;

	virtual bool GetUseAccumBuffer( const VistaWindow* pWindow  ) const = 0;
	virtual bool SetUseAccumBuffer( VistaWindow* pWindow, const bool bSet ) = 0;

	virtual bool GetUseStencilBuffer( const VistaWindow* pWindow  ) const = 0;
	virtual bool SetUseStencilBuffer( VistaWindow* pWindow, const bool bSet ) = 0;

	virtual bool SetCursorIsEnabled( VistaWindow* pWindow, bool bSet ) = 0;
	virtual bool GetCursorIsEnabled( const VistaWindow* pWindow  ) = 0;

	/**
	 * Bind sets the current window as target for rendering
	 */
	virtual void BindWindow( VistaWindow* pWindow ) = 0;


	/**
	 * The IVistaTextEntitty is needed to tell the
	 * IVistaSimpleTextOverlay how the text should look like.
	 */
	virtual IVistaTextEntity* CreateTextEntity() = 0;

	/**
	 * Returns the unique Id of the Window. The definite semantics is
	 * specified by the toolkit-specific implementation.
	 */
	virtual int GetWindowId( const VistaWindow* pWindow  ) const = 0;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif

