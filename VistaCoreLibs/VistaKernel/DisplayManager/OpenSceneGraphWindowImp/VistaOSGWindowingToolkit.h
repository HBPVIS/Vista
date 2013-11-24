/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#ifndef _VISTAOPENSCENEGRAPHWINDOWINGTOOLKIT_H
#define _VISTAOPENSCENEGRAPHWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>

#include <string>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
struct OSGWindowInfo;
namespace osg
{
	class GraphicsContext;
	template<typename T> class ref_ptr;
}
namespace osgGA
{
	class GUIEventAdapter;
}
namespace osgViewer
{
	class GraphicsWindow;
}

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * GLUT implementation of IVistaWindowingToolkit. See IVistaWindowingToolkit.h
 * for documentation.
 */
class VISTAKERNELAPI VistaOSGWindowingToolkit : public IVistaWindowingToolkit
{
public:
	VistaOSGWindowingToolkit();
	~VistaOSGWindowingToolkit();

	virtual void Run();
	virtual void Quit();

	virtual void DisplayWindow( VistaWindow* pWindow );
	virtual void DisplayAllWindows();

	virtual bool RegisterWindow( VistaWindow* pWindow );
	virtual bool UnregisterWindow( VistaWindow* pWindow );

	virtual bool InitWindow( VistaWindow* pWindow );	

	virtual bool SetWindowUpdateCallback( IVistaExplicitCallbackInterface* pCallback );

	virtual bool GetWindowPosition( const VistaWindow* pWindow, int &iX, int& iY  );
	virtual bool SetWindowPosition( VistaWindow* pWindow, const int iX , const int iY );
	virtual bool GetWindowSize( const VistaWindow* pWindow, int& iWidth, int& iHeight  );
	virtual bool SetWindowSize( VistaWindow* pWindow, const int iWidth, const int iHeight );

	virtual bool GetFullscreen( const VistaWindow* pWindow  ) const;
	virtual bool SetFullscreen( VistaWindow* pWindow, const bool bSet );	
	
	virtual std::string GetWindowTitle( const VistaWindow* pWindow  );
	virtual bool SetWindowTitle( VistaWindow* pWindow, const std::string& sTitle );

	virtual bool SetCursorIsEnabled( VistaWindow* pWindow, bool bSet ) const;
	virtual bool GetCursorIsEnabled( const VistaWindow* pWindow  );

	virtual bool GetUseStereo( const VistaWindow* pWindow  ) const;
	virtual bool SetUseStereo( VistaWindow* pWindow, const bool bSet );

	virtual bool GetUseAccumBuffer( const VistaWindow* pWindow  ) const;
	virtual bool SetUseAccumBuffer( VistaWindow* pWindow, const bool bSet );

	virtual bool GetUseStencilBuffer( const VistaWindow* pWindow  ) const;
	virtual bool SetUseStencilBuffer( VistaWindow* pWindow, const bool bSet );

	enum
	{
		VSYNC_STATE_UNAVAILABLE = -2,
		VSYNC_STATE_UNKNOWN = -1,
		VSYNC_DISABLED = 0,
		VSYNC_ENABLED = 1
	};
	virtual bool GetVSyncCanBeModified( const VistaWindow* pWindow  );
	virtual bool SetVSyncMode( VistaWindow* pWindow, const bool bEnabled );
	virtual int GetVSyncMode( const VistaWindow* pWindow  );

	virtual bool GetDrawBorder( const VistaWindow* pWindow ) const;
	virtual bool SetDrawBorder( VistaWindow* pWindow, const bool bSet );
	

	virtual IVistaTextEntity* CreateTextEntity();

	virtual int GetWindowId( const VistaWindow* pWindow  ) const;	

	void BindWindow( VistaWindow* pWindow );

	// Toolkit-specific, returns Events @OSGTODO void-ptr, because
	// include would have double-class-definitions due to two osg namespaces
	std::list< osg::ref_ptr<osgGA::GUIEventAdapter> >& GetEventsForWindow( const VistaWindow* pWindow );
	osgViewer::GraphicsWindow* GetOsgWindowForWindow( const VistaWindow* pWindow );

private:
	OSGWindowInfo* GetWindowInfo( const VistaWindow* pWindow  ) const;


private:	
	typedef std::map<const VistaWindow*, OSGWindowInfo*>	WindowInfoMap;
	WindowInfoMap						m_mapWindowInfo;
	bool								m_bQuitLoop;
	IVistaExplicitCallbackInterface*	m_pUpdateCallback;
	IVistaExplicitCallbackInterface*	m_pResizeCallback;
	int									m_iTmpWindowID;
	int									m_iGlobalVSyncAvailability;

	osg::GraphicsContext*				m_pSharedContext;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif
