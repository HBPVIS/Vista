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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaGlutWindowingToolkit.h"
#include "VistaGlutSimpleTextOverlay.h"
#include "VistaGlut3DTextOverlay.h"
#include "VistaGlutTextEntity.h"

#include "../VistaSimpleTextOverlay.h"

#include "../../VistaKernelConfig.h"
#include "../../VistaSystem.h"

#include <VistaBase/VistaExceptionBase.h>

#if defined(DARWIN) // we use the mac os GLUT framework on darwin
  #include <GLUT/glut.h>
#else  
  #if defined(USE_NATIVE_GLUT)
    #include <GL/glut.h>
  #else
    #include <GL/freeglut.h>
  #endif
#endif


#include <string>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
typedef int (APIENTRY* WGLSWAPINTERVALEXT) (int);
WGLSWAPINTERVALEXT SwapIntervalFunction = NULL;
#elif UNIX
#include <GL/gl.h>
typedef int (* PFNGLXSWAPINTERVALSGIPROC) (int interval);
PFNGLXSWAPINTERVALSGIPROC SwapIntervalFunction = NULL;
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#if defined(USE_NATIVE_GLUT)
// for the native glut exit-MainLoop hack
class VistaOpenSGQuitException : public VistaExceptionBase
{
public:
	VistaOpenSGQuitException()
		: VistaExceptionBase("QUIT-OPENSG/GLUT", "VistaOpenSGQuitException", __LINE__, 0)
	{}

	~VistaOpenSGQuitException() throw() {}
};
#endif

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutWindowingToolkit::VistaGlutWindowingToolkit(
		VistaSystem* pVistaSystem, int argc, char** argv)
: m_pVistaSystem(pVistaSystem)
, m_bQuitLoop( false )
, m_iVSyncMode( ~0 )
{

	RGB		= GLUT_RGB;
	RGBA	= GLUT_RGBA;
	INDEX	= GLUT_INDEX;
	SINGLE	= GLUT_SINGLE;
	DOUBLE	= GLUT_DOUBLE;
	ALPHA	= GLUT_ALPHA;
	DEPTH	= GLUT_DEPTH;
	STEREO	= GLUT_STEREO;

	CURSOR_NORMAL	= GLUT_CURSOR_CROSSHAIR;
	CURSOR_NONE		= GLUT_CURSOR_NONE;

	glutInit(&argc, argv);
}

/*============================================================================*/
/* FACTORY METHODS                                                            */
/*============================================================================*/
IVistaSimpleTextOverlay* VistaGlutWindowingToolkit::CreateSimpleTextOverlay()
{
	return new VistaGlutSimpleTextOverlay(m_pVistaSystem->GetDisplayManager(),
			m_pVistaSystem->GetEventManager());
}
IVista3DTextOverlay* VistaGlutWindowingToolkit::Create3DTextOverlay()
{
	return new VistaGlut3DTextOverlay(m_pVistaSystem->GetDisplayManager());
}

IVistaTextEntity* VistaGlutWindowingToolkit::CreateTextEntity()
{
	return new VistaGlutTextEntity();
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaGlutWindowingToolkit::Run()
{
#if defined(USE_NATIVE_GLUT)

#if defined(WIN32) && defined(_MSC_VER)
	// we will simply swallow any exception that will break the loop
	__try
	{
		glutMainLoop();
	}
	__except(EXCEPTION_EXECUTE_HANDLER) //VistaOpenSGQuitException &x)
	{
		std::cout << "Leaving GLUT mainloop" << std::endl;
	}
#else
	// native GLUT doesn't support step-wise main-loop execution
	// -> we use the exception hack to quit the glut main-loop.
	try
	{
		glutMainLoop() ;
	}
	catch(VistaOpenSGQuitException &x)
	{
		std::cout << "Leaving GLUT mainloop" << std::endl;
	}
#endif

#else
	while( !m_bQuitLoop )
		glutMainLoopEvent();
#endif
}

void VistaGlutWindowingToolkit::Quit()
{
#if defined(USE_NATIVE_GLUT)
	// we throw the exception here in the quit call for now to keep
	// the opensg sysclassfactory decoupled from the glut window
	// toolkit.  this is pretty dirty since the current frame will not
	// be fully processed (as it was before, when the exception was
	// thrown in sysclassfactory::Update). has to be checked if
	// cluster slaves shut down properly this way. anyhow, freeglut
	// should be standard and native glut is only a last resort for
	// platforms where freeglut makes trouble.

	// HACK
	VistaOpenSGQuitException x;
	throw x;
#else
	m_bQuitLoop = true;
#endif
}

void VistaGlutWindowingToolkit::SetWindow( int id )
{
	if( id != glutGetWindow() )
		glutSetWindow(id);
}

int VistaGlutWindowingToolkit::GetWindow()
{
	return glutGetWindow();
}

void VistaGlutWindowingToolkit::SwapBuffers()
{
	glutSwapBuffers();
}

void VistaGlutWindowingToolkit::Redisplay()
{
	glutPostRedisplay();
}

void VistaGlutWindowingToolkit::SetDisplayMode( unsigned int mode )
{
	glutInitDisplayMode(mode);
}

int VistaGlutWindowingToolkit::CreateOpenGLContext( std::string title )
{
	return glutCreateWindow(title.c_str());
}

void VistaGlutWindowingToolkit::DestroyWindow( int id )
{
	glutDestroyWindow(id);
}

void VistaGlutWindowingToolkit::DisplayFunctor( void (* functor)( void ) )
{
	glutDisplayFunc(functor);
}

void VistaGlutWindowingToolkit::IdleFunctor( void (* functor)( void ) )
{
	glutIdleFunc(functor);
}

void VistaGlutWindowingToolkit::ReshapeFunctor( void (* functor)( int, int ) )
{
	glutReshapeFunc(functor);
}

int* VistaGlutWindowingToolkit::GetWindowPosition()
{
	int *ret = new int[2];
	ret[0] = glutGet(GLUT_WINDOW_X);
	ret[1] = glutGet(GLUT_WINDOW_Y);
	return ret;
}

void VistaGlutWindowingToolkit::SetWindowPosition( int x, int y )
{
	glutPositionWindow(x,y);
}

int* VistaGlutWindowingToolkit::GetWindowSize()
{
	int *ret = new int[2];
	ret[0] = glutGet(GLUT_WINDOW_WIDTH);
	ret[1] = glutGet(GLUT_WINDOW_HEIGHT);
	return ret;
}

void VistaGlutWindowingToolkit::SetWindowSize( int w, int h )
{
	glutReshapeWindow(w,h);
}

void VistaGlutWindowingToolkit::ActivateFullscreen()
{
	glutFullScreen();
}
void VistaGlutWindowingToolkit::SetTitle( std::string title )
{
	glutSetWindowTitle(title.c_str());
}

void VistaGlutWindowingToolkit::SetCursor( int cursor )
{
	glutSetCursor(cursor);
}

bool VistaGlutWindowingToolkit::GetVSyncCanBeModified()
{
	return CheckVSyncAvailability();
}

int VistaGlutWindowingToolkit::GetVSyncMode()
{
	CheckVSyncAvailability();
	return m_iVSyncMode;
}

bool VistaGlutWindowingToolkit::SetVSyncEnabled( bool bEnabled )
{
	if( CheckVSyncAvailability() == false )
		return false;

	int iInterval = bEnabled ? 1 : 0;
	if( SwapIntervalFunction( iInterval ) != 0 )
	{
		m_iVSyncMode = iInterval;
	}
	else
		m_iVSyncMode = VSYNC_STATE_UNKNOWN;

	return true;
}

bool VistaGlutWindowingToolkit::CheckVSyncAvailability()
{
	if( m_iVSyncMode != ~0 )
		return ( m_iVSyncMode != VSYNC_STATE_UNAVAILABLE );

#ifdef WIN32
	const char* sGLExtension = (const char*)glGetString( GL_EXTENSIONS );
	if( strstr( sGLExtension, "WGL_EXT_swap_control" ) == 0 )
	{
		m_iVSyncMode = VSYNC_STATE_UNAVAILABLE;
	}
	else
	{
		SwapIntervalFunction = (WGLSWAPINTERVALEXT)wglGetProcAddress( "wglSwapIntervalEXT" );
		if( SwapIntervalFunction )
			m_iVSyncMode = VSYNC_STATE_UNKNOWN;
		else
			m_iVSyncMode = VSYNC_STATE_UNAVAILABLE;
	}
#elif defined LINUX
	const char* sGLExtension = (const char*)glGetString( GL_EXTENSIONS );
	if( strstr( sGLExtension, "GLX_SGI_swap_control" ) == 0 )
	{
		m_iVSyncMode = VSYNC_STATE_UNAVAILABLE;
	}
	else
	{
		SwapIntervalFunction = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddressARB( "GLX_SGI_swap_control" );
		if( SwapIntervalFunction )
			m_iVSyncMode = VSYNC_STATE_UNKNOWN;
		else
			m_iVSyncMode = VSYNC_STATE_UNAVAILABLE;
	}
#else
	m_iVSyncMode = VSYNC_STATE_UNAVAILABLE;
#endif

	return ( m_iVSyncMode != VSYNC_STATE_UNAVAILABLE );
}

