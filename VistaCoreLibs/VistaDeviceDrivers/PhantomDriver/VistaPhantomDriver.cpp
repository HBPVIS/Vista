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

#include "VistaPhantomDriver.h"

#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceIdentificationAspect.h>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaReferenceCountable.h>


#include <HD/hd.h>
#include <HD/hdDevice.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaPhantomDriverFactory : public IVistaDriverCreationMethod
{
public:
		virtual IVistaDeviceDriver *operator()()
		{
			return new VistaPhantomDriver;
		}
};

class VistaPhantomDriverMeasureTranscode : public IVistaMeasureTranscode
{
public:
	VistaPhantomDriverMeasureTranscode()
	{
		// inherited as protected member
		m_nNumberOfScalars = 13;
	}

	virtual ~VistaPhantomDriverMeasureTranscode() {}
	static std::string GetTypeString() { return "VistaPhantomDriverMeasureTranscode"; }
	REFL_INLINEIMP(VistaPhantomDriverMeasureTranscode, IVistaMeasureTranscode);
};

class VistaPhantomPosTranscode : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaPhantomPosTranscode()
		: IVistaMeasureTranscode::CV3Get("POSITION",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
								"phantom 3D position values") {}

	virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaPhantomDriver::_sPhantomMeasure *m =
			(VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

		return VistaVector3D(m->m_afPosition[0],
			m->m_afPosition[1],
			m->m_afPosition[2]);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
	{
		v3Pos = GetValue(pMeasure);
		return true;
	}
};


class VistaPhantomVelocityTranscode : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaPhantomVelocityTranscode()
		: IVistaMeasureTranscode::CV3Get("VELOCITY",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
								"phantom 3D position surface contact point values") {}

	virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

		return VistaVector3D(m->m_afVelocity[0],
			m->m_afVelocity[1],
			m->m_afVelocity[2]);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
	{
		v3Pos = GetValue(pMeasure);
		return true;
	}
};

class VistaPhantomForceReadTranscode : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaPhantomForceReadTranscode()
		: IVistaMeasureTranscode::CV3Get("FORCE",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
								"phantom 3D force values") {}

	virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

		return VistaVector3D(m->m_afForce[0],
			m->m_afForce[1],
			m->m_afForce[2]);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
	{
		v3Pos = GetValue(pMeasure);
		return true;
	}
};

class VistaPhantomScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaPhantomScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("DSCALAR",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
								"Phantom scalar values") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure)
			return false;

		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

		switch(nIndex)
		{
		case 0:
		case 1:
			dScalar = (m->m_nButtonState == nIndex+1) ? 1.0 : 0.0;
			break;
		case 2:
			{
				float dVal = m->m_afOverheatState[0];

				for(int i=1; i<6; i++)
					if(dVal < m->m_afOverheatState[i])
						dVal = m->m_afOverheatState[i];
				dScalar = dVal;
				break;
			}
		case 3:
			{
				dScalar = m->m_afForce[0];
				break;
			}
		case 4:
			{
				dScalar = m->m_afForce[1];
				break;
			}
		case 5:
			{
				dScalar = m->m_afForce[2];
				break;
			}
		case 6:
			{
				dScalar = m->m_afVelocity[0];
				break;
			}
		case 7:
			{
				dScalar = m->m_afVelocity[1];
				break;
			}
		case 8:
			{
				dScalar = m->m_afVelocity[2];
				break;
			}
		case 12:
			dScalar = m->m_nUpdateRate;
			break;

		default:
			return false;
		}
		return true;
	}
};

class VistaPhantomRotationGet : public IVistaMeasureTranscode::CQuatGet
{
public:
	VistaPhantomRotationGet()
		: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
		"phantom body orientation") {}

	VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaQuaternion q;
		GetValue(pMeasure, q);
		return q;
	}

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaQuaternion &qQuat) const
	{
		if(!pMeasure)
			return false;

		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
		VistaTransformMatrix t (
			float(m->m_afRotMatrix[0]), float(m->m_afRotMatrix[1]), float(m->m_afRotMatrix[2]), 0,
			float(m->m_afRotMatrix[4]), float(m->m_afRotMatrix[5]), float(m->m_afRotMatrix[6]), 0,
			float(m->m_afRotMatrix[8]), float(m->m_afRotMatrix[9]), float(m->m_afRotMatrix[10]), 0,
									 0,                          0,                          0, 1);

		qQuat = -VistaQuaternion(t);
		return true;
	}
};

class VistaPhantomVecFieldGet : public IVistaMeasureTranscode::CV3Get
{
public:
	enum eField
	{
		FLD_ANG_VEL=0,
		FLD_JOINT_ANG,
		FLG_GIMBAL_ANG,
		FLD_TORQUE
	};


	VistaPhantomVecFieldGet(eField eFld,
		const std::string &strName, const std::string &strDesc)
		: IVistaMeasureTranscode::CV3Get(strName,
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
		strDesc),
		m_eFld(eFld)
	{}

	VistaVector3D GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaVector3D v3;
		GetValue(pMeasure, v3);
		return v3;
	}

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaVector3D &v3) const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

		switch( m_eFld )
		{
		case FLD_ANG_VEL:
			{
				v3[0] = m->m_afAngularVelocity[0];
				v3[1] = m->m_afAngularVelocity[1];
				v3[2] = m->m_afAngularVelocity[2];
				return true;
			}
		case FLD_JOINT_ANG:
			{
				v3[0] = m->m_afJointAngles[0];
				v3[1] = m->m_afJointAngles[1];
				v3[2] = m->m_afJointAngles[2];
				return true;
			}
		case FLG_GIMBAL_ANG:
			{
				v3[0] = m->m_afGimbalAngles[0];
				v3[1] = m->m_afGimbalAngles[1];
				v3[2] = m->m_afGimbalAngles[2];
				return true;
			}
		case FLD_TORQUE:
			{
				v3[0] = m->m_afTorque[0];
				v3[1] = m->m_afTorque[1];
				v3[2] = m->m_afTorque[2];
				return true;
			}
		default:
			break;
		}
		return false;
	}
private:
	eField m_eFld;
};


template<class T>
class VistaPhantomVectorFloatGet : public IVistaMeasureTranscode::TTranscodeValueGet< typename std::vector< T > >
{
public:
	enum eField
	{
		FLD_OVERHEATSTATE = 0,
		FLD_ENCODER_VALUES,
		FLD_ROTMATRIX
	};

	VistaPhantomVectorFloatGet( eField eFld,
		const std::string &strName, const std::string &strDesc )
		: IVistaMeasureTranscode::TTranscodeValueGet< std::vector< T > >(strName,
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
		strDesc),
		m_eFld(eFld)
	{
	}

	std::vector<T> GetValue(const VistaSensorMeasure *pMeasure) const
	{
		std::vector<T> v3;
		GetValue(pMeasure, v3);
		return v3;
	}

	bool GetValue(const VistaSensorMeasure * pMeasure, std::vector<T> &v3) const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
		switch(m_eFld)
		{
		case FLD_OVERHEATSTATE:
			{
				v3.resize(6);
				for(typename std::vector<T>::size_type n = 0; n<6; ++n)
					v3[n] = m->m_afOverheatState[n];
				return true;
			}
		case FLD_ENCODER_VALUES:
			{
				v3.resize(6);
				for(typename std::vector<T>::size_type n = 0; n<6; ++n)
					v3[n] = T(m->m_nEncoderValues[n]);
				return true;
			}
		case FLD_ROTMATRIX:
			{
				v3.resize(16);
				for(typename std::vector<T>::size_type n = 0; n<16; ++n)
					v3[n] = m->m_afRotMatrix[n];
				return true;
			}
		default:
			break;
		}

		return false;
	}
private:
	eField m_eFld;

};

class VistaPhantomBtGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
{
public:
	enum eField
	{
		FLD_BT1=0,
		FLD_BT2,
		FLD_INKSWITCH
	};

	VistaPhantomBtGet( eField eFld,
		const std::string &strName, const std::string &strDesc )
		: IVistaMeasureTranscode::TTranscodeValueGet<bool>( strName,
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			strDesc ),
			m_eFld(eFld)
	{
	}

	bool GetValue(const VistaSensorMeasure *pMeasure) const
	{
		bool bRet;
		if(GetValue(pMeasure, bRet))
			return bRet;

		return false;
	}

	bool GetValue(const VistaSensorMeasure * pMeasure, bool &v3) const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
		switch(m_eFld)
		{
		case FLD_BT1:
		case FLD_BT2:
			{
				v3 = ((m->m_nButtonState & int(m_eFld)+1) ? true : false);
				return true;
			}
		case FLD_INKSWITCH:
			{
				v3 = !m->m_bInkwellSwitch;
				return true;
			}
		default:
			break;
		}

		return false;
	}

private:
	eField m_eFld;

};

class VistaPhantomTransformationGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
{
public:
	VistaPhantomTransformationGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "TRANSFORMATION",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
		"the complete transformation as a vista transform matrix" )
	{
	}


	VistaTransformMatrix GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaTransformMatrix m;
		GetValue(pMeasure, m);
		return m;
	}

	bool GetValue(const VistaSensorMeasure *pMeasure, VistaTransformMatrix &mt) const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
		VistaTransformMatrix mt1(m->m_afRotMatrix);
		mt1.GetTranspose(mt); // phantom reports in column major, vista needs row-major
		return true;
	}
};

class VistaPhantomButtonMaskGet : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
public:
	VistaPhantomButtonMaskGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<int>( "BUTTONMASK",
		VistaPhantomDriverMeasureTranscode::GetTypeString(),
		"the buttonmask as an int" )
	{
	}

	int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		VistaPhantomDriver::_sPhantomMeasure *m = (VistaPhantomDriver::_sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
		return m->m_nButtonState;
	}

	bool GetValue( const VistaSensorMeasure *pMeasure, int &value ) const
	{
		value = GetValue(pMeasure);
		return true;
	}
};

static IVistaPropertyGetFunctor *SapGetter[] =
{
	new VistaPhantomPosTranscode,
	new VistaPhantomForceReadTranscode,
	new VistaPhantomScalarTranscode,
	new VistaPhantomVelocityTranscode,
	new VistaPhantomRotationGet,
	new VistaPhantomTransformationGet,
	new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLD_ANG_VEL, "ANGULAR_VELOCITY",
	                               "the angular velocity as reported by the phantom"),
	new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLD_JOINT_ANG, "JOINT_ANGLE",
	                               "the joint angle of the gimbal as reported by the phantom"),
	new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLG_GIMBAL_ANG, "GIMBAL_ANGLE",
	                               "the gimbal angle as reported by the phantom"),
	new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLD_TORQUE, "TORQUE",
	                               "the torque as reported by the phantom"),

    new VistaPhantomVectorFloatGet<float>( VistaPhantomVectorFloatGet<float>::FLD_OVERHEATSTATE,
								"OVERHEATSTATE", "the overheat state vector (6-float)"),
    new VistaPhantomVectorFloatGet<float>( VistaPhantomVectorFloatGet<float>::FLD_OVERHEATSTATE,
								"VROTMATRIX", "the rotation matrix as vector of (16-float)"),
    new VistaPhantomVectorFloatGet<float>( VistaPhantomVectorFloatGet<float>::FLD_ENCODER_VALUES,
								"ENCODER_VALUES", "the raw encoder value vector (6-long given as floats)"),
	new VistaPhantomBtGet( VistaPhantomBtGet::FLD_BT1, "BUTTON_1", "state of button 1 (front)" ),
	new VistaPhantomBtGet( VistaPhantomBtGet::FLD_BT2, "BUTTON_2", "state of button 2 (back)" ),
	new VistaPhantomBtGet( VistaPhantomBtGet::FLD_INKSWITCH, "INKWELLSWITCH", "state of the inkwell switch" ),
	new VistaPhantomButtonMaskGet,
	NULL
};


class VistaPhantomTranscoderFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaPhantomDriverMeasureTranscode;
	}
};
//
//class VistaPhantomDriverProtocolAspect : public IVistaDriverProtocolAspect
//{
//public:
//	VistaPhantomDriverProtocolAspect( VistaPhantomDriver *pDriver )
//		: IVistaDriverProtocolAspect(),
//		  m_pParent(pDriver)
//	{
//
//	}
//
//	virtual bool SetProtocol( const _cVersionTag &oTag )
//	{
//		m_pParent->SetDeviceString( oTag.m_strProtocolName );
//		return true;
//	}
//private:
//	VistaPhantomDriver *m_pParent;
//};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

class _CSchedulerHlp : public IVistaReferenceCountable
{
public:
	_CSchedulerHlp()
	{
		hdStartScheduler();
	}

	~_CSchedulerHlp()
	{
		hdStopScheduler();
		m_pScheduler = NULL;
	}

	static _CSchedulerHlp *m_pScheduler;
};

_CSchedulerHlp *_CSchedulerHlp::m_pScheduler = NULL;

struct VistaPhantomDriver::_sPhantomPrivate
{
	_sPhantomPrivate()
		: m_hHD(~0),
		  m_effectID(0)
	{
	}

	HHD				m_hHD;   // haptic device
	std::string     m_strDeviceString;
	int				m_effectID;
};

namespace
{
	HDCallbackCode HDCALLBACK LoopCallback(void *data)
	{
		((VistaPhantomDriver*) data)->Update();
		//((VistaPhantomDriver*) data)->m_effect.CalcEffectForce();
		return HD_CALLBACK_CONTINUE;
	}

    HDCallbackCode HDCALLBACK dutyCycleCallback(void *data)
    {
        double timeElapsed = hdGetSchedulerTimeStamp();
        if (timeElapsed > .001)
        {
            //assert(false && "Scheduler has exceeded 1ms.");
            std::cout << "Scheduler has exceeded 1ms.\n";
        }
        return HD_CALLBACK_CONTINUE;
    }


	VistaPhantomDriverFactory *SpFactory = NULL;
}


IVistaDriverCreationMethod *VistaPhantomDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaPhantomDriverFactory;
		SpFactory->RegisterSensorType( "", sizeof(_sPhantomMeasure), 1000,
			new VistaPhantomTranscoderFactory,
			VistaPhantomDriverMeasureTranscode::GetTypeString() );
	}
	return SpFactory;
}


VistaPhantomDriver::VistaPhantomDriver()
: IVistaDeviceDriver(),
  m_pWorkSpace( new VistaDriverWorkspaceAspect ),
  m_pInfo( new VistaDriverInfoAspect ),
  m_pIdentification( new VistaDeviceIdentificationAspect ),
  //m_pProtocol( NULL ),
  m_pPrivate( new _sPhantomPrivate ),
  m_pForceFeedBack(NULL)
{
	if(SpFactory == NULL)
		GetDriverFactoryMethod(); // create factory

	SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

	// create and register a sensor for the phantom
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( SpFactory->GetTypeFor("") );

	AddDeviceSensor(pSensor);

	pSensor->SetMeasureTranscode( new VistaPhantomDriverMeasureTranscode );


	//m_pProtocol = new VistaPhantomDriverProtocolAspect(this);
	//RegisterAspect( m_pProtocol );

	RegisterAspect( m_pWorkSpace );
	RegisterAspect( m_pInfo );
	RegisterAspect( m_pIdentification );

}

VistaPhantomDriver::~VistaPhantomDriver()
{
	if(m_pPrivate->m_hHD != ~0)
	{
		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		if( _CSchedulerHlp::m_pScheduler )
			_CSchedulerHlp::refdown( _CSchedulerHlp::m_pScheduler );

		if(m_pPrivate->m_effectID != 0)
		{
			hdUnschedule(m_pPrivate->m_effectID);
			m_pPrivate->m_effectID = 0;
		}

		hdDisableDevice(m_pPrivate->m_hHD);
		m_pPrivate->m_hHD = ~0;
	}

	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	RemDeviceSensor(pSensor);
	IVistaMeasureTranscode *pTC = pSensor->GetMeasureTranscode();
	pSensor->SetMeasureTranscode(NULL);
	delete pSensor;
	delete pTC;

	UnregisterAspect( GetAspectById( VistaDriverWorkspaceAspect::GetAspectId() ) );
	//UnregisterAspect( GetAspectById( IVistaDriverProtocolAspect::GetAspectId() ) );
	UnregisterAspect( GetAspectById( VistaDriverInfoAspect::GetAspectId() ) );
	UnregisterAspect( GetAspectById( IVistaDriverForceFeedbackAspect::GetAspectId() ) );
	UnregisterAspect( GetAspectById( VistaDeviceIdentificationAspect::GetAspectId() ) );

	delete m_pPrivate;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPhantomDriver::Connect()
{
	HDErrorInfo error;

	m_pPrivate->m_strDeviceString = m_pIdentification->GetDeviceName();

	m_pPrivate->m_hHD = hdInitDevice(m_pPrivate->m_strDeviceString.c_str());
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		std::cerr << "[PhantomDriver]: Device string given: ["
			<< (m_pPrivate->m_strDeviceString.empty() ? "DEFAULT_DEVICE" : m_pPrivate->m_strDeviceString)
			<< std::string("]") << std::endl;
		m_pPrivate->m_hHD = ~0; // should be 0 anyways...?
		return false;
	}
	else
	{
		VistaPropertyList &oInfo = m_pInfo->GetInfoPropsWrite();

		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		HDstring deviceVendor = hdGetString(HD_DEVICE_VENDOR);
		HDstring modelType = hdGetString(HD_DEVICE_MODEL_TYPE);
		HDstring driverVersion = hdGetString(HD_DEVICE_DRIVER_VERSION);
		HDstring serial = hdGetString(HD_DEVICE_SERIAL_NUMBER);

		HDdouble nFirmware = 0;
		hdGetDoublev( HD_DEVICE_FIRMWARE_VERSION, &nFirmware );

		oInfo.SetDoubleValue( "FIRMWARE_REVISION", nFirmware );
		oInfo.SetStringValue( "VENDOR", std::string( (deviceVendor ? deviceVendor : "<none>") ) );
		oInfo.SetStringValue( "MODEL", std::string( (modelType ? modelType : "<none>") ) );
		oInfo.SetStringValue( "DRIVERVERSION", std::string( (driverVersion ? driverVersion : "<none>") ) );
		oInfo.SetStringValue( "SERIALNUMBER", std::string( (serial ? serial : "<none>") ) );

		HDint nCalStyle = 0;
		HDint calibrationStyle = 0;

		hdGetIntegerv( HD_CALIBRATION_STYLE, &nCalStyle );

		std::string sCalStyle;
		if(nCalStyle & HD_CALIBRATION_AUTO)
			sCalStyle = "AUTO ";
		if(nCalStyle & HD_CALIBRATION_ENCODER_RESET)
			sCalStyle += "ENCODER_RESET ";
		if(nCalStyle & HD_CALIBRATION_INKWELL )
			sCalStyle += "INKWELL ";

		oInfo.SetStringValue("CALSTYLES", sCalStyle);
		oInfo.SetIntValue( "CALFLAGS", nCalStyle );

		if(hdCheckCalibration()==HD_CALIBRATION_NEEDS_UPDATE)
		{
			hdUpdateCalibration(nCalStyle);
		}

		HDfloat nTableOffset = 0;
		hdGetFloatv( HD_TABLETOP_OFFSET, &nTableOffset );

		oInfo.SetDoubleValue( "TABLETOP_OFFSET", nTableOffset );

		float maxStiffness, maxForce, maxDamping;

		hdGetFloatv(HD_NOMINAL_MAX_STIFFNESS, &maxStiffness);
		hdGetFloatv(HD_NOMINAL_MAX_FORCE, &maxForce);
        hdGetFloatv(HD_NOMINAL_MAX_DAMPING, &maxDamping);

		oInfo.SetDoubleValue( "MAXSTIFFNESS", double(maxStiffness) );
		oInfo.SetDoubleValue( "MAXFORCE", double(maxForce) );

		float maxWorkspace[6],
			  maxUsableWorkspace[6];

		hdGetFloatv(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);
		hdGetFloatv(HD_USABLE_WORKSPACE_DIMENSIONS, maxUsableWorkspace);

		m_pWorkSpace->SetWorkspace( "MAXWORKSPACE", VistaBoundingBox( &maxWorkspace[0], &maxWorkspace[3] ) );
		m_pWorkSpace->SetWorkspace( "USABLEWORKSPACE", VistaBoundingBox( &maxUsableWorkspace[0], &maxUsableWorkspace[3] ) );

		// register update function
		if(m_pPrivate->m_effectID == 0)
			m_pPrivate->m_effectID = hdScheduleAsynchronous( LoopCallback, this, HD_DEFAULT_SCHEDULER_PRIORITY);

        hdScheduleAsynchronous( dutyCycleCallback, this, HD_MIN_SCHEDULER_PRIORITY );

		if (HD_DEVICE_ERROR(error = hdGetError()))
		{
			hduPrintError(stderr, &error, "some error");
		}

		int nInputDOF, nOutputDOF;

		hdGetIntegerv(HD_INPUT_DOF,	 &nInputDOF);
		hdGetIntegerv(HD_OUTPUT_DOF, &nOutputDOF);

		if(nOutputDOF > 0)
		{
			m_pForceFeedBack = new VistaPhantomForceFeedbackAspect(this);
			m_pForceFeedBack->m_nInputDOF = nInputDOF;
			m_pForceFeedBack->m_nOutputDOF = nOutputDOF;
            m_pForceFeedBack->m_nMaxStiffness = maxStiffness;
            m_pForceFeedBack->m_nMaxForce = maxForce;
			RegisterAspect( m_pForceFeedBack );
		}

		return true;
	}
}

bool VistaPhantomDriver::DoSensorUpdate(microtime nTs)
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);

	MeasureStart(0, nTs);
	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the Phantom
	_sPhantomMeasure *s = reinterpret_cast<_sPhantomMeasure*>(&( *pM ).m_vecMeasures[0]);

	hdBeginFrame( m_pPrivate->m_hHD );

	hdGetFloatv(HD_CURRENT_POSITION,           s->m_afPosition);
    hdGetFloatv(HD_CURRENT_VELOCITY,           s->m_afVelocity);
	hdGetFloatv(HD_CURRENT_ANGULAR_VELOCITY,   s->m_afAngularVelocity);


	if(m_pForceFeedBack->GetForcesEnabled())
	{
		VistaVector3D v3Force;
		VistaQuaternion qTorque;

		IVistaDriverForceFeedbackAspect::IForceAlgorithm *pMd
            = m_pForceFeedBack->GetForceAlgorithm();
		if(pMd)
		{
			VistaVector3D pos( s->m_afPosition[0], s->m_afPosition[1], s->m_afPosition[2] );
            VistaVector3D vel( s->m_afVelocity[0], s->m_afVelocity[1], s->m_afVelocity[2] );
			VistaQuaternion qAngVel(VistaVector3D(0,0,1),
					                 VistaVector3D(s->m_afAngularVelocity[0],
													s->m_afAngularVelocity[1],
													s->m_afAngularVelocity[2]) );
			// calc update force from that information
			// @todo: think about the timestamps
			pMd->UpdateForce( double(nTs), pos, vel, qAngVel, v3Force, qTorque );
		}
		else
		{
			v3Force = m_pForceFeedBack->m_v3Force;
			qTorque = m_pForceFeedBack->m_qAngularForce;
		}


		if(m_pForceFeedBack->m_nOutputDOF >= 3)
		{
			hdSetFloatv( HD_CURRENT_FORCE, &v3Force[0] );
		}

		if(m_pForceFeedBack->m_nOutputDOF == 6)
			hdSetFloatv( HD_CURRENT_ANGULAR_VELOCITY, &qTorque[0] );
	}

	hdGetIntegerv(HD_CURRENT_BUTTONS,         &s->m_nButtonState);
	hdGetFloatv(HD_MOTOR_TEMPERATURE,          s->m_afOverheatState);
	hdGetFloatv(HD_INSTANTANEOUS_UPDATE_RATE, &s->m_nUpdateRate);
	hdGetFloatv(HD_CURRENT_TRANSFORM,          s->m_afRotMatrix);
	hdGetFloatv(HD_CURRENT_JOINT_ANGLES,       s->m_afJointAngles);
	hdGetFloatv(HD_CURRENT_GIMBAL_ANGLES,      s->m_afGimbalAngles);
	hdGetLongv(HD_CURRENT_ENCODER_VALUES,      s->m_nEncoderValues);
	HDboolean bSwitch = 0;

	hdGetBooleanv(HD_CURRENT_INKWELL_SWITCH,  &bSwitch);
	s->m_bInkwellSwitch = bSwitch ? true : false;


	hdGetFloatv(HD_CURRENT_FORCE,              s->m_afForce);
	hdGetFloatv(HD_CURRENT_VELOCITY,           s->m_afVelocity);

	hdEndFrame( m_pPrivate->m_hHD );

    HDErrorInfo error;
    /* Check if an error occurred while attempting to render the force */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "");
	}

	// we are done. Indicate that to the history
	MeasureStop(0);
	pSensor->SetUpdateTimeStamp(nTs);

	return true;
}

bool VistaPhantomDriver::PhysicalEnable(bool bEnable)
{
	if(bEnable)
	{
		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		if(_CSchedulerHlp::m_pScheduler == NULL )
			_CSchedulerHlp::m_pScheduler = new _CSchedulerHlp;

		_CSchedulerHlp::refup( _CSchedulerHlp::m_pScheduler );

		return true;
	}
	else
	{
		if(m_pPrivate->m_hHD == ~0)
			return true; // disabled and not initialized stays disabled
		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		if(_CSchedulerHlp::m_pScheduler)
			_CSchedulerHlp::refdown( _CSchedulerHlp::m_pScheduler );

		return true;
	}
}

std::string VistaPhantomDriver::GetDeviceString() const
{
	return m_pPrivate->m_strDeviceString;
}

void VistaPhantomDriver::SetDeviceString(const std::string &strDevice)
{
	if(m_pPrivate->m_hHD == ~0)
		m_pPrivate->m_strDeviceString = strDevice;
}


unsigned int VistaPhantomDriver::GetSensorMeasureSize() const
{
	return sizeof(_sPhantomMeasure);
}


// #############################################################################

VistaPhantomDriver::VistaPhantomForceFeedbackAspect::VistaPhantomForceFeedbackAspect( VistaPhantomDriver *pDriver )
: IVistaDriverForceFeedbackAspect(),
  m_pParent(pDriver),
  m_nInputDOF(0),
  m_nOutputDOF(0),
  m_nMaxForce(0),
  m_nMaxStiffness(0)
{
}

VistaPhantomDriver::VistaPhantomForceFeedbackAspect::~VistaPhantomForceFeedbackAspect()
{
}


bool VistaPhantomDriver::VistaPhantomForceFeedbackAspect::SetForce( const VistaVector3D   & v3Force,
																	  const VistaQuaternion & qAngularForce)
{
	// check thread safety here!
	m_v3Force       = v3Force;
	m_qAngularForce = qAngularForce;
	return true;
}

bool VistaPhantomDriver::VistaPhantomForceFeedbackAspect::SetForcesEnabled(bool bEnabled)
{
	hdMakeCurrentDevice( m_pParent->m_pPrivate->m_hHD );
	if(bEnabled)
		hdEnable( HD_FORCE_OUTPUT );
	else
		hdDisable( HD_FORCE_OUTPUT );

	return (GetForcesEnabled() == bEnabled);
}


bool VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetForcesEnabled() const
{
	hdMakeCurrentDevice( m_pParent->m_pPrivate->m_hHD );
	return (hdIsEnabled(HD_FORCE_OUTPUT) ? true : false);
}

int VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetNumInputDOF() const
{
	return m_nInputDOF;
}

int VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetNumOutputDOF() const
{
	return m_nOutputDOF;
}

float VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetMaximumStiffness() const
{
    return m_nMaxStiffness;
}

float VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetMaximumForce() const
{
    return m_nMaxForce;
}

