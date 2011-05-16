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

#include "VistaSystemCommands.h"

#include "VistaSystem.h"
#include "DisplayManager/VistaDisplayManager.h"
#include "DisplayManager/VistaDisplaySystem.h"
#include "DisplayManager/VistaDisplayBridge.h"
#include "DisplayManager/VistaWindow.h"
#include "GraphicsManager/VistaGraphicsManager.h"
#include "EventManager/VistaEventManager.h"
#include "EventManager/VistaSystemEvent.h"
#include "Stuff/VistaKernelProfiling.h"

#include "VistaClusterMaster.h"
#include "InteractionManager/VistaInteractionManager.h"
#include "InteractionManager/VistaInteractionContext.h"
#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnGraph.h>

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
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

VistaToggleFramerateCommand::VistaToggleFramerateCommand(VistaGraphicsManager *pGrMgr)
: m_pGrMgr(pGrMgr)
{
}

bool VistaToggleFramerateCommand::Do()
{
	m_pGrMgr->SetShowInfo(!m_pGrMgr->GetShowInfo());
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
	vkernout << m_pCtrl->GetKeyBindingTableString() << std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaReloadContextGraphCommand::VistaReloadContextGraphCommand(VistaSystem *pSys,
                                                                 VistaInteractionContext *pCtx,
                                                                 const std::string &strRoleId,
                                                                 bool bDumpGraph, bool bWritePorts )
                            : IVistaExplicitCallbackInterface(),
                              m_pCtx(pCtx),
                              m_strRoleId(strRoleId),
                              m_pSys(pSys),
                              m_bDumpGraph(bDumpGraph),
                              m_bWritePorts(bWritePorts)
{
}

bool VistaReloadContextGraphCommand::Do()
{
    VdfnGraph *pOldGraph = m_pCtx->GetTransformGraph();
    std::string strGraphFile = m_pCtx->GetGraphSource();


    // load new graph
	VdfnGraph *pGraph = m_pSys->ProperGraphLoad(strGraphFile, m_strRoleId);
    if(pGraph)
    {
        pGraph->EvaluateGraph(0); // initialize
        m_pCtx->SetTransformGraph(pGraph);
        delete pOldGraph;

        if( m_bDumpGraph )
        {
            std::string strFileName = m_strRoleId + std::string(".dot");
            VdfnPersistence::SaveAsDot( pGraph, strFileName, m_strRoleId, m_bWritePorts );
        }
    }
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
	m_pGfxMgr->SetCullingEnabled(!m_pGfxMgr->GetCullingEnabled());
	vkernout << "[ViSys]: Frustum culling is now "
		<< (m_pGfxMgr->GetCullingEnabled() ? "ENABLED" : "DISABLED")
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
	vkernout << "[ViSys]: Occlusion culling is now "
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
	vkernout << "[ViSys]: BBox drawing is now "
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
		std::streamsize iOldPrecision = vkernout.precision( 3 );
		vkernout << "Changed x-axis eye offsets of DisplaySystem ["
				<< pDisplaySys->GetNameForNameable()
				<< "] to [" << v3LeftEyeOffset[0] << ", "
				<< v3RightEyeOffset[0] << "]" << std::endl;
		vkernout.precision( iOldPrecision );		
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
			std::cout << "***WARNING*** Setting VSync FAILED for Window ["
					<< (*itWin).second->GetNameForNameable() << "]" << std::endl;
		}
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

