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

#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/VistaDriverInfoAspect.h>
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

class VistaJswJoystickTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(VistaJswJoystickTranscoder, IVistaMeasureTranscode);
public:
	VistaJswJoystickTranscoder()
	{
		// inherited as protected member
		m_nNumberOfScalars = 17;
	}

	  static std::string GetTypeString() { return "VistaJswJoystickTranscoder"; }
};

class VistaJswJoystickTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaJswJoystickTranscoder;
	}
};

//	void _ll_printError(status)
//	{
//		/* Error occured opening joystick? */
//		if(status != JSSuccess)
//		{
//			/* There was an error opening the joystick, print by the
//			 * Received error code from JSInit().
//			 */
//			switch(status)
//			{
//			  case JSBadValue:
//			fprintf(
//				stderr,
//				"%s: Invalid value encountered while initializing joystick.\n",
//				device
//			);
//			break;
//
//			  case JSNoAccess:
//			fprintf(
//				stderr,
//				"%s: Cannot access resources, check permissions and file locations.\n",
//						device
//					);
//			break;
//
//			  case JSNoBuffers:
//					fprintf(
//				stderr,
//				"%s: Insufficient memory to initialize.\n",
//						device
//					);
//			break;
//
//			  default:	/* JSError */
//					fprintf(
//				stderr,
//				"%s: Error occured while initializing.\n",
//						device
//					);
//			break;
//			}
//
//			/* Print some helpful advice. */
//			fprintf(
//			stderr,
//				"Make sure that:\n\
//					1. Your joystick is connected (and turned on as needed).\n\
//					2. Your joystick modules are loaded (`modprobe <driver>').\n\
//					3. Your joystick needs to be calibrated (use `jscalibrator').\n\
//				\n"
//				);
//		}
//	}


// TRANSCODERS
class VistaAxisTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
	public:
		VistaAxisTranscode( const std::string &strAxis,
										  const std::string &strHelp,
										  unsigned int nAxis)
	: IVistaMeasureTranscode::TTranscodeValueGet<int>(strAxis,
									VistaJswJoystickTranscoder::GetTypeString(),
									 strHelp),
									 m_nAxis(nAxis)
	{
	}

	virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
		if(m_nAxis < 16)
			return m->m_nAxes[m_nAxis];
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nValue) const
	{
		nValue = GetValue(pMeasure);
		return true;
	}

	private:
		unsigned int m_nAxis;
};

class VistaButtonMaskTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
	public:
		VistaButtonMaskTranscode()
	: IVistaMeasureTranscode::TTranscodeValueGet<int>("BUTTONMASK",
									VistaJswJoystickTranscoder::GetTypeString(),
									 "button mask as int (32bit)")
	{

	}

	virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
		int nMask=0;
		for(unsigned int n=0; n < 32; ++n)
		{
			if(m->m_nButtons[n] == true)
				nMask += 1 << n;

//			if(m->m_nButtons[n] == true)
//				std::cout << "n="<< n << " : " <<  (m->m_nButtons[n] ? "true":"false")<< std::endl;
		}
		return nMask;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nValue) const
	{
		nValue = GetValue(pMeasure);
		return true;
	}

	private:
		unsigned int m_nAxis;
};


class VistaJoystickAxisScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaJoystickAxisScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("AXIS",
				VistaJswJoystickTranscoder::GetTypeString(),
								"axis scalar get (0-16)") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex >= 16)
			return false;

		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
		dScalar = m->m_nAxes[nIndex];
		return true;
	}
};

class VistaJoystickAxisTsScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaJoystickAxisTsScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("AXIS_TS",
				VistaJswJoystickTranscoder::GetTypeString(),
								"axis timestamp scalar get (0-16)") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex >= 16)
			return false;

		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
		dScalar = double(m->m_nAxesTs[nIndex]);
		return true;
	}
};

class VistaJoystickButtonScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaJoystickButtonScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("BUTTON",
				VistaJswJoystickTranscoder::GetTypeString(),
								"button scalar get (0-32)") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex >= 32)
			return false;

		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
		dScalar = double(m->m_nButtons[nIndex]);
		return true;
	}
};

class VistaJoystickButtonTsScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaJoystickButtonTsScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("BUTTON_TS",
				VistaJswJoystickTranscoder::GetTypeString(),
								"button timestamp scalar get (0-32)") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex >= 32)
			return false;

		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
		dScalar = double(m->m_nButtonsTs[nIndex]);
		return true;
	}
};


class VistaJoystickAxisVecGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaJoystickAxisVecGet( const std::string &strAxis,
							  const std::string &strDesc,
							  unsigned int nFirst, unsigned int nSecond, unsigned int nThird = ~0 )
	: IVistaMeasureTranscode::CV3Get( strAxis, VistaJswJoystickTranscoder::GetTypeString(), strDesc ),
	  m_nFirst(nFirst), m_nSecond(nSecond), m_nThird(nThird)
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaJswJoystickDriver::_sJoyMeasure *m
			= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];

		VistaVector3D vec;

		if(m_nFirst != ~0 && m_nFirst < 16)
			vec[0] = m->m_nAxes[m_nFirst];

		if(m_nSecond != ~0 && m_nSecond < 16)
			vec[1] = m->m_nAxes[m_nSecond];

		if(m_nThird != ~0 && m_nThird < 16)
			vec[2] = m->m_nAxes[m_nThird];


		return vec;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Value) const
	{
		v3Value = GetValue(pMeasure);
		return true;
	}

private:
	unsigned int m_nFirst, m_nSecond, m_nThird;
};

static IVistaPropertyGetFunctor *SaGetter[] =
{
	new VistaJoystickAxisScalarTranscode,
	new VistaJoystickAxisTsScalarTranscode,
	new VistaJoystickButtonScalarTranscode,
	new VistaJoystickButtonTsScalarTranscode,
	new VistaButtonMaskTranscode,
	new VistaJoystickAxisVecGet("AXIS_1", "axis 1/2 as vec3d", 0, 1),
	new VistaJoystickAxisVecGet("AXIS_2", "axis 3/4 as vec3d", 2, 3),
	new VistaJoystickAxisVecGet("AXIS_3", "axis 5/6 as vec3d", 4, 5),
	NULL
};


namespace
{
	class VistaJswJoystickDriverCreationMethod : public IVistaDriverCreationMethod
	{
	public:
		virtual IVistaDeviceDriver *operator()()
		{
			return new VistaJswJoystickDriver;
		}
	};

	VistaJswJoystickDriverCreationMethod *g_SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaJswJoystickDriver::GetDriverFactoryMethod()
{
	if( g_SpFactory == NULL )
	{
		g_SpFactory = new VistaJswJoystickDriverCreationMethod;

		// register one unnamed sensor type
		// todo: better estimate for update frequency!
		g_SpFactory->RegisterSensorType( "",
										 sizeof(VistaJswJoystickDriver::_sJoyMeasure),
										 100, new VistaJswJoystickTranscodeFactory,
										 VistaJswJoystickTranscoder::GetTypeString() );
	}
	return g_SpFactory;

}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaJswJoystickDriver::VistaJswJoystickDriver()
: IVistaDeviceDriver(),
  m_bOpened(false),
  m_pWorkspace( new VistaDriverWorkspaceAspect ),
  m_pProtocol( new IVistaDriverProtocolAspect ),
  m_pInfo(new VistaDriverInfoAspect )
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);

	memset(&m_jsd, 0, sizeof(js_data_struct));
	memset(&m_nState, 0, sizeof(_sJoyMeasure));
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetMeasureTranscode( new VistaJswJoystickTranscoder );
	AddDeviceSensor(pSensor);

	RegisterAspect( m_pWorkspace );
	RegisterAspect( m_pProtocol );
	RegisterAspect( m_pInfo );
}

VistaJswJoystickDriver::~VistaJswJoystickDriver()
{
	if(m_bOpened)
		JSClose(&m_jsd);

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

	oWrite.PrintPropertyList();


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


unsigned int VistaJswJoystickDriver::GetSensorMeasureSize() const
{
	return sizeof(_sJoyMeasure);
}


bool VistaJswJoystickDriver::DoSensorUpdate(microtime nTs)
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

		MeasureStart( 0, nTs );
		VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);
		std::memcpy( &(*pM).m_vecMeasures[0], &nState, sizeof(_sJoyMeasure) );
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

