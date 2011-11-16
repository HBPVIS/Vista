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

#include "VistaSystemCommands.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/VistaFrameLoop.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnGraph.h>

#include <VistaAspects/VistaAspectsUtils.h>

#include <list>
#include <iostream>
#include <algorithm>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaQuitCommand::VistaQuitCommand(VistaSystem *pSys)
: IVistaExplicitCallbackInterface(),
m_pSys(pSys)
{
}

bool VistaQuitCommand::Do()
{
	if(m_pSys)
	{
		VistaSystemEvent oSysEvent;
		oSysEvent.SetId( VistaSystemEvent::VSE_QUIT );
		m_pSys->GetEventManager()->ProcessEvent(&oSysEvent);
		return oSysEvent.IsHandled();
	}
	return false;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleFramerateCommand::VistaToggleFramerateCommand( VistaFrameLoop* pLoop )
: m_pLoop( pLoop )
{
}

bool VistaToggleFramerateCommand::Do()
{
	m_pLoop->SetFrameRateDisplayEnabled( !m_pLoop->GetFrameRateDisplayEnabled() );
	return true;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleCursorCommand::VistaToggleCursorCommand(VistaDisplayManager *pDisMgr)
: m_pDisMgr(pDisMgr)
{
}

bool VistaToggleCursorCommand::Do()
{
	m_pDisMgr->GetDisplayBridge()->SetShowCursor(!m_pDisMgr->GetDisplayBridge()->GetShowCursor());
	return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaShowAvailableCommands::VistaShowAvailableCommands(VistaKeyboardSystemControl *pCtrl,
														 VistaSystem *pSys)
: m_pCtrl(pCtrl),
  m_pSys(pSys)
{
}

bool VistaShowAvailableCommands::Do()
{	
	vstr::out() << m_pCtrl->GetKeyBindingTableString() << std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaReloadContextGraphCommand::VistaReloadContextGraphCommand(VistaSystem *pSys,
                                                                 VistaInteractionContext *pCtx,
                                                                 const std::string &strRoleId,
                                                                 bool bDumpGraph, bool bWritePorts )
: IVistaExplicitCallbackInterface(),
  m_pInteractionContext(pCtx),
  m_sRoleId(strRoleId),
  m_pSys(pSys),
  m_bDumpGraph(bDumpGraph),
  m_bWritePorts(bWritePorts)
{
}

bool VistaReloadContextGraphCommand::Do()
{
	return m_pSys->GetInteractionManager()->ReloadGraphForContext( 
							m_pInteractionContext,
							m_pSys->GetClusterMode()->GetNodeName(),
							m_bDumpGraph, m_bWritePorts );

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaDebugContextGraphCommand::VistaDebugContextGraphCommand( VistaInteractionContext* pCtx )
: m_pInteractionContext( pCtx )
{
}

bool VistaDebugContextGraphCommand::Do()
{
	m_pInteractionContext->PrintDebuggingInfo();
	return true;

}

bool VistaPrintProfilerOutputCommand::Do()
{
	std::cout << "\nProfilingInfo:\n";
	if( VistaBasicProfiler::GetSingleton() )
		VistaBasicProfiler::GetSingleton()->PrintProfile( std::cout, 5 );
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaToggleFrustumCullingCommand::VistaToggleFrustumCullingCommand( VistaGraphicsManager *pGfxMgr ) 
: IVistaExplicitCallbackInterface()
, m_pGfxMgr(pGfxMgr)
{

}

bool VistaToggleFrustumCullingCommand::Do()
{
	m_pGfxMgr->SetFrustumCullingEnabled(!m_pGfxMgr->GetFrustumCullingEnabled());
	vstr::outi() << "[ViSys]: Frustum culling is now "
		<< (m_pGfxMgr->GetFrustumCullingEnabled() ? "ENABLED" : "DISABLED")
		<< std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleOcclusionCullingCommand::VistaToggleOcclusionCullingCommand( VistaGraphicsManager *pGfxMgr )
: IVistaExplicitCallbackInterface()
, m_pGfxMgr(pGfxMgr)
{

}

bool VistaToggleOcclusionCullingCommand::Do()
{
	m_pGfxMgr->SetOcclusionCullingEnabled(!m_pGfxMgr->GetOcclusionCullingEnabled());
	vstr::outi() << "[ViSys]: Occlusion culling is now "
		<< (m_pGfxMgr->GetOcclusionCullingEnabled() ? "ENABLED" : "DISABLED")
		<< std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleBBoxDrawingCommand::VistaToggleBBoxDrawingCommand( VistaGraphicsManager *pGfxMgr )
: IVistaExplicitCallbackInterface()
, m_pGfxMgr(pGfxMgr)
{

}

bool VistaToggleBBoxDrawingCommand::Do()
{
	m_pGfxMgr->SetBBoxDrawingEnabled(!m_pGfxMgr->GetBBoxDrawingEnabled());
	vstr::outi() << "[ViSys]: BBox drawing is now "
		<< (m_pGfxMgr->GetBBoxDrawingEnabled() ? "ENABLED" : "DISABLED")
		<< std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaChangeEyeDistanceCommand::VistaChangeEyeDistanceCommand( const float fChangeValue,
												VistaDisplayManager* pDisplayManager )
: IVistaExplicitCallbackInterface()
, m_fChangeValue( fChangeValue )
, m_pDisplayManager( pDisplayManager )
{
}

bool VistaChangeEyeDistanceCommand::Do()
{
	for( int i = 0; i < m_pDisplayManager->GetNumberOfDisplaySystems(); ++i )
	{
		VistaDisplaySystem* pDisplaySys = m_pDisplayManager->GetDisplaySystem( i );
		VistaVector3D v3LeftEyeOffset, v3RightEyeOffset;
		pDisplaySys->GetDisplaySystemProperties()->GetEyeOffsets( v3LeftEyeOffset, v3RightEyeOffset );
		v3LeftEyeOffset[0] -= m_fChangeValue;
		v3RightEyeOffset[0] += m_fChangeValue;
		pDisplaySys->GetDisplaySystemProperties()->SetEyeOffsets( v3LeftEyeOffset, v3RightEyeOffset );
		std::streamsize iOldPrecision = vstr::out().precision( 3 );
		vstr::outi() << "Changed x-axis eye offsets of DisplaySystem ["
				<< pDisplaySys->GetNameForNameable()
				<< "] to [" << v3LeftEyeOffset[0] << ", "
				<< v3RightEyeOffset[0] << "]" << std::endl;
		vstr::out().precision( iOldPrecision );		
	}
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleVSyncCommand::VistaToggleVSyncCommand( VistaDisplayManager* pDisplayManager )
: IVistaExplicitCallbackInterface()
, m_pDisplayManager( pDisplayManager )
{
}

bool VistaToggleVSyncCommand::Do()
{
	const std::map<std::string, VistaWindow*>& m_mapWindows = m_pDisplayManager->GetWindowsConstRef();
	bool bMode = false;
	bool bModeSet = false;
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = m_mapWindows.begin();
				itWin != m_mapWindows.end(); ++itWin )
	{
		if( !bModeSet )
		{
			int iWinMode = (*itWin).second->GetWindowProperties()->GetVSyncEnabled();
			bMode = ( iWinMode != 1 );
			bModeSet = true;
		}
		if( (*itWin).second->GetWindowProperties()->SetVSyncEnabled( bMode ) )
		{
			std::cout << "VSync [" << ( bMode ? "Enabled" : "Disabled" )
						<< "] for Window [" << (*itWin).second->GetNameForNameable()
						<< "]" << std::endl;
		}
		else
		{
			vstr::warnp() << "Setting VSync FAILED for Window ["
					<< (*itWin).second->GetNameForNameable() << "]" << std::endl;
		}
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

