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

#ifndef _VISTAWINDOWINGTOOLKIT_H
#define _VISTAWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaKernelConfig.h"

#include <VistaAspects/VistaObserveable.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSimpleTextOverlay;
class IVista3DTextOverlay;
class IVistaTextEntity;

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

	/*============================================================================*/
	/* NOTE! EXPERIMEMNTAL!														  */
	/* The struct and enum are currently not in use!							  */
	/*============================================================================*/
	/**
	 * Define all needed mouse data. Those can be accessed
	 * by IVistaObserveable API
	 */
	struct MOUSE_DATA
	{
		int x;
		int y;
		int button;
		int buttonState;
		int wheelDirection;
		int wheelState;
	};

	enum MOUSE_BITMASK
	{
		X				= 2,
		Y				= 4,
		BUTTON			= 8,
		BUTTON_STATE	= 16,
		WHEEL_DIR		= 32,
		WHEEL_STATE		= 64
	};

	/**
	 * Those methods are factory methods to create new instances depending
	 * on the actual windowing toolkit
	 *
	 * All methods wich start with "Create" will return a pointer
	 * note the the method's caller is responsible for this pointer
	 * and have to clean up those by them self!
	 */
	/**
	 *  An IVistaSimpleTextOverlay allows you to show up text on the screen in 2D space
	 */
	virtual IVistaSimpleTextOverlay* CreateSimpleTextOverlay() = 0;
	/**
	 *  An IVista3DTextOverlay allows you to show up text on the screen in 3D space
	 */
	virtual IVista3DTextOverlay* Create3DTextOverlay() = 0;
	/**
	 * The IVistaTextEntitty is needed to tell the IVistaSimpleTextOverlay
	 * how the text should looklike
	 */
	virtual IVistaTextEntity* CreateTextEntity() = 0;

	/**
	 * Starts the main loop
	 */
	virtual void Run() = 0;
	/**
	 * Stops all activities for the given toolkit
	 */
	virtual void Quit() = 0;

	/**
	 * Activates the window with the given id
	 * \param window id
	 */
	virtual void SetWindow( int id ) = 0;
	/**
	 * Gets the id from the active window
	 * \return window id
	 */
	virtual int  GetWindow() = 0;
	/**
	 * Swaps the back and front buffer
	 */
	virtual void SwapBuffers() = 0;
	/**
	 * Calls the toolkit specific method to trigger
	 * a redisplay
	 */
	virtual void Redisplay() = 0;
	/**
	 * Sets the DisplayMode for the OpenGL context as bitmask
	 * The common values are given as unsigned int and will be
	 * redefined for each toolkit.
	 * \param displaymode
	 */
	virtual void SetDisplayMode( unsigned int mask ) = 0;
	/**
	 * Opens up a new OpenGL context. Other cases are that you have one
	 * or more OpenGL context already open and you want ViSTA to draw
	 * into them, so you should return them here
	 *
	 * \param window title
	 * \return window id
	 */
	virtual int  CreateOpenGLContext( std::string title ) = 0;
	/**
	 * Closes then window given by the id
	 * \param window id
	 */
	virtual void DestroyWindow( int id ) = 0;
	/**
	 * Register a parameterless functor which is called on each
	 * frame.
	 * \param function pointer
	 */
	virtual void DisplayFunctor( void (*) ( void ) ) = 0;
	/**
	 * Register a parameterless functor which is called, when
	 * OpenGL is in idle mode
	 * \param function pointer
	 */
	virtual void IdleFunctor( void (*) ( void ) ) = 0;
	/**
	 * Register a functor with to int parameters for x and y
	 * for the window size
	 * \param function pointer
	 */
	virtual void ReshapeFunctor( void (*) ( int, int ) ) = 0;

	/**
	 * Gets the actual window position
	 * \return 2 component integer array with x and y coordinate
	 */
	virtual int* GetWindowPosition() = 0;
	/**
	 * Sets the window position to the given parameters.
	 * \param x
	 * \paran y
	 */
	virtual void SetWindowPosition( int x , int y ) = 0;

	/**
	 * Gets the actuial window site
	 * \return 2 component integer array with w and h
	 */
	virtual int* GetWindowSize() = 0;
	/**
	 * Set the window size to the given size
	 * \param w - width
	 * \param h - height
	 */
	virtual void SetWindowSize( int w, int h ) = 0;
	/**
	 * Activates fullscreen mode
	 */
	virtual void ActivateFullscreen() = 0;
	/**
	 * Sets the title to the given string
	 * \param title
	 */
	virtual void SetTitle( std::string title ) = 0;
	/**
	 * Sets the cursor to the given one.
	 * Two cursors are defined in this abstraction.
	 */
	virtual void SetCursor( int type ) = 0;

	/**
	 * Common bit mask parameters with concret values
	 * in the concret implementations of this interface
	 */
	unsigned int RGB;
	unsigned int RGBA;
	unsigned int INDEX;
	unsigned int SINGLE;
	unsigned int DOUBLE;
	unsigned int ALPHA;
	unsigned int DEPTH;
	unsigned int STEREO;

	/**
	 * Common cursor types with concret values
	 * in the concret implementations of this interface
	 */
	unsigned int CURSOR_NORMAL;
	unsigned int CURSOR_NONE;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif

