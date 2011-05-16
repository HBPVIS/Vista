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

#include "VdfnNodeCreators.h"
#include "VdfnPortFactory.h"
#include "VdfnPersistence.h"
#include "VdfnGraph.h"


#include "VdfnShallowNode.h"
#include "VdfnTimerNode.h"
#include "VdfnUpdateThresholdNode.h"

#include "VdfnGetTransformNode.h"
#include "VdfnSetTransformNode.h"
#include "VdfnApplyTransformNode.h"

#include "VdfnLoggerNode.h"
#include "VdfnCompositeNode.h"


#include "VdfnHistoryProjectNode.h"
#include "VdfnDriverSensorNode.h"
#include "VdfnObjectRegistry.h"

#include "VdfnConstantValueNode.h"

#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverSensorMappingAspect.h>

#include <VistaTools/VistaEnvironment.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVdfnNode *VdfnShallowNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	VdfnShallowNode *pShallowNode = new VdfnShallowNode;
	try
	{
		VdfnPortFactory *pFac = VdfnPortFactory::GetSingleton();
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(subs.HasProperty("inports"))
		{
			const VistaPropertyList &oInports = subs.GetPropertyConstRef("inports").GetPropertyListConstRef();

			for(VistaPropertyList::const_iterator cit = oInports.begin();
				cit != oInports.end(); ++cit)
			{
				std::string strName = (*cit).first;
				std::string strValue = (*cit).second.GetValue();

				VdfnPortFactory::CPortAccess *pAccess = pFac->GetPortAccess( strValue );
				if(pAccess)
				{
					IVdfnPort *pPort = pAccess->m_pCreationMethod->CreatePort();
					pShallowNode->SetInPort( strName, pPort );
				}
			}
		}

		if(subs.HasProperty("outports"))
		{
			const VistaPropertyList &oOutports = subs.GetPropertyConstRef("outports").GetPropertyListConstRef();

			for(VistaPropertyList::const_iterator cit = oOutports.begin();
				cit != oOutports.end(); ++cit)
			{
				std::string strName = (*cit).first;
				std::string strValue = (*cit).second.GetValue();

				VdfnPortFactory::CPortAccess *pAccess = pFac->GetPortAccess( strValue );
				if(pAccess)
				{
					IVdfnPort *pPort = pAccess->m_pCreationMethod->CreatePort();
					pShallowNode->SetOutPort( strName, pPort );
				}
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return pShallowNode;
}


// #############################################################################

IVdfnNode *VdfnTimerNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
	bool reset = false;

	if( oSubs.HasProperty( "reset_on_activate" ) )
		reset = oSubs.GetBoolValue( "reset_on_activate" );

	return new VdfnTimerNode( new CTimerGetTime, reset );
}

// #############################################################################

IVdfnNode *VdfnTickTimerNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VdfnTickTimerNode( new CTimerGetTime );
}

// #############################################################################

IVdfnNode *VdfnUpdateThresholdNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	double dThreshold = 0.0;
	const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	if( subs.HasProperty("threshold") )
		 dThreshold = subs.GetDoubleValue("threshold");

	return new VdfnUpdateThresholdNode( dThreshold );
}


// #############################################################################

VdfnGetTransformNodeCreate::VdfnGetTransformNodeCreate(VdfnObjectRegistry *pReg)
: m_pReg(pReg)
{
}

IVdfnNode *VdfnGetTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strObj = subs.GetStringValue("object");
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );

        std::string strMode = subs.GetStringValue("mode");
        if(strMode.empty())
            strMode = "RELATIVE";

        VdfnGetTransformNode *pNode;
		if(pTrans)
        {
			pNode = new VdfnGetTransformNode(pTrans);
        }
		else
			pNode = new VdfnGetTransformNode( m_pReg, strObj );

        if(VistaAspectsComparisonStuff::StringEquals(strMode, "WORLD", false))
            pNode->SetTransformGetMode( VdfnGetTransformNode::TRANS_WORLD );
        else
            pNode->SetTransformGetMode( VdfnGetTransformNode::TRANS_RELATIVE );

        return pNode;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VdfnSetTransformNodeCreate::VdfnSetTransformNodeCreate(VdfnObjectRegistry *pReg)
: m_pReg(pReg)
{
}

IVdfnNode *VdfnSetTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strObj = subs.GetStringValue("object");
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );
		if(pTrans)
			return new VdfnSetTransformNode(pTrans);
		else
			return new VdfnSetTransformNode(m_pReg, strObj);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VdfnApplyTransformNodeCreate::VdfnApplyTransformNodeCreate(VdfnObjectRegistry *pReg)
: m_pReg(pReg)
{
}

IVdfnNode *VdfnApplyTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strObj = subs.GetStringValue("object");
		bool applyLocal = subs.GetBoolValue("local");
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );
		if(pTrans)
			return new VdfnApplyTransformNode(pTrans, applyLocal);
		else
			return new VdfnApplyTransformNode(m_pReg, strObj, applyLocal);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}


// #############################################################################


IVdfnNode *VdfnHistoryProjectNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::list<std::string> liProjects;
		subs.GetStringListValue("project", liProjects );

		return new VdfnHistoryProjectNode(liProjects);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}


// #############################################################################

IVdfnNode *VdfnDriverSensorNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(subs.HasProperty("driver") && (subs.HasProperty("sensor") || subs.HasProperty("sensor_id")))
		{
			std::string strDriver = subs.GetStringValue("driver");
			int nIdx;
			if(subs.HasProperty("sensor"))
				nIdx = subs.GetIntValue("sensor");
			else
				nIdx = -1;
			std::string strSensorId = subs.GetStringValue("sensor_id");

			if(nIdx == -1 && strSensorId.empty())
				return NULL;

			IVistaDeviceDriver *pDriver = m_pMap->GetDeviceDriver( strDriver );
			if(pDriver)
			{
				// check for a sensor mapping aspect
				VistaDeviceSensor *pSensor = NULL;
				VistaDriverSensorMappingAspect *pAspect =
						dynamic_cast<VistaDriverSensorMappingAspect*>(pDriver->GetAspectById(
						VistaDriverSensorMappingAspect::GetAspectId() ) );
				if(pAspect)
				{
					std::string strSensorType = subs.GetStringValue("type");
					unsigned int nType = pAspect->GetTypeId(strSensorType);
					if(nType != ~0) // ok, valid type
					{
						unsigned int nId = pAspect->GetSensorId( nType, nIdx );
						if(nId != ~0)
							pSensor = pDriver->GetSensorByIndex( nId );
						else if( !strSensorId.empty() )
							pSensor = pDriver->GetSensorByName( strSensorId );
						else
							vdfnerr << "DRIVER [" << strDriver << "] has a sensor mapping aspect, "
							          << "but a sensor of type [" << strSensorType << "] was not found, "
							          << "neither an id was given. Giving up." << std::endl;
					}
					else
					{
						vdfnerr << "DRIVER [" << strDriver << "] has a sensor mapping aspect, "
								<< "but no sensor type with name [" << strSensorType << "].\n"
								<< "Type names are: \n";
						std::list<std::string> liTypes = pAspect->GetTypeNames();
						for(std::list<std::string>::const_iterator it = liTypes.begin();
							it != liTypes.end(); ++it )
						{
							vdfnerr << "[ " << *it << "]\n";
						}
						vdfnerr << std::endl;
					}
				}
				else
				{
					if(nIdx != -1)
						pSensor = pDriver->GetSensorByIndex(nIdx);
					else
						pSensor = pDriver->GetSensorByName( strSensorId );
				}

				if(pSensor)
					return new VdfnDriverSensorNode(pSensor);
				else
				{
					vdfnerr << "COULD NOT CREATE DRIVER SENSOR NODE\n"
					          << "NAME given: [" << strSensorId << "]\n"
					          << "IDX  given: [" << nIdx << "]\n";
				}
			}
			else
			{
				vdfnerr << "DRIVER [" << strDriver << "] NOT FOUND IN MAP."
						  << " AVAILABLE DRIVERS ARE\n";

				for(VistaDriverMap::const_iterator cit = m_pMap->begin();
					cit != m_pMap->end(); ++cit)
				{
					vdfnerr << (*cit).first << std::endl;
				}

			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


IVdfnNode *VdfnLoggerNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	if(!subs.HasProperty("prefix"))
		return false;

	bool bWriteHeader = subs.GetBoolValue("writeheader");
	bool bWriteTime = subs.GetBoolValue("writetime");
	bool bWriteDiff = subs.GetBoolValue("writediff");
	bool bLogToConsole = subs.GetBoolValue("logtoconsole");

	std::list<std::string> liPorts;
	subs.GetStringListValue( "sortlist", liPorts );

	std::list<std::string> liTriggers;
	subs.GetStringListValue( "triggerlist", liTriggers );

	char cSep = ' ';
	std::string strSep = subs.GetStringValue("seperator");
	if( !strSep.empty())
	{
		if( strSep == "\\t")
			cSep = '\t';
		else
			cSep = *strSep.begin();
	}


	if(!bWriteTime)
		bWriteDiff = false; // override user settings

	return new VdfnLoggerNode( subs.GetStringValue( "prefix" ),
		                         bWriteHeader,
								 bWriteTime,
								 bWriteDiff,
								 bLogToConsole,
								 liPorts,
								 liTriggers, cSep );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IVdfnNode *VdfnCompositeNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if(!subs.HasProperty("graph"))
			return NULL;

		std::string strGraphFileName = subs.GetStringValue("graph");
		std::string strNodeName = oParams.GetStringValue("name");
		std::string strParameterFile = oParams.GetStringValue("parameterfile");


		VdfnGraph *pGraph = VdfnPersistence::LoadGraph( strGraphFileName, strNodeName, false, true, strParameterFile );
		if(!pGraph)
		{
			vdfnerr << "VdfnCompositeNodeCreate::CreateNode() -- file: ["
			          << strGraphFileName
			          << "]"
			          << std::endl;
			return NULL;
		}

		if( oParams.HasProperty("dumpparams")
			&& oParams.GetPropertyConstRef("dumpparams").GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
		{
			const VistaPropertyList &dumpParams = oParams.GetPropertyConstRef("dumpparams").GetPropertyListConstRef();
			bool bDump = dumpParams.GetBoolValue("dump");

			if(bDump)
			{
				std::string strGraphDotFile = dumpParams.GetStringValue("dotfilename");
				bool bWritePorts = dumpParams.GetBoolValue("writeports");
				bool bWriteTypes = dumpParams.GetBoolValue("writetypes");

				VdfnPersistence::SaveAsDot( pGraph, strGraphDotFile, strNodeName, bWritePorts, bWriteTypes );
			}
		}

		return new VdfnCompositeNode( pGraph );
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################



IVdfnNode *VdfnEnvStringValueNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	std::string strEnvVarName = prams.GetStringValue("varname");
	std::string strEnvDefault = prams.GetStringValue("default");

	bool bIsStrict = prams.GetBoolValue("strict");

	std::string strEnvVarValue = VistaEnvironment::GetEnv(strEnvVarName);

	// is it strict? then we do not accept empty vars
	if(bIsStrict && strEnvVarValue.empty())
		return NULL;

	// it is not strict, but empty... did the user give a default value?
	if( strEnvVarValue.empty() && !strEnvDefault.empty() )
		strEnvVarValue = strEnvDefault;
	// else: no, probably an empty string is ok, too

	TVdfnConstantValueNode<std::string> *pEnvNode = new TVdfnConstantValueNode<std::string>;
	pEnvNode->SetValue(strEnvVarValue);
	return pEnvNode;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


