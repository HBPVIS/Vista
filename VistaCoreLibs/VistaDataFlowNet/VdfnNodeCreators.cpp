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
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>

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
	oSubs.GetValue( "reset_on_activate", reset );

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
	const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

	double dThreshold = 0.0;
	subs.GetValue( "threshold", dThreshold );

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
		std::string strObj;
		if( subs.GetValue("object", strObj) == false )
		{
			vstr::warnp() << "VdfnGetTransformNodeCreate::CreateNode() -- "
				<< "Required parameter [object] has not been specified!" << std::endl;
		}
		IVistaTransformable *pTrans = m_pReg->GetObjectTransform( strObj );

		std::string strMode = subs.GetValueOrDefault<std::string>( "mode", "RELATIVE" );

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
		std::string strObj;
		if( subs.GetValue( "object", strObj ) == false )
		{
			vstr::warnp() << "VdfnSetTransformNodeCreate::CreateNode() -- "
				<< "Required parameter [object] has not been specified!" << std::endl;
		}
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
		std::string strObj;
		if( subs.GetValue( "object", strObj ) == false )
		{
			vstr::warnp() << "VdfnApplyTransformNodeCreate::CreateNode() -- "
				<< "Required parameter [object] has not been specified!" << std::endl;
		}
		bool applyLocal = subs.GetValueOrDefault<bool>( "local", false );
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
		subs.GetValue<std::list<std::string> >("project", liProjects );

		std::string sMode("LAZY");
		if( subs.HasProperty("mode") )
			sMode = subs.GetValue<std::string>("mode");

		VdfnHistoryProjectNode::eMode eMode = VdfnHistoryProjectNode::MD_LAZY;
		if( sMode == "HOT" )
			eMode = VdfnHistoryProjectNode::MD_HOT;
		else if( sMode == "ITERATE" )
			eMode = VdfnHistoryProjectNode::MD_ITERATE;
		else if( sMode == "INDEXED" )
			eMode = VdfnHistoryProjectNode::MD_INDEXED;

		if( liProjects.empty() )
			liProjects.push_back( std::string("*") );

		return new VdfnHistoryProjectNode(liProjects, eMode);
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
		VistaDeviceSensor* pSensor = NULL;
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string sDriver;
		if( subs.GetValue( "driver", sDriver ) == false )
		{
			vstr::warnp() << "[SensorNodeCreate]: - "
					<< "No parameter [driver] has been specified" << std::endl;
			return NULL;
		}
		int nSensorID = -1;
		std::string sSensorName;
		
		const bool bHasSensorIndex = subs.GetValue( "sensor_index", nSensorID );
		const bool bHasSensorName = subs.GetValue( "sensor_name", sSensorName );

		IVistaDeviceDriver *pDriver = m_pMap->GetDeviceDriver( sDriver );
		if( pDriver == NULL )
		{
			vstr::warnp() << "[DriverSensorNodeCreate]: Driver [" << sDriver << "] not found in map\n"
						<< vstr::indent << " Available Drivers:" << std::endl;
			vstr::IndentObject oIndent;
			for(VistaDriverMap::const_iterator cit = m_pMap->begin();
				cit != m_pMap->end(); ++cit)
			{
				vstr::warni()  << (*cit).first << std::endl;
			}
			return NULL;
		}

		// check for a sensor mapping aspect		
		VistaDriverSensorMappingAspect *pMappingAspect =
				dynamic_cast<VistaDriverSensorMappingAspect*>(pDriver->GetAspectById(
				VistaDriverSensorMappingAspect::GetAspectId() ) );

		if( pMappingAspect )
		{
			std::string sSensorType = subs.GetValueOrDefault<std::string>( "type", "" );
			unsigned int nType = pMappingAspect->GetTypeId( sSensorType );
			if( nType == ~0 )
			{
				vstr::warnp() << "[SensorNodeCreate]: - "
						<< "Driver [" << sDriver << "] has a sensor mapping aspect, "
						<< "but no sensor type with name [" << sSensorType << "].\n"
						<< vstr::indent << "Type names are: " << std::endl;
				std::list<std::string> liTypes = pMappingAspect->GetTypeNames();
				vstr::IndentObject oIndent;
				for(std::list<std::string>::const_iterator it = liTypes.begin();
					it != liTypes.end(); ++it )
				{
					vstr::warni() << "[" << *it << "]" << std::endl;
				}
				return NULL;
			}

			// drivers with sensor mapping always need specific sensor targets
			// Exception: There is exactly one type and one sensor
			if( !bHasSensorIndex && !bHasSensorName )
			{
				if( pMappingAspect->GetNumberOfRegisteredTypes() != 1
					&& pMappingAspect->GetNumRegisteredSensorsForType( nType ) > 1 )
				{
					vstr::warnp() << "[SensorNodeCreate]: - Driver ["
							<< sDriver << "] requires a sensor to be specified!"
							<< " Use \"sensor_name\" or \"sensor_index\"" << std::endl;
					return NULL;
				}
				unsigned int nId = pMappingAspect->GetSensorId( nType, 0 );
				pSensor = pDriver->GetSensorByIndex( nId );
			}			
			else if( bHasSensorIndex )
			{
				unsigned int nId = pMappingAspect->GetSensorId( nType, nSensorID );
				pSensor = pDriver->GetSensorByIndex( nId );
			}
			else if( bHasSensorName )
			{
				pSensor = pDriver->GetSensorByName( sSensorName );
			}

			if( pSensor == NULL )
			{
				if( bHasSensorName )
				{
					vstr::warnp() << "[SensorNodeCreate]: - "
						<< "Driver [" << sDriver << "] has no sensor with name [" << sSensorName
						<< "] for type [" << sSensorType << "].\n";
				}
				else
				{
					vstr::warnp() << "[SensorNodeCreate]: - "
						<< "Driver [" << sDriver << "] has no sensor with Id [" << nSensorID
						<< "] for type [" << sSensorType << "].\n";
				}
			}
		}
		else
		{
			// no SensorMappingAspect
			if( bHasSensorIndex )
			{
				pSensor = pDriver->GetSensorByIndex( nSensorID );
			}
			else if( bHasSensorName )
			{
				pSensor = pDriver->GetSensorByName( sSensorName );
			}
			else if( pDriver->GetNumberOfSensors() == 1 )
			{
				// we only have one sensors, so we'll take this one
				pSensor = pDriver->GetSensorByIndex( 0 );
			}
			else
			{
				vstr::warnp() << "[SensorNodeCreate]: - Driver ["
						<< sDriver << "] requires a sensor to be specified! "
						<< "Use \"sensor_name\" or \"sensor_index\"" << std::endl;
				return NULL;
			}

			if( pSensor == NULL )
			{
				if( bHasSensorName )
				{
					vstr::warnp() << "[SensorNodeCreate]: - "
						<< "Driver [" << sDriver << "] has no sensor with name [" << sSensorName << "].\n";
				}
				else
				{
					vstr::warnp() << "[SensorNodeCreate]: - "
						<< "Driver [" << sDriver << "] has no sensor with Id [" << nSensorID << "].\n";					
				}
				vstr::warn() << "Available sensors are: \n";
				vstr::IndentObject oIndent;
				for( int i = 0; i < (int)pDriver->GetNumberOfSensors(); ++i  )
				{
					vstr::warn() << "Id: " << i 
							<<  "\tName: " << pDriver->GetSensorByIndex(i)->GetSensorName() 
							<< std::endl;
				}
				return NULL;
			}
		}

		if( pSensor )
		{
			return new VdfnDriverSensorNode(pSensor);
		}
		else
		{
			vstr::warnp() << "[DriverSensorNodeCreate]: Could not create SensorNode\n";
			vstr::IndentObject oIndent;
			vstr::warni() << "DRIVER       : [" << sDriver << "]\n";
			vstr::warni() << "SENSOR NAME  : [" << sSensorName << "]\n";
			vstr::warni() << "SENSOR INDEX : [" << nSensorID << "]" << std::endl;
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
		return NULL;

	bool bWriteHeader = subs.GetValueOrDefault<bool>( "writeheader", false );
	bool bWriteTime = subs.GetValueOrDefault<bool>( "writetime", false );
	bool bWriteDiff = subs.GetValueOrDefault<bool>( "writediff", false );
	bool bLogToConsole = subs.GetValueOrDefault<bool>( "logtoconsole", false );

	std::list<std::string> liPorts;
	subs.GetValue( "sortlist", liPorts );

	std::list<std::string> liTriggers;
	subs.GetValue( "triggerlist", liTriggers );

	char cSep = ' ';
	std::string strSep;	
	if( subs.GetValue( "seperator", strSep ) == false )
	{
		if( strSep == "\\t")
			cSep = '\t';
		else
			cSep = *strSep.begin();
	}


	if(!bWriteTime)
		bWriteDiff = false; // override user settings

	return new VdfnLoggerNode( subs.GetValueOrDefault<std::string>( "prefix", "" ),
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

		std::string strGraphFileName;
		if( subs.GetValue( "graph", strGraphFileName ) == false )
		{
			vstr::warnp() << "VdfnCompositeNodeCreate() -- no value for [graph] has been given!"
					<< std::endl;
			return NULL;
		}
		std::string strNodeName;
		oParams.GetValue( "name", strNodeName );
		std::string strParameterFile;
		oParams.GetValue( "parameterfile", strNodeName );


		VdfnGraph *pGraph = VdfnPersistence::LoadGraph( strGraphFileName, strNodeName, false, true, strParameterFile );
		if(!pGraph)
		{
			vstr::warnp() << "[VdfnCompositeNodeCreate]: -- file: ["
						<< strGraphFileName
						<< "] could not be loaded"
						 << std::endl;
			return NULL;
		}

		if( oParams.HasProperty("dumpparams")
			&& oParams.GetPropertyConstRef("dumpparams").GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
		{
			const VistaPropertyList &dumpParams = oParams.GetPropertyConstRef("dumpparams").GetPropertyListConstRef();
			bool bDump = dumpParams.GetValueOrDefault( "dump", false );

			if(bDump)
			{
				std::string strGraphDotFile;
				dumpParams.GetValue( "dotfilename", strGraphDotFile );
				bool bWritePorts = dumpParams.GetValueOrDefault<bool>( "writeports", false );
				bool bWriteTypes = dumpParams.GetValueOrDefault<bool>( "writetypes", false );

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

	std::string strEnvVarName;
	std::string strEnvDefault;
	prams.GetValue( "varname", strEnvVarName );
	prams.GetValue( "default", strEnvDefault );

	bool bIsStrict = prams.GetValueOrDefault<bool>( "strict", false );

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


