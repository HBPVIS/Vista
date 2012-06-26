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
// $Id: VistaOSGWindowingToolkit.cpp 29489 2012-03-08 12:37:27Z dr165799 $

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaOSGWindowingToolkit.h"

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaDisplay.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>

#include <osgViewer/GraphicsWindow>

#include "../GlutWindowImp/VistaGlutTextEntity.h"

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glut.h>
#include <GL/gl.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

struct OSGWindowInfo
{
	OSGWindowInfo( VistaWindow* pWindow )
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
	, m_sWindowTitle( "ViSTA" )
	, m_iVSyncMode( VistaOSGWindowingToolkit::VSYNC_STATE_UNKNOWN )
	, m_bCursorEnabled( true )
	, m_pOSGWindow( NULL )
	{
	}

	VistaWindow*		m_pWindow;
	osgViewer::GraphicsWindow*	m_pOSGWindow;
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
	std::string			m_sWindowTitle;
	int					m_iVSyncMode;
	bool				m_bCursorEnabled;
	osgGA::EventQueue::Events m_oEvents;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOSGWindowingToolkit::VistaOSGWindowingToolkit( VistaDisplayManager* pDisplayManager )
: m_pDisplayManager( pDisplayManager )
, m_bQuitLoop( false )
, m_pResizeCallback( NULL )
, m_pUpdateCallback( NULL )
, m_iTmpWindowID( -1 )
, m_iGlobalVSyncAvailability( ~0 )
, m_pSharedContext( NULL )
{
	int nArgC = 0;
	glutInit( &nArgC, NULL ); //@OSGTODO for text
}


VistaOSGWindowingToolkit::~VistaOSGWindowingToolkit()
{
	m_pSharedContext->unref();
}


/*============================================================================*/
/* FACTORY METHODS                                                            */
/*============================================================================*/

IVistaTextEntity* VistaOSGWindowingToolkit::CreateTextEntity()
{
	return new VistaGlutTextEntity; // @OSGTODO
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaOSGWindowingToolkit::Run()
{
	while( m_bQuitLoop == false )
	{
		for( std::map<const VistaWindow*, OSGWindowInfo*>::iterator itWin = m_mapWindowInfo.begin();
				itWin != m_mapWindowInfo.end(); ++itWin )
		{
			osgViewer::GraphicsWindow* pOSGWin = (*itWin).second->m_pOSGWindow;
			(*itWin).second->m_oEvents.clear();
			pOSGWin->checkEvents();
			pOSGWin->getEventQueue()->takeEvents( (*itWin).second->m_oEvents );

			osgGA::EventQueue::Events::iterator itEvent;
			for( itEvent = (*itWin).second->m_oEvents.begin();
				itEvent != (*itWin).second->m_oEvents.end();
				++itEvent )
			{
				osgGA::GUIEventAdapter* pEvent = itEvent->get();
				switch( pEvent->getEventType() )
				{
					case osgGA::GUIEventAdapter::CLOSE_WINDOW:
					{
						pEvent->setHandled( true );
						vstr::outi() << "[OSGWindow]: CloseWindow event - shutting down Vista" << std::endl;
						Quit();
						return;
					}
					case osgGA::GUIEventAdapter::RESIZE:
					{
						pEvent->setHandled( true );
						(*itWin).second->m_pWindow->GetProperties()->Notify(
							VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE );
					}
					default:
						break;
				}
			}
		}

		m_pUpdateCallback->Do();
	}
}

void VistaOSGWindowingToolkit::Quit()
{
	m_bQuitLoop = true;
}

void VistaOSGWindowingToolkit::DisplayWindow( VistaWindow* pWindow )
{
	GetWindowInfo( pWindow )->m_pOSGWindow->swapBuffers();
}
void VistaOSGWindowingToolkit::DisplayAllWindows()
{
	if( m_mapWindowInfo.empty() )
		return;

	for( WindowInfoMap::const_iterator 
			itWindowID = m_mapWindowInfo.begin(); 
			itWindowID != m_mapWindowInfo.end();
			++itWindowID )
	{
		(*itWindowID).second->m_pOSGWindow->swapBuffers();
	}
}

bool VistaOSGWindowingToolkit::RegisterWindow( VistaWindow* pWindow )
{
	WindowInfoMap::const_iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists != m_mapWindowInfo.end() )
		return false;
	OSGWindowInfo* pInfo = new OSGWindowInfo( pWindow );	
	m_mapWindowInfo[pWindow] = pInfo;
	return true;
}
bool VistaOSGWindowingToolkit::UnregisterWindow( VistaWindow* pWindow )
{
	WindowInfoMap::iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists == m_mapWindowInfo.end() )
		return false;
	
	if( (*itExists).second->m_pOSGWindow != m_pSharedContext )
	{
		(*itExists).second->m_pOSGWindow->unref();
	}
	else
	{
		// OSGTODO how to deal with this?
	}
	delete (*itExists).second;

	m_mapWindowInfo.erase( itExists );
	return true;
}

bool VistaOSGWindowingToolkit::InitWindow( VistaWindow* pWindow )
{
	WindowInfoMap::iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists == m_mapWindowInfo.end() )
	{
		vstr::errp() << "[OSGWindowingTollkit]: "
				<< "Trying to initialize Window that was not registered before"
				<< std::endl;
		return false;
	}
	OSGWindowInfo* pInfo = (*itExists).second;

	if( pInfo->m_pOSGWindow != NULL )
	{
		vstr::errp() << "[OSGWindowingTollkit]: "
				<< "Trying to initialize Window [" << pWindow->GetNameForNameable()
				<< "] which was already initialized" << std::endl;
		return false;
	}
	
	osg::ref_ptr<osg::GraphicsContext::Traits> pTraits = new osg::GraphicsContext::Traits;

	if( pWindow->GetDisplay() != NULL )
	{
		std::string sDisplayName = pWindow->GetDisplay()->GetDisplayProperties()->GetDisplayString();
		pTraits->setScreenIdentifier( sDisplayName );
	}
	else		
		pTraits->screenNum = 0; // OSGTodo
	pTraits->x = pInfo->m_iCurrentPosX;
	pTraits->y = pInfo->m_iCurrentPosY;
	pTraits->width = pInfo->m_iCurrentSizeX;
	pTraits->height = pInfo->m_iCurrentSizeY;
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	pTraits->sharedContext = m_pSharedContext;
	pTraits->windowName = pInfo->m_sWindowTitle;
	pTraits->quadBufferStereo = pInfo->m_bUseStereo;
	pTraits->useCursor = pInfo->m_bCursorEnabled;
	pTraits->vsync = ( pInfo->m_iVSyncMode == VSYNC_ENABLED );

	if( pInfo->m_bUseStencilBuffer )
		pTraits->stencil = 8;
	else
		pTraits->stencil = 0;

	osg::GraphicsContext* pContext = osg::GraphicsContext::createGraphicsContext( pTraits );
	osgViewer::GraphicsWindow* pOSGWin = dynamic_cast<osgViewer::GraphicsWindow*>( pContext );
	pOSGWin->realize();
	pOSGWin->ref();
	if( pOSGWin == NULL || pOSGWin->isRealized() == false )
		VISTA_THROW( "OSG Window creation failed", -1 );
	pInfo->m_pOSGWindow = pOSGWin;

	pOSGWin->makeCurrent();

	// @OSGTODO: pInfo->m_bUseAccumBuffer
	// @OSGTODO: resize callback
	
	if( m_pSharedContext == NULL )
	{
		m_pSharedContext = pContext;
		m_pSharedContext->ref();
	}

	if( pInfo->m_bFullscreenActive )
	{
		SetFullscreen( pWindow, true );
	}

	pOSGWin->setSyncToVBlank( pInfo->m_iVSyncMode == VSYNC_ENABLED );

	return true;
}

bool VistaOSGWindowingToolkit::SetWindowUpdateCallback( 
									IVistaExplicitCallbackInterface* pCallback )
{
	m_pUpdateCallback = pCallback;
	return true;
}
bool VistaOSGWindowingToolkit::GetWindowPosition( const VistaWindow* pWindow,
												  int &iX, int& iY )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( pInfo->m_pOSGWindow )
	{
		int nHeight, nWidth;
		pInfo->m_pOSGWindow->getWindowRectangle( iX, iY, nWidth, nHeight );
	}
	else
	{
		OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
		iX = pInfo->m_iCurrentPosX;
		iY = pInfo->m_iCurrentPosY;
	}
	return true;
}

bool VistaOSGWindowingToolkit::SetWindowPosition( VistaWindow* pWindow,
												  const int iX, const int iY )
{	
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );	

	if( pInfo->m_pOSGWindow )
	{
		if( pInfo->m_bFullscreenActive )
		{
			pInfo->m_iPreFullscreenPosX = iX;
			pInfo->m_iPreFullscreenPosY = iY;
		}
		else
		{
			pInfo->m_iCurrentPosX = iX;
			pInfo->m_iCurrentPosY = iY;		
			pInfo->m_pOSGWindow->setWindowRectangle( pInfo->m_iCurrentPosX,
													pInfo->m_iCurrentPosY,
													pInfo->m_iCurrentSizeX,
													pInfo->m_iCurrentSizeY );
		}
	}
	else
	{
		pInfo->m_iCurrentPosX = iX;
		pInfo->m_iCurrentPosY = iY;		
	}
	return true;
}

bool VistaOSGWindowingToolkit::GetWindowSize( const VistaWindow* pWindow,
											  int& iWidth, int& iHeight )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( pInfo->m_pOSGWindow )
	{
		int nX, nY;
		pInfo->m_pOSGWindow->getWindowRectangle( nX, nY, iWidth, iHeight );
	}
	else
	{
		iWidth = pInfo->m_iCurrentSizeX;
		iHeight = pInfo->m_iCurrentSizeY;
	}
	return true;
}

bool VistaOSGWindowingToolkit::SetWindowSize( VistaWindow* pWindow,
											  int iWidth, int iHeight )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );	

	if( pInfo->m_pOSGWindow )
	{
		if( pInfo->m_bFullscreenActive )
		{
			pInfo->m_iPreFullscreenSizeX = iWidth;
			pInfo->m_iPreFullscreenSizeY = iHeight;
		}
		else
		{
			pInfo->m_iCurrentSizeX = iWidth;
			pInfo->m_iCurrentSizeY = iHeight;		
			pInfo->m_pOSGWindow->setWindowRectangle( pInfo->m_iCurrentPosX,
												pInfo->m_iCurrentPosY,
												pInfo->m_iCurrentSizeX,
												pInfo->m_iCurrentSizeY );
		}
	}
	else
	{
		pInfo->m_iCurrentSizeX = iWidth;
		pInfo->m_iCurrentSizeY = iHeight;		
	}
	return true;
}

bool VistaOSGWindowingToolkit::GetFullscreen( const VistaWindow* pWindow  ) const
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bFullscreenActive;
}

bool VistaOSGWindowingToolkit::SetFullscreen( VistaWindow* pWindow,
												   const bool bEnabled )
{	
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bFullscreenActive == bEnabled )
		return true;

	if( pInfo->m_pOSGWindow == NULL )
	{
		// before initialization - store for later
		pInfo->m_bFullscreenActive = true;
		return true;
	}

	if( bEnabled )
	{
		// @OSGTODO which screen?
		unsigned int nWidth, nHeight;
		pInfo->m_pOSGWindow->getWindowingSystemInterface()->getScreenResolution(
					osg::GraphicsContext::ScreenIdentifier( 0 ), nWidth, nHeight );
		pInfo->m_pOSGWindow->setWindowRectangle( 0, 0, nWidth, nHeight );
		pInfo->m_pOSGWindow->setWindowDecoration( false );
	}
	else
	{		
		pInfo->m_pOSGWindow->setWindowRectangle( pInfo->m_iCurrentPosX,
												pInfo->m_iCurrentPosY,
												pInfo->m_iCurrentSizeX,
												pInfo->m_iCurrentSizeY );
		pInfo->m_pOSGWindow->setWindowDecoration( true );
		pInfo->m_bFullscreenActive = false;	
	}
	return true;
}
bool VistaOSGWindowingToolkit::SetWindowTitle( VistaWindow* pWindow, 
											   const std::string& sTitle )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	pInfo->m_sWindowTitle = sTitle;
	if( pInfo->m_pOSGWindow )
	{
		pInfo->m_pOSGWindow->setWindowName( sTitle );
	}
	return true;
}

std::string VistaOSGWindowingToolkit::GetWindowTitle( const VistaWindow* pWindow  )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_sWindowTitle;
}

bool VistaOSGWindowingToolkit::SetCursorIsEnabled( VistaWindow* pWindow, bool bSet )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	pInfo->m_bCursorEnabled = bSet;
	if( pInfo->m_pOSGWindow )
	{
		pInfo->m_pOSGWindow->useCursor( bSet );
	}
	return true;
}
bool VistaOSGWindowingToolkit::GetCursorIsEnabled( const VistaWindow* pWindow  )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bCursorEnabled;
}

bool VistaOSGWindowingToolkit::GetUseStereo( const VistaWindow* pWindow  ) const
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( pInfo->m_pOSGWindow )
	{
		return pInfo->m_pOSGWindow->getTraits()->quadBufferStereo;
	}
	else
	{
		return pInfo->m_bUseStereo;
	}
}
bool VistaOSGWindowingToolkit::SetUseStereo( VistaWindow* pWindow, const bool bSet )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_pOSGWindow )
	{
		vstr::warnp() << "[OSGWindow]: Trying to change stereo mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bUseStereo = bSet;
	return true;
}

bool VistaOSGWindowingToolkit::GetUseAccumBuffer( const VistaWindow* pWindow  ) const
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bUseAccumBuffer;
}
bool VistaOSGWindowingToolkit::SetUseAccumBuffer( VistaWindow* pWindow, const bool bSet )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_pOSGWindow )
	{
		vstr::warnp() << "[OSGWindow]: Trying to change accum buffer mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		false;
	}

	pInfo->m_bUseAccumBuffer = bSet;
	return true;
}

bool VistaOSGWindowingToolkit::GetUseStencilBuffer( const VistaWindow* pWindow  ) const
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bUseStencilBuffer;
}
bool VistaOSGWindowingToolkit::SetUseStencilBuffer( VistaWindow* pWindow, const bool bSet )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_pOSGWindow )
	{
		vstr::warnp() << "[OSGWindow]: Trying to change stencil buffer mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bUseStencilBuffer = bSet;
	return true;
}

int VistaOSGWindowingToolkit::GetWindowId( const VistaWindow* pWindow  ) const
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( pInfo->m_pOSGWindow == NULL )
		return -1;
	return 0; // @OSGTODO
}
void VistaOSGWindowingToolkit::BindWindow( VistaWindow* pWindow )
{
	GetWindowInfo( pWindow )->m_pOSGWindow->makeCurrent();
}


bool VistaOSGWindowingToolkit::GetVSyncCanBeModified( const VistaWindow* pWindow  )
{
	return true;
}

int VistaOSGWindowingToolkit::GetVSyncMode( const VistaWindow* pWindow  )
{
	return GetWindowInfo( pWindow )->m_iVSyncMode;
}

bool VistaOSGWindowingToolkit::SetVSyncMode( VistaWindow* pWindow, const bool bEnabled )
{
	OSGWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( bEnabled )
		pInfo->m_iVSyncMode = VSYNC_ENABLED;
	else
		pInfo->m_iVSyncMode = VSYNC_DISABLED;

	if( pInfo->m_pOSGWindow == NULL )
	{
		// pre-init, just store the value
		return true;
	}

	pInfo->m_pOSGWindow->setSyncToVBlank( bEnabled );
	return true;
}

OSGWindowInfo* VistaOSGWindowingToolkit::GetWindowInfo( const VistaWindow* pWindow  ) const
{
	WindowInfoMap::const_iterator itWindow = m_mapWindowInfo.find( pWindow );
	if( itWindow == m_mapWindowInfo.end() )
		return NULL;
	return (*itWindow).second;
}

void* VistaOSGWindowingToolkit::GetEventsForWindow(
												const VistaWindow* pWindow )
{
	WindowInfoMap::const_iterator itWindow = m_mapWindowInfo.find( pWindow );
	if( itWindow == m_mapWindowInfo.end() )
		VISTA_THROW( "Requested Events from invalid Window", -1 );
	return &(*itWindow).second->m_oEvents;
}



