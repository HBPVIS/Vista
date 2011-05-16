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

// include header here

#if defined(WIN32)

#include "VistaDirectXGamepad.h"

#include <VistaDeviceDriversBase/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverProtocolAspect.h>
#include <iostream>
#include <algorithm>

namespace
{
	class GamepadProtocolAspect : public IVistaDriverProtocolAspect
	{
	public:
		GamepadProtocolAspect( VistaDirectXGamepad *pParent )
			: m_pParent(pParent)
		{
		}

		virtual bool SetProtocol( const _cVersionTag &oTag )
		{
			if(oTag.m_strProtocolName == "DIRECTX")
			{
				m_pParent->SetJoystickName( oTag.m_strProtocolRevision );
				return true;
			}
			return false;
		}

	private:
		VistaDirectXGamepad *m_pParent;
	};
}

class GamepadAttach : public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{
public:

	GamepadAttach(VistaDirectXGamepad *pParent)
		: m_nHwnd(0),
		  m_pParent(pParent)
	{
	}

	virtual bool AttachSequence(CWindowHandle *pWindow)
	{
		if(m_nHwnd != NULL)
			return false;

		// unfortunately, there seems to be now way to
		// retrieve the window id in a reliable way from ViSTA
		// as GLUT, for example, does not allow to give out
		// the window handle "as-raw" handle.
		// so we have to lookup the "global-active-top-level" window
		// from windows itself.
		m_nHwnd = pWindow->id(); //FindWindow( NULL, pWindow->GetWindowProperties()->GetTitle().c_str() );

		if(m_nHwnd != NULL)
		{
			m_liWindows.push_back(pWindow);
			return true;
		}
		return false;
	}

	virtual bool DetachSequence(CWindowHandle *pWindow)
	{
		if(m_nHwnd == NULL)
			return true;

		m_liWindows.remove(pWindow);
		m_nHwnd = NULL;
		return true;
	}

	virtual std::list<CWindowHandle*> GetWindowList() const
	{
		return m_liWindows;
	}


	HWND m_nHwnd;
	std::list<CWindowHandle*> m_liWindows;
	VistaDirectXGamepad *m_pParent;
};


class VistaDirectXGamepadMeasureTranscode : public IVistaMeasureTranscode
{
public:
	VistaDirectXGamepadMeasureTranscode()
	{
		m_nNumberOfScalars = 0;
	}

	virtual ~VistaDirectXGamepadMeasureTranscode() {}
	static std::string GetTypeString() { return "VistaDirectXGamepadMeasureTranscode"; }
	REFL_INLINEIMP(VistaDirectXGamepadMeasureTranscode, IVistaMeasureTranscode);
};


class VistaDirectXGamepadScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaDirectXGamepadScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("DSCALAR",
		VistaDirectXGamepadMeasureTranscode::GetTypeString(),
								"get scalars as doubles from directX gamepad"),
								m_nBtOffset(0)
	{}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure)
			return false;


		DIJOYSTATE2 *m = (DIJOYSTATE2*)&(*pMeasure).m_vecMeasures[0];

		if( nIndex < m_nBtOffset )
		{
			dScalar = m->rgbButtons[nIndex] ? 1.0 : 0.0;
			return true;
		}

		int nNormIdx = nIndex - m_nBtOffset;
		switch(nNormIdx)
		{
		case VistaDirectXGamepad::AXIS_X1:
			dScalar = m->lX;
			return true;
		case VistaDirectXGamepad::AXIS_Y1:
			dScalar = m->lY;
			return true;
		case VistaDirectXGamepad::AXIS_X2:
			dScalar = m->lRx;
			return true;
		case VistaDirectXGamepad::AXIS_Y2:
			dScalar = m->lRy;
			return true;
		case VistaDirectXGamepad::AXIS_Z1:
			dScalar = m->lZ;
			return true;
		case VistaDirectXGamepad::AXIS_Z2:
			dScalar = m->lRz;
			return true;
		case VistaDirectXGamepad::AXIS_POV:
			if( (LOWORD(m->rgdwPOV[0]) == 0xFFFF) )
				dScalar = -1.0;
			else
				dScalar = m->rgdwPOV[0];
			return true;

		default:
			break;
		}
  //  LONG    lX;                     /* x-axis position              */
  //  LONG    lY;                     /* y-axis position              */
  //  LONG    lZ;                     /* z-axis position              */
  //  LONG    lRx;                    /* x-axis rotation              */
  //  LONG    lRy;                    /* y-axis rotation              */
  //  LONG    lRz;                    /* z-axis rotation              */
  //  LONG    rglSlider[2];           /* extra axes positions         */
  //  DWORD   rgdwPOV[4];             /* POV directions               */
  //  BYTE    rgbButtons[128];        /* 128 buttons                  */
  //  LONG    lVX;                    /* x-axis velocity              */
  //  LONG    lVY;                    /* y-axis velocity              */
  //  LONG    lVZ;                    /* z-axis velocity              */
  //  LONG    lVRx;                   /* x-axis angular velocity      */
  //  LONG    lVRy;                   /* y-axis angular velocity      */
  //  LONG    lVRz;                   /* z-axis angular velocity      */
  //  LONG    rglVSlider[2];          /* extra axes velocities        */
  //  LONG    lAX;                    /* x-axis acceleration          */
  //  LONG    lAY;                    /* y-axis acceleration          */
  //  LONG    lAZ;                    /* z-axis acceleration          */
  //  LONG    lARx;                   /* x-axis angular acceleration  */
  //  LONG    lARy;                   /* y-axis angular acceleration  */
  //  LONG    lARz;                   /* z-axis angular acceleration  */
  //  LONG    rglASlider[2];          /* extra axes accelerations     */
  //  LONG    lFX;                    /* x-axis force                 */
  //  LONG    lFY;                    /* y-axis force                 */
  //  LONG    lFZ;                    /* z-axis force                 */
  //  LONG    lFRx;                   /* x-axis torque                */
  //  LONG    lFRy;                   /* y-axis torque                */
  //  LONG    lFRz;                   /* z-axis torque                */
  //  LONG    rglFSlider[2];          /* extra axes forces            */
  //      }
		//dScalar = m->m_nButtons[nIndex];
		return false;
	}

	unsigned int m_nBtOffset;
};

class VistaDirectXGamepadAxisVectorTranscode : public IVistaMeasureTranscode::CV3Get
{
public:
	enum eMode
	{
		TR_POS=0,
		TR_ROT,
		TR_VEL,
		TR_AVEL,
		TR_ACCEL,
		TR_AACCEL,
		TR_FORCE,
		TR_TORQUE,
		TR_AXIS1,
		TR_AXIS2,
		TR_POVAXIS,
	};
	VistaDirectXGamepadAxisVectorTranscode(eMode eMd, const std::string &strName, const std::string &strDesc)
		: IVistaMeasureTranscode::CV3Get(strName,
		VistaDirectXGamepadMeasureTranscode::GetTypeString(),
								strDesc),
								m_eMode(eMd)
	{}

	virtual VistaVector3D    GetValue(const VistaSensorMeasure *pM)    const
	{
		DIJOYSTATE2 *m = (DIJOYSTATE2*)&(*pM).m_vecMeasures[0];

		switch(m_eMode)
		{
		case TR_AXIS2:
			{
				return VistaVector3D(float(m->lZ), float(m->lRz), 0);
			}
		case TR_AXIS1:
			{
				return VistaVector3D(float(m->lX), float(m->lY), 0);
			}
		case TR_POS:
			{
				return VistaVector3D(float(m->lX), float(m->lY), float(m->lZ));
			}
		case TR_ROT:
			{
				return VistaVector3D(float(m->lRx), float(m->lRy), float(m->lRz));
			}
		case TR_VEL:
			{
				return VistaVector3D(float(m->lVX), float(m->lVY), float(m->lVZ));
			}
		case TR_AVEL:
			{
				return VistaVector3D(float(m->lVRx), float(m->lVRy), float(m->lVRz));
			}
		case TR_ACCEL:
			{
				return VistaVector3D(float(m->lAX), float(m->lAY), float(m->lAZ));
			}
		case TR_AACCEL:
			{
				return VistaVector3D(float(m->lARx), float(m->lARy), float(m->lARz));
			}
		case TR_FORCE:
			{
				return VistaVector3D(float(m->lFX), float(m->lFY), float(m->lFZ));
			}
		case TR_TORQUE:
			{
				return VistaVector3D(float(m->lFRx), float(m->lFRy), float(m->lFRz));
			}
		case TR_POVAXIS:
			{
				return GetPOV2Vec( m->rgdwPOV );
			}
		default:
			break;
		}
		return VistaVector3D();
	}

	virtual bool GetValue(const VistaSensorMeasure *pM, VistaVector3D &v3Out) const
	{
		if(!pM)
			return false;

		v3Out = GetValue(pM);
		return true;
	}

	static VistaVector3D GetPOV2Vec( DWORD rgdwPOV[4] )
	{
		if( (LOWORD(rgdwPOV) == 0xFFFF) )
			return VistaVector3D(0,0,0); // not pressed

		DWORD val = rgdwPOV[0];
		switch(val)
		{
			case 0:
				{
					// ahead
					return VistaVector3D(0,0,-1);
				}
			case 4500:
				{
					// upper right
					VistaVector3D vRet(1,0,-1);
					vRet.Normalize();
					return vRet;

				}
			case 9000:
				{
					// right
					return VistaVector3D(1,0,0);
				}
			case 13500:
				{
					// lower right
					VistaVector3D vRet(1,0,1);
					vRet.Normalize();
					return vRet;
				}
			case 18000:
				{
					// back
					VistaVector3D vRet(0,0,1);
					vRet.Normalize();
					return vRet;
				}
			case 22500:
				{
					// lower left
					VistaVector3D vRet(-1,0,1);
					vRet.Normalize();
					return vRet;
				}
			case 27000:
				{
					// left
					return VistaVector3D(-1,0,0);
				}
			case 31500:
				{
					// upper left
					VistaVector3D vRet(-1,0,-1);
					vRet.Normalize();
					return vRet;
				}
			default:
				break;
			}

		return VistaVector3D(); // no value
	}
private:
	eMode m_eMode;

};

class VistaDirectXGamepadPOVGetTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
public:
	VistaDirectXGamepadPOVGetTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<int>("POV",
		VistaDirectXGamepadMeasureTranscode::GetTypeString(),
								"return the angle value of the POV")
	{
	}


	virtual int GetValue(const VistaSensorMeasure *pM)    const
	{
		DIJOYSTATE2 *m = (DIJOYSTATE2*)&(*pM).m_vecMeasures[0];

		if( (LOWORD(m->rgdwPOV[0]) == 0xFFFF) )
			return -1;
		else
			return (m->rgdwPOV[0]);
	}

	virtual bool GetValue(const VistaSensorMeasure *pM, int &nOut) const
	{
		if(!pM)
			return false;

		nOut = GetValue(pM);
		return true;
	}
};

class VistaDirectXGamepadDIJOYSTATE2Get : public IVistaMeasureTranscode::TTranscodeValueGet<DIJOYSTATE2>
{
public:
	VistaDirectXGamepadDIJOYSTATE2Get()
		: IVistaMeasureTranscode::TTranscodeValueGet<DIJOYSTATE2>("DIJOYSTATE",
		VistaDirectXGamepadMeasureTranscode::GetTypeString(),
		"return the raw DIJOYSTATE from DI")
	{
	}


	virtual DIJOYSTATE2 GetValue(const VistaSensorMeasure *pM)    const
	{
		DIJOYSTATE2 *m = (DIJOYSTATE2*)&(*pM).m_vecMeasures[0];
		return *m;
	}

	virtual bool GetValue(const VistaSensorMeasure *pM, DIJOYSTATE2 &oOut) const
	{
		if(!pM)
			return false;

		DIJOYSTATE2 *m = (DIJOYSTATE2*)&(*pM).m_vecMeasures[0];
		oOut = *m;
		return true;
	}
};


class VistaDirectXGamepadPOVRotGet : public IVistaMeasureTranscode::CQuatGet
{
public:
	VistaDirectXGamepadPOVRotGet()
		: IVistaMeasureTranscode::CQuatGet( "POVROT",
			VistaDirectXGamepadMeasureTranscode::GetTypeString(),
			"the POV as a quat, rotating 0,0,-1 to the axis of the POV in the xz plane")
	{}

	virtual VistaQuaternion GetValue(const VistaSensorMeasure *pM)    const
	{
		DIJOYSTATE2 *m = (DIJOYSTATE2*)&(*pM).m_vecMeasures[0];


		if( (LOWORD(m->rgdwPOV[0]) == 0xFFFF) )
			return VistaQuaternion(0,0,0,1); // zero rotation
		else
		{
			VistaVector3D vRes = VistaDirectXGamepadAxisVectorTranscode::GetPOV2Vec(m->rgdwPOV);
			return VistaQuaternion( VistaVector3D(0,0,-1), vRes );
		}
	}

	virtual bool GetValue(const VistaSensorMeasure *pM, VistaQuaternion &nOut) const
	{
		if(!pM)
			return false;

		nOut = GetValue(pM);
		return true;
	}
};


static IVistaPropertyGetFunctor *SapGetter[] =
{
	new VistaDirectXGamepadDIJOYSTATE2Get,
	new VistaDirectXGamepadScalarTranscode,
	new VistaDirectXGamepadPOVGetTranscode,
	new VistaDirectXGamepadPOVRotGet,
	new VistaDirectXGamepadAxisVectorTranscode(
		VistaDirectXGamepadAxisVectorTranscode::TR_POS,
		"POSITION", "axis position"),
	new VistaDirectXGamepadAxisVectorTranscode(
		VistaDirectXGamepadAxisVectorTranscode::TR_AXIS1,
		"AXIS1", "axis 1"),
	new VistaDirectXGamepadAxisVectorTranscode(
		VistaDirectXGamepadAxisVectorTranscode::TR_ROT,
		"ROTATION", "axis rotation"),
	new VistaDirectXGamepadAxisVectorTranscode(
		VistaDirectXGamepadAxisVectorTranscode::TR_AXIS2,
		"AXIS2", "axis 2"),
	new VistaDirectXGamepadAxisVectorTranscode(
		VistaDirectXGamepadAxisVectorTranscode::TR_VEL,
		"VELOCITY", "axis velocity"),

	new VistaDirectXGamepadAxisVectorTranscode(
		VistaDirectXGamepadAxisVectorTranscode::TR_AVEL,
		"ANGULAR_VELOCITY", "axis angular velocity"),

	new VistaDirectXGamepadAxisVectorTranscode(
	VistaDirectXGamepadAxisVectorTranscode::TR_ACCEL,
		"ACCELERATION", "axis accelleration"),

	new VistaDirectXGamepadAxisVectorTranscode(
	VistaDirectXGamepadAxisVectorTranscode::TR_AACCEL,
		"ANGULAR_ACCELERATION", "axis angular accelleration"),

	new VistaDirectXGamepadAxisVectorTranscode(
	VistaDirectXGamepadAxisVectorTranscode::TR_FORCE,
		"FORCE", "axis force"),

	new VistaDirectXGamepadAxisVectorTranscode(
	VistaDirectXGamepadAxisVectorTranscode::TR_TORQUE,
		"TORQUE", "axis torque"),

	new VistaDirectXGamepadAxisVectorTranscode(
	VistaDirectXGamepadAxisVectorTranscode::TR_POVAXIS,
		"POVAXIS", "the POV as a vector in the xz plane"),

		NULL
};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

struct DI_ENUM_CONTEXT
{
	DI_ENUM_CONTEXT()
		: pPreferredJoyCfg(NULL),
		  m_pJoystick(NULL),
		  bPreferredJoyCfgValid(false),
		  m_pDI(NULL),
		  m_nCount(0)
	{}


	DIJOYCONFIG*         pPreferredJoyCfg;
	LPDIRECTINPUTDEVICE8 m_pJoystick;
	bool                 bPreferredJoyCfgValid;
	bool                 bFilterOutXinputDevices;
	LPDIRECTINPUT8       m_pDI;
	int                  m_nCount;
	std::string          m_strJoyName;
};


static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
											VOID* pContext )
{
	DI_ENUM_CONTEXT* pEnumContext = (DI_ENUM_CONTEXT*) pContext;
	HRESULT hr;

	pEnumContext->m_nCount++; // increase joystick count

	// Skip anything other than the perferred Joystick device as defined by the control panel.
	// Instead you could store all the enumerated Joysticks and let the user pick.
	if( pEnumContext->bPreferredJoyCfgValid &&
		!IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
		return DIENUM_CONTINUE;

	// Obtain an interface to the enumerated Joystick.
	hr = pEnumContext->m_pDI->CreateDevice( pdidInstance->guidInstance,
											&pEnumContext->m_pJoystick, NULL );

	// query joy name!
	DIDEVICEINSTANCE DeviceInfo;
	DeviceInfo.dwSize=sizeof(DIDEVICEINSTANCE);
	hr = pEnumContext->m_pJoystick->GetDeviceInfo(&DeviceInfo);
	if(hr != S_OK)
		return DIENUM_CONTINUE; // can not use this joystick!

	pEnumContext->m_strJoyName = DeviceInfo.tszInstanceName;


	// If it failed, then we can't use this Joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if( FAILED(hr) )
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

static BOOL CALLBACK FindJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
											VOID* pContext )
{
	DI_ENUM_CONTEXT* pEnumContext = (DI_ENUM_CONTEXT*) pContext;
	HRESULT hr;

	pEnumContext->m_nCount++; // increase joystick count

	// Skip anything other than the perferred Joystick device as defined by the control panel.
	// Instead you could store all the enumerated Joysticks and let the user pick.
	if( pEnumContext->bPreferredJoyCfgValid &&
		!IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
		return DIENUM_CONTINUE;

	if(!pEnumContext->m_strJoyName.empty())
	{
		DIDEVICEINSTANCE DeviceInfo;
		DeviceInfo.dwSize=sizeof(DIDEVICEINSTANCE);
		hr = pEnumContext->m_pJoystick->GetDeviceInfo(&DeviceInfo);
		if(hr != S_OK)
			return DIENUM_CONTINUE;
		if( pEnumContext->m_strJoyName == std::string(DeviceInfo.tszInstanceName) )
			hr = pEnumContext->m_pDI->CreateDevice( pdidInstance->guidInstance,
											 &pEnumContext->m_pJoystick, NULL );
	}
	// Obtain an interface to the enumerated Joystick.


	// If it failed, then we can't use this Joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if( FAILED(hr) )
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------

struct _sEnumJoystickContext
{
	_sEnumJoystickContext(LPDIRECTINPUTDEVICE8 pJoystick,
						  int nAxesRangeMin = -1000,
						  int nAxesRangeMax = +1000)
		: m_nBtNum(0),
		  m_nAxesRangeMin(nAxesRangeMin),
		  m_nAxesRangeMax(nAxesRangeMax),
		  m_pJoystick(pJoystick),
		  m_nForceFeedbackAxes(0)
	{}

	int m_nBtNum,
		m_nAxesRangeMin,
		m_nAxesRangeMax,
		m_nForceFeedbackAxes;

	LPDIRECTINPUTDEVICE8 m_pJoystick;
};

BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
								   VOID* pContext )
{
	_sEnumJoystickContext *ctx=(_sEnumJoystickContext*)pContext;

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize       = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow        = DIPH_BYID;
		diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin              = ctx->m_nAxesRangeMin;
		diprg.lMax              = ctx->m_nAxesRangeMax;
		if( (pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
			ctx->m_nForceFeedbackAxes++;

		// Set the range for the axis
		if( FAILED( ctx->m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
			return DIENUM_STOP;


	}

	if( pdidoi->dwType & DIDFT_BUTTON )
	{
		ctx->m_nBtNum++;
	}

	return DIENUM_CONTINUE;
}

class VistaDirectXGamepadCreationMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()()
	{
		return new VistaDirectXGamepad;
	}
};

class VistaDirectXGamepadTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDirectXGamepadMeasureTranscode;
	}
};


namespace
{
	VistaDirectXGamepadCreationMethod *SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaDirectXGamepad::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaDirectXGamepadCreationMethod;

		SpFactory->RegisterSensorType( "", sizeof(DIJOYSTATE2),
										20,
										new VistaDirectXGamepadTranscodeFactory,
										VistaDirectXGamepadMeasureTranscode::GetTypeString() );
	}
	return SpFactory;
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDirectXGamepad::VistaDirectXGamepad()
: IVistaDeviceDriver(),
  m_eDriver(TP_UNKNOWN),
  m_pJoystick(NULL),
  m_nNumberOfButtons(0),
  m_pForceAspect(NULL),
  m_pWindowAspect(NULL),
  m_pProtocol(NULL)
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_EXPLICIT_POLL );
	HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,
							 IID_IDirectInput8, (VOID**)&m_pDI, NULL );

	m_pProtocol = new GamepadProtocolAspect(this);
	m_pProtocol->RegisterProtocol( IVistaDriverProtocolAspect::_cVersionTag( "DIRECTX", "" ) );
	m_pProtocol->RegisterProtocol( IVistaDriverProtocolAspect::_cVersionTag( "XINPUT", "" ) );

	memset(&m_oldState, 0, sizeof(DIJOYSTATE2));
}

VistaDirectXGamepad::~VistaDirectXGamepad()
{
	VistaDeviceSensor *pSen = GetSensorByIndex(0);
	if( pSen )
	{
		VistaDirectXGamepadMeasureTranscode *pTrans
			= static_cast<VistaDirectXGamepadMeasureTranscode*>(pSen->GetMeasureTranscode());
		pSen->SetMeasureTranscode( NULL );
		delete pTrans;

		//m_pHistoryAspect->UnregisterSensor(pSen);
		RemDeviceSensor(pSen);
	}
	

	if(m_pProtocol)
	{
		UnregisterAspect(m_pProtocol, false);
		delete m_pProtocol;
	}

	if(m_pWindowAspect)
	{
		UnregisterAspect(m_pWindowAspect, false);
		delete m_pWindowAspect;
	}

	if(m_pForceAspect)
		UnregisterAspect(m_pForceAspect, false);
	delete m_pForceAspect;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDirectXGamepad::Connect()
{
	if(InitInput())
	{
		// create and attach a single sensor
		VistaDeviceSensor *pSensor = new VistaDeviceSensor;
		pSensor->SetTypeHint( VistaDirectXGamepad::GetDriverFactoryMethod()->GetTypeFor("") );

		pSensor->SetMeasureTranscode( new VistaDirectXGamepadMeasureTranscode );

		// this call will allocate the default number of history slots
		// (hopefully set for by the user)
		AddDeviceSensor(pSensor);

		// set the correct button number in the transcoder
		IVistaMeasureTranscode *pTrans = pSensor->GetMeasureTranscode();

		pTrans->SetNumberOfScalars( 7 + m_nNumberOfButtons );

		VistaDirectXGamepadScalarTranscode *pTranscode
		= dynamic_cast<VistaDirectXGamepadScalarTranscode*>
			(pTrans->GetMeasureProperty("DSCALAR"));

		pTranscode->m_nBtOffset = m_nNumberOfButtons;
        return true;
	}
    return false;
}

unsigned int VistaDirectXGamepad::GetSensorMeasureSize() const
{
	return sizeof(DIJOYSTATE2);
}


bool VistaDirectXGamepad::DoSensorUpdate(microtime dTs)
{
	if(m_pJoystick)
	{
		return UpdateDirectXInput(dTs);
	}
	return false;
}

bool VistaDirectXGamepad::UpdateDirectXInput(microtime dTs)
{
	HRESULT hr = 0;

	// Poll the device to read the current state
	hr = m_pJoystick->Poll();

	// DI_NOEFFECT can happen, as we do not always have a force-feedback
	// state without which we do not really need a poll().
	if(hr != S_OK && (hr!=DI_NOEFFECT))
	{
		switch(hr)
		{
		case DIERR_INPUTLOST:
			{
				std::cout << "DIERR_INPUTLOST\n";
				break;
			}
		case DIERR_NOTACQUIRED:
			{
				std::cout << "DIERR_NOTACQUIRED\n";
				break;
			}
		case DIERR_NOTINITIALIZED:
			{
				std::cout << "DIERR_NOTINITIALIZED\n";
				break;
			}
		default:
			std::cout << "ERROR?\n";
			break;
		}
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = m_pJoystick->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = m_pJoystick->Acquire();

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of
		// switching, so just try again later
	   return false;
	}

	// the rational for the update routine is as follows:

	// we claim a new state from the device
	// then we will compare the state with the last one and see iff
	// there is a difference (using memcmp). Iff not, we will skip this
	// update and return false (no update happened), else we will
	// copy the newstate as a new compare state (m_oldState)
	// AND we will copy the new state to the measure history!
	// the latter is necessary as we do not know how long the history is
	// and we will not affect old readings / writings
	DIJOYSTATE2 newjs;
	// Get the input's device state
	hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &newjs );
	if( hr != S_OK )
	{
		std::cout<<"\nGetDeviceState failed!\n";
		memset(&newjs, 0, sizeof(DIJOYSTATE2));
	}
	else
	{
		if( memcmp( &m_oldState, &newjs, sizeof(DIJOYSTATE2) ) != 0 )
		{
			// copy over old block
			memcpy( &m_oldState, &newjs, sizeof(DIJOYSTATE2) );

			// pass to buffer

			// pre-cache the sensor instance to update
			VistaDeviceSensor *pSen = GetSensorByIndex(0);
			MeasureStart(0, dTs);
			VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSen);
			DIJOYSTATE2 *p = (DIJOYSTATE2*)&(*pM).m_vecMeasures[0];
			memcpy( p, &newjs, sizeof(DIJOYSTATE) );
			MeasureStop(0); // advance write cursor
			pSen->SetUpdateTimeStamp(dTs);
			return true;
		}
	}
	return false;
}

bool VistaDirectXGamepad::InitInput()
{
	switch(m_eDriver)
	{
	case TP_DIRECTX:
		{
			return InitDirectXInput();
		}
	case TP_UNKNOWN:
		{
			if(InitDirectXInput())
				return true;
			else
			{
				// try to init xinput
				if(InitXInput())
					return true;
			}
		}
	case TP_XINPUT:
		{
			return InitXInput();
		}
	default:
		break;
	}

	return false;
}


bool VistaDirectXGamepad::InitDirectXInput()
{

	// get window handle from touch sequence
	GamepadAttach *pAttach = dynamic_cast<GamepadAttach*>(m_pWindowAspect->GetTouchSequence());
	if(pAttach && pAttach->m_nHwnd == 0L)
		return false;


	DIJOYCONFIG PreferredJoyCfg = {0};
	DI_ENUM_CONTEXT enumContext;
	memset(&enumContext, 0, sizeof(DI_ENUM_CONTEXT));

	enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
	enumContext.bPreferredJoyCfgValid = false;
	enumContext.m_pDI = m_pDI;
	enumContext.m_pJoystick = m_pJoystick;

	IDirectInputJoyConfig8* pJoyConfig = NULL;
	HRESULT hr;

	// claim a joystick config structure
	hr = m_pDI->QueryInterface( IID_IDirectInputJoyConfig8, (void **) &pJoyConfig );

	// fill the config structure with information from the
	// windows system database
	PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);

	// simply try to claim config for joystick 0
	// This function is expected to fail if no joystick is attached
	// as index 0 is not present
	if( SUCCEEDED( pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE ) ) )
		enumContext.bPreferredJoyCfgValid = true;

	// give away this structure
	if(pJoyConfig)
		pJoyConfig->Release();

	// Look for a  joystick
	if(m_strJoyName.empty())
	{
		hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
											 EnumJoysticksCallback, &enumContext,
											 DIEDFL_ATTACHEDONLY );
	}
	else
	{
		enumContext.m_strJoyName = m_strJoyName;
		enumContext.bPreferredJoyCfgValid = false;

		hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
											 FindJoysticksCallback, &enumContext,
											 DIEDFL_ATTACHEDONLY );
	}

	if( hr == S_OK  && enumContext.m_pJoystick )
	{
		// copy the joystick allocation routine to
		// our member pointer
		m_pJoystick = enumContext.m_pJoystick;

		DIDEVICEINSTANCE DeviceInfo;
		DeviceInfo.dwSize=sizeof(DIDEVICEINSTANCE);
		if( FAILED( hr = m_pJoystick->GetDeviceInfo(&DeviceInfo))){
			std::cerr<<"GetDeviceInfo  failed!\n";
			return false;
		}

		std::cout << "VistaDirectXGamepad::InitDirextX() -- found ["
			<< DeviceInfo.tszInstanceName << "] controller\n";

		// set the desired data-format
		// c_dfDIJoystick2 is a global enum from directX
		hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick2 );
		if(hr != S_OK )
		{
			std::cerr << "Could not set DataFormat for Joystick.\n";
		}
		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.

		hr = m_pJoystick->SetCooperativeLevel( pAttach->m_nHwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND );
		if( hr != S_OK )
		{
			std::cerr << "Could not set cooperative level\n";
			return false;
		}

		// normalize axes and determine number of buttons
		_sEnumJoystickContext s(m_pJoystick);
		hr = m_pJoystick->EnumObjects( EnumObjectsCallback,
									  (VOID*)&s , DIDFT_AXIS | DIDFT_BUTTON );
		if( hr != S_OK )
			std::cout<<"\nEnumObjects  failed (no axis or buttons?)!";

		m_nNumberOfButtons = s.m_nBtNum;
		std::cout << "This controller has [" << s.m_nBtNum << "] buttons.\n";

		DIPROPDWORD  dipdw;
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = DIPROPAXISMODE_ABS;  //DIPROPAXISMODE_ABS or DIPROPAXISMODE_REL
		hr = m_pJoystick->SetProperty(DIPROP_AXISMODE , &dipdw.diph);
		if(hr != S_OK)
			std::cerr << "Could not set axis mode\n";

		dipdw.dwData = 100; // = 1%
		hr = m_pJoystick->SetProperty(DIPROP_DEADZONE   , &dipdw.diph);
		if(hr != S_OK)
			std::cerr << "Could not set DEADZONE\n";

		dipdw.dwData = 10000; // = 100%
		hr = m_pJoystick->SetProperty(DIPROP_SATURATION    , &dipdw.diph);
		if(hr != S_OK)
			std::cerr << "Could not set SATURATION\n";

		hr = m_pJoystick->Acquire();
		if( hr != S_OK )
			std::cerr << "Could not acquire initially!\n";

		m_eDriver = TP_DIRECTX;


		// try to figure out whether this driver has a force feedback support or not
		if(s.m_nForceFeedbackAxes != 0)
		{
			// yes!
			VistaDirectXForceFeedbackAspect *pForce = new VistaDirectXForceFeedbackAspect(this);

			// configure force aspect
			// assign and register as aspect
			m_pForceAspect = pForce;
			RegisterAspect( m_pForceAspect );
		}
		return true;
	}
	else
	{
		std::cerr << "VistaDirectXGamepad::InitDirectXInput() "
				  << "-- FAILED to enumerate a joystick"
				  << std::endl;
		if(enumContext.m_nCount == 0)
			std::cerr << "VistaDirectXGamepad::InitDirectXInput() -- "
					  << "Maybe no joystick attached?\n";
	}

	return false;
}

unsigned int VistaDirectXGamepad::GetNumberOfButtons() const
{
	return m_nNumberOfButtons;
}

bool VistaDirectXGamepad::GetRanges( int &xMin, int &yMin, int &zMin,
				int &xMax, int &yMax, int &zMax) const
{
	if(m_pJoystick == NULL)
		return false;

	xMin = yMin = zMin = -1000;
	xMax = yMax = zMax =  1000;

	return true;
}

void VistaDirectXGamepad::SetJoystickName( const std::string &strJoyName )
{
	if(m_pJoystick)
		return;
	m_strJoyName = strJoyName;
}

std::string VistaDirectXGamepad::GetJoystickName() const
{
	return m_strJoyName;
}


bool VistaDirectXGamepad::InitXInput()
{
	return false;
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect *
	   VistaDirectXGamepad::GetDirectXForceFeedbackAspect() const
{
	return dynamic_cast<VistaDirectXForceFeedbackAspect*>(m_pForceAspect);
}

bool VistaDirectXGamepad::RegisterAspect(IVistaDeviceDriverAspect *pAspect)
{
	bool bRet = IVistaDeviceDriver::RegisterAspect(pAspect);
	if( bRet && (pAspect->GetId() == VistaDriverAbstractWindowAspect::GetAspectId()) )
	{
		// register window aspect
		m_pWindowAspect = dynamic_cast<VistaDriverAbstractWindowAspect*>(pAspect);
		m_pWindowAspect->SetTouchSequence( new GamepadAttach(this) );
	}

	return bRet;
}

bool VistaDirectXGamepad::UnregisterAspect(IVistaDeviceDriverAspect *pAspect, bool bDelete)
{
	if( m_pWindowAspect && (pAspect->GetAspectId() == VistaDriverAbstractWindowAspect::GetAspectId()) )
	{
		std::list<CWindowHandle*> liWds = m_pWindowAspect->GetWindowList();
		for(std::list<CWindowHandle*>::iterator it = liWds.begin(); it != liWds.end(); ++it)
			m_pWindowAspect->DetachFromWindow(*it);

		VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence *pTouch
			= m_pWindowAspect->GetTouchSequence();
		m_pWindowAspect->SetTouchSequence(NULL);
		delete pTouch;

		m_pWindowAspect = NULL;
	}

	return IVistaDeviceDriver::UnregisterAspect(pAspect, bDelete);
}

// #############################################################################
// FORCE FEEDBACK ASPECT
// #############################################################################

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::VistaDirectXForceFeedbackAspect(
	VistaDirectXGamepad *pPad)
: IVistaDriverForceFeedbackAspect(),
  m_pCurrentEffect(NULL),
  m_pParent(pPad)
{
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::~VistaDirectXForceFeedbackAspect()
{
	// unregister all uploaded effects
	for(std::vector<CEffect*>::iterator it = m_vecEffects.begin();
		it != m_vecEffects.end(); ++it)
	{
		(*it)->m_pEffect->Unload();
		delete (*it);
	}
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect*
	VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CreateEffect(REFGUID effectType)
{
	// we can assume that a valid joy configuration is present,
	// otherwise this aspect would not be present

	CEffect *pEffect = new CEffect;

	LONG rglDirection[2] = { 0,0 };
	DWORD  rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };

	DICONSTANTFORCE cf = { 0 };
	DIEFFECT eff;
	memset(&eff, 0, sizeof(eff));

	eff.dwSize                = sizeof(DIEFFECT);
	eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes                 = 2;
	eff.rglDirection          = rglDirection;
	eff.lpEnvelope            = 0;
	eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay            = 0;
	eff.rgdwAxes                = rgdwAxes;
	eff.dwDuration              = INFINITE;
	eff.dwSamplePeriod          = 0;
	eff.dwGain                  = DI_FFNOMINALMAX;
	eff.dwTriggerButton         = DIEB_NOTRIGGER;
	eff.dwTriggerRepeatInterval = 0;

	HRESULT hr = m_pParent->m_pJoystick->CreateEffect( effectType, &eff, &pEffect->m_pEffect, NULL );
	if( hr != S_OK )
		delete pEffect;

	m_vecEffects.push_back(pEffect);

	return pEffect;
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::DeleteEffect(CEffect *pEffect)
{
	pEffect->m_pEffect->Unload();

	// remove from internal structures
	std::vector<CEffect*>::iterator it = std::remove(m_vecEffects.begin(),
													 m_vecEffects.end(), pEffect);
	m_vecEffects.erase(it, m_vecEffects.end());

	delete pEffect;

	return true;
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::SetForce( const VistaVector3D   & force,
																	   const VistaQuaternion & )
{
	if(!m_pCurrentEffect)
		return false;

	DIEFFECT eff;
	memset(&eff, 0, sizeof(eff));

	// interpret the force vector as a projection
	// on the x-z plane
	VistaVector3D norm(force);
	norm[1] = 0;
	norm.Normalize();

	LONG rglDirection[2] = { LONG(norm[0]), LONG(norm[2]) };
	DWORD  rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };

	DICONSTANTFORCE cf;
	cf.lMagnitude = (LONG)force.GetLength();

	eff.dwSize                = sizeof(DIEFFECT);
	eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes                 = 2;
	eff.rglDirection          = rglDirection;
	eff.lpEnvelope            = 0;
	eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay            = 0;
	eff.rgdwAxes                = rgdwAxes;
	eff.dwDuration              = INFINITE;
	eff.dwSamplePeriod          = 0;
	eff.dwGain                  = DI_FFNOMINALMAX;
	eff.dwTriggerButton         = DIEB_NOTRIGGER;
	eff.dwTriggerRepeatInterval = 0;

	m_pParent->m_pJoystick->Acquire();
	if(m_pCurrentEffect->SetParameters(eff))
		return Start(m_pCurrentEffect, 1);
	return false;
}


bool     VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::SetCurrentEffect( CEffect *pEffect )
{
	m_pCurrentEffect = pEffect;
	return true;
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect *
	 VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetCurrentEffect() const
{
	return m_pCurrentEffect;
}


bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect::SetParameters(DIEFFECT &params)
{
	return (m_pEffect->SetParameters( &params, DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS ) == S_OK);
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect::GetParameters( DIEFFECT &params )
{
	return (m_pEffect->GetParameters( &params, 0 ) == S_OK);
}


bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Start(CEffect *pEffect, int nIterations)
{
	m_pParent->m_pJoystick->Acquire();
	return (pEffect->m_pEffect->Start(nIterations, 0) == S_OK);
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Stop(CEffect *pEffect)
{
	m_pParent->m_pJoystick->Acquire();
	return (pEffect->m_pEffect->Stop() == S_OK);
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect::CEffect()
: m_pEffect(NULL)
{
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect::~CEffect()
{

}


int VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetNumInputDOF() const
{
	return 0;
}

int VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetNumOutputDOF() const
{
	return 3; // force-vector
}


bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::SetForcesEnabled(bool bEnabled)
{
	return false;
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetForcesEnabled() const
{
	return false;
}


float VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetMaximumStiffness() const
{
    return 1.0f;
}

float VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetMaximumForce() const
{
    return 1.0f;
}

#endif // WIN32

