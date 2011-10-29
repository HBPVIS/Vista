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

#include "VistaKernelDfnNodeCreators.h"

#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnTimerNode.h>

#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnEventSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnWindowSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnTrackballNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnViewerSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnViewerSinkNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnViewportSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnProjectionSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnSensorFrameNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnSystemTriggerControlNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnNavigationNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfn3DMouseTransformNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnMouseWheelChangeDetectNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnReferenceFrameTransformNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnDumpHistoryNodeClusterCreate.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnClusterNodeInfoNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnDeviceDebugNode.h>

#include <list>
#include <iomanip>
#include <sstream>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaKernelDfnNodeCreators::RegisterNodeCreates( VistaSystem* pVistaSystem )
{
	VdfnNodeFactory* pFac = VdfnNodeFactory::GetSingleton();

	pFac->SetNodeCreator( "ViewerSource", 
					new VistaDfnViewerSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "ViewerSink", 
					new VistaDfnViewerSinkNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "WindowSource", 
					new VistaDfnWindowSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "ViewportSource",
					new VistaDfnViewportSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "ProjectionSource", 
					new VistaDfnProjectionSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "3DMouseTransform",
					new Vista3DMouseTransformNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "MousewheelChangeDetect",
					new VistaDfnMouseWheelChangeDetectNodeCreate );
	pFac->SetNodeCreator( "ReferenceFrameTransform",
					new VistaDfnReferenceFrameTransformNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "SensorFrame",
					new TVdfnDefaultNodeCreate<VistaDfnSensorFrameNode> );
	pFac->SetNodeCreator( "DeviceDebug",
					new VistaDfnDeviceDebugNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "SystemTriggerControl",
					new VistaDfnSystemTriggerControlNodeCreate( pVistaSystem->GetKeyboardSystemControl() ) );
	pFac->SetNodeCreator( "Navigation", 
					new VistaDfnNavigationNodeCreate() );
	pFac->SetNodeCreator( "ClusterNodeInfo", 
					new VdfnClusterNodeInfoNodeCreate( pVistaSystem->GetClusterMode() ) );
	pFac->SetNodeCreator( "Frameclock",
					new VistaFrameclockNodeCreate( pVistaSystem->GetClusterMode() ) );

	// this is an override, the old pointer is lost, so expect a memory lead
	// in a profile. The policy of overriding is currently not defined. @todo
	pFac->SetNodeCreator( "DumpHistory",
					new VdfnDumpHistoryNodeClusterCreate( pVistaSystem->GetClusterMode() ) );

	pFac->SetNodeCreator( "Trackball",
					new VistaDfnTrackballNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "EventSource",
					new VistaDfnEventSourceNodeCreate(pVistaSystem-> GetEventManager(), pVistaSystem->GetInteractionManager() ) );
	pFac->SetNodeCreator( "TextOverlay[int]",
					new VistaDfnTextOverlayNodeCreate<int>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[unsigned int]",
					new VistaDfnTextOverlayNodeCreate<unsigned int>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[int64]",
					new VistaDfnTextOverlayNodeCreate<VistaType::sint64>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[bool]",
					new VistaDfnTextOverlayNodeCreate<bool>( pVistaSystem->GetDisplayManager()  ) );
	pFac->SetNodeCreator( "TextOverlay[double]", new VistaDfnTextOverlayNodeCreate<double>( pVistaSystem->GetDisplayManager()  ) );
	pFac->SetNodeCreator( "TextOverlay[microtime]",
					new VistaDfnTextOverlayNodeCreate<VistaType::microtime>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[float]",
					new VistaDfnTextOverlayNodeCreate<float>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[string]",
		new VistaDfnTextOverlayNodeCreate<std::string>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[VistaVector3D]",
					new VistaDfnTextOverlayNodeCreate<VistaVector3D>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[VistaQuaternion]",
					new VistaDfnTextOverlayNodeCreate<VistaQuaternion>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[VistaTransformMatrix]",
					new VistaDfnTextOverlayNodeCreate<VistaTransformMatrix>( pVistaSystem->GetDisplayManager() ) );

	return true;
}


// #############################################################################

Vista3DMouseTransformNodeCreate::Vista3DMouseTransformNodeCreate(VistaDisplayManager *pSys)
	: m_pMgr(pSys)
{
}

IVdfnNode *Vista3DMouseTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{

		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strDisplaySystem;
		std::string strViewport;
		if( subs.GetValue( "displaysystem", strDisplaySystem )
			&& subs.GetValue( "viewport", strViewport ) )
		{
			VistaDisplaySystem *pSys = m_pMgr->GetDisplaySystemByName(strDisplaySystem);
			VistaViewport *pViewport = m_pMgr->GetViewportByName( strViewport );
			if(pViewport && pSys)
			{
				float fOriginOffset = 0;
				subs.GetValue( "origin_offset_along_ray", fOriginOffset );
				VistaVdfn3DMouseTransformNode *pNd = new VistaVdfn3DMouseTransformNode(
																	pSys, pViewport, fOriginOffset );
				bool bToFrame = subs.GetValueOrDefault<bool>( "in_world_coordinates", false );
				pNd->SetTransformPositionFromFrame(bToFrame);
				return pNd;
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}


// #############################################################################

VdfnClusterNodeInfoNodeCreate::VdfnClusterNodeInfoNodeCreate(VistaClusterMode *pClusterMode)
	: m_pClusterMode(pClusterMode)
{
}

IVdfnNode *VdfnClusterNodeInfoNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		return new VdfnClusterNodeInfoNode(m_pClusterMode);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################


VistaDfnDeviceDebugNodeCreate::VistaDfnDeviceDebugNodeCreate( VistaDisplayManager* pDisplayManager )
: m_pDisplayManager( pDisplayManager )
{
}

IVdfnNode *VistaDfnDeviceDebugNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strTagName;
		if( subs.GetValue( "drivername", strTagName ) == false || strTagName.empty() )
			strTagName = "<none>";

		std::list<std::string> strPorts;
		subs.GetValue( "showlist", strPorts );

		VistaDfnDeviceDebugNode *pRet = new VistaDfnDeviceDebugNode( m_pDisplayManager, strTagName, strPorts);

		if( subs.HasProperty( "red" )
			|| subs.HasProperty( "green" )
			|| subs.HasProperty( "blue" ) )
		{
			float nRed = subs.GetValueOrDefault<float>( "red", 1.0f );
			float nGreen = subs.GetValueOrDefault<float>( "green", 1.0f );
			float nBlue = subs.GetValueOrDefault<float>( "blue", 0.0f );

			pRet->SetColor( nRed, nGreen, nBlue );
		}

		bool bShowType = subs.GetValueOrDefault<bool>( "showtype", false );
		pRet->SetShowType(bShowType);

		return pRet;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################

VdfnDumpHistoryNodeClusterCreate::VdfnDumpHistoryNodeClusterCreate( VistaClusterMode *pClusterMode )
	: VdfnNodeFactory::IVdfnNodeCreator()
	, m_pClusterMode(pClusterMode)
{

}

IVdfnNode *VdfnDumpHistoryNodeClusterCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strFileName;
		subs.GetValue( "file", strFileName );
		std::string strHostName = m_pClusterMode->GetNodeName();
		std::string strFinalName = strHostName.empty() ? strFileName
								   : strHostName+"."+strFileName;

		return new VdfnDumpHistoryNode(strFinalName);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################

VistaDfnEventSourceNodeCreate::VistaDfnEventSourceNodeCreate( VistaEventManager *pEvMgr,
								VistaInteractionManager *pInMa )
								: VdfnNodeFactory::IVdfnNodeCreator(),
								m_pEvMgr(pEvMgr),
								m_pInMa(pInMa)
{
}


IVdfnNode* VistaDfnEventSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaDfnEventSourceNode( m_pEvMgr, m_pInMa, oParams.GetValueOrDefault<std::string>( "tag", "" ) );
}

// #############################################################################

IVdfnNode *VistaDfnNavigationNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	assert( oParams.HasSubList("param" ) );
	const VistaPropertyList &oPars = oParams.GetSubListConstRef( "param" );

	int iDefaultMode = oPars.GetValueOrDefault<int>( "default_navigation_mode", 0 );

	float fDefaultLinearVelocity = oPars.GetValueOrDefault<float>( "default_linear_velocity", 1.0f );

	float fDefaultAngularVelocity = oPars.GetValueOrDefault<float>( "default_angular_velocity", Vista::Pi );

	return new VistaDfnNavigationNode( iDefaultMode,
										fDefaultLinearVelocity,
										fDefaultAngularVelocity );
}


// #############################################################################

VistaDfnWindowSourceNodeCreate::VistaDfnWindowSourceNodeCreate( VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnWindowSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strWindowName;
		if( subs.GetValue( "value", strWindowName ) )
		{			
			VistaWindow *pWindow = m_pMgr->GetWindowByName(strWindowName);
			if(pWindow)
			{
				return new VistaDfnWindowSourceNode( pWindow );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VistaDfnViewportSourceNodeCreate::VistaDfnViewportSourceNodeCreate( VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnViewportSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strWindowName;
		if( subs.GetValue( "value", strWindowName ) )
		{			
			VistaViewport *pVp = m_pMgr->GetViewportByName(strWindowName);
			if(pVp)
			{
				return new VistaDfnViewportSourceNode( pVp );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VistaDfnViewerSinkNodeCreate::VistaDfnViewerSinkNodeCreate(VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnViewerSinkNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(subs.HasProperty("displaysystem"))
		{
			std::string strDispSysName;
			subs.GetValue( "displaysystem", strDispSysName );
			VistaDisplaySystem *pSystem = m_pMgr->GetDisplaySystemByName(strDispSysName);
			if(pSystem)
			{
				return new VistaDfnViewerSinkNode( pSystem );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VistaDfnTrackballNodeCreate::VistaDfnTrackballNodeCreate(VistaDisplayManager *pDispMgr)
	: m_pDispMgr(pDispMgr)
{}

IVdfnNode *VistaDfnTrackballNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaVdfnTrackball();
}

// #############################################################################

VistaDfnSystemTriggerControlNodeCreate::VistaDfnSystemTriggerControlNodeCreate(VistaKeyboardSystemControl *pCtrl )
								: VdfnNodeFactory::IVdfnNodeCreator(), m_pCtrl(pCtrl)

{
}


IVdfnNode *VistaDfnSystemTriggerControlNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaDfnSystemTriggerControlNode( m_pCtrl );
}

// #############################################################################

VistaDfnReferenceFrameTransformNodeCreate::VistaDfnReferenceFrameTransformNodeCreate(
						VistaDisplayManager* pDisplayManager )
:	m_pDisplayManager( pDisplayManager )
{
}

IVdfnNode* VistaDfnReferenceFrameTransformNodeCreate::CreateNode( 
												const VistaPropertyList &oParams ) const
{
	VistaDisplaySystem* pDisplaySystem;
	std::string sDisplaySystemName;
	if( oParams.GetValue( "displaysystem", sDisplaySystemName ) )
	{		
		pDisplaySystem = m_pDisplayManager->GetDisplaySystemByName( sDisplaySystemName );		
	}
	else
	{
		pDisplaySystem = m_pDisplayManager->GetDisplaySystem();
	}
	if( pDisplaySystem == NULL )
			return NULL;

	assert( oParams.HasSubList("param" ) );
	const VistaPropertyList& oSubParams = oParams.GetSubListConstRef( "param" );
	bool bTransformToFrame = false;
	std::string sTransformMode;
	if( oSubParams.GetValue( "transform_mode", sTransformMode ) )
	{
		if( sTransformMode == "to_frame" )
			bTransformToFrame = true;
		else if( sTransformMode == "from_frame" )
			bTransformToFrame = false;
		else
		{
			vstr::warnp() << "VistaDfnReferenceFrameTransformNodeCreate -- "
				<< " Parameter [transform_mode] has unknown value [" << sTransformMode
				<< "] - valid options are to_frame and from_frame" << std::endl;
			return NULL; 
		}
	}

	int iNrPositionPorts = oSubParams.GetValueOrDefault<int>( "num_position_ports", 1 );
	int iNrOrientationPorts = oSubParams.GetValueOrDefault<int>( "num_orientation_ports", 1 );
	int iNrMatrixPorts = oSubParams.GetValueOrDefault<int>( "num_matrix_ports", 0 );

	return ( new VistaDfnReferenceFrameTransformNode(
									pDisplaySystem->GetReferenceFrame(),
									bTransformToFrame,
									iNrPositionPorts,
									iNrOrientationPorts,
									iNrMatrixPorts ) );
}


// #############################################################################

VistaDfnProjectionSourceNodeCreate::VistaDfnProjectionSourceNodeCreate( VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnProjectionSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strWindowName;
		if( subs.GetValue( "value", strWindowName ) )
		{			
			VistaProjection *pVp = m_pMgr->GetProjectionByName(strWindowName);
			if(pVp)
			{
				return new VistaDfnProjectionSourceNode( pVp );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

class VistaFrameclockNodeCreate::TimerNodeFrameclockGet : public VdfnTimerNode::CGetTime
{
public:
	TimerNodeFrameclockGet( VistaClusterMode* pClusterAux )
	: m_pClusterAux( pClusterAux )
	{
	}
	virtual double GetTime()
	{
		return m_pClusterAux->GetFrameClock();
	}
private:
	VistaClusterMode* m_pClusterAux;
};

VistaFrameclockNodeCreate::VistaFrameclockNodeCreate( VistaClusterMode* pClusterAux ) 
: VdfnNodeFactory::IVdfnNodeCreator()
, m_pClusterAux( pClusterAux )
{
}

IVdfnNode* VistaFrameclockNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VdfnTimerNode( new TimerNodeFrameclockGet( m_pClusterAux ), false );
}

// #############################################################################

// #############################################################################

// #############################################################################

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


