/*============================================================================*/
/*                    ViSTA VR toolkit - Jsw Joystick driver                  */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id$

#include "VistaJswJoystickDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaAspects/VistaReferenceCountable.h>
#include <stdio.h>
#include <cstring>
#include <VistaMath/VistaBoundingBox.h>
#include <VistaAspects/VistaAspectsUtils.h>

#include <string.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaJswJoystickDriverCreationMethod::VistaJswJoystickDriverCreationMethod(IVistaTranscoderFactoryFactory *fac)
: IVistaDriverCreationMethod(fac)
{
	RegisterSensorType( "",
		sizeof(VistaJswJoystickDriver::_sJoyMeasure),
		100,
		fac->CreateFactoryForType("") );
}


IVistaDeviceDriver *VistaJswJoystickDriverCreationMethod::CreateDriver()
{
	return new VistaJswJoystickDriver(this);
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaJswJoystickDriver::VistaJswJoystickDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm)
, m_bOpened(false)
, m_pWorkspace( new VistaDriverWorkspaceAspect )
, m_pProtocol( new IVistaDriverProtocolAspect )
, m_pInfo(new VistaDriverInfoAspect )
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);

	memset(&m_jsd, 0, sizeof(js_data_struct));
	memset(&m_nState, 0, sizeof(_sJoyMeasure));
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetMeasureTranscode( crm->GetTranscoderFactoryForSensor("")->CreateTranscoder() );
	pSensor->SetTypeHint( crm->GetTypeFor("") );
	AddDeviceSensor(pSensor);

	RegisterAspect( m_pWorkspace );
	RegisterAspect( m_pProtocol );
	RegisterAspect( m_pInfo );
}

VistaJswJoystickDriver::~VistaJswJoystickDriver()
{
	if(m_bOpened)
		JSClose(&m_jsd);

	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder( pSensor->GetMeasureTranscode() );

	RemDeviceSensor(pSensor);
	delete pSensor;

	UnregisterAspect( m_pWorkspace, false );
	delete m_pWorkspace;

	UnregisterAspect( m_pProtocol, false );
	delete m_pProtocol;

	UnregisterAspect( m_pInfo, false );
	delete m_pInfo;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaJswJoystickDriver::Connect()
{
	int status = 0;

	const char *device = JSDefaultDevice;
	const char *calib  = JSDefaultCalibration;

	IVistaDriverProtocolAspect::_cVersionTag tag;
	if( m_pProtocol->GetProtocol(tag) )
	{
		if(!tag.m_strProtocolName.empty())
			device = tag.m_strProtocolName.c_str();
		if(!tag.m_strProtocolRevision.empty())
			calib  = tag.m_strProtocolRevision.c_str();
	}

	status = JSInit(
		&m_jsd,
		device,
		calib,
		JSFlagNonBlocking );

	if(status != JSSuccess)
	{
	    JSClose(&m_jsd);
		return false;
	}

	VistaPropertyList &oWrite = m_pInfo->GetInfoPropsWrite();
	oWrite.SetStringValue("NAME", (m_jsd.name ? std::string(m_jsd.name) : "<none>"));
	oWrite.SetStringValue("DEVICENAME", (m_jsd.device_name ? std::string(m_jsd.device_name) : "<none>"));
	oWrite.SetStringValue("CALIBRATIONFILE", (m_jsd.calibration_file ? std::string(m_jsd.calibration_file) : "<none>"));

	for(unsigned int n=0; n < 16; ++n)
	{
		if(JSIsAxisAllocated(&m_jsd, n))
		{
			// ok, we have that axis
			js_axis_struct *axis_ptr = m_jsd.axis[n];
			float nMin[3], nMax[3];
			nMin[0] = nMin[1] = nMin[2] = 0.0f;
			nMax[0] = nMax[1] = nMax[2] = 0.0f;

			nMin[0] = axis_ptr->min;
			nMax[0] = axis_ptr->max;


			m_pWorkspace->SetWorkspace("AXIS_"+VistaAspectsConversionStuff::ConvertToString(n), VistaBoundingBox(nMin, nMax));

			nMin[0] = axis_ptr->dz_min;
			nMax[0] = axis_ptr->dz_max;

			m_pWorkspace->SetWorkspace("DEADZONE_"+VistaAspectsConversionStuff::ConvertToString(n), VistaBoundingBox(nMin, nMax));

			nMin[0] = axis_ptr->corr_coeff_min1;
			nMax[0] = axis_ptr->corr_coeff_max1;

			m_pWorkspace->SetWorkspace("CORR_COEFF1_"+VistaAspectsConversionStuff::ConvertToString(n), VistaBoundingBox(nMin, nMax));

			nMin[0] = axis_ptr->corr_coeff_min2;
			nMax[0] = axis_ptr->corr_coeff_max2;

			m_pWorkspace->SetWorkspace("CORR_COEFF2_"+VistaAspectsConversionStuff::ConvertToString(n), VistaBoundingBox(nMin, nMax));
		}
	}


	m_bOpened = true;

	return true;
}


bool VistaJswJoystickDriver::DoSensorUpdate(VistaType::microtime nTs)
{
	if(JSUpdate(&m_jsd) == JSGotEvent)
	{
		_sJoyMeasure nState;

		for(unsigned int n=0; n < 16; ++n)
		{
			if(JSIsAxisAllocated(&m_jsd, n))
			{
				// ok, we have that axis
				js_axis_struct *axis_ptr = m_jsd.axis[n];
				nState.m_nAxesTs[n]        = axis_ptr->time;
				nState.m_nAxes[n]          = axis_ptr->cur;
			}
			else
			{
				nState.m_nAxesTs[n] = 0;
				nState.m_nAxes[n] = 0;
			}
		}

		for(unsigned int b=0; b < 32; ++b)
		{
			if(JSIsButtonAllocated(&m_jsd, b))
			{
				js_button_struct *button_ptr = m_jsd.button[b];
				nState.m_nButtons[b]   = (button_ptr->state ? true : false);
				nState.m_nButtonsTs[b] = button_ptr->time;
			}
			else
			{
				nState.m_nButtons[b] = false;
				nState.m_nButtonsTs[b] = 0;
			}

		}

		VistaDeviceSensor *pSensor = GetSensorByIndex(0);

		VistaSensorMeasure *pM = MeasureStart( 0, nTs, true );
		std::memcpy( (*pM).getWrite<void>(), &nState, sizeof(_sJoyMeasure) );
		MeasureStop( 0 );
		pSensor->SetUpdateTimeStamp(nTs);

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
