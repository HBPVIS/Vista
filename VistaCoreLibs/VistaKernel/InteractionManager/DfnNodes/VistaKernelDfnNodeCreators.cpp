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

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/VistaClusterAux.h>
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

static std::string ConvertV3ToString( const VistaVector3D &v3Value )
{
	std::list<float> liTemp;
	for (int i=0; i<4; ++i)
		liTemp.push_back(v3Value[i]);

	std::stringstream stream("");
	stream << std::fixed << std::setw(9)
		   << v3Value[0] << " "
		   << v3Value[1] << " "
		   << v3Value[2] << " " 
		   << v3Value[3]; 

	return stream.str();
}

static std::string ConvertQToString( const VistaQuaternion &qValue )
{
	std::list<float> liTemp;
	for (int i=0; i<4; ++i)
		liTemp.push_back(qValue[i]);

	return VistaAspectsConversionStuff::ConvertToString(liTemp);
}

static std::string ConvertMatToString( const VistaTransformMatrix &matrix )
{
	std::string str = "";
	std::ostringstream ss(str);
	for( int r = 0 ; r < 4 ; r++ )
	{
		for( int c = 0 ; c < 4 ; c++ )
		{
			ss << std::fixed << std::setw(9) << matrix.GetValue(r,c) << " ";
		}
		ss << "| ";
	}

	return ss.str();
}

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
					new VistaDfnWindowSourceNodeCreate( pVistaSystem->GetEventManager(), pVistaSystem->GetDisplayManager() ) );
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
					new VdfnDeviceDebugNodeCreate( pVistaSystem->GetWindowingToolkit() ) );
	pFac->SetNodeCreator( "SystemTriggerControl",
					new VistaDfnSystemTriggerControlNodeCreate( pVistaSystem->GetKeyboardSystemControl() ) );
	pFac->SetNodeCreator( "Navigation", 
					new VistaDfnNavigationNodeCreate() );
	pFac->SetNodeCreator( "ClusterNodeInfo", 
					new VdfnClusterNodeInfoNodeCreate( pVistaSystem->GetClusterAux() ) );
	pFac->SetNodeCreator( "Frameclock",
					new VistaFrameclockNodeCreate( pVistaSystem->GetClusterAux() ) );

	// this is an override, the old pointer is lost, so expect a memory lead
	// in a profile. The policy of overriding is currently not defined. @todo
	pFac->SetNodeCreator( "DumpHistory",
					new VdfnDumpHistoryNodeClusterCreate( pVistaSystem->GetClusterAux() ) );

	pFac->SetNodeCreator( "Trackball",
					new VistaDfnTrackballNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "EventSource",
					new VistaDfnEventSourceNodeCreate(pVistaSystem-> GetEventManager(), pVistaSystem->GetInteractionManager() ) );
	pFac->SetNodeCreator( "TextOverlay[int]",
					new VistaDfnTextOverlayNodeCreate<int>( pVistaSystem->GetDisplayManager(), &ConvertToString<int>) );
	pFac->SetNodeCreator( "TextOverlay[unsigned int]",
					new VistaDfnTextOverlayNodeCreate<unsigned int>( pVistaSystem->GetDisplayManager(), &ConvertToString<unsigned int>) );
	pFac->SetNodeCreator( "TextOverlay[int64]",
					new VistaDfnTextOverlayNodeCreate<VistaType::sint64>( pVistaSystem->GetDisplayManager(), &ConvertToString<VistaType::sint64>) );
	pFac->SetNodeCreator( "TextOverlay[bool]",
					new VistaDfnTextOverlayNodeCreate<bool>( pVistaSystem->GetDisplayManager(), &ConvertToString<bool> ) );
	pFac->SetNodeCreator( "TextOverlay[double]", new VistaDfnTextOverlayNodeCreate<double>( pVistaSystem->GetDisplayManager(), &ConvertToString<double> ) );
	pFac->SetNodeCreator( "TextOverlay[VistaType::microtime]",
					new VistaDfnTextOverlayNodeCreate<VistaType::microtime>( pVistaSystem->GetDisplayManager(), &ConvertToString<VistaType::microtime> ) );
	pFac->SetNodeCreator( "TextOverlay[float]",
					new VistaDfnTextOverlayNodeCreate<float>( pVistaSystem->GetDisplayManager(), &ConvertToString<float> ) );
	pFac->SetNodeCreator( "TextOverlay[string]",
		new VistaDfnTextOverlayNodeCreate<std::string>( pVistaSystem->GetDisplayManager(), &ConvertToString<std::string> ) );
	pFac->SetNodeCreator( "TextOverlay[VistaVector3D]",
					new VistaDfnTextOverlayNodeCreate<VistaVector3D>( pVistaSystem->GetDisplayManager(), &ConvertV3ToString ) );
	pFac->SetNodeCreator( "TextOverlay[VistaQuaternion]",
					new VistaDfnTextOverlayNodeCreate<VistaQuaternion>( pVistaSystem->GetDisplayManager(), &ConvertQToString ) );
	pFac->SetNodeCreator( "TextOverlay[VistaTransformMatrix]",
					new VistaDfnTextOverlayNodeCreate<VistaTransformMatrix>( pVistaSystem->GetDisplayManager(), &ConvertMatToString ) );

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

		if(subs.HasProperty("displaysystem") && subs.HasProperty("viewport"))
		{
			std::string strDisplaySystem = subs.GetStringValue("displaysystem");
			VistaDisplaySystem *pSys = m_pMgr->GetDisplaySystemByName(strDisplaySystem);

			std::string strViewport = subs.GetStringValue("viewport");
			VistaViewport *pViewport = m_pMgr->GetViewportByName( strViewport );
			if(pViewport && pSys)
			{
				float fOriginOffset = 0;
				if( subs.HasProperty( "origin_offset_along_ray" ) )
					fOriginOffset = (float)subs.GetDoubleValue( "origin_offset_along_ray" );
				VistaVdfn3DMouseTransformNode *pNd = new VistaVdfn3DMouseTransformNode(
																	pSys, pViewport, fOriginOffset );
				bool bToFrame = subs.GetBoolValue("in_world_coordinates");
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


VdfnDeviceDebugNodeCreate::VdfnDeviceDebugNodeCreate(IVistaWindowingToolkit *wta)
: m_pWta(wta)
{

}
// #############################################################################

VdfnClusterNodeInfoNodeCreate::VdfnClusterNodeInfoNodeCreate(VistaClusterAux *pAux)
	: m_pAux(pAux)
{
}

IVdfnNode *VdfnClusterNodeInfoNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		return new VdfnClusterNodeInfoNode(m_pAux);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################

IVdfnNode *VdfnDeviceDebugNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{

		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strTagName = subs.GetStringValue("drivername");
		if(strTagName.empty())
			strTagName = "<none>";

		std::list<std::string> strPorts;
		subs.GetStringListValue("showlist", strPorts);

		VdfnDeviceDebugNode *pRet = new VdfnDeviceDebugNode( m_pWta, strTagName, strPorts);


		float nRed = float(subs.GetDoubleValue("red"));
		float nGreen = float(subs.GetDoubleValue("green"));
		float nBlue = float(subs.GetDoubleValue("blue"));
		if( subs.HasProperty( "red" )
			|| subs.HasProperty( "green" )
			|| subs.HasProperty( "blue" ) )
		{
			pRet->SetColor(nRed,nGreen,nBlue);
		}

		bool bShowType = subs.GetBoolValue("showtype");
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

VdfnDumpHistoryNodeClusterCreate::VdfnDumpHistoryNodeClusterCreate( VistaClusterAux *pAux )
	: VdfnNodeFactory::IVdfnNodeCreator()
	, m_pAux(pAux)
{

}

IVdfnNode *VdfnDumpHistoryNodeClusterCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strFileName = subs.GetStringValue("file");
		std::string strHostName = m_pAux->GetNodeName();
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


IVdfnNode *VistaDfnEventSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaDfnEventSourceNode( m_pEvMgr, m_pInMa, oParams.GetStringValue("tag") );
}

// #############################################################################

IVdfnNode *VistaDfnNavigationNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &oPars = oParams.GetPropertyListValue( "param" );

	int iDefaultMode = 0;	
	if( oPars.HasProperty( "default_navigation_mode" ) )
		iDefaultMode = oPars.GetIntValue( "default_navigation_mode" );

	float fDefaultLinearVelocity = 1.0f;
	if( oPars.HasProperty( "default_linear_velocity" ) )
		fDefaultLinearVelocity = (float)oPars.GetDoubleValue( "default_linear_velocity" );

	float fDefaultAngularVelocity = 3.14159f;
	if( oPars.HasProperty( "default_angular_velocity" ) )
		fDefaultAngularVelocity = (float)oPars.GetDoubleValue( "default_angular_velocity" );

	return new VistaDfnNavigationNode( iDefaultMode,
										fDefaultLinearVelocity,
										fDefaultAngularVelocity );
}


// #############################################################################

VistaDfnWindowSourceNodeCreate::VistaDfnWindowSourceNodeCreate( VistaEventManager *pEvMgr,
													  VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr),
  m_pEvMgr(pEvMgr)
{
}

IVdfnNode *VistaDfnWindowSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(subs.HasProperty("value"))
		{
			std::string strWindowName = subs.GetStringValue("value");
			VistaWindow *pWindow = m_pMgr->GetWindowByName(strWindowName);
			if(pWindow)
			{
				return new VistaDfnWindowSourceNode( m_pEvMgr, pWindow );
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

		if(subs.HasProperty("value"))
		{
			std::string strWindowName = subs.GetStringValue("value");
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
			std::string strDispSysName = subs.GetStringValue("displaysystem");
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
	const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

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
	if( oParams.HasProperty( "displaysystem" ) )
	{
		std::string sDisplaySystemName = oParams.GetStringValue( "displaysystem" );
		pDisplaySystem = m_pDisplayManager->GetDisplaySystemByName( sDisplaySystemName );		
	}
	else
	{
		pDisplaySystem = m_pDisplayManager->GetDisplaySystem();
	}
	if( pDisplaySystem == NULL )
			return NULL;

	VistaPropertyList oSubParams = oParams.GetPropertyListValue( "param" );
	bool bTransformToFrame = false;
	if( oSubParams.HasProperty( "transform_mode" ) )
	{
		if( oSubParams.GetStringValue( "transform_mode" ) == "to_frame" )
			bTransformToFrame = true;
		else if( oSubParams.GetStringValue( "transform_mode" ) == "from_frame" )
			bTransformToFrame = false;
		else
			return NULL;
	}

	int iNrPositionPorts = 1;
	int iNrOrientationPorts = 1;
	int iNrMatrixPorts = 0;
	if( oSubParams.HasProperty( "num_position_ports" ) )
		iNrPositionPorts = oSubParams.GetIntValue( "num_position_ports" );
	if( oSubParams.HasProperty( "num_orientation_ports" ) )
		iNrOrientationPorts = oSubParams.GetIntValue( "num_orientation_ports" );
	if( oSubParams.HasProperty( "num_matrix_ports" ) )
		iNrMatrixPorts = oSubParams.GetIntValue( "num_matrix_ports" );


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

		if(subs.HasProperty("value"))
		{
			std::string strWindowName = subs.GetStringValue("value");
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
	TimerNodeFrameclockGet( VistaClusterAux* pClusterAux )
	: m_pClusterAux( pClusterAux )
	{
	}
	virtual double GetTime()
	{
		return m_pClusterAux->GetFrameClock();
	}
private:
	VistaClusterAux* m_pClusterAux;
};

VistaFrameclockNodeCreate::VistaFrameclockNodeCreate( VistaClusterAux* pClusterAux ) 
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


