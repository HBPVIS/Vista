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

#if defined(SUNOS)
#include <sunmath.h>
#endif

#include "VistaSystemConfigurators.h"
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaAspects/VistaPropertyFunctorRegistry.h>
#include <VistaKernel/InteractionManager/VistaDriverWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverLoggingAspect.h>
#include <VistaDeviceDriversBase/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceIdentificationAspect.h>
#include <VistaDeviceDriversBase/VistaDriverReferenceFrameAspect.h>
#include <VistaDeviceDriversBase/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/Drivers/VistaDTrackDriver.h>

//#include <VistaKernel/InteractionManager/VistaSensorFrameTransform.h>
//#include <VistaKernel/InteractionManager/VistaDisjunctTransform.h>
//#include <VistaKernel/InteractionManager/VistaCommandTransform.h>
//#include <VistaKernel/Stuff/VistaMouseNormalize.h>
//#include <VistaKernel/Stuff/Vista3DMouseTransform.h>
//#include <VistaKernel/Stuff/VistaGenericCommandMapTransform.h>
//#include <VistaKernel/Stuff/VistaPipedTransform.h>
//#include <VistaKernel/Stuff/VistaRefFrameTransform.h>
//#include <VistaKernel/Stuff/VistaSlotValueMultiplexTransform.h>

#include <VistaTools/VistaProfiler.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaInterProcComm/Connections/VistaConnectionFile.h>
#include <VistaInterProcComm/Connections/VistaConnectionFileTimed.h>
#include <VistaInterProcComm/Connections/VistaConnectionUSB.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaConnectionSerial.h>

#include "VistaKernelOut.h"


#include <string>
#include <set>
#include <list>
using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

void VistaConnectionConfigurator::IConnectionFactoryMethod::FailOnConnection(
	VistaConnection *pCon,
	const std::string &sReason) const
{
	delete pCon;
	vkernout << " [VistaIM] Failed on connection. Cause given: " << sReason << std::endl;
	vkernout << " [VistaIM] Giving up. This connection will NOT work.\n";
}

class VistaSerialConnectionFactoryMethod : public VistaConnectionConfigurator::IConnectionFactoryMethod
{
public:
	VistaSerialConnectionFactoryMethod()
		: IConnectionFactoryMethod() {}

	VistaConnection *CreateConnection(const VistaPropertyList &oProps)
		{
			VistaConnectionSerial *pConn = NULL;

			// Read information from config file and set connection properties
			std::string sPortName = oProps.GetStringValue("PORTNAME");
			int nPort = -1;

			if(sPortName.empty())
			{
				if(oProps.GetIntValue("PORT"))
				{
					nPort = oProps.GetIntValue("PORT");
					pConn = new VistaConnectionSerial;
					pConn->SetPortByIndex(nPort);
				}
			}
			else
			{
				pConn = new VistaConnectionSerial;
				pConn->SetPortByName(sPortName);
			}

			if(!pConn)
				return NULL;


			if(oProps.HasProperty("SPEED"))
			{
				int nSpeed = oProps.GetIntValue("SPEED");
				pConn->SetSpeed(nSpeed);
			}
			else
				pConn->SetSpeed(9600);

			if(oProps.HasProperty("PARITY"))
			{
				std::string sParity = oProps.GetStringValue("PARITY");
				if(VistaAspectsComparisonStuff::StringEquals(sParity, "odd", false))
				{
					pConn->SetParity( 1 );
				}
				else if(VistaAspectsComparisonStuff::StringEquals(sParity, "even", false))
				{
					pConn->SetParity( 2 );
				}
				else if(VistaAspectsComparisonStuff::StringEquals(sParity, "none", false))
				{
					pConn->SetParity( 0 );
				}
			}
			else
				pConn->SetParity(0);


			if(oProps.HasProperty("DATABITS"))
			{
				int nBits = oProps.GetIntValue("DATABITS");
				pConn->SetDataBits(nBits);
			}
			else
				pConn->SetDataBits(8);


			if(oProps.HasProperty("STOPBITS"))
			{
				std::string sStopBits = oProps.GetStringValue("STOPBITS");
				if(sStopBits == "2")
					pConn->SetStopBits(2.0f);
				else if(sStopBits == "1.5" || sStopBits == "1,5")
					pConn->SetStopBits(1.5f);
				else
					pConn->SetStopBits(1);
			}
			else
				pConn->SetStopBits(1);

			if(oProps.HasProperty("HARDWAREFLOW"))
			{
				bool bHwFlow = oProps.GetBoolValue("HARDWAREFLOW");
				pConn->SetHardwareFlow(bHwFlow);
			}
			else
				pConn->SetHardwareFlow(false);

			if(oProps.HasProperty("SOFTWAREFLOW"))
			{
				bool bSwFlow = oProps.GetBoolValue("SOFTWAREFLOW");
				pConn->SetSoftwareFlow(bSwFlow);
			}
			else
				pConn->SetSoftwareFlow(false);

			if(!pConn->Open())
			{
				// will delete pConn for me!
				FailOnConnection(pConn, "COULD NOT OPEN SERIAL PORT");
				return NULL;
			}

			return pConn;
		}
};

class VistaUSBConnectionFactoryMethod : public VistaConnectionConfigurator::IConnectionFactoryMethod
{
public:
	VistaConnection *CreateConnection(const VistaPropertyList &oProps)
		{
			unsigned int nVendorId  = oProps.GetIntValue("VENDORID");
			unsigned int nProductId = oProps.GetIntValue("PRODUCTID");

			VistaConnectionUSB *pCon = new VistaConnectionUSB( nVendorId, nProductId );

			if(!pCon->Open())
			{
				FailOnConnection(pCon, "COULD NOT OPEN USB CONNECTION");
				return NULL;
			}

			return pCon;
		}
};

class VistaIPConnectionFactoryMethod : public VistaConnectionConfigurator::IConnectionFactoryMethod
{
public:
	VistaConnection *CreateConnection(const VistaPropertyList &oProps)
		{
			std::string strHost = oProps.GetStringValue("ADDRESS");
			int nPort = oProps.GetIntValue("PORT");

			if(strHost.empty() || nPort <= 0)
				return NULL;

			std::string sType = oProps.GetStringValue("TYPE");
			VistaConnectionIP *pCon = NULL;
			if(sType == "TCP")
				pCon = CreateTCPConnection(strHost, nPort, oProps);
			else if (sType == "UDP")
				pCon = CreateUDPConnection(strHost, nPort, oProps);

			if(!pCon)
				return NULL;

			bool bBlocking = true;
			bool bBuffering = false;
			bool bBufferSwap = true;

			if(oProps.HasProperty("BLOCKING"))
				bBlocking = oProps.GetBoolValue("BLOCKING");

			if(oProps.HasProperty("BUFFERING"))
				bBuffering = oProps.GetBoolValue("BUFFERING");

			if(oProps.HasProperty("BYTESWAP"))
				bBufferSwap = oProps.GetBoolValue("BYTESWAP");


			pCon->SetIsBlocking(bBlocking);
			pCon->SetIsBuffering(bBuffering);
			pCon->SetByteorderSwapFlag(bBufferSwap);
			pCon->SetLingerTime( oProps.GetIntValue("LINGERTIME") );

			return pCon;
		}

	VistaConnectionIP *CreateUDPConnection(const std::string &strHost,
											int nPort,
											const VistaPropertyList &oProps)
		{
			std::string sDirection = oProps.GetStringValue("DIRECTION");
			VistaConnectionIP *pConn = NULL;
			if(VistaAspectsComparisonStuff::StringEquals(sDirection, "INCOMING", false))
			{
				pConn = new VistaConnectionIP(strHost, nPort);
			}
			else if(VistaAspectsComparisonStuff::StringEquals(sDirection, "OUTGOING", false))
			{
				pConn = new VistaConnectionIP(VistaConnectionIP::CT_UDP, strHost, nPort);
			}

			return pConn;
		}

	VistaConnectionIP *CreateTCPConnection(const std::string &strHost,
											int nPort,
											const VistaPropertyList &oProps)
		{
			return new VistaConnectionIP(VistaConnectionIP::CT_TCP, strHost, nPort);
		}

};


class VistaTimedFileConnectionFactoryMethod : public VistaConnectionConfigurator::IConnectionFactoryMethod
{
public:
	VistaConnection *CreateConnection(const VistaPropertyList &oProps)
		{
			std::string sFileName = oProps.GetStringValue("FILENAME");
			float fFrequency = 1;
			bool bPulse = false;
			bool bByteSwap = false;

			if(oProps.HasProperty("FREQUENCY"))
				fFrequency = float(oProps.GetDoubleValue("FREQUENCY"));

			if(oProps.HasProperty("PULSE"))
				bPulse = oProps.GetBoolValue("PULSE");

			if(oProps.HasProperty("BYTESWAP"))
				bByteSwap = oProps.GetBoolValue("BYTESWAP");

			std::string strDirection = oProps.GetStringValue("DIRECTION");
			if(strDirection.empty())
				return NULL;

			bool bIncoming = false;
			if(VistaAspectsComparisonStuff::StringEquals(strDirection, "INCOMING", false))
				bIncoming = true;

			VistaConnectionFileTimed *pConn = new VistaConnectionFileTimed(sFileName, bPulse, fFrequency, bIncoming);

			pConn->SetByteorderSwapFlag(bByteSwap);

			return pConn;
		}
};

class VistaFileConnectionFactoryMethod : public VistaConnectionConfigurator::IConnectionFactoryMethod
{
public:
	VistaConnection *CreateConnection(const VistaPropertyList &oProps)
		{
			std::string sFileName = oProps.GetStringValue("FILENAME");
			bool bByteSwap = false;


			if(oProps.HasProperty("BYTESWAP"))
				bByteSwap = oProps.GetBoolValue("BYTESWAP");

			std::string strDirection = oProps.GetStringValue("DIRECTION");
			if(strDirection.empty())
				return NULL;

			int mode = VistaConnectionFile::READ;
			//if(VistaAspectsComparisonStuff::StringEquals(strDirection, "INCOMING", false))
			//  mode = VistaConnectionFile::READ;
			if(VistaAspectsComparisonStuff::StringEquals(strDirection, "OUTGOING", false))
				mode = VistaConnectionFile::WRITE;
		else
				if(VistaAspectsComparisonStuff::StringEquals(strDirection, "DUPLEX", false))
					mode = VistaConnectionFile::READWRITE;

			VistaConnectionFile *pConn = new VistaConnectionFile(sFileName,mode);
			pConn->SetByteorderSwapFlag(bByteSwap);

			return pConn;
		}
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


class CWindowSetter : public IVistaPropertySetFunctor
{
public:
	CWindowSetter(VistaDisplayManager *pDispMgr,
				  const std::string &sPropName,
				  const std::string &sClassName)
		: IVistaPropertySetFunctor(sPropName, sClassName, "Attach to WINDOWS as a given string list"),
		  m_pDispMgr(pDispMgr)
		{
		}

	bool operator()(IVistaPropertyAwareable &oObj, const VistaProperty &oProp)
		{
			VistaWindowConfigurator *pConf = dynamic_cast<VistaWindowConfigurator*>(&oObj);
			std::list<VistaWindow*> liWindows;
			std::list<std::string> liWindowNames = VistaAspectsConversionStuff::ConvertToStringList(oProp.GetValue());

			for(std::list<std::string>::const_iterator cit = liWindowNames.begin();
				cit != liWindowNames.end(); ++cit)
			{
				VistaWindow *pWindow = m_pDispMgr->GetWindowByName( *cit );
				if(pWindow)
					liWindows.push_back(pWindow);
			}
			return pConf->SetWindowsByNameList( liWindows );
		}

private:
	VistaDisplayManager *m_pDispMgr;
};


// ######################################################################################


std::string VistaWindowConfigurator::SsReflectionType("VistaWindowConfigurator");

VistaWindowConfigurator::VistaWindowConfigurator(VistaDisplayManager *pDispSys)
	: m_pDispMgr(pDispSys)
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();

	if(!pReg->GetSetFunctor("WINDOWS", VistaWindowConfigurator::SsReflectionType, GetBaseTypeList()))
	{
		pReg->RegisterSetter("WINDOWS",
							 VistaWindowConfigurator::SsReflectionType,
							 new CWindowSetter(m_pDispMgr,
											   "WINDOWS",
											   VistaWindowConfigurator::SsReflectionType));

		pReg->RegisterSetter("DEFAULTWINDOW",
							 VistaWindowConfigurator::SsReflectionType,
							 new CWindowSetter(m_pDispMgr,
											   "DEFAULTWINDOW",
											   VistaWindowConfigurator::SsReflectionType));
	}
}


VistaWindowConfigurator::~VistaWindowConfigurator()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

std::list<VistaWindow*> VistaWindowConfigurator::GetWindowsNameList() const
{
	if(!m_pDriver)
	{
		VistaDriverWindowAspect *pAsp =
			dynamic_cast<VistaDriverWindowAspect*>(m_pDriver->GetAspectById(VistaDriverWindowAspect::GetAspectId()));
		if(pAsp)
		{
			std::list<VistaWindow*> liRet;
			(*pAsp).GetWindowList(liRet);
			return liRet;
		}
	}

	return std::list<VistaWindow*>();
}

bool VistaWindowConfigurator::SetWindowsByNameList(const std::list<VistaWindow*> &liAttachWindows)
{
	// set means: remove all old, set the incoming as new

	if(!m_pDriver)
		return false;

	VistaDriverWindowAspect *pAsp =
		dynamic_cast<VistaDriverWindowAspect*>(m_pDriver->GetAspectById(VistaDriverWindowAspect::GetAspectId()));
	if(!pAsp)
		return false;

	std::list<VistaWindow*> liWindows;
	pAsp->GetWindowList(liWindows);
	for(std::list<VistaWindow*>::const_iterator cit = liWindows.begin();
		cit != liWindows.end(); ++cit)
	{
		pAsp->DetachFromWindow( *cit );
	}


	for(std::list<VistaWindow*>::const_iterator atWin = liAttachWindows.begin();
		atWin != liAttachWindows.end(); ++atWin)
	{
		pAsp->AttachToWindow( *atWin );
	}
	return true;
}

bool VistaWindowConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
														   const std::string &sRootSection,
														   VistaPropertyList &oProps)
{
	VistaProfiler profile;

	std::string strTmp = profile.GetTheProfileString(sRootSection, "WINDOWS", "", strFile);
	if(strTmp.empty())
	{
		bool bDefaultWindow = profile.GetTheProfileBool(sRootSection, "DEFAULTWINDOW", true, strFile);
		if(bDefaultWindow)
		{
			const std::map<std::string, VistaWindow*> &mp = m_pDispMgr->GetWindowsConstRef();
			if(mp.empty())
				return false;
			unsigned int n=0;
			for(std::map<std::string, VistaWindow*>::const_iterator cit = mp.begin();
				cit != mp.end(); ++cit, ++n)
			{
				strTmp = strTmp + (*cit).first;
				if(n < mp.size()-1)
					strTmp = strTmp + std::string(",");
			}
		}
	}

	oProps.SetStringValue("WINDOWS", strTmp);
	return true;
}

string VistaWindowConfigurator::GetReflectionableType() const
{
	return SsReflectionType;
}

int VistaWindowConfigurator::AddToBaseTypeList(list<string> &rBtList) const
{
	int nRet = VistaDriverPropertyConfigurator::IConfigurator::AddToBaseTypeList(rBtList);
	rBtList.push_back(SsReflectionType);
	return nRet+1;
}

// ############################################################################
std::string VistaSensorMappingConfigurator::SsReflectionType("VistaSensorMappingConfigurator");

VistaSensorMappingConfigurator::VistaSensorMappingConfigurator()
	: VistaDriverPropertyConfigurator::IConfigurator()
{}

VistaSensorMappingConfigurator::~VistaSensorMappingConfigurator()
{
}

bool VistaSensorMappingConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
																  const std::string &sRootSection,
																  VistaPropertyList &oProps)
{
	VistaProfiler profile;

	// need the string to store later
	std::string strSensors = profile.GetTheProfileString(sRootSection, "SENSORS", "", strFile);

	if(strSensors.empty())
		return false;

	oProps.SetProperty(VistaProperty("SENSORS",
									  strSensors,
									  VistaProperty::PROPT_LIST,
									  VistaProperty::PROPT_STRING));

	std::string sDriverHistorySize = profile.GetTheProfileString(sRootSection, "HISTORY", "1", strFile);

	std::list<std::string> liSensorNames = VistaAspectsConversionStuff::ConvertToStringList(strSensors);
	for(std::list<std::string>::const_iterator cit = liSensorNames.begin();
		cit != liSensorNames.end(); ++cit)
	{
		// create a sub-PropertyList for every sensor in the list
		std::string strRawId = profile.GetTheProfileString( *cit, "RAWID", "", strFile );
		if(strRawId.empty())
			continue; // we do not allow a section without a rawid tag

		std::string strType = profile.GetTheProfileString( *cit, "TYPE", "", strFile );
		std::string strName = profile.GetTheProfileString( *cit, "NAME", *cit, strFile );
		// type == "" is a valid type!!

		//if(strType.empty())
		//	continue;

		std::string sSensorHistory = profile.GetTheProfileString( *cit, "HISTORY", "",strFile);
		if(sSensorHistory.empty())
		{
			if(!sDriverHistorySize.empty())
				sSensorHistory = sDriverHistorySize;
		}

		VistaPropertyList sensor;

		sensor.SetStringValue("TYPE", strType);
		sensor.SetStringValue("NAME", strName);
		// be type specific
		sensor.SetProperty(VistaProperty("RAWID", strRawId, VistaProperty::PROPT_INT));
		sensor.SetProperty(VistaProperty("HISTORY", sSensorHistory, VistaProperty::PROPT_INT));

		oProps.SetPropertyListValue( *cit, sensor );
	}

	if(oProps.size() == 1) // all sensor sections were corrupt!
	{
		oProps.clear();
		return false;
	}
	return true;
}

bool VistaSensorMappingConfigurator::Configure(IVistaDeviceDriver *pDriver,
												const VistaPropertyList &props)
{

	VistaDriverSensorMappingAspect *pSensorAsp =
		dynamic_cast<VistaDriverSensorMappingAspect*>(pDriver->GetAspectById(VistaDriverSensorMappingAspect::GetAspectId()));

	std::list<std::string> liSensors;
	props.GetStringListValue("SENSORS", liSensors);

	for(std::list<std::string>::const_iterator cit = liSensors.begin();
		cit != liSensors.end(); ++cit)
	{
		// get sensor PropertyList
		if( props.HasProperty( *cit ) )
		{
			VistaPropertyList oSensor = props.GetPropertyListValue( *cit );
			std::string sType = oSensor.GetStringValue("TYPE");
			std::string strSensorName = oSensor.GetStringValue("NAME");

			if(pSensorAsp) // driver has sensor mapping, so use this to set up the sensors...
			{
				unsigned int nSensorType = pSensorAsp->GetTypeId(sType);
				if(nSensorType == ~0)
					continue;

				int nRawId = oSensor.GetIntValue("RAWID");
				int nHistorySize = std::max<int>(2, oSensor.GetIntValue("HISTORY"));

				VistaDeviceSensor *pSensor = new VistaDeviceSensor;
				pSensor->SetTypeHint(nSensorType);
				pSensor->SetSensorName(strSensorName);

				unsigned int nDriverSensorId = pDriver->AddDeviceSensor(pSensor);
				if(nDriverSensorId != ~0)
				{
					pSensorAsp->SetSensorId(nSensorType, nRawId, nDriverSensorId);
					VistaDriverMeasureHistoryAspect *pHist =
						dynamic_cast<VistaDriverMeasureHistoryAspect*>(pDriver->GetAspectById(VistaDriverMeasureHistoryAspect::GetAspectId()));
					if(pHist)
					{
						// should already have happened during AddDeviceSensor()
						//pHist->RegisterSensor(pSensor);

						// calculation:
						// the update estimator gives the refresh rate in Hz (best-case)
						// so, for a 15 frame min update, we need:
						// rt: 1000/estimator -> msecs per sample
						// mr: 1000/15 -> msecs per frame (max. read-time) -> 66msecs (including rendering, but hey...)
						// mr/rt = # additional packets we need in order not to create
						// an overrun during write
						int nNum = int(ceilf(66.6f/float(1000.0f/float(pSensorAsp->GetUpdateRateEstimatorForType(nSensorType)))));
						// be conservative: add 2-times the required space
						pHist->SetHistorySize(pSensor,
											  nHistorySize, (2*nNum),
											  pSensorAsp->GetMeasureSizeForType(nSensorType));
					}

					// create transcoder for sensor
					IVistaMeasureTranscoderFactory *pFac = pSensorAsp->GetTranscoderFactoryForType(nSensorType);
					if(pFac)
					{
						pSensor->SetMeasureTranscode( (*pFac).CreateTranscoder() );
					}
				}
			} // no sensor mapping...
			else
			{
				// try to claim sensor by id.
				int nRawId = oSensor.GetIntValue("RAWID");
				std::string strName = oSensor.GetStringValue("NAME");

				VistaDeviceSensor *pSensor = pDriver->GetSensorByIndex(nRawId);
				if(pSensor)
				{
					// ok, this, at least worked..
					pSensor->SetSensorName(strName);
					// while here... we setup the history properly
					VistaDriverMeasureHistoryAspect *pHist =
						dynamic_cast<VistaDriverMeasureHistoryAspect*>(pDriver->GetAspectById(VistaDriverMeasureHistoryAspect::GetAspectId()));
					if(pHist)
					{
						int nCurSize = pSensor->GetMeasures().m_nClientReadSize;
						int nHistorySize = std::max<int>(nCurSize, oSensor.GetIntValue("HISTORY"));

						if(nHistorySize != nCurSize)
						{
							// calculation:
							// the update estimator gives the refresh rate in Hz (best-case)
							// so, for a 15 frame min update, we need:
							// rt: 1000/estimator -> msecs per sample
							// mr: 1000/15 -> msecs per frame (max. read-time) -> 66msecs (including rendering, but hey...)
							// mr/rt = # additional packets we need in order not to create
							// an overrun during write

							/** @todo someone should clean up the API for where to get the update estimate from */
							// is it the sensor? the driver? the factory? the history aspect?
							// not hard to decide, but should be done.
							unsigned int nUpdateEstimate = 100; //pMd->GetUpdateEstimatorFor(pSensor->GetTypeHint());
							if(nUpdateEstimate > 0)
							{
								int nNum = int(ceilf(66.6f/float(1000.0f/float(nUpdateEstimate))));
								// be conservative: add 2-times the required space
								pHist->SetHistorySize(pSensor,
													  nHistorySize, (2*nNum),
													  pSensor->GetMeasureHistorySize());
							}
						}
					}
				}
			}
		}
	}


	return true;
}

string VistaSensorMappingConfigurator::GetReflectionableType() const
{
	return VistaSensorMappingConfigurator::SsReflectionType;
}

int VistaSensorMappingConfigurator::AddToBaseTypeList(list<string> &rBtList) const
{
	int nRet = VistaDriverPropertyConfigurator::IConfigurator::AddToBaseTypeList(rBtList);
	rBtList.push_back(VistaSensorMappingConfigurator::SsReflectionType);
	return nRet + 1;
}


std::list<std::string> VistaSensorMappingConfigurator::GetSensorNameList() const
{
	return std::list<std::string>();
}

bool VistaSensorMappingConfigurator::SetSensorByNameList(const std::list<std::string> &liSensors)
{
	return false;
}

// #############################################################################

bool VistaDriverProtocolConfigurator::Configure(IVistaDeviceDriver *pDriver, const VistaPropertyList &props)
{
	IVistaDriverProtocolAspect *pProt
		= dynamic_cast<IVistaDriverProtocolAspect*>(
		pDriver->GetAspectById( IVistaDriverProtocolAspect::GetAspectId() ) );
	if(!pProt)
		return true; // optional aspect

	IVistaDriverProtocolAspect::_cVersionTag cTag(props.GetStringValue("NAME"), props.GetStringValue("REVISION"));
	if(pProt->SetProtocol( cTag ) == false)
		return false; // protocol given, but could not be set... this is an error!
	return true; // ok
}

bool VistaDriverProtocolConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
	const std::string &sRootSection,
	VistaPropertyList &oProps)
{
	VistaProfiler profile;

	std::string strProtocolSectionName = profile.GetTheProfileString( sRootSection, "PROTOCOL", "", strFile );
	if(strProtocolSectionName.empty())
		return false; // not defined, not accepted

	std::string strProtocolName;
	profile.GetTheProfileString( strProtocolSectionName, "NAME", "", strProtocolName, strFile );
	if(strProtocolName.empty())
		return false; // mandatory key
	std::string strProtocolVersion = profile.GetTheProfileString( strProtocolSectionName, "REVISION", "", strFile );

	oProps.SetStringValue( "NAME", strProtocolName );
	if(!strProtocolVersion.empty())
		oProps.SetStringValue("REVISION", strProtocolVersion);

	return true;
}

// #############################################################################

std::string VistaGenericHistoryConfigurator::SsReflectionType("VistaSensorMappingConfigurator");

VistaGenericHistoryConfigurator::VistaGenericHistoryConfigurator()
{
}

VistaGenericHistoryConfigurator::~VistaGenericHistoryConfigurator()
{
}


bool VistaGenericHistoryConfigurator::Configure(IVistaDeviceDriver *pDriver,
												 const VistaPropertyList &props)
{
	if(!props.HasProperty("HISTORY"))
		return false;

	unsigned int nHistorySize = props.GetIntValue("HISTORY");

	VistaDriverMeasureHistoryAspect *pHist =
		dynamic_cast<VistaDriverMeasureHistoryAspect*>(pDriver->GetAspectById(VistaDriverMeasureHistoryAspect::GetAspectId()));
	if(pHist && pDriver->GetSensorMeasureSize())
	{
		if(pDriver->GetNumberOfSensors() == 0)
		{
			pDriver->SetDefaultHistorySize( std::max<unsigned int>(1,nHistorySize) );
		}
		else
		{
			for(unsigned int n=0; n<pDriver->GetNumberOfSensors(); ++n)
			{
				if(pHist->GetIsRegistered(pDriver->GetSensorByIndex(n)))
				{
					pHist->SetHistorySize(pDriver->GetSensorByIndex(n),
										  std::max<unsigned int>(1,nHistorySize), 1,
										  pDriver->GetSensorMeasureSize());
				}
			}
		}
	}
	return true;
}

bool VistaGenericHistoryConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
																   const std::string &sRootSection,
																   VistaPropertyList &oProps)
{
	VistaProfiler profile;

	oProps.SetStringValue( "HISTORY", profile.GetTheProfileString(sRootSection, "HISTORY", "1", strFile ) );

	return true;
}


string VistaGenericHistoryConfigurator::GetReflectionableType() const
{
	return SsReflectionType;
}

int VistaGenericHistoryConfigurator::AddToBaseTypeList(list<string> &rBtList) const
{
	int nRet = VistaDriverPropertyConfigurator::IConfigurator::AddToBaseTypeList(rBtList);
	rBtList.push_back(SsReflectionType);
	return nRet + 1;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaConnectionConfigurator::VistaConnectionConfigurator()
	: VistaDriverPropertyConfigurator::IConfigurator()
{
	m_mpFactories["SERIAL"] = new VistaSerialConnectionFactoryMethod;
	m_mpFactories["USB"] = new VistaUSBConnectionFactoryMethod;
	m_mpFactories["UDP"] = new VistaIPConnectionFactoryMethod;
	m_mpFactories["TCP"] = new VistaIPConnectionFactoryMethod;
	m_mpFactories["TIMEFILE"] = new VistaTimedFileConnectionFactoryMethod;
	m_mpFactories["FILE"] = new VistaFileConnectionFactoryMethod;
}

VistaConnectionConfigurator::~VistaConnectionConfigurator()
{
	for(std::map<std::string, IConnectionFactoryMethod*>::iterator it = m_mpFactories.begin();
		it != m_mpFactories.end(); ++it)
		delete (*it).second;
}

bool VistaConnectionConfigurator::Configure(IVistaDeviceDriver *pDriver,
											 const VistaPropertyList &props)
{
	VistaDriverConnectionAspect *pConAsp =
		dynamic_cast<VistaDriverConnectionAspect*>(pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));

	if(pConAsp)
	{
		std::list<std::string> liConnections;
		props.GetStringListValue("CONNECTIONS", liConnections);
		for(std::list<std::string>::const_iterator cit = liConnections.begin();
			cit != liConnections.end(); ++cit)
		{
			VistaConnection *pCon= NULL;
			VistaPropertyList oCon = props.GetPropertyListValue( *cit );
			// determine con-settings (type)
			std::string sType = oCon("TYPE").GetValue();
			if(sType.empty())
				continue;

			std::string sDriverRole = oCon("DRIVERROLE").GetValue();
			if(sDriverRole.empty())
				continue;

			unsigned int nRoleId = pConAsp->GetIndexForRole(sDriverRole);
			if(nRoleId == ~0)
				continue;

			const_iterator fcIt = m_mpFactories.find(sType);
			if(fcIt == m_mpFactories.end())
				continue;

			pCon = (*fcIt).second->CreateConnection(oCon);
			if(pCon == NULL)
			{
				// FAILED!
				return false;
			}
			else
				pConAsp->SetConnection(nRoleId, pCon, sDriverRole, false);
		}
		return true;
	}
	else
	{
		vkernerr << "trying to configure connections for driver that "
				  << "does not have a connection aspect. -- FAIL.\n";
	}

	return false;
}


bool VistaConnectionConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
															   const std::string &sRootSection,
															   VistaPropertyList &oProps)
{
	VistaProfiler profile;
	std::string strConnections = profile.GetTheProfileString( sRootSection,  "CONNECTIONS", "", strFile );
	if(strConnections.empty())
		return false;

	oProps.SetStringValue("CONNECTIONS", strConnections);
	std::list<std::string> liConnections = VistaAspectsConversionStuff::ConvertToStringList(strConnections);

	for(std::list<std::string>::const_iterator cit = liConnections.begin();
		cit != liConnections.end(); ++cit)
	{
		// read off complete sections for connection configuration
		VistaPropertyList oCon;
		std::list<std::string> liKeys;
		profile.GetTheProfileSectionEntries( *cit , liKeys, strFile );
		for(std::list<std::string>::const_iterator keys = liKeys.begin();
			keys != liKeys.end(); ++keys)
		{
			oCon.SetStringValue( *keys, profile.GetTheProfileString( *cit, *keys, "", strFile ) );
		}

		oProps.SetPropertyListValue( *cit, oCon );
	}

	return true;
}

VistaConnectionConfigurator::const_iterator VistaConnectionConfigurator::begin() const
{
	return m_mpFactories.begin();
}

VistaConnectionConfigurator::iterator       VistaConnectionConfigurator::begin()
{
	return m_mpFactories.begin();
}

VistaConnectionConfigurator::const_iterator VistaConnectionConfigurator::end() const
{
	return m_mpFactories.end();
}

VistaConnectionConfigurator::iterator       VistaConnectionConfigurator::end()
{
	return m_mpFactories.end();
}

bool VistaConnectionConfigurator::AddConFacMethod(const std::string &sKey, IConnectionFactoryMethod*pMt)
{
	if(m_mpFactories.find(sKey) != m_mpFactories.end())
		return false;

	m_mpFactories[sKey] = pMt;
	return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDriverLoggingConfigurator::VistaDriverLoggingConfigurator()
{
}

VistaDriverLoggingConfigurator::~VistaDriverLoggingConfigurator()
{
}

bool VistaDriverLoggingConfigurator::Configure(IVistaDeviceDriver *pDriver,
												const VistaPropertyList &props)
{
	/** @todo maybe a lookup structure would be better and more dynamic */
	VistaDriverLoggingAspect *pAsp = NULL;
	if(props.GetStringValue("TYPE") == "DEFAULT")
	{
		// check for old logging aspect
		IVistaDeviceDriver::IVistaDeviceDriverAspect *pOld
			= pDriver->GetAspectById(VistaDriverLoggingAspect::GetAspectId());
		if(pOld)
		{
			pDriver->UnregisterAspect(pOld, false); // remove, but do not delete, maybe a leak?
		}

		pDriver->RegisterAspect((pAsp = new VistaDriverLoggingAspect));
	}

	if(pAsp)
	{
		if(props.HasProperty("ENABLED"))
			pAsp->SetEnabled(props.GetBoolValue("ENABLED"));

		if(props.HasProperty("LOGLEVEL"))
			pAsp->SetLogLevel(props.GetIntValue("LOGLEVEL"));
		if(props.HasProperty("SYMBOL"))
			pAsp->SetMnemonic(props.GetStringValue("SYMBOL"));

	}
	return (pAsp != NULL);
}

bool VistaDriverLoggingConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
																  const std::string &sRootSection,
																  VistaPropertyList &oProps)
{
	VistaProfiler profile;

	std::string strLogSection = profile.GetTheProfileString(sRootSection, "DRIVERLOGGING", "", strFile);
	if(strLogSection.empty())
		return false;

	std::list<std::string> liEntries;

	profile.GetTheProfileSectionEntries(strLogSection, liEntries, strFile);

	for(std::list<std::string>::const_iterator cit = liEntries.begin();
		cit != liEntries.end(); ++cit)
	{
		std::string strTmp = profile.GetTheProfileString(strLogSection, *cit, "", strFile);
		if(!strTmp.empty())
		{
			oProps.SetStringValue( *cit , strTmp );
		}
	}

	return oProps.HasProperty("TYPE");
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDriverAttachOnlyConfigurator::VistaDriverAttachOnlyConfigurator()
{
}

VistaDriverAttachOnlyConfigurator::~VistaDriverAttachOnlyConfigurator()
{
}

bool VistaDriverAttachOnlyConfigurator::Configure(IVistaDeviceDriver *pDriver,
												const VistaPropertyList &props)
{
	/** @todo maybe a lookup structure would be better and more dynamic */
	pDriver->SetAttachOnly(props.GetBoolValue("ATTACHONLY"));
	return true;
}

bool VistaDriverAttachOnlyConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
																  const std::string &sRootSection,
																  VistaPropertyList &oProps)
{
	VistaProfiler profile;

	std::string strAttachOnly = profile.GetTheProfileString(sRootSection, "ATTACHONLY", "", strFile);
	if(strAttachOnly.empty())
		return true; // not given is ok


	oProps.SetProperty( VistaProperty( "ATTACHONLY", strAttachOnly, VistaProperty::PROPT_BOOL ) );


	return true;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDeviceIdentificationConfigurator::VistaDeviceIdentificationConfigurator()
{
}

VistaDeviceIdentificationConfigurator::~VistaDeviceIdentificationConfigurator()
{
}

bool VistaDeviceIdentificationConfigurator::Configure(IVistaDeviceDriver *pDriver,
												const VistaPropertyList &props)
{
	VistaDeviceIdentificationAspect *pAsp = NULL;

	pAsp = static_cast<VistaDeviceIdentificationAspect*>(pDriver->GetAspectById(VistaDeviceIdentificationAspect::GetAspectId()));
	if( !pAsp ) 
	{
		vkernerr << "IDENTIFICATION configurator given for a driver without DeviceIdentificationAspect, creating the aspect." << std::endl;
		pDriver->RegisterAspect((pAsp = new VistaDeviceIdentificationAspect));
	}
	
	if(props.HasProperty("VENDORID"))
		pAsp->SetVendorID(props.GetIntValue("VENDORID"));
	if(props.HasProperty("PRODUCTID"))
		pAsp->SetProductID(props.GetIntValue("PRODUCTID"));
	if(props.HasProperty("SERIALNUMBER"))
		pAsp->SetSerialNumber(props.GetIntValue("SERIALNUMBER"));
	if(props.HasProperty("DEVICENAME"))
		pAsp->SetDeviceName(props.GetStringValue("DEVICENAME"));

	return (true);
}

bool VistaDeviceIdentificationConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
																  const std::string &sRootSection,
																  VistaPropertyList &oProps)
{
	VistaProfiler profile;

	std::string strIdentSection = profile.GetTheProfileString(sRootSection, "IDENTIFICATION", "", strFile);
	if(strIdentSection.empty())
		return false; // if the key is given, a section must be referenced

	std::list<std::string> liEntries;

	profile.GetTheProfileSectionEntries(strIdentSection, liEntries, strFile);

	for(std::list<std::string>::const_iterator cit = liEntries.begin();
		cit != liEntries.end(); ++cit)
	{
		std::string strTmp = profile.GetTheProfileString(strIdentSection, *cit, "", strFile);
		if(!strTmp.empty())
		{
			oProps.SetStringValue( *cit , strTmp );
		}
	}

	return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VistaDriverReferenceFrameConfigurator::VistaDriverReferenceFrameConfigurator()
{
}

VistaDriverReferenceFrameConfigurator::~VistaDriverReferenceFrameConfigurator()
{
}


	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################

bool VistaDriverReferenceFrameConfigurator::Configure(IVistaDeviceDriver *pDriver,
			   const VistaPropertyList &props)
{
	IVistaDriverReferenceFrameAspect* pAspect
				= dynamic_cast<IVistaDriverReferenceFrameAspect*>( 
					pDriver->GetAspectById( 
							IVistaDriverReferenceFrameAspect::GetAspectId()) );

	if( pAspect == false )
		return false;

	if( props.HasProperty( "HEMISPHERE" ) )
	{
		std::string sHemisphere = props.GetStringValue( "HEMISPHERE" );
		IVistaDriverReferenceFrameAspect::eHemisphereCode iHemisphere 
						= IVistaDriverReferenceFrameAspect::eHemisphereCode(-1);
		if( sHemisphere == "UP" )
			iHemisphere = IVistaDriverReferenceFrameAspect::HS_UP;
		else if( sHemisphere == "DOWN" )
			iHemisphere =IVistaDriverReferenceFrameAspect::HS_DOWN;
		else if( sHemisphere == "LEFT" )
			iHemisphere = IVistaDriverReferenceFrameAspect::HS_LEFT;
		else if( sHemisphere == "RIGHT" )
			iHemisphere = IVistaDriverReferenceFrameAspect::HS_RIGHT;
		else if( sHemisphere == "FRONT" )
			iHemisphere = IVistaDriverReferenceFrameAspect::HS_FRONT;
		else if( sHemisphere == "AFT" || sHemisphere == "BACK" )
			iHemisphere = IVistaDriverReferenceFrameAspect::HS_AFT;
		if( iHemisphere != -1 )
		{
			if( pAspect->SetHemisphere( iHemisphere ) == false )
			{
				vkernout << "[ReferenceFrameConfigurator]: SetHemisphere with parameter ["
						<< sHemisphere << "] failed! Maybe its not supported by the driver"
						<< std::endl;
			}
		}
		else
		{
			vkernout << "[ReferenceFrameConfigurator]: Hemisphere parameter ["
					<< sHemisphere << "] is invalid!" << std::endl;
		}
	}

	std::list<std::string> liStringList;
	if( props.HasProperty( "EMITTER_ALIGNMENT" ) )
	{
		props.GetStringListValue( "EMITTER_ALIGNMENT", liStringList );
		if( liStringList.size() == 3 )
		{
			VistaEulerAngles eulAngles( 0, 0, 0 );
			std::list<std::string>::const_iterator itValue = liStringList.begin();

			eulAngles.a = VistaAspectsConversionStuff::ConvertToFloat( (*itValue++) );
			eulAngles.b = VistaAspectsConversionStuff::ConvertToFloat( (*itValue++) );
			eulAngles.c = VistaAspectsConversionStuff::ConvertToFloat( (*itValue) );
			
			if( pAspect->SetEmitterAlignment( eulAngles ) == false )
			{
				vkernout << "[ReferenceFrameConfigurator]: SetEmitterAlignment( angles )"
					<< " returned false. Maybe it is not supported by the driver?" << std::endl;
			}
		}
		else if( liStringList.size() == 16 )
		{
			VistaTransformMatrix matTransform;
			std::list<std::string>::const_iterator itValue = liStringList.begin();
			for( int i = 0; i < 4; ++i )
			{
				for( int j = 0; j < 4; ++j )
				{
					matTransform[i][j] = VistaAspectsConversionStuff::ConvertToFloat( (*itValue) );
					++itValue;
				}
			}
			if( pAspect->SetEmitterAlignment( matTransform ) == false )
			{
				vkernout << "[ReferenceFrameConfigurator]: SetEmitterAlignment( matrix )"
					<< " returned false. Maybe it is not supported by the driver?" << std::endl;
			}
		}
		else
		{
			vkernout << "[ReferenceFrameConfigurator]: Emitter Alignment value has"
					<< " invalid length. Use either angles (3 floats) or matrix (16 floats)."
					<< std::endl;
		}
	}


	std::list<std::string> liSensorSections;
	props.GetStringListValue( "SENSOR_ALIGNMENT", liSensorSections );
	for( std::list<std::string>::const_iterator cit = liSensorSections.begin();
			cit != liSensorSections.end(); ++cit )
	{
		VistaPropertyList oSubProps = props.GetPropertyListValue( (*cit) );

		if( oSubProps.HasProperty( "SENSOR_ID" )
			&& oSubProps.HasProperty( "TRANSFORM" ) )
		{
			int iSensorID = oSubProps.GetIntValue( "SENSOR_ID" );

			VistaTransformMatrix matTransform;
			liStringList.clear();
			oSubProps.GetStringListValue( "TRANSFORM", liStringList );
			if( liStringList.size() == 16 )
			{
				std::list<std::string>::const_iterator itValue = liStringList.begin();
				for( int i = 0; i < 4; ++i )
				{
					for( int j = 0; j < 4; ++j )
					{
						matTransform[i][j] = VistaAspectsConversionStuff::ConvertToFloat( (*itValue) );
						++itValue;
					}
				}
			}
			else
			{
				vkernout << "[ReferenceFrameConfigurator]: Sensor Alignment Section ["
					<< (*cit) << "] has a TRANSFORM entry not representing a matrix!"
					<< std::endl;
			}

			if( pAspect->SetSensorAlignment( matTransform, iSensorID ) == false )
			{
				vkernout << "[ReferenceFrameConfigurator]: SetSensorAlignment for SensorID ["
					<< iSensorID << "from Section [" << (*cit) << "] returned false!"
					<< " Maybe the ID is wrong, or sensor alignment is not supported by the driver."
					<< std::endl;
			}
			
		}
		else
		{
			vkernout << "[ReferenceFrameConfigurator]: Sensor Alignment Section ["
					<< (*cit) << "] must contain entries for SENSOR_ID and ALIGNMENT!"
					<< std::endl;
		}
	}

	
	return true;
}


bool VistaDriverReferenceFrameConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
								 const std::string &sRootSection,
								 VistaPropertyList& oProps)
{
	VistaProfiler profile;

	std::string strIdentSection = profile.GetTheProfileString(
						sRootSection, "REFERENCE_FRAME", "", strFile );
	if( strIdentSection.empty() )
		return false;

	std::list<std::string> liEntries;

	profile.GetTheProfileSectionEntries(strIdentSection, liEntries, strFile);

	std::list<std::string> liSensorEntries;

	for( std::list<std::string>::const_iterator cit = liEntries.begin();
		cit != liEntries.end(); ++cit )
	{
		std::string strTmp = profile.GetTheProfileString(strIdentSection, *cit, "", strFile);
		if(!strTmp.empty())
		{
			oProps.SetStringValue( *cit , strTmp );
			if( (*cit) == "SENSOR_ALIGNMENT" )
				oProps.GetStringListValue( (*cit), liSensorEntries );
		}		
	}

	for( std::list<std::string>::const_iterator cit = liSensorEntries.begin();
			cit != liSensorEntries.end(); ++cit )
	{
		VistaPropertyList oSubProps;
		liEntries.clear();
		profile.GetTheProfileSectionEntries( (*cit), liEntries, strFile );

		for(std::list<std::string>::const_iterator citEntry = liEntries.begin();
			citEntry != liEntries.end(); ++citEntry)
		{
			std::string strTmp = profile.GetTheProfileString(
						(*cit), (*citEntry), "", strFile );
			if(!strTmp.empty())
			{
				oSubProps.SetStringValue( (*citEntry) , strTmp );
			}
		}
		oProps.SetPropertyListValue( (*cit), oSubProps );
	}

	return true;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VistaDriverParameterConfigurator::VistaDriverParameterConfigurator()
{
}

VistaDriverParameterConfigurator::~VistaDriverParameterConfigurator()
{
}



bool VistaDriverParameterConfigurator::Configure(IVistaDeviceDriver *pDriver,
               const VistaPropertyList &props)
{
	VistaDriverGenericParameterAspect* pParamAspect 
			= dynamic_cast<VistaDriverGenericParameterAspect*>(
					pDriver->GetAspectById( VistaDriverGenericParameterAspect::GetAspectId() ) );
	
	if( pParamAspect == NULL )
		return false;
#ifdef DEBUG
	for( VistaPropertyList::const_iterator itProp = props.begin();
			itProp != props.end(); ++itProp )
	{
		if( pParamAspect->GetParameterContainer()->SetProperty( (*itProp).second ) == false )
		{
			vkernout << "[DriverParameterConfigurator]: Could not set parameter ["
				<< (*itProp).first <<"] - not known by driver" << std::endl;
		}
	}
#else
	pParamAspect->GetParameterContainer()->SetPropertiesByList( props );	
#endif

    return true;
}


bool VistaDriverParameterConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
                                 const std::string &sRootSection,
                                 VistaPropertyList& oProps,
                                 const std::list<std::string> &liSearchpath )
{
    VistaProfiler profile;

	std::string strParamSection = profile.GetTheProfileString( sRootSection, "PARAMETERS", "", strFile );
	if( strParamSection.empty() )
		return false; // if the key is given, a section must be referenced

	std::list<std::string> liEntries;

	profile.GetTheProfileSectionEntries( strParamSection, liEntries, strFile );

	for(std::list<std::string>::const_iterator cit = liEntries.begin();
		cit != liEntries.end(); ++cit)
	{
		std::string strTmp = profile.GetTheProfileString(strParamSection, *cit, "", strFile);
		if(!strTmp.empty())
		{
			oProps.SetStringValue( *cit , strTmp );
		}
	}

	return true;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//// #############################################################################
//// SENSOR TRANSFORMS
//// #############################################################################
//
//IVistaSensorTransform *VistaIdentityTransformCreationMethod::CreateTransform()
//{
//	return new VistaIdentityTransform;
//}
//
//IVistaSensorTransform *VistaSensorFrameTransformCreationMethod::CreateTransform()
//{
//	return new VistaSensorFrameTransform;
//}
//
//IVistaSensorTransform *VistaDisjunctTransformCreationMethod::CreateTransform()
//{
//	return new VistaDisjunctTransform;
//}
//
//
//VistaSensorFrameTransformConfigurator::VistaSensorFrameTransformConfigurator()
//{}
//
//VistaSensorFrameTransformConfigurator::~VistaSensorFrameTransformConfigurator()
//{}
//
//bool VistaSensorFrameTransformConfigurator::Configure(IVistaSensorTransform *pTrans,
//													   const PropertyList &props)
//{
//	VistaSensorFrameTransform *pSF = dynamic_cast<VistaSensorFrameTransform *>(pTrans);
//	if(!pSF)
//		return false;
//
//	float nScale = float(props.GetDoubleValue("SCALE"));
//	VistaVector3D trans;
//	VistaQuaternion ori, lcOri;
//
//	trans = IConfigurator::ConvertToVistaVector3D(props.GetStringValue("TOORIGIN"));
//	ori = IConfigurator::ConvertToVistaQuaternion(props.GetStringValue("GLOBALORI"));
//	lcOri = IConfigurator::ConvertToVistaQuaternion(props.GetStringValue("LOCALORI"));
//
//
//	pSF->SetLocalSensorOrientation(lcOri);
//	pSF->SetOrientationTransform(ori);
//	pSF->SetPositionTranslation(trans);
//	pSF->SetTransformScale(nScale);
//
//	return true;
//}
//
//
//bool VistaSensorFrameTransformConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
//																		 const std::string &sRootSection,
//																		 PropertyList &oProps,
//																		 const std::list<std::string> &liSearchpath)
//{
//	VistaProfiler profile(liSearchpath);
//
//	oProps.SetStringValue( "SCALE", profile.GetTheProfileString(sRootSection, "SCALE", "1.0", strFile) );
//	oProps.SetStringValue( "TOORIGIN", profile.GetTheProfileString(sRootSection, "TOORIGIN", "0,0,0", strFile) );
//	oProps.SetStringValue( "GLOBALORI", profile.GetTheProfileString(sRootSection, "GLOBALORI", "0,0,0,0", strFile) );
//	oProps.SetStringValue( "LOCALORI", profile.GetTheProfileString(sRootSection, "LOCALORI", "0,0,0,0", strFile) );
//
//	return true;
//}
//
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//VistaDisjunctTransformConfigurator::VistaDisjunctTransformConfigurator(VistaSensorTransformConfigurator *pConfigure)
//	: m_pTransConfig(pConfigure)
//{
//}
//
//VistaDisjunctTransformConfigurator::~VistaDisjunctTransformConfigurator()
//{
//}
//
//IVistaSensorTransform *VistaDisjunctTransformConfigurator::CreateSensorTransform(
//	const std::string &strType,
//	const PropertyList &oProps)
//{
//	VistaSensorTransformConfigurator::ITransformCreationMethod *pMd
//		= m_pTransConfig->RetrieveTransformCreator(strType);
//	IVistaSensorTransform *pRet = NULL;
//	if(pMd)
//	{
//		pRet = (*pMd).CreateTransform();
//		if(pRet)
//		{
//			VistaSensorTransformConfigurator::IConfigurator *pConfig
//				= m_pTransConfig->RetrieveConfigurator(strType);
//			if(pConfig)
//			{
//				if(!pConfig->Configure(pRet, oProps))
//				{
//					delete pRet;
//					return NULL; // config registered and FAILED, delete and return NULL
//				}
//			} // no config registered, pass transform AS-IS
//		}
//	}
//	return pRet;
//}
//
//bool VistaDisjunctTransformConfigurator::Configure(IVistaSensorTransform *pTrans,
//													const PropertyList &props)
//{
//	VistaDisjunctTransform *pDT = dynamic_cast<VistaDisjunctTransform*>(pTrans);
//	if(!pDT)
//		return false;
//
//	std::string sCoordProp = props.GetStringValue("COORD");
//	if(!sCoordProp.empty() && props.HasProperty(sCoordProp))
//	{
//		PropertyList pr = props.GetPropPropertyListValue(sCoordProp);
//		IVistaSensorTransform *pST = CreateSensorTransform(props.GetStringValue("COORDTYPE"), pr);
//		IVistaCoordTransform *pCT = dynamic_cast<IVistaCoordTransform*>(pST);
//		if(pCT)
//			pDT->SetCoordTransform(pCT);
//		else
//			delete pST;
//	}
//
//
//	std::string sCmdProp = props.GetStringValue("COMMAND");
//	if(!sCmdProp.empty() && props.HasProperty(sCmdProp))
//	{
//		PropertyList pr = props.GetPropPropertyListValue(sCmdProp);
//		IVistaSensorTransform *pST = CreateSensorTransform(props.GetStringValue("COMMANDTYPE"), pr);
//		IVistaCommandTransform *pCT = dynamic_cast<IVistaCommandTransform*>(pST);
//		if(pCT)
//			pDT->SetCommandTransform(pCT);
//		else
//		{
//			delete pST;
//			props.PrintPropertyList();
//		}
//	}
//
//	return true;
//}
//
//
//bool VistaDisjunctTransformConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
//																	  const std::string &sRootSection,
//																	  PropertyList &oProps,
//																	  const std::list<std::string> &liSearchpath)
//{
//	VistaProfiler profile(liSearchpath);
//
//	std::string sCoordSection = profile.GetTheProfileString(sRootSection, "COORD", "", strFile);
//	std::string sCmdSection = profile.GetTheProfileString(sRootSection, "COMMAND", "", strFile);
//
//	if(!sCoordSection.empty())
//	{
//		oProps.SetStringValue("COORD", sCoordSection);
//
//		std::string sCoordType = profile.GetTheProfileString( sCoordSection, "TYPE", "", strFile);
//		oProps.SetStringValue("COORDTYPE", sCoordType);
//
//		VistaSensorTransformConfigurator::IConfigurator *pConfig = m_pTransConfig->RetrieveConfigurator(sCoordType);
//		if(pConfig)
//		{
//			// create PropertyList according to type information
//			PropertyList oSubProps;
//			pConfig->GeneratePropertyListFromInifile(strFile, sCoordSection, oSubProps, liSearchpath);
//			oProps.SetPropPropertyListValue(sCoordSection, oSubProps);
//		}
//		else
//		{
//			// push empty PropertyList
//			PropertyList o;
//			o.SetStringValue("TYPE", sCoordType);
//			oProps.SetPropPropertyListValue(sCoordSection, o);
//		}
//	}
//
//	if(!sCmdSection.empty())
//	{
//		oProps.SetStringValue("COMMAND", sCmdSection);
//
//		std::string sComdType = profile.GetTheProfileString( sCmdSection, "TYPE", "", strFile);
//		oProps.SetStringValue("COMMANDTYPE", sComdType);
//
//		VistaSensorTransformConfigurator::IConfigurator *pConfig = m_pTransConfig->RetrieveConfigurator(sComdType);
//		if(pConfig)
//		{
//			// create PropertyList according to type information
//			PropertyList oSubProps;
//			pConfig->GeneratePropertyListFromInifile(strFile, sCmdSection, oSubProps, liSearchpath);
//			oProps.SetPropPropertyListValue(sCmdSection, oSubProps);
//		}
//		else
//		{
//			PropertyList o;
//			o.SetStringValue("TYPE", sComdType);
//			oProps.SetPropPropertyListValue(sCmdSection, o);
//		}
//
//	}
//	return true;
//}
//
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//VistaNormalizeTransformConfigurator::VistaNormalizeTransformConfigurator(VistaDisplayManager *pDispMgr)
//	: m_pDispMgr(pDispMgr)
//{
//
//}
//
//VistaNormalizeTransformConfigurator::~VistaNormalizeTransformConfigurator()
//{
//}
//
//
//// #########################################################################
//// ICONFIGURATOR API
//// #########################################################################
//
//bool VistaNormalizeTransformConfigurator::Configure(IVistaSensorTransform *pTrans,
//													 const PropertyList &props)
//{
//	VistaMouseNormalize *pNT = dynamic_cast<VistaMouseNormalize*>(pTrans);
//	if(!pNT)
//		return false;
//
//	VistaWindow *pWindow = m_pDispMgr->GetWindowByName(props.GetStringValue("WINDOW"));
//	if(!pWindow)
//		return false;
//
//	pNT->SetWindow(pWindow);
//
//	return true;
//}
//
//
//
//bool VistaNormalizeTransformConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
//																	   const std::string &sRootSection,
//																	   PropertyList &oProps,
//																	   const std::list<std::string> &liSearchpath)
//{
//	VistaProfiler profile(liSearchpath);
//
//	std::string strWindow = profile.GetTheProfileString( sRootSection, "WINDOW", "", strFile);
//	if(strWindow.empty())
//		return false;
//
//	oProps.SetStringValue("WINDOW", strWindow);
//	return true;
//}
//
//
//VistaMouseNormalizeCreationMethod::VistaMouseNormalizeCreationMethod(VistaEventManager *pEvMgr)
//	: m_pEvMgr(pEvMgr)
//{
//}
//
//IVistaSensorTransform *VistaMouseNormalizeCreationMethod::CreateTransform()
//{
//	return new VistaMouseNormalize(m_pEvMgr, NULL);
//}
//
//
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//Vista3DMouseTransformConfigurator::Vista3DMouseTransformConfigurator(VistaDisplayManager *pDispMgr)
//	: m_pDispMgr(pDispMgr)
//{
//
//}
//
//Vista3DMouseTransformConfigurator::~Vista3DMouseTransformConfigurator()
//{
//}
//
//
//// #########################################################################
//// ICONFIGURATOR API
//// #########################################################################
//
//bool Vista3DMouseTransformConfigurator::Configure(IVistaSensorTransform *pTrans,
//												   const PropertyList &props)
//{
//	Vista3DMouseTransform *pNT = dynamic_cast<Vista3DMouseTransform*>(pTrans);
//	if(!pNT)
//		return false;
//
//	VistaWindow *pWindow = m_pDispMgr->GetWindowByName(props.GetStringValue("WINDOW"));
//	if(!pWindow)
//		return false;
//
//	pNT->SetWindow(pWindow);
//
//	VistaDisplaySystem *pSys = m_pDispMgr->GetDisplaySystemByName( props.GetStringValue( "DISPLAYSYSTEM" ) );
//
//	if(pSys)
//	{
//		VistaVirtualPlatform *pPf = pSys->GetReferenceFrame();
//		pNT->SetReferenceFrame(pPf);
//	}
//	return true;
//}
//
//
//
//bool Vista3DMouseTransformConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
//																	 const std::string &sRootSection,
//																	 PropertyList &oProps,
//																	 const std::list<std::string> &liSearchpath)
//{
//	VistaProfiler profile(liSearchpath);
//
//	std::string strWindow = profile.GetTheProfileString( sRootSection, "WINDOW", "", strFile);
//	if(strWindow.empty())
//		return false;
//
//	oProps.SetStringValue("WINDOW", strWindow);
//
//	std::string strDispSys = profile.GetTheProfileString(sRootSection, "DISPLAYSYSTEM", "", strFile );
//	if(!strDispSys.empty())
//		oProps.SetStringValue("DISPLAYSYSTEM", strDispSys);
//
//	return true;
//}
//
//Vista3DMouseTransformCreationMethod::Vista3DMouseTransformCreationMethod(VistaEventManager *pEvMgr)
//	: m_pEvMgr(pEvMgr)
//{
//}
//
//IVistaSensorTransform *Vista3DMouseTransformCreationMethod::CreateTransform()
//{
//	return new Vista3DMouseTransform(m_pEvMgr, NULL);
//}
//
//// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//IVistaSensorTransform *VistaCoordIdentityTransformCreationMethod::CreateTransform()
//{
//	return new VistaIdentityCoordTransform;
//}
//
//
//IVistaSensorTransform *VistaCommandIdentityTransformCreationMethod::CreateTransform()
//{
//	return new VistaCommandIdentityTransform;
//}
//
//
//// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//bool VistaGenericCommandMapTransformConfigurator::Configure(IVistaSensorTransform *pTrans,
//															 const PropertyList &props)
//{
//	VistaGenericCommandMapTransform *pGeT = dynamic_cast<VistaGenericCommandMapTransform*>(pTrans);
//	if(!pTrans)
//		return false;
//
//	for(PropertyList::const_iterator cit = props.begin();
//		cit != props.end(); ++cit)
//	{
//		int nMap = VistaAspectsConversionStuff::ConvertToInt( (*cit).first );
//		int nMapTo = VistaAspectsConversionStuff::ConvertToInt( (*cit).second.GetValue() );
//		(*pGeT).Map(nMap, nMapTo);
//	}
//	return true;
//}
//
//
//bool VistaGenericCommandMapTransformConfigurator::GeneratePropertyListFromInifile(const std::string &strFile,
//																			   const std::string &sRootSection,
//																			   PropertyList &oProps,
//																			   const std::list<std::string> &liSearchpath)
//{
//	VistaProfiler profile(liSearchpath);
//
//	std::list<std::string> liEntries;
//	profile.GetTheProfileSectionEntries(sRootSection, liEntries, strFile);
//
//	for(std::list<std::string>::const_iterator cit = liEntries.begin();
//		cit != liEntries.end(); ++cit)
//	{
//		char *endptr = NULL;
//		const char *begptr = (*cit).c_str();
//		int nMap = strtol( (*cit).c_str(), &endptr, 10 );
//		if(nMap == 0 && ( begptr == endptr) )
//			continue; // no conversion
//
//		std::string sValue = profile.GetTheProfileString( sRootSection, *cit, "", strFile );
//		oProps.SetStringValue( *cit, sValue );
//	}
//
//	return true;
//}
//
//
//
//IVistaSensorTransform *VistaGenericCommandMapTransformCreationMethod::CreateTransform()
//{
//	return new VistaGenericCommandMapTransform;
//}
//
//
//
//// ################################################################################
//
//VistaPipedTransformConfig::VistaPipedTransformConfig(VistaSensorTransformConfigurator *pConfigure)
//	: m_pTransConfig(pConfigure)
//{}
//
//
//IVistaSensorTransform *VistaPipedTransformConfig::CreateSensorTransform(const std::string &strType,
//																		 const PropertyList &oProps)
//{
//	VistaSensorTransformConfigurator::ITransformCreationMethod *pMd
//		= m_pTransConfig->RetrieveTransformCreator(strType);
//	IVistaSensorTransform *pRet = NULL;
//	if(pMd)
//	{
//		pRet = (*pMd).CreateTransform();
//		if(pRet)
//		{
//			VistaSensorTransformConfigurator::IConfigurator *pConfig
//				= m_pTransConfig->RetrieveConfigurator(strType);
//			if(pConfig)
//			{
//				if(!pConfig->Configure(pRet, oProps))
//				{
//					delete pRet;
//					return NULL; // config registered and FAILED, delete and return NULL
//				}
//			} // no config registered, pass transform AS-IS
//		}
//	}
//	return pRet;
//}
//
//bool VistaPipedTransformConfig::Configure(IVistaSensorTransform *pTrans, const PropertyList &oProps)
//{
//	oProps.PrintPropertyList();
//
//	VistaPipedTransform *pTr = dynamic_cast<VistaPipedTransform*>(pTrans);
//	if(!pTr)
//		return false;
//
//	std::string sCoordProp = oProps.GetStringValue("FIRST");
//	if(!sCoordProp.empty() && oProps.HasProperty(sCoordProp))
//	{
//		PropertyList pr = oProps.GetPropPropertyListValue(sCoordProp);
//		IVistaSensorTransform *pST = CreateSensorTransform(oProps.GetStringValue("FIRSTTYPE"), pr);
//		if(pST)
//			pTr->SetFirst(pST);
//		else
//			delete pST;
//	}
//
//
//	std::string sCmdProp = oProps.GetStringValue("SECOND");
//	if(!sCmdProp.empty() && oProps.HasProperty(sCmdProp))
//	{
//		PropertyList pr = oProps.GetPropPropertyListValue(sCmdProp);
//		IVistaSensorTransform *pST = CreateSensorTransform(oProps.GetStringValue("SECONDTYPE"), pr);
//		if(pST)
//			pTr->SetSecond(pST);
//		else
//		{
//			delete pST;
//			oProps.PrintPropertyList();
//		}
//	}
//
//	return true;
//}
//
//bool VistaPipedTransformConfig::GeneratePropertyListFromInifile( const std::string &strFile,
//															  const std::string &strRootSection,
//															  PropertyList &oStorage,
//															  const std::list<std::string> &liSearchPath)
//{
//	std::cout << "root = " << strRootSection << "; file = " << strFile << std::endl;
//	VistaProfiler profile(liSearchPath);
//
//	std::string strFirst = profile.GetTheProfileString(strRootSection, "FIRST", "", strFile);
//	std::string strSecond = profile.GetTheProfileString(strRootSection, "SECOND", "", strFile);
//
//	if(strFirst.empty() || strSecond.empty())
//		return false;
//
//	oStorage.SetStringValue("FIRST", strFirst);
//	oStorage.SetStringValue("SECOND", strSecond);
//
//	std::string sFirstType = profile.GetTheProfileString( strFirst, "TYPE", "", strFile);
//	oStorage.SetStringValue("FIRSTTYPE", sFirstType);
//
//	VistaSensorTransformConfigurator::IConfigurator *pConfig = m_pTransConfig->RetrieveConfigurator(sFirstType);
//	if(pConfig)
//	{
//		// create PropertyList according to type information
//		PropertyList oSubProps;
//		pConfig->GeneratePropertyListFromInifile(strFile, strFirst, oSubProps, liSearchPath);
//		oStorage.SetPropPropertyListValue(strFirst, oSubProps);
//	}
//	else
//	{
//		// push empty PropertyList
//		PropertyList o;
//		o.SetStringValue("TYPE", sFirstType);
//		oStorage.SetPropPropertyListValue(strFirst, o);
//	}
//
//	std::string sSecondType = profile.GetTheProfileString( strSecond, "TYPE", "", strFile);
//	oStorage.SetStringValue("SECONDTYPE", sSecondType);
//
//	pConfig = m_pTransConfig->RetrieveConfigurator(sSecondType);
//	if(pConfig)
//	{
//		// create PropertyList according to type information
//		PropertyList oSubProps;
//		pConfig->GeneratePropertyListFromInifile(strFile, strSecond, oSubProps, liSearchPath);
//		oStorage.SetPropPropertyListValue(strSecond, oSubProps);
//	}
//	else
//	{
//		// push empty PropertyList
//		PropertyList o;
//		o.SetStringValue("TYPE", sSecondType);
//		oStorage.SetPropPropertyListValue(strSecond, o);
//	}
//	return true;
//}
//
//IVistaSensorTransform *VistaPipedTransformCreate::CreateTransform()
//{
//	return new VistaPipedTransform;
//}
//
//
//// #############################################################################
//VistaRefFrameTransformConfig::VistaRefFrameTransformConfig(VistaDisplayManager *pSys)
//	: m_pSys(pSys) {}
//
//
//bool VistaRefFrameTransformConfig::Configure(IVistaSensorTransform *pTrans, const PropertyList &oProps)
//{
//	oProps.PrintPropertyList();
//
//	VistaRefFrameCoordTransform *pTr = dynamic_cast<VistaRefFrameCoordTransform*>(pTrans);
//	if(!pTr)
//		return false;
//
//	std::string sRefFrame = oProps.GetStringValue("REFFRAME");
//	std::string sMode = oProps.GetStringValue("MODE");
//
//	VistaRefFrameCoordTransform::eMode nMode = VistaRefFrameCoordTransform::MODE_NONE;
//
//	if(VistaAspectsComparisonStuff::StringEquals(sMode, "TOFRAME", false))
//		nMode = VistaRefFrameCoordTransform::TO_FRAME;
//	else if(VistaAspectsComparisonStuff::StringEquals(sMode, "FROMFRAME", false))
//		nMode = VistaRefFrameCoordTransform::FROM_FRAME;
//
//	VistaDisplaySystem *pSys = NULL;
//	if(sRefFrame.empty())
//		pSys = m_pSys->GetDisplaySystem(0);
//	else
//		pSys = m_pSys->GetDisplaySystemByName(sRefFrame);
//
//
//	if(!pSys)
//		return false;
//
//	VistaVirtualPlatform *pVp = pSys->GetReferenceFrame();
//
//	pTr->SetVirtualPlatform(pVp);
//	pTr->SetMode(nMode);
//
//	return true;
//}
//
//bool VistaRefFrameTransformConfig::GeneratePropertyListFromInifile( const std::string &strFile,
//																 const std::string &strRootSection,
//																 PropertyList &oStorage,
//																 const std::list<std::string> &liSearchPath)
//{
//	std::cout << "root = " << strRootSection << "; file = " << strFile << std::endl;
//	VistaProfiler profile(liSearchPath);
//
//	std::string sRefFrame = profile.GetTheProfileString(strRootSection, "REFFRAME", "", strFile);
//	std::string sMode = profile.GetTheProfileString(strRootSection, "MODE", "", strFile);
//
//	if(sMode.empty())
//		return false;
//
//	oStorage.SetStringValue("REFRAME", sRefFrame);
//	oStorage.SetStringValue("MODE", sMode);
//
//	return true;
//}
//
//IVistaSensorTransform *VistaRefFrameTransformCreate::CreateTransform()
//{
//	return new VistaRefFrameCoordTransform;
//}
//
//
//// #############################################################################
//
//VistaSlotValueMultiplexTransformConfig::VistaSlotValueMultiplexTransformConfig()
//{
//}
//
//
//bool VistaSlotValueMultiplexTransformConfig::Configure(IVistaSensorTransform *pTrans,
//														const PropertyList &oProps)
//{
//	VistaSlotValueMultiplexTransform *pSlv = dynamic_cast<VistaSlotValueMultiplexTransform*>(pTrans);
//
//	if(pSlv == NULL)
//		return false;
//
//	unsigned int nInSlot = (unsigned int)oProps.GetIntValue( "INSLOT" );
//	unsigned int nOutSlot = (unsigned int)oProps.GetIntValue("OUTSLOT");
//
//	pSlv->SetInSlot(nInSlot);
//	pSlv->SetOutSlot(nOutSlot);
//
//	PropertyList oMapping = oProps.GetPropPropertyListValue( "MAPPING" );
//	for(PropertyList::const_iterator cit = oMapping.begin(); cit != oMapping.end(); ++cit)
//	{
//		double dInValue = VistaAspectsConversionStuff::ConvertToDouble((*cit).second.GetNameForNameable());
//
//		// the next one is a list of two strings
//		std::list<double> liMap;
//		liMap = VistaAspectsConversionStuff::ConvertToDoubleList( (*cit).second.GetValue() );
//		if(liMap.size() == 2)
//		{
//			std::list<double>::const_iterator num = liMap.begin();
//
//			double dValue = *(num++);
//			unsigned int nIdx = (unsigned int) *num;
//
//			pSlv->Map( dInValue, dValue, nIdx );
//		}
//		else if(liMap.size() == 1)
//		{
//			std::list<double>::const_iterator num = liMap.begin();
//			pSlv->Map( dInValue, *num );
//		}
//	}
//	return true;
//}
//
//
//bool VistaSlotValueMultiplexTransformConfig::GeneratePropertyListFromInifile( const std::string &strFile,
//								  const std::string &strRootSection,
//								  PropertyList &oStorage,
//								  const std::list<std::string> &liSearchPath)
//{
//	VistaProfiler profile(liSearchPath);
//
//	std::string sInSlot = profile.GetTheProfileString(strRootSection, "INSLOT", "", strFile);
//	std::string sOutSlot = profile.GetTheProfileString(strRootSection, "OUTSLOT", "", strFile );
//
//	if( sInSlot.empty() || sOutSlot.empty() )
//		return false; // needed!
//
//	oStorage.SetStringValue( "INSLOT", sInSlot );
//	oStorage.SetStringValue( "OUTSLOT", sOutSlot );
//
//	// build slot mapping
//	std::list<std::string> liEntries;
//	PropertyList oMap;
//
//	profile.GetTheProfileSectionEntries(strRootSection, liEntries, strFile);
//	for(std::list<std::string>::const_iterator cit = liEntries.begin();
//		cit != liEntries.end(); ++cit)
//	{
//		char *endptr = NULL;
//		const char *begptr = (*cit).c_str();
//		int nMap = strtol( (*cit).c_str(), &endptr, 10 );
//		if(nMap == 0 && ( begptr == endptr) )
//			continue; // no conversion
//
//		std::string sValue = profile.GetTheProfileString( strRootSection, *cit, "", strFile );
//		oMap.SetStringValue( *cit, sValue );
//	}
//
//	oStorage.SetPropPropertyListValue( "MAPPING", oMap );
//
//	return true;
//}
//
//
//IVistaSensorTransform *VistaSlotValueMultiplexTransformCreate::CreateTransform()
//{
//	return new VistaSlotValueMultiplexTransform;
//}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


