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

#ifndef _VISTAGLUTWINDOWINGTOOLKIT_H
#define _VISTAGLUTWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaWindowingToolkit.h"

#include "../../VistaKernelConfig.h"

#include <string>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSimpleTextOverlay;
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * GLUT implementation of IVistaWindowingToolkit. See IVistaWindowingToolkit.h
 * for documentation.
 */
class VISTAKERNELAPI VistaGlutWindowingToolkit : public IVistaWindowingToolkit
{
public:
	/**
	 * The constructor registers the own instance
	 * at the nameless namespace blow this class.
	 */
	VistaGlutWindowingToolkit( VistaSystem *pVistaSystem, int argc, char** argv );

	IVistaSimpleTextOverlay* CreateSimpleTextOverlay();
	IVista3DTextOverlay* Create3DTextOverlay();
	IVistaTextEntity* CreateTextEntity();

	void Run();
	void Quit();

	void SetWindow( int );
	int  GetWindow();
	void SwapBuffers();
	void Redisplay();
	void SetDisplayMode( unsigned int );

	/**
	 * All Glut callbacks get registered here for mouse and keyboardinteraction
	 * so if you register any callback after this method here those callbacks
	 * will not work properly. To get access to the data given by the callbacks
	 * use the IVistaObserver / IVistaOberserveable API.
	 */
	int CreateOpenGLContext( std::string title );
	void DestroyWindow( int );

	void DisplayFunctor( void (*) ( void ) );
	void IdleFunctor( void (*) ( void ) );
	void ReshapeFunctor( void (*) ( int, int ) );

	int* GetWindowPosition();
	void SetWindowPosition( int, int );

	int* GetWindowSize();
	void SetWindowSize( int, int );

	void ActivateFullscreen();

	void SetTitle( std::string title );
	void SetCursor( int );

private:

	VistaSystem*						m_pVistaSystem;
	bool								m_bQuitLoop;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif
