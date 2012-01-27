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

#include <DTrack.hpp>

#include "VistaDTrackSDKDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>

/*VistaDTrackSDKParameterContainerCreate : 
	public VistaDriverGenericParameterAspect::IContainerCreate
{
public:
	IParameterContainer *CreateContainer();
	bool DeleteContainer( IParameterContainer *pContainer );
};
*/
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaDTrackSDKDriver::VistaDTrackSDKDriver(IVistaDriverCreationMethod *crm)
	: IVistaDeviceDriver(crm)
	, m_pDriver(NULL)
	, m_pThread(NULL)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( GetFactory()->GetTypeFor("BODY") );
	pSensor->SetSensorName("BODY0");
	AddDeviceSensor( pSensor );
	//pSensor->SetMeasureTranscode( new VistaDTrackSDKBodyTranscode );
	pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("BODY")->CreateTranscoder());

	pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( GetFactory()->GetTypeFor("MARKER") );
	AddDeviceSensor( pSensor );
	pSensor->SetSensorName("MARKER0");
	//pSensor->SetMeasureTranscode( new VistaDTrackSDKMarkerTranscode );
	pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("MARKER")->CreateTranscoder());

//	m_pParameters = new VistaGenericParameterAspect

	m_pThread = new VistaDriverThreadAspect(this);
	RegisterAspect(m_pThread);
}

VistaDTrackSDKDriver::~VistaDTrackSDKDriver()
{
	UnregisterAspect(m_pThread,false);
	delete m_pThread;

	VistaDeviceSensor *pBodySensor = GetSensorByIndex(0);
	VistaDeviceSensor *pMarkerSensor = GetSensorByIndex(1);

	RemDeviceSensor( pBodySensor );
	RemDeviceSensor( pMarkerSensor );
	delete pBodySensor;
	delete pMarkerSensor;

	delete m_pDriver;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaDTrackSDKDriver::PhysicalEnable(bool bEnabled)
{
	if(!m_pDriver)
		return true;

	if( m_pDriver->cmd_cameras(bEnabled) )
		return true;

	m_pDriver->udperror(); // print error
	return false;
}

bool VistaDTrackSDKDriver::Connect()
{
	if(m_pDriver)
		return false; // do not connect on state connected

	m_pDriver = new DTrack(5000, "dtrack", 5001);

#ifdef DEBUG
	std::cout << "[VistaDTrackSDKDriver] Connect(): instantiated driver." 
			  << std::endl;
#endif
	
	if(m_pDriver->valid())
	{
#ifdef DEBUG
		std::cout << "[VistaDTrackSDKDriver] DTrack driver is valid, i.e. UDP socket could be created." 
				  << std::endl;
#endif
		return true;
	}
	else
		m_pDriver->udperror();

	std::cerr << "[VistaDTrackSDKDriver] failed to create UDP socket to tracker, aborting Connect()!" 
			  << std::endl;

	delete m_pDriver;
	m_pDriver = NULL; // user may try re-connect

	return false;
}

bool VistaDTrackSDKDriver::UpdateBodySensor( VistaType::microtime dTs, int nIdx )
{
	VistaDeviceSensor *pBodySensor = GetSensorByIndex(0);
	MeasureStart( nIdx, dTs );
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pBodySensor);

	if(pM == NULL)
	{
		MeasureStop(0);
		return false;
	}

	dtrack_body_type *m = (dtrack_body_type*)&(*pM).m_vecMeasures[0];

	*m = m_pDriver->get_body(nIdx);

	MeasureStop(nIdx);

	pBodySensor->SetUpdateTimeStamp( dTs );

	return true;
}

bool VistaDTrackSDKDriver::UpdateMarker( VistaType::microtime dTs, int nIdx )
{
	VistaDeviceSensor *pMarkerSensor = GetSensorByIndex(1);
	MeasureStart( nIdx, dTs );
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pMarkerSensor);

	if(pM == NULL)
	{
		MeasureStop(1);
		return false;
	}

	dtrack_body_type *m = (dtrack_body_type*)&(*pM).m_vecMeasures[0];
	*m = m_pDriver->get_body(nIdx);


	MeasureStop(nIdx);

	pMarkerSensor->SetUpdateTimeStamp( dTs );

	return true;
}


bool VistaDTrackSDKDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	if(m_pDriver->receive())
	{
		UpdateBodySensor(dTs, 0);
		UpdateMarker(dTs, 1);
	}
	else
	{
		m_pDriver->udperror();
		m_pDriver->parseerror();
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

