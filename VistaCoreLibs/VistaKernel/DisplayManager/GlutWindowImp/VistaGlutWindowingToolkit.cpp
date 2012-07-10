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
// $Id$

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaGlutWindowingToolkit.h"

#include "VistaGlutTextEntity.h"

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>

#if defined(USE_NATIVE_GLUT)
  #if defined(DARWIN) // we use the mac os GLUT framework on darwin
    #include <GLUT/glut.h>
  #else
    #include <GL/glut.h>
  #endif
#else
    #include <GL/freeglut.h>
#endif

#include <string>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
typedef bool (APIENTRY* PFNWGLSWAPINTERVALEXT) (int);
typedef int (APIENTRY* PFNWGLGETSWAPINTERVALEXT) (void);
PFNWGLSWAPINTERVALEXT		SetSwapIntervalFunction = NULL;
PFNWGLGETSWAPINTERVALEXT    GetSwapIntervalFunction = NULL;
#elif defined LINUX
#include <GL/gl.h>
#include <GL/glx.h>
typedef int (* PFNGLXSWAPINTERVALSGIPROC) (int interval);
typedef int (* PFNGLXGETSWAPINTERVALSGIPROC) ();
PFNGLXSWAPINTERVALSGIPROC SetSwapIntervalFunction = NULL;
PFNGLXGETSWAPINTERVALSGIPROC GetSwapIntervalFunction = NULL;
#endif


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

// Maintain maps of all windows in order to be able to match incoming
// glut callbacks to the correct callback. Since all windows usually use the
// same callback, this is only important if multiple displaysystems/VistaSystems
// are around

struct GlutWindowInfo
{
	GlutWindowInfo( VistaWindow* pWindow )
	: m_pWindow( pWindow )
	, m_pUpdateCallback( NULL )
	, m_iCurrentSizeX( 0 )
	, m_iCurrentSizeY( 0 )
	, m_iCurrentPosX( 0 )
	, m_iCurrentPosY( 0 )
	, m_iPreFullscreenSizeX( 0 )
	, m_iPreFullscreenSizeY( 0 )
	, m_iPreFullscreenPosX( 0 )
	, m_iPreFullscreenPosY( 0 )
	, m_bFullscreenActive( false )
	, m_bUseStereo( false )
	, m_bUseAccumBuffer( false )
	, m_bUseStencilBuffer( false )
	, m_iWindowID( -1 )
	, m_sWindowTitle( "ViSTA" )
	, m_iVSyncMode( VistaGlutWindowingToolkit::VSYNC_STATE_UNKNOWN )
	, m_bCursorEnabled( true )
	{
	}

	VistaWindow*		m_pWindow;
	IVistaExplicitCallbackInterface*
						m_pUpdateCallback;
	int					m_iCurrentSizeX;
	int					m_iCurrentSizeY;
	int					m_iCurrentPosX;
	int					m_iCurrentPosY;
	int					m_iPreFullscreenSizeX;
	int					m_iPreFullscreenSizeY;
	int					m_iPreFullscreenPosX;
	int					m_iPreFullscreenPosY;
	bool				m_bFullscreenActive;
	bool				m_bUseStereo;
	bool				m_bUseAccumBuffer;
	bool				m_bUseStencilBuffer;
	int					m_iWindowID;
	std::string			m_sWindowTitle;
	int					m_iVSyncMode;
	bool				m_bCursorEnabled;
};

namespace
{
	std::map<int, GlutWindowInfo*> S_mapWindowInfo;

	void DisplayUpdate()
	{
		std::map<int, GlutWindowInfo*>::const_iterator itWindowInfo =
												S_mapWindowInfo.find( glutGetWindow() );
		assert( itWindowInfo != S_mapWindowInfo.end() );
		(*itWindowInfo).second->m_pUpdateCallback->Do();
	}

	void DisplayReshape( int iWidth, int iHeight )
	{
		std::map<int, GlutWindowInfo*>::const_iterator itWindowInfo =
												S_mapWindowInfo.find( glutGetWindow() );
		assert( itWindowInfo != S_mapWindowInfo.end() );
		GlutWindowInfo* pInfo = itWindowInfo->second;
		if( pInfo->m_iCurrentSizeX != iWidth || pInfo->m_iCurrentSizeY != iHeight )
		{
			pInfo->m_iCurrentSizeX = iWidth;
			pInfo->m_iCurrentSizeY = iHeight;
			(*itWindowInfo).second->m_pWindow->GetProperties()->Notify(
								VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE );
		}	
	}
}



#if defined( USE_NATIVE_GLUT )
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
VistaGlutWindowingToolkit::VistaGlutWindowingToolkit( VistaDisplayManager* pDisplayManager )
: m_pDisplayManager( pDisplayManager )
, m_bQuitLoop( false )
, m_pResizeCallback( NULL )
, m_pUpdateCallback( NULL )
, m_iTmpWindowID( -1 )
, m_iGlobalVSyncAvailability( ~0 )
{
	int iArgs = 0;
	glutInit( &iArgs, NULL );	
}

/*============================================================================*/
/* FACTORY METHODS                                                            */
/*============================================================================*/

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
		vstr::outi() << "Leaving GLUT mainloop" << std::endl;
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
		vstr::outi() << "Leaving GLUT mainloop" << std::endl;
	}
#endif

#else
	while( !m_bQuitLoop )
	{
		if( m_mapWindowInfo.empty() )
			m_pUpdateCallback->Do();
		else
			glutMainLoopEvent();
	}
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

void VistaGlutWindowingToolkit::DisplayWindow( VistaWindow* pWindow )
{
	PushWindow( pWindow );
	glutSwapBuffers();
	glutPostRedisplay();
	PopWindow();
}
void VistaGlutWindowingToolkit::DisplayAllWindows()
{
	if( m_mapWindowInfo.empty() )
		return;

	int iActiveID = glutGetWindow();
	for( WindowInfoMap::const_iterator 
			itWindowID = m_mapWindowInfo.begin(); 
			itWindowID != m_mapWindowInfo.end();
			++itWindowID )
	{
		glutSetWindow( (*itWindowID).second->m_iWindowID );
		glutSwapBuffers();
		glutPostRedisplay();
	}
	glutSetWindow( iActiveID );
}

bool VistaGlutWindowingToolkit::RegisterWindow( VistaWindow* pWindow )
{
	WindowInfoMap::const_iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists != m_mapWindowInfo.end() )
		return false;
	GlutWindowInfo* pInfo = new GlutWindowInfo( pWindow );	
	m_mapWindowInfo[pWindow] = pInfo;
	return true;
}
bool VistaGlutWindowingToolkit::UnregisterWindow( VistaWindow* pWindow )
{
	WindowInfoMap::iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists == m_mapWindowInfo.end() )
		return false;
	int iID = (*itExists).second->m_iWindowID;
	if( iID != -1 )
	{
		glutDestroyWindow( iID );
		S_mapWindowInfo.erase( iID );
	}
	delete (*itExists).second;
	m_mapWindowInfo.erase( itExists );
	return true;
}

bool VistaGlutWindowingToolkit::InitWindow( VistaWindow* pWindow )
{
	WindowInfoMap::iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists == m_mapWindowInfo.end() )
	{
		vstr::errp() << "[GlutWindowingTollkit]: "
				<< "Trying to initialize Window that was not registered before"
				<< std::endl;
		return false;
	}
	GlutWindowInfo* pInfo = (*itExists).second;

	if( pInfo->m_iWindowID != -1 )
	{
		vstr::errp() << "[GlutWindowingTollkit]: "
				<< "Trying to initialize Window [" << pWindow->GetNameForNameable()
				<< "] which was already initialized" << std::endl;
		return false;
	}

	int iDisplayMode = GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;
	if( pInfo->m_bUseStereo )
		iDisplayMode = iDisplayMode | GLUT_STEREO;
	if( pInfo->m_bUseAccumBuffer )
		iDisplayMode = iDisplayMode | GLUT_ACCUM;
	if( pInfo->m_bUseStencilBuffer )
		iDisplayMode = iDisplayMode | GLUT_STENCIL;

	glutInitDisplayMode( iDisplayMode );
	if( pInfo->m_iCurrentPosX != -1 && pInfo->m_iCurrentPosY != -1 )
	{
		glutInitWindowPosition( pInfo->m_iCurrentPosX, pInfo->m_iCurrentPosY );
	}
	if( pInfo->m_iCurrentSizeX != -1 && pInfo->m_iCurrentSizeY != -1 )
	{
		glutInitWindowSize(  pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	}

	// @todo: this always returns false...
	//if( glutGet( GLUT_DISPLAY_MODE_POSSIBLE ) == 0 )
	//{
	//	vstr::warnp() << "[GlutWindowingToolkit]: "
	//			<< "Glut says the desired DisplayMode [ RGB | DEPTH | DOUBLE "
	//			<< ( bStereo ? ( " | STEREO " ) : ( "" ) )
	//			<< ( bUseAccumBuffer ? ( " | ACCUM " ) : ( "" ) )
	//			<< ( bUseStencilBuffer ? ( " | STENCIL " ) : ( "" ) )
	//			<< "] is not supported!" << std::endl;
	//}	
	
	pInfo->m_iWindowID = glutCreateWindow( pInfo->m_sWindowTitle.c_str() );

	m_mapWindowInfo[pWindow] = pInfo;
	S_mapWindowInfo[pInfo->m_iWindowID] = pInfo;

	glutSetWindow( pInfo->m_iWindowID );

	if( m_pUpdateCallback )
	{
		pInfo->m_pUpdateCallback = m_pUpdateCallback;
		glutDisplayFunc( &DisplayUpdate );
		glutIdleFunc( &DisplayUpdate );
	}
	glutReshapeFunc( &DisplayReshape );

	if( pInfo->m_bFullscreenActive )
	{
		// store size/position to restore it when turning off game mode
		pInfo->m_iPreFullscreenPosX = glutGet( GLUT_WINDOW_X );
		pInfo->m_iPreFullscreenPosY = glutGet( GLUT_WINDOW_Y );
		pInfo->m_iPreFullscreenSizeX = glutGet( GLUT_WINDOW_WIDTH );
		pInfo->m_iPreFullscreenSizeY = glutGet( GLUT_WINDOW_HEIGHT );
		glutFullScreen();
	}

	// retrieve actual size/position
	pInfo->m_iCurrentPosX = glutGet( GLUT_WINDOW_X );
	pInfo->m_iCurrentPosY = glutGet( GLUT_WINDOW_Y );
	pInfo->m_iCurrentSizeX = glutGet( GLUT_WINDOW_WIDTH );
	pInfo->m_iCurrentSizeY = glutGet( GLUT_WINDOW_HEIGHT );

	if( pInfo->m_iVSyncMode == VSYNC_ENABLED )
		SetVSyncMode( pWindow, true );
	else if( pInfo->m_iVSyncMode == VSYNC_DISABLED )
		SetVSyncMode( pWindow, false );

	glutPostRedisplay();
	return true;
}

bool VistaGlutWindowingToolkit::SetWindowUpdateCallback( 
									IVistaExplicitCallbackInterface* pCallback )
{
	m_pUpdateCallback = pCallback;

	int iActiveID = glutGetWindow();
	for( WindowInfoMap::const_iterator itWindow = m_mapWindowInfo.begin();
			itWindow != m_mapWindowInfo.end(); ++itWindow )
	{
		(*itWindow).second->m_pUpdateCallback = pCallback;
		int iID = (*itWindow).second->m_iWindowID;
		if( iID != -1 )
		{
			glutSetWindow( iID );
			glutDisplayFunc( &DisplayUpdate );
			glutIdleFunc( &DisplayUpdate );
		}
		
	}
	if( m_mapWindowInfo.empty() == false )
		glutPostRedisplay();
	glutSetWindow( iActiveID );
	return true;
}
bool VistaGlutWindowingToolkit::GetWindowPosition( const VistaWindow* pWindow,
												  int &iX, int& iY )
{
	if( PushWindow( pWindow ) )
	{
		iX = glutGet( GLUT_WINDOW_X );
		iY = glutGet( GLUT_WINDOW_Y );	
		PopWindow();
	}
	else
	{
		GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
		iX = pInfo->m_iCurrentPosX;
		iY = pInfo->m_iCurrentPosY;
	}
	return true;
}

bool VistaGlutWindowingToolkit::SetWindowPosition( VistaWindow* pWindow,
												  const int iX, const int iY )
{	
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );	

	if( PushWindow( pInfo ) )
	{
		if( pInfo->m_bFullscreenActive )
		{
			pInfo->m_iPreFullscreenPosX = iX;
			pInfo->m_iPreFullscreenPosY = iY;
		}
		else
		{
			glutPositionWindow( iX, iY );			
			pInfo->m_iCurrentPosX = iX;
			pInfo->m_iCurrentPosY = iY;		
		}
		PopWindow();
	}
	else
	{
		pInfo->m_iCurrentPosX = iX;
		pInfo->m_iCurrentPosY = iY;		
	}
	return true;
}

bool VistaGlutWindowingToolkit::GetWindowSize( const VistaWindow* pWindow,
											  int& iWidth, int& iHeight )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) )
	{
		iWidth = glutGet( GLUT_WINDOW_WIDTH );
		iHeight = glutGet( GLUT_WINDOW_HEIGHT );
		PopWindow();
	}
	else
	{
		iWidth = pInfo->m_iCurrentSizeX;
		iHeight = pInfo->m_iCurrentSizeY;
	}
	return true;
}

bool VistaGlutWindowingToolkit::SetWindowSize( VistaWindow* pWindow,
											  int iWidth, int iHeight )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );	

	if( PushWindow( pInfo ) )
	{
		if( pInfo->m_bFullscreenActive )
		{
			pInfo->m_iPreFullscreenSizeX = iWidth;
			pInfo->m_iPreFullscreenSizeY = iHeight;
		}
		else
		{
			glutReshapeWindow( iWidth, iHeight );
			// we dont store the size in the Info - this will
			// be done in the glut reshape callback
		}
		PopWindow();
	}
	else
	{
		pInfo->m_iCurrentSizeX = iWidth;
		pInfo->m_iCurrentSizeY = iHeight;		
	}
	return true;
}

bool VistaGlutWindowingToolkit::GetFullscreen( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bFullscreenActive;
}

bool VistaGlutWindowingToolkit::SetFullscreen( VistaWindow* pWindow,
												   const bool bEnabled )
{	
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	int iID = pInfo->m_iWindowID;

	if( pInfo->m_bFullscreenActive == bEnabled )
		return true;

	if( iID == -1 )
	{
		// before initialization - store for later
		pInfo->m_bFullscreenActive = true;
		return true;
	}

	if( bEnabled )
	{
		PushWindow( pInfo );
		pInfo->m_iPreFullscreenSizeX = glutGet( GLUT_WINDOW_WIDTH );
		pInfo->m_iPreFullscreenSizeY = glutGet( GLUT_WINDOW_HEIGHT );
		pInfo->m_iPreFullscreenPosX = glutGet( GLUT_WINDOW_X );
		pInfo->m_iPreFullscreenPosY = glutGet( GLUT_WINDOW_Y );
		
		glutFullScreen();
		PopWindow();
		pInfo->m_bFullscreenActive = true;
	}
	else
	{		
		PushWindow( pInfo );
		glutPositionWindow( pInfo->m_iPreFullscreenPosX, pInfo->m_iPreFullscreenPosY );
		glutReshapeWindow( pInfo->m_iPreFullscreenSizeX, pInfo->m_iPreFullscreenSizeY );
		PopWindow();
		pInfo->m_bFullscreenActive = false;	
	}
	return true;
}
bool VistaGlutWindowingToolkit::SetWindowTitle( VistaWindow* pWindow, 
											   const std::string& sTitle )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	pInfo->m_sWindowTitle = sTitle;
	if( PushWindow( pInfo ) )
	{
		glutSetWindowTitle( sTitle.c_str() );
		PopWindow();
	}
	return true;
}

std::string VistaGlutWindowingToolkit::GetWindowTitle( const VistaWindow* pWindow  )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_sWindowTitle;
}

bool VistaGlutWindowingToolkit::SetCursorIsEnabled( VistaWindow* pWindow, bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	pInfo->m_bCursorEnabled = bSet;
	if( PushWindow( pWindow ) )
	{
		if( bSet )
			glutSetCursor( GLUT_CURSOR_CROSSHAIR );
		else
			glutSetCursor( GLUT_CURSOR_NONE );
		PopWindow();
	}
	return true;
}
bool VistaGlutWindowingToolkit::GetCursorIsEnabled( const VistaWindow* pWindow  )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bCursorEnabled;
}

bool VistaGlutWindowingToolkit::GetUseStereo( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bUseStereo;
}
bool VistaGlutWindowingToolkit::SetUseStereo( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_iWindowID != -1 )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change stereo mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
	}

	pInfo->m_bUseStereo = bSet;
	return true;
}

bool VistaGlutWindowingToolkit::GetUseAccumBuffer( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bUseAccumBuffer;
}
bool VistaGlutWindowingToolkit::SetUseAccumBuffer( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_iWindowID != -1 )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change accum buffer mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
	}

	pInfo->m_bUseAccumBuffer = bSet;
	return true;
}

bool VistaGlutWindowingToolkit::GetUseStencilBuffer( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bUseStencilBuffer;
}
bool VistaGlutWindowingToolkit::SetUseStencilBuffer( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_iWindowID != -1 )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change stencil buffer mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
	}

	pInfo->m_bUseStencilBuffer = bSet;
	return true;
}

int VistaGlutWindowingToolkit::GetWindowId( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_iWindowID;
}
void VistaGlutWindowingToolkit::BindWindow( VistaWindow* pWindow )
{
	WindowInfoMap::const_iterator itWindowInfo =
					m_mapWindowInfo.find( pWindow );
	assert( itWindowInfo != m_mapWindowInfo.end() );
	glutSetWindow( (*itWindowInfo).second->m_iWindowID );
}

bool VistaGlutWindowingToolkit::PushWindow( const VistaWindow* pWindow )
{
	WindowInfoMap::const_iterator itWindowInfo =
					m_mapWindowInfo.find( pWindow );
	assert( itWindowInfo != m_mapWindowInfo.end() );
	int iID = (*itWindowInfo).second->m_iWindowID;	

	if( iID == -1 )
	{
		// okay, not initialized yet
		return false;
	}

	if( m_iTmpWindowID == -1 )
		m_iTmpWindowID = iID;
	glutSetWindow( iID );
	return true;
}

bool VistaGlutWindowingToolkit::PushWindow( const GlutWindowInfo* pInfo )
{
	int iID = pInfo->m_iWindowID;	

	if( iID == -1 )
	{
		// okay, not initialized yet
		return false;
	}

	if( m_iTmpWindowID == -1 )
		m_iTmpWindowID = iID;
	glutSetWindow( iID );
	return true;
}

void VistaGlutWindowingToolkit::PopWindow()
{
	if( m_iTmpWindowID != -1 )
	{
		glutSetWindow( m_iTmpWindowID );
		m_iTmpWindowID = -1;
	}	
}

bool VistaGlutWindowingToolkit::GetVSyncCanBeModified( const VistaWindow* pWindow  )
{
	return CheckVSyncAvailability();
}

int VistaGlutWindowingToolkit::GetVSyncMode( const VistaWindow* pWindow  )
{
	if( CheckVSyncAvailability() == VSYNC_DISABLED )
		return VSYNC_STATE_UNAVAILABLE;
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( GetSwapIntervalFunction && pInfo->m_iVSyncMode == VSYNC_STATE_UNKNOWN )
	{
		if( PushWindow( pWindow ) )
		{			
			int iInterval = GetSwapIntervalFunction();
			if( iInterval == 0 )
				m_iGlobalVSyncAvailability = VSYNC_DISABLED;
			else if( iInterval >= 1 )
				m_iGlobalVSyncAvailability = VSYNC_ENABLED;

			PopWindow();
		}
	}
	return m_iGlobalVSyncAvailability;
}

bool VistaGlutWindowingToolkit::SetVSyncMode( VistaWindow* pWindow, const bool bEnabled )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_iWindowID == -1 )
	{
		// pre-init, just store the value
		if( bEnabled )
			pInfo->m_iVSyncMode = VSYNC_ENABLED;
		else
			pInfo->m_iVSyncMode = VSYNC_DISABLED;
		return true;
	}

	if( PushWindow( pWindow ) == false )
		return false;

	if( CheckVSyncAvailability() == false )
		return false;		

	int iInterval = bEnabled ? 1 : 0;	
#ifdef WIN32
	if( SetSwapIntervalFunction( iInterval ) )
	{
		if( GetSwapIntervalFunction )
		{
			int iGetValue = GetSwapIntervalFunction();
			if( iGetValue == iInterval )
				m_iGlobalVSyncAvailability = iInterval;
			else
			{
				vstr::errp() << "VistaGlutWindowingToolkit::SetVSyncEnabled -"
						<< "Setting VSync failed - does driver config enforce on/off?"
						<< std::endl;
				m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
			}
		}
		else
			m_iGlobalVSyncAvailability = iInterval;
	}
	else
	{
		vstr::errp() << "VistaGlutWindowingToolkit::SetVSyncEnabled -"
				<< "Setting VSync failed" << std::endl;
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	}
#elif defined LINUX
	if( SetSwapIntervalFunction( iInterval ) == 0 )
	{
		m_iGlobalVSyncAvailability = iInterval;
	}
	else
	{
		vstr::errp() << "VistaGlutWindowingToolkit::SetVSyncEnabled -"
			<< "Setting VSync failed - does driver config enforce on/off?"
			<< std::endl;
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	}
#endif

	PopWindow();
	return ( m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE );
}

bool VistaGlutWindowingToolkit::CheckVSyncAvailability()
{
	if( m_iGlobalVSyncAvailability != ~0 )
		return ( m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE );

#ifdef USE_NATIVE_GLUT
	return m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE;
#else

#ifdef WIN32
	m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	SetSwapIntervalFunction = (PFNWGLSWAPINTERVALEXT)glutGetProcAddress( "wglSwapIntervalEXT" );
	if( SetSwapIntervalFunction )
	{
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNKNOWN;

		GetSwapIntervalFunction = (PFNWGLGETSWAPINTERVALEXT)glutGetProcAddress("wglGetSwapIntervalEXT");
	}
#elif defined LINUX
	SetSwapIntervalFunction = (PFNGLXSWAPINTERVALSGIPROC)glutGetProcAddress( "glXSwapIntervalSGI" );
	if( SetSwapIntervalFunction )
	{
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNKNOWN;
	}
	else
	{
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	}

#else
	m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
#endif

	return ( m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE );

#endif
}

GlutWindowInfo* VistaGlutWindowingToolkit::GetWindowInfo( const VistaWindow* pWindow  ) const
{
	WindowInfoMap::const_iterator itWindow = m_mapWindowInfo.find( pWindow );
	if( itWindow == m_mapWindowInfo.end() )
		return NULL;
	return (*itWindow).second;
}



