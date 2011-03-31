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

#include "VistaWiimoteDriver.h"
#include <VistaDeviceDriversBase/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/VistaDriverForceFeedbackAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>
#include <VistaDeviceDriversBase/VistaDriverThreadAspect.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <stdio.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#ifdef WIN32
#include <float.h>
	#define myisnan _isnan
#else
#define myisnan std::isnan
#endif

class VistaWiimoteTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(VistaWiimoteTranscoder, IVistaMeasureTranscode);
public:
	VistaWiimoteTranscoder()
	{
		// inherited as protected member
		m_nNumberOfScalars = 13;
	}

	  static std::string GetTypeString() { return "VistaWiimoteTranscoder"; }
};

class VistaWiimoteTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaWiimoteTranscoder;
	}
};


// ############################################################################

// create wiimote getters.

class VistaWiimoteIndexedButtonGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaWiimoteIndexedButtonGet()
	: IVistaMeasureTranscode::CScalarDoubleGet( "BUTTON",
			                                    VistaWiimoteTranscoder::GetTypeString(),
			                                    "retrieve button states (0-11)")
			                                    {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex >= 13)
			return false;

		VistaWiimoteDriver::_wiimoteMeasure *m
			= (VistaWiimoteDriver::_wiimoteMeasure*)&(*pMeasure).m_vecMeasures[0];

		switch(nIndex)
		{
			case 0:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_ONE) ? 1:0);
				break;
			}
			case 1:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_TWO) ? 1:0);
				break;
			}
			case 2:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_B) ? 1:0);
				break;
			}
			case 3:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_MINUS) ? 1:0);
				break;
			}
			case 4:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_HOME) ? 1:0);
				break;
			}
			case 5:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_PLUS) ? 1:0);
				break;
			}
			case 6:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_A) ? 1:0);
				break;
			}
			case 7:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_LEFT) ? 1:0);
				break;
			}
			case 8:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_RIGHT) ? 1:0);
				break;
			}
			case 9:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_UP) ? 1:0);
				break;
			}
			case 10:
			{
				dScalar = ((m->m_buttons & WIIMOTE_BUTTON_DOWN) ? 1:0);
				break;
			}

			default:
				break;
		}

//		if(dScalar)
//			std::cout << "dScalar(" << nIndex << ") = " << dScalar << std::endl;

		return true;
	}
};

class VistaWiimoteAccelerationGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaWiimoteAccelerationGet()
	: IVistaMeasureTranscode::CV3Get("ACCELERATION",
			VistaWiimoteTranscoder::GetTypeString(),
			"retrieve acceleration values from wiimote device"  )
	{

	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}


	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;

		VistaWiimoteDriver::_wiimoteMeasure *m
			= (VistaWiimoteDriver::_wiimoteMeasure*)&(*pMeasure).m_vecMeasures[0];

		v3[Vista::X] = m->m_Acceleration.x;
		v3[Vista::Y] = m->m_Acceleration.y;
		v3[Vista::Z] = m->m_Acceleration.z;
		return true;
	}
};


class VistaWiimotePosGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaWiimotePosGet()
	: IVistaMeasureTranscode::CV3Get( "IRPOSITION",
			                  VistaWiimoteTranscoder::GetTypeString(),
			                  "retrieve the IR reported position as VistaVec3D(x,y,0)")
    {}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;

		VistaWiimoteDriver::_wiimoteMeasure *m
			= (VistaWiimoteDriver::_wiimoteMeasure*)&(*pMeasure).m_vecMeasures[0];

		v3[Vista::X] = (float)m->m_IR.ax;
		v3[Vista::Y] = (float)m->m_IR.ay;
		v3[Vista::Z] = 0.0f;
		return true;
	}
};


class VistaWiimoteSingleButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
{
public:
	VistaWiimoteSingleButtonGet( unsigned short nIndex, const std::string &sName, const std::string &strDesc)
	: IVistaMeasureTranscode::TTranscodeValueGet<bool>( sName,
			VistaWiimoteTranscoder::GetTypeString(),
			                          strDesc ),
			                          m_nIndex(nIndex)
	{
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		bool bRet = false;
		GetValue( pMeasure, bRet );
		return bRet;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &nBtn) const
	{
		if(!pMeasure)
			return false;

		const VistaWiimoteDriver::_wiimoteMeasure *m
			= reinterpret_cast<const VistaWiimoteDriver::_wiimoteMeasure*>(&(*pMeasure).m_vecMeasures[0]);

		// this is the same as the IS_PRESSED() macro from wiiuse,
	    // but works direcly on the button, not on a wiimote struct

		nBtn = ( m->m_buttons & m_nIndex ? true:false);
//		std::cout << nBtn << " " << m_nIndex << " " << m->m_buttons << std::endl;
		return true;
	}

	unsigned short m_nIndex;
};

class VistaWiimoteIRMarkerGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaVector3D>
{
public:
	VistaWiimoteIRMarkerGet( unsigned short nIndex, const std::string &sName, const std::string &strDesc)
	: IVistaMeasureTranscode::TTranscodeValueGet<VistaVector3D>( sName,
			VistaWiimoteTranscoder::GetTypeString(),
			                          strDesc ),
			m_nIndex(nIndex)
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3Result;
		GetValue( pMeasure, v3Result );
		return v3Result;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
	{
		if(!pMeasure)
			return false;

		const VistaWiimoteDriver::_wiimoteMeasure *m
			= reinterpret_cast<const VistaWiimoteDriver::_wiimoteMeasure*>(&(*pMeasure).m_vecMeasures[0]);

		const ir_dot_t& oDot = m->m_IR.dot[m_nIndex];
		if( oDot.visible )
		{
			v3Pos[0] = oDot.rx;
			v3Pos[1] = oDot.ry;
			v3Pos[2] = oDot.size;
		}
		else
			v3Pos[2] = -1;

		return true;
	}

	unsigned short m_nIndex;
};
// ############################################################################


class VistaNunchukTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(VistaNunchukTranscoder, IVistaMeasureTranscode);
public:
	VistaNunchukTranscoder()
	{
		// inherited as protected member
		m_nNumberOfScalars = 4;
	}

	  static std::string GetTypeString() { return "VistaNunchukTranscoder"; }
};

class VistaNunchukTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaNunchukTranscoder;
	}
};


class VistaNunchukPaddleGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaNunchukPaddleGet()
	: IVistaMeasureTranscode::CV3Get( "PADDLE",
			VistaNunchukTranscoder::GetTypeString(),
			                          "getter for the nunchuk paddle / analog joystick as v3(x,y,0)"
			                        )
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;
		const nunchuk_t *m = reinterpret_cast<const nunchuk_t*>(&(*pMeasure).m_vecMeasures[0]);

		// ok the documentation on the wiiuse does not seem to be correct,
		// or the nunchuk I have here is... broken...
		// in case the paddle is centered, I still measure 90deg and a magnitude
		// of 0.01 absolute (constant).
		// so here I mask out all measures that show a magnitude below 0.01f
		// or (as documented) the angle isnan.
		if( std::fabs(m->js.mag - 0.01f) < std::numeric_limits<float>::epsilon() || myisnan(m->js.ang) )
		{
			// joystick is "straight up"
			// interaction code should expect a NULL vector!
			v3.SetToZeroVector();
			return true;
		}


		// in contrast to the documentation on the wiiuse lib (0.12 that is)
		// m->js.ang is in degrees: 0 up, 90 right, 180 down, 270 left

		// for the output, construct a rotation that will rotate 0,0,1 ('forward')
		// around the absolute Y-axis by angle angle
		VistaQuaternion q(VistaAxisAndAngle( VistaVector3D(0,1,0), Vista::DegToRad(m->js.ang) ) );

		// rotate the null ('forward') using q, scale by mag
		v3 = m->js.mag * ( q.Rotate(VistaVector3D(0,0,1) ) );

		return true;
	}
};

class VistaNunchukButtonGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaNunchukButtonGet()
	: IVistaMeasureTranscode::CScalarDoubleGet( "BUTTON",
			VistaNunchukTranscoder::GetTypeString(),
			"get nunchuk buttons by index")
	{

	}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
	{
		if(!pMeasure)
			return false;

		const nunchuk_t *m = reinterpret_cast<const nunchuk_t*>(&(*pMeasure).m_vecMeasures[0]);

		dScalar = ( m->btns & (1<<nIndex) ? 1:0 );
		return true;
	}
};


class VistaNunchukAccelerationGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaNunchukAccelerationGet()
	: IVistaMeasureTranscode::CV3Get( "ACCELERATION",
			VistaNunchukTranscoder::GetTypeString(),
			                          "get nunchuk acceleration values"
			                        )
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;

		const nunchuk_t *m = reinterpret_cast<const nunchuk_t*>(&(*pMeasure).m_vecMeasures[0]);

		v3[Vista::X] = m->accel.x;
		v3[Vista::Y] = m->accel.y;
		v3[Vista::Z] = m->accel.z;

		return true;
	}
};

class VistaNunchukOrientationGet : public IVistaMeasureTranscode::CQuatGet
{
public:
	VistaNunchukOrientationGet()
	: IVistaMeasureTranscode::CQuatGet( "ORIENTATION",
			VistaNunchukTranscoder::GetTypeString(),
			                          "get nunchuk orientation value (wiiuse-lib)"
			                        )
	{
	}

	virtual VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaQuaternion q;
		GetValue( pMeasure, q );
		return q;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &q) const
	{
		if(!pMeasure)
			return false;

		const nunchuk_t *m = reinterpret_cast<const nunchuk_t*>(&(*pMeasure).m_vecMeasures[0]);

		VistaEulerAngles oAngles( m->orient.yaw, m->orient.roll, m->orient.pitch );
		q = VistaQuaternion( oAngles );

		return true;
	}
};

class VistaNunchukGravityGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaNunchukGravityGet()
	: IVistaMeasureTranscode::CV3Get( "GRAVITY",
			VistaNunchukTranscoder::GetTypeString(),
			                          "get nunchuk gravity report values"
			                        )
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;

		const nunchuk_t *m = reinterpret_cast<const nunchuk_t*>(&(*pMeasure).m_vecMeasures[0]);

		v3[Vista::X] = m->gforce.x;
		v3[Vista::Y] = m->gforce.y;
		v3[Vista::Z] = m->gforce.z;

		return true;
	}
};


class VistaNunchukSingleButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
{
public:
	VistaNunchukSingleButtonGet( int nIndex, const std::string &sName, const std::string &strDesc)
	: IVistaMeasureTranscode::TTranscodeValueGet<bool>( sName,
			VistaNunchukTranscoder::GetTypeString(),
			                          strDesc ),
			                          m_nIndex(nIndex)
	{
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		bool bRet = false;
		GetValue( pMeasure, bRet );
		return bRet;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &nBtn) const
	{
		if(!pMeasure)
			return false;

		const nunchuk_t *m = reinterpret_cast<const nunchuk_t*>(&(*pMeasure).m_vecMeasures[0]);

		nBtn = ( m->btns & (1<<m_nIndex) ? true:false);

		return true;
	}

	int m_nIndex;
};


// ############################################################################


class VistaClassicTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(VistaClassicTranscoder, IVistaMeasureTranscode);
public:
	VistaClassicTranscoder()
	{
		// inherited as protected member
		m_nNumberOfScalars = 20;
	}

	  static std::string GetTypeString() { return "VistaClassicTranscoder"; }
};

class VistaClassicTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaClassicTranscoder;
	}
};


class VistaClassicPaddleGet : public IVistaMeasureTranscode::CV3Get
{
public:
	enum ePad
	{
		PAD_LEFT = 0,
		PAD_RIGHT = 1,
	};

	VistaClassicPaddleGet( ePad nIndex, const std::string &strName, const std::string &strDesc )
	: IVistaMeasureTranscode::CV3Get( strName,
			VistaClassicTranscoder::GetTypeString(),
			                          strDesc )
	, m_nIndex(nIndex)
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;
		const classic_ctrl_t *m = reinterpret_cast<const classic_ctrl_t*>(&(*pMeasure).m_vecMeasures[0]);

		switch(m_nIndex)
		{
			case PAD_LEFT:
			{
				return GetPaddle( m->ljs, v3 );
			}
			case PAD_RIGHT:
			{
				return GetPaddle( m->rjs, v3 );
			}
		}

		return false;
	}

	bool GetPaddle(const joystick_t &js, VistaVector3D &v3) const
	{
		// ok the documentation on the wiiuse does not seem to be correct,
		// or the nunchuk I have here is... broken...
		// in case the paddle is centered, I still measure 90deg and a magnitude
		// of 0.01 absolute (constant).
		// so here I mask out all measures that show a magnitude below 0.01f
		// or (as documented) the angle isnan.
		if( std::fabs(js.mag - 0.01f) < std::numeric_limits<float>::epsilon() || myisnan(js.ang) )
		{
			// joystick is "straight up"
			// interaction code should expect a NULL vector!
			v3.SetToZeroVector();
			return true;
		}


		// in contrast to the documentation on the wiiuse lib (0.12 that is)
		// m->js.ang is in degrees: 0 up, 90 right, 180 down, 270 left

		// for the output, construct a rotation that will rotate 0,0,1 ('forward')
		// around the absolute Y-axis by angle angle
		VistaQuaternion q(VistaAxisAndAngle( VistaVector3D(0,1,0), Vista::DegToRad(js.ang) ) );

		// rotate the null ('forward') using q, scale by mag
		v3 = js.mag * ( q.Rotate(VistaVector3D(0,0,1) ) );
		return true;
	}

	ePad m_nIndex;
};


class VistaClassicButtonGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaClassicButtonGet()
	: IVistaMeasureTranscode::CScalarDoubleGet( "BUTTON",
			VistaClassicTranscoder::GetTypeString(),
			"get classic control buttons by index")
	{

	}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
	{
		if(!pMeasure)
			return false;
		const classic_ctrl_t *m = reinterpret_cast<const classic_ctrl_t*>(&(*pMeasure).m_vecMeasures[0]);

		if(nIndex >= 0 && nIndex <= 15)
			dScalar = m->btns & (1<<nIndex);
		else
		{
			// shoulder keys
			switch( nIndex )
			{
			case 16:
				dScalar = double(m->l_shoulder);
				return true;
			case 17:
				dScalar = double(m->r_shoulder);
				return true;
			default:
				return false;
			}
		}
		return true;
	}
};

class VistaClassicShoulderGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
{
public:
	enum ePad
	{
		SHOULDER_LEFT = 0,
		SHOULDER_RIGHT = 1,
	};

	VistaClassicShoulderGet( ePad pd, const std::string &strName, const std::string &strDesc )
	: IVistaMeasureTranscode::TTranscodeValueGet<float>( strName,
			VistaClassicTranscoder::GetTypeString(),
						strDesc ),
						m_ePd(pd)
						{

						}

	virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		float v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &v3) const
	{
		switch(m_ePd)
		{
		case SHOULDER_LEFT:
			break;
		case SHOULDER_RIGHT:
			break;
		default:
			break;
		}
		return false;
	}

	ePad m_ePd;
};



// ############################################################################


class VistaStatusTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(VistaStatusTranscoder, IVistaMeasureTranscode);
public:
	VistaStatusTranscoder()
	{
		// inherited as protected member
		m_nNumberOfScalars = 20;
	}

	  static std::string GetTypeString() { return "VistaStatusTranscoder"; }
};

class VistaStatusTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaStatusTranscoder;
	}
};

class VistaStatusBatteryGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
{

public:
	VistaStatusBatteryGet()
	: IVistaMeasureTranscode::TTranscodeValueGet<float>( "BATTERY",
			                                           VistaStatusTranscoder::GetTypeString(),
			                                           "retrieve the battery state of the wiimote [0..1]" )
   {
   }

	float GetValue( const VistaSensorMeasure *pMeasure ) const
   {
		float nRet = 0;
	   GetValue( pMeasure, nRet );
	   return nRet;
   }

   bool GetValue( const VistaSensorMeasure *pMeasure, float &nValue ) const
   {
	   if(!pMeasure)
		   return false;

	   const VistaWiimoteDriver::_wiistateMeasure *m
		          = reinterpret_cast<const VistaWiimoteDriver::_wiistateMeasure*>(&(*pMeasure).m_vecMeasures[0]);

	   nValue = m->m_nBatteryLevel;

	   return true;
   }
};

class VistaStatusExpansionStateGet : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{

public:
	VistaStatusExpansionStateGet()
	: IVistaMeasureTranscode::TTranscodeValueGet<int>( "EXPANSIONSTATE",
			                                           VistaStatusTranscoder::GetTypeString(),
			                                           "retrieve the current expansion state as bitmask" )
   {
   }

	int GetValue( const VistaSensorMeasure *pMeasure ) const
   {
	   int nRet = 0;
	   GetValue( pMeasure, nRet );
	   return nRet;
   }

   bool GetValue( const VistaSensorMeasure *pMeasure, int &nValue ) const
   {
	   if(!pMeasure)
		   return false;

	   const VistaWiimoteDriver::_wiistateMeasure *m
		          = reinterpret_cast<const VistaWiimoteDriver::_wiistateMeasure*>(&(*pMeasure).m_vecMeasures[0]);

	   nValue = m->m_nExpansionState;

	   return true;
   }
};

// ############################################################################


class VistaGuitarTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(VistaGuitarTranscoder, IVistaMeasureTranscode);
public:
	VistaGuitarTranscoder()
	{
		// inherited as protected member
		// @todo check guitar stuff
		m_nNumberOfScalars = 10;
	}

	  static std::string GetTypeString() { return "VistaGuitarTranscoder"; }
};

class VistaGuitarTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaGuitarTranscoder;
	}
};

class VistaGuitarPaddleGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaGuitarPaddleGet()
	: IVistaMeasureTranscode::CV3Get( "PADDLE",
			VistaGuitarTranscoder::GetTypeString(),
			                          "getter for the guitar paddle / analog joystick as v3(x,y,0)"
			                        )
	{
	}

	virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		VistaVector3D v3;
		GetValue( pMeasure, v3 );
		return v3;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
	{
		if(!pMeasure)
			return false;
		const guitar_hero_3_t *m = reinterpret_cast<const guitar_hero_3_t*>(&(*pMeasure).m_vecMeasures[0]);

		// ok the documentation on the wiiuse does not seem to be correct,
		// or the nunchuk I have here is... broken...
		// in case the paddle is centered, I still measure 90deg and a magnitude
		// of 0.01 absolute (constant).
		// so here I mask out all measures that show a magnitude below 0.01f
		// or (as documented) the angle isnan.
		if( std::fabs(m->js.mag - 0.01f) < std::numeric_limits<float>::epsilon() || myisnan(m->js.ang) )
		{
			// joystick is "straight up"
			// interaction code should expect a NULL vector!
			v3.SetToZeroVector();
			return true;
		}


		// in contrast to the documentation on the wiiuse lib (0.12 that is)
		// m->js.ang is in degrees: 0 up, 90 right, 180 down, 270 left

		// for the output, construct a rotation that will rotate 0,0,1 ('forward')
		// around the absolute Y-axis by angle angle
		VistaQuaternion q(VistaAxisAndAngle( VistaVector3D(0,1,0), Vista::DegToRad(m->js.ang) ) );

		// rotate the null ('forward') using q, scale by mag
		v3 = m->js.mag * ( q.Rotate(VistaVector3D(0,0,1) ) );

		return true;
	}
};

class VistaGuitarButtonGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaGuitarButtonGet()
	: IVistaMeasureTranscode::CScalarDoubleGet( "BUTTON",
			VistaGuitarTranscoder::GetTypeString(),
			"get guitar control buttons by index [0-8] btns [10] whammy bar")
	{

	}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
	{
		if(!pMeasure)
			return false;
		const guitar_hero_3_t *m = reinterpret_cast<const guitar_hero_3_t*>(&(*pMeasure).m_vecMeasures[0]);

		if(nIndex == 9)
		{
			dScalar = m->whammy_bar;
		}
		else
			dScalar = m->btns & (1<<nIndex);
		return true;
	}
};

class VistaGuitarWhammyGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
{

public:
	VistaGuitarWhammyGet()
	: IVistaMeasureTranscode::TTranscodeValueGet<float>( "WHAMMY",
														VistaGuitarTranscoder::GetTypeString(),
			                                           "retrieve the state of the whammy bar" )
   {
   }

	float GetValue( const VistaSensorMeasure *pMeasure ) const
   {
		float nRet = 0;
	   GetValue( pMeasure, nRet );
	   return nRet;
   }

   bool GetValue( const VistaSensorMeasure *pMeasure, float &nValue ) const
   {
	   if(!pMeasure)
		   return false;

	   const guitar_hero_3_t *m
		          = reinterpret_cast<const guitar_hero_3_t*>(&(*pMeasure).m_vecMeasures[0]);

	   nValue = m->whammy_bar;

	   return true;
   }
};


// ##############################################################################
// PROPERTY GETTERS FOR DATA ACCESS USING TRANSCODERS
// ##############################################################################

namespace
{
    const std::string SsReflectionName("VistaWiimoteDriver::CWiiMoteParameters");

	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaWiimoteIndexedButtonGet,
		new VistaWiimoteAccelerationGet,
		new VistaWiimotePosGet,
		new VistaWiimoteIRMarkerGet( 0, "IR_MARKER_0", "position of marker 0 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteIRMarkerGet( 1, "IR_MARKER_1", "position of marker 1 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteIRMarkerGet( 2, "IR_MARKER_2", "position of marker 2 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteIRMarkerGet( 3, "IR_MARKER_3", "position of marker 3 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_ONE, "BTN_1", "state of button labeled '1'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_TWO, "BTN_2", "state of button labeled '2'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_MINUS, "BTN_MINUS", "state of button labeled '-'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_PLUS, "BTN_PLUS", "state of button labeled '+'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_ONE, "BTN_HOME", "state of button labeled 'home'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_A, "BTN_A", "state of button labeled 'A'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_B, "BTN_B", "state of button labeled 'B' (fire)"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_UP, "BTN_UP", "state of button on cross direction up"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_LEFT, "BTN_LEFT", "state of button on cross direction left"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_RIGHT, "BTN_RIGHT", "state of button on cross direction right"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_DOWN, "BTN_DOWN", "state of button on cross direction down"),
		new VistaNunchukPaddleGet,
		new VistaNunchukAccelerationGet,
		new VistaNunchukGravityGet,
		new VistaNunchukButtonGet,
		new VistaNunchukSingleButtonGet(0, "BTN_Z", "retrieve button C"),
		new VistaNunchukSingleButtonGet(1, "BTN_C", "retrieve button Z"),
		new VistaNunchukOrientationGet,
		new VistaClassicPaddleGet( VistaClassicPaddleGet::PAD_LEFT,
				                    "LPADDLE", "retrieve left paddle as v3"),
		new VistaClassicPaddleGet( VistaClassicPaddleGet::PAD_RIGHT,
		                            "LPADDLE", "retrieve right paddle as v3"),
		new VistaClassicButtonGet,
		new VistaClassicShoulderGet( VistaClassicShoulderGet::SHOULDER_LEFT, "L_SHOULDER",
				                      "retrieve button state of left shoulder button"),
		new VistaClassicShoulderGet( VistaClassicShoulderGet::SHOULDER_RIGHT, "R_SHOULDER",
									  "retrieve button state of right shoulder button"),
		new VistaStatusBatteryGet,
		new VistaStatusExpansionStateGet,

		new VistaGuitarPaddleGet,
		new VistaGuitarButtonGet,
		new VistaGuitarWhammyGet,

		NULL
	};


	IVistaPropertyGetFunctor *SaParameterGetter[] =
	{
			new TVistaPropertyGet<bool,
						VistaWiimoteDriver::CWiiMoteParameters,
						VistaProperty::PROPT_BOOL> (
						"DOESREPORTACCELERATION",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::GetDoesAccelerationReports,
						"returns the state of reporting acceleration values from wiimote"),
				new TVistaPropertyGet<bool,
						VistaWiimoteDriver::CWiiMoteParameters,
						VistaProperty::PROPT_BOOL> (
						"DOESREPORTIR",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::GetDoesIRReports,
						"returns the state of reporting IR values from wiimote"),
				new TVistaPropertyGet<float,
						VistaWiimoteDriver::CWiiMoteParameters,
						VistaProperty::PROPT_DOUBLE> (
						"SMOOTHALPHA",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::GetSmoothAlpha,
						"returns the value of the currently used smoothing alpha"),
				new TVistaPropertyGet<int,
						VistaWiimoteDriver::CWiiMoteParameters,
						VistaProperty::PROPT_INT> (
						"ACCELERATIONTHRESHOLD",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::GetAccelerationThreshold,
						"returns the value of the currently used threshold for reporting accelaration"),
				new TVistaPropertyGet<int,
						VistaWiimoteDriver::CWiiMoteParameters,
						VistaProperty::PROPT_INT> (
						"WIIMOTE_ID",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::GetWaitTimeout,
						"returns the value of the currently used timeout for connecting (in secs)"),
				new TVistaPropertyGet<std::string,
						VistaWiimoteDriver::CWiiMoteParameters,
						VistaProperty::PROPT_STRING> (
						"WAITTIMEOUT",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::GetWiiMoteId,
						"returns the current wiimote id"),
			NULL
	};

	IVistaPropertySetFunctor *SaParameterSetter[] =
	{
			new TVistaPropertySet<bool, bool,
						VistaWiimoteDriver::CWiiMoteParameters> (
						"DOESREPORTACCELERATION",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::SetDoesAccelerationReports,
						&VistaAspectsConversionStuff::ConvertToBool,
						"enables / disables the reporting of acceleration values from the wiimote"),
			new TVistaPropertySet<bool, bool,
						VistaWiimoteDriver::CWiiMoteParameters> (
						"DOESREPORTIR",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::SetDoesIRReports,
						&VistaAspectsConversionStuff::ConvertToBool,
						"enables / disables the reporting of IR values from the wiimote"),
				new TVistaPropertySet<float, float,
						VistaWiimoteDriver::CWiiMoteParameters> (
						"SMOOTHALPHA",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::SetSmoothAlpha,
						&VistaAspectsConversionStuff::ConvertToFloat1,
						"sets the smoothing alpha to use for orientation values"),
				new TVistaPropertySet<int, int,
						VistaWiimoteDriver::CWiiMoteParameters> (
						"ACCELERATIONTHRESHOLD",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::SetAccelerationThreshold,
						&VistaAspectsConversionStuff::ConvertToInt,
						"sets the threshold for reporting of acceleration changes"),
				new TVistaPropertySet<int, int,
						VistaWiimoteDriver::CWiiMoteParameters> (
						"WAITTIMEOUT",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::SetWaitTimeout,
						&VistaAspectsConversionStuff::ConvertToInt,
						"sets the timeout to use for waiting during connect"),
				new TVistaPropertySet<const std::string&, std::string,
						VistaWiimoteDriver::CWiiMoteParameters> (
						"WIIMOTE_ID",
						SsReflectionName,
						&VistaWiimoteDriver::CWiiMoteParameters::SetWiiMoteId,
						&VistaAspectsConversionStuff::ConvertToString,
						"sets the current wiimote id"),
			NULL
	};

	class VistaWiimoteDriverCreationMethod : public IVistaDriverCreationMethod
	{
	public:
		virtual IVistaDeviceDriver *operator()()
		{
			return new VistaWiimoteDriver;
		}

		virtual void OnUnload()
		{
			// call superclass OnUnload()
			IVistaDriverCreationMethod::OnUnload();
			// now unload all getters for the parameters...

			for( IVistaPropertyGetFunctor **pGet = SaParameterGetter; *pGet != NULL; ++pGet)
				delete *pGet;
			for( IVistaPropertySetFunctor **pSet = SaParameterSetter; *pSet != NULL; ++pSet)
				delete *pSet;
		}
	};

	VistaWiimoteDriverCreationMethod *g_SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaWiimoteDriver::GetDriverFactoryMethod()
{
	if( g_SpFactory == NULL )
	{
		g_SpFactory = new VistaWiimoteDriverCreationMethod;

		// register one unnamed sensor type
		// todo: better estimate for update frequency!
		g_SpFactory->RegisterSensorType( "WIIMOTE",
										 sizeof(_wiimoteMeasure),
										 100, new VistaWiimoteTranscodeFactory,
										 VistaWiimoteTranscoder::GetTypeString() );
		g_SpFactory->RegisterSensorType( "GUITAR",
										 sizeof(guitar_hero_3_t),
										 100, new VistaGuitarTranscodeFactory,
										 VistaGuitarTranscoder::GetTypeString() );
		g_SpFactory->RegisterSensorType( "NUNCHUK",
										 sizeof(nunchuk_t),
										 100, new VistaNunchukTranscodeFactory,
										 VistaNunchukTranscoder::GetTypeString() );
		g_SpFactory->RegisterSensorType( "CLASSIC",
										 sizeof(classic_ctrl_t),
										 100, new VistaClassicTranscodeFactory,
										 VistaClassicTranscoder::GetTypeString() );
		g_SpFactory->RegisterSensorType( "STATUS",
										 sizeof(_wiistateMeasure),
										 100, new VistaStatusTranscodeFactory,
										 VistaStatusTranscoder::GetTypeString() );
	}
	return g_SpFactory;

}



// ****************************************************************************
// PARAMETER API IMPLEMENTATION
// ****************************************************************************

namespace
{

	class CWiiMoteParameterCreate : public VistaDriverGenericParameterAspect::IContainerCreate
	{
	public:
		CWiiMoteParameterCreate( VistaWiimoteDriver* pDriver )
		: m_pDriver( pDriver )
		{
		}

		virtual VistaDriverGenericParameterAspect::IParameterContainer *CreateContainer()
		{
			return new VistaWiimoteDriver::CWiiMoteParameters( m_pDriver );
		}

		virtual bool DeleteContainer( VistaDriverGenericParameterAspect::IParameterContainer *pCont )
		{
			delete pCont;
			return true;
		}
	private:
		VistaWiimoteDriver* m_pDriver;
	};


	// helper functions
	int FindCorrectWiimote( wiimote **motes, int nNum, const std::string &strId )
	{
		for(int n=0; n < nNum; ++n)
		{
#if !defined(WIN32)
			if(motes[n] && ( std::string(motes[n]->bdaddr_str) == strId ) )
				return n;
#else
			/** @todo how to identify those? */
			if( motes[n] && ( motes[n]->dev_handle ) )
				return n;
#endif
		}
		return -1;
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SYMBOLIC GETTERS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



REFL_IMPLEMENT(VistaWiimoteDriver::CWiiMoteParameters,
		       VistaDriverGenericParameterAspect::IParameterContainer);

VistaWiimoteDriver::CWiiMoteParameters::CWiiMoteParameters( VistaWiimoteDriver* pDriver )
: VistaDriverGenericParameterAspect::IParameterContainer(),
  m_pDriver( pDriver ),
  m_bDoesAcc(true),
  m_bDoesIR(false),
  m_nSmoothAlpha(0.0f),
  m_nAccelThreshold(0),
  m_nWaitTimeout(5),
  m_pLock( new VistaMutex )
{}

std::string VistaWiimoteDriver::CWiiMoteParameters::GetWiiMoteId() const
{
	return m_strWiiMoteId;
}


bool VistaWiimoteDriver::CWiiMoteParameters::GetDoesAccelerationReports() const
{
	return m_bDoesAcc;
}

bool VistaWiimoteDriver::CWiiMoteParameters::SetDoesAccelerationReports( bool bDoesIt )
{
	VistaMutexLock oLock( *m_pLock );
	if( bDoesIt != m_bDoesAcc )
	{
		m_bDoesAcc = bDoesIt;
		m_pDriver->SetParamChangeFlag();
		Notify( MSG_ACCELREPORT_CHG );
		return true;
	}
	return true;
}

bool VistaWiimoteDriver::CWiiMoteParameters::GetDoesIRReports() const
{
	return m_bDoesIR;
}

bool VistaWiimoteDriver::CWiiMoteParameters::SetDoesIRReports( bool bDoesIt )
{
	VistaMutexLock oLock( *m_pLock );
	if( bDoesIt != m_bDoesIR )
	{
		m_bDoesIR = bDoesIt;
		m_pDriver->SetParamChangeFlag();
		Notify( MSG_IRREPORT_CHG );
		return true;
	}
	return true;
}

bool VistaWiimoteDriver::CWiiMoteParameters::SetWiiMoteId( const std::string &strWiiMoteId )
{
	VistaMutexLock oLock( *m_pLock );
	if( m_pDriver->GetIsConnected() )
	{
		std::cout << "[WiiMoteParameters::SetWiiMoteId]: Already connected, can't set ID anymore" << std::endl;
		return false;
	}
	return compAssignAndNotify<std::string>(strWiiMoteId, m_strWiiMoteId, *this, MSG_MOTEID_CHG);
}

float VistaWiimoteDriver::CWiiMoteParameters::GetSmoothAlpha() const
{
	return m_nSmoothAlpha;
}

bool  VistaWiimoteDriver::CWiiMoteParameters::SetSmoothAlpha( float nAlpha )
{
	VistaMutexLock oLock( *m_pLock );
	if( nAlpha != m_nSmoothAlpha )
	{
		m_nSmoothAlpha = nAlpha;
		m_pDriver->SetParamChangeFlag();
		Notify( MSG_SMOOTHALPHA_CHG );
		return true;
	}
	return false;
}

int VistaWiimoteDriver::CWiiMoteParameters::GetAccelerationThreshold() const
{
	return m_nAccelThreshold;
}

bool VistaWiimoteDriver::CWiiMoteParameters::SetAccelerationThreshold( int nThreshold )
{
	VistaMutexLock oLock( *m_pLock );
	if( nThreshold != m_nAccelThreshold )
	{
		m_nAccelThreshold = nThreshold;
		m_pDriver->SetParamChangeFlag();
		Notify( MSG_ACCELTHRESHOLD_CHG );
		return true;
	}
	return false;
}

int VistaWiimoteDriver::CWiiMoteParameters::GetWaitTimeout() const
{
	return m_nWaitTimeout;
}

bool VistaWiimoteDriver::CWiiMoteParameters::SetWaitTimeout( int nTimeoutInSecs )
{
	VistaMutexLock oLock( *m_pLock );
	if( m_pDriver->GetIsConnected() )
	{
		std::cout << "[WiiMoteParameters::SetWiiMoteId]: Already connected, can't set wait timeout anymore" << std::endl;
		return false;
	}
	return compAssignAndNotify<int>( nTimeoutInSecs, m_nWaitTimeout, *this, MSG_WAITTIMEOUT_CHG);
}


// ############################################################################
// RUMBLE FORCE FEEDBACK
// ############################################################################

class CWiimoteRumble : public IVistaDriverForceFeedbackAspect
{
public:
	CWiimoteRumble()
	: IVistaDriverForceFeedbackAspect(),
	  m_pMote(NULL)
	{

	}

	/**
	 * Low-level API to apply forces, this should be called
	 * only in very simple cases, for example vibrating joysticks. For more advanced
	 * support, check the SetForceConstraintModel() API
	 * @see SetForceConstraintModel()
	 * @param v3Force the force vector to set on the device API (dir+length)
	 * @param qTorque the angular force to apply, not all devices may support this
	 */
	virtual bool SetForce( const VistaVector3D   & v3Force,
						   const VistaQuaternion &qTorque )
	{
		if(!m_pMote)
			return false;

		wiiuse_rumble( m_pMote,
				      (v3Force.GetLengthSquared() > std::numeric_limits<float>::epsilon() ? 1:0));
		return true;
	}

	/**
	 * @return the number of DOF for force input (0 for none)
	 */
	virtual int GetNumInputDOF() const  { return 0; }

	/**
	 * @return the number of DOF for force output (0 for none)
	 */
	virtual int GetNumOutputDOF() const { return 1; }

	/**
	 * Returns the maximum stiffness of this device.
	 * @return 0 when this parameter is not present, > 0 else
	 */
	virtual float GetMaximumStiffness() const { return 0; }

	/**
	 * Returns the maximal force to be output by the device.
	 * @return 0 for not applicable, > 0 else
	 */
	virtual float GetMaximumForce() const { return 0; }

	/**
	 * Enable / Disable the force output. Calls to SetForce() can
	 * still be done in state disabled, but have no effect.
	 */
	virtual bool SetForcesEnabled(bool bEnabled) { return true; }

	/**
	 * @return true when forces are enabled for this aspect, false else.
	 * @see SetForcesEnabled()
	 */
	virtual bool GetForcesEnabled() const { return true; }


	wiimote_t *m_pMote;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWiimoteDriver::VistaWiimoteDriver()
: IVistaDeviceDriver(),
  m_pInfo(new VistaDriverInfoAspect ),
//  m_pUpdate(NULL),
  m_pProtocol( new IVistaDriverProtocolAspect ),
  m_pMappingAspect( new VistaDriverSensorMappingAspect ),  
  m_nWiimoteId(~0),
  m_nNunchukId(~0),
  m_nStatusId(~0),
  m_nClassicId(~0),
  m_pMote(NULL),
  m_nState(ST_NOEXP),
  m_bConnected(false),
  m_pRumble(new CWiimoteRumble),
  m_bParamChangeFlag( false )
{
	m_pParams = new VistaDriverGenericParameterAspect( new CWiiMoteParameterCreate( this ) );
	m_pThreadAspect = new VistaDriverThreadAspect( this );
	SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);
	RegisterAspect( m_pInfo );
	RegisterAspect( m_pProtocol );
	RegisterAspect( m_pMappingAspect );
	RegisterAspect( m_pParams );
	RegisterAspect( m_pRumble );
	RegisterAspect( m_pThreadAspect );

	m_pMappingAspect->RegisterType( "WIIMOTE",
			                        sizeof(_wiimoteMeasure),
			                        100,
			                        new VistaWiimoteTranscodeFactory );

	m_pMappingAspect->RegisterType( "GUITAR",
			                        sizeof(guitar_hero_3_t),
			                        100,
			                        new VistaGuitarTranscodeFactory );

	m_pMappingAspect->RegisterType( "NUNCHUK",
			                        sizeof(nunchuk_t),
			                        100,
			                        new VistaNunchukTranscodeFactory );

	m_pMappingAspect->RegisterType( "CLASSIC",
			                        sizeof(classic_ctrl_t),
			                        100,
			                        new VistaClassicTranscodeFactory );

	m_pMappingAspect->RegisterType( "STATUS",
			                        sizeof(_wiistateMeasure),
			                        100,
			                        new VistaStatusTranscodeFactory );
}

VistaWiimoteDriver::~VistaWiimoteDriver()
{
	UnregisterAspect( m_pInfo, false );
	delete m_pInfo;

	UnregisterAspect( m_pProtocol, false );
	delete m_pProtocol;

	UnregisterAspect( m_pMappingAspect, false );
	delete m_pMappingAspect;

	UnregisterAspect(m_pRumble, false);
	delete m_pRumble;

	if(m_pMote)
	{
		wiiuse_set_leds( m_pMote[0], WIIMOTE_LED_NONE );
		wiiuse_cleanup( m_pMote, 1 );
	}

}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaWiimoteDriver::Connect()
{
	if(m_bConnected)
		return true;

	std::cout << "VistaWiimoteDriver::Connect() -- put mote in discover mode.\n";
	CWiiMoteParameters *pParams = dynamic_cast<CWiiMoteParameters*>(m_pParams->GetParameterContainer());

#ifndef WIN32
	wiimote** wiimotes;
	wiimotes =  wiiuse_init(4); // 4 seems to be the max	

	int found = wiiuse_find(wiimotes, 4, pParams->GetWaitTimeout() );
	if (!found) {
		std::cout << "[WiimoteDriver::Connect]: No wiimotes found." << std::endl;
		return 0;
	}
	else
		std::cout << "[WiimoteDriver::Connect]: found " << found << " wiimotes" << std::endl;


	std::string strWiiMoteId = pParams->GetWiiMoteId();
	int nIndex = (strWiiMoteId.empty() ? 0 : FindCorrectWiimote( wiimotes, found, strWiiMoteId ));
	if(nIndex == -1)
		return false;

	m_pMote = wiiuse_init(1);

	// copy-in the correct/desired index
	memcpy( m_pMote[0], wiimotes[nIndex], sizeof( wiimote_t ) );

	for(int n=0; n < found; n++)
		// we can not call wiiuse_cleanup here,
		// as this will free AND disconnect the structure
		free( wiimotes[n] ); // give back some memory

	// give back the container memory
	free( wiimotes );
#else
	wiimote** wiimotes;
	wiimotes =  wiiuse_init(4);

	int found = wiiuse_find( wiimotes, 4, pParams->GetWaitTimeout() );
	if (!found) {
		std::cout << "[WiimoteDriver::Connect]: No wiimotes found." << std::endl;
		return 0;
	}
	else
		std::cout << "[WiimoteDriver::Connect]: found " << found << " wiimotes" << std::endl;


	std::string strWiiMoteId = pParams->GetWiiMoteId();
	int nIndex = VistaAspectsConversionStuff::ConvertToInt( strWiiMoteId );
	if( nIndex < 0 || nIndex > found )
	{
		std::cout << "[WiimoteDriver::Connect]: Wiimote index " << nIndex 
					<< " out of range" << std::endl;
		return false;
	}

	m_pMote = wiiuse_init(1);

	// copy-in the correct/desired index
	memcpy( m_pMote[0], wiimotes[nIndex], sizeof( wiimote_t ) );

	//@todo: why does this crash?
	//for( int n=0; n < found; n++ )
	//	// we can not call wiiuse_cleanup here,
	//	// as this will free AND disconnect the structure
	//	free( wiimotes[n] ); // give back some memory

	// give back the container memory
	//free( wiimotes );

	int iLed;
	switch( nIndex )
	{
		case 0:
			iLed = WIIMOTE_LED_1;
			break;
		case 1:
			iLed = WIIMOTE_LED_2;
			break;
		case 2:
			iLed = WIIMOTE_LED_3;
			break;
		case 3:
			iLed = WIIMOTE_LED_4;
			break;
		default:
			iLed = WIIMOTE_LED_NONE;
			break;
	}
	wiiuse_set_leds( m_pMote[0], iLed );
#endif

	// now connect to the wiimote of desire...
	if( wiiuse_connect(m_pMote, 1) == 1 )
	{
		// setup other parameters as given by the user:
		m_bConnected = true;
		m_pRumble->m_pMote = m_pMote[0];
		return SetupWiiMote();
	}
	else
	{
		// should cleanup
		wiiuse_cleanup(m_pMote,1);
		m_pMote = NULL;
	}
	return false;
}


bool VistaWiimoteDriver::SetupWiiMote()
{
	CWiiMoteParameters *pParams = dynamic_cast<CWiiMoteParameters*>(m_pParams->GetParameterContainer());

	wiiuse_motion_sensing( m_pMote[0], pParams->GetDoesAccelerationReports() ? 1:0 );
	wiiuse_set_ir( m_pMote[0], pParams->GetDoesIRReports() ? 1:0);

	int nThreshold = pParams->GetAccelerationThreshold();
	wiiuse_set_accel_threshold( m_pMote[0],  nThreshold );
	if(nThreshold == 0)
		wiiuse_set_flags( m_pMote[0], 0, WIIUSE_ORIENT_THRESH );
	else
		wiiuse_set_flags( m_pMote[0], WIIUSE_ORIENT_THRESH, 0 );

	m_bParamChangeFlag = false;
	
	return true;
}


bool VistaWiimoteDriver::PhysicalEnable(bool bEnable)
{
	if(bEnable)
		DetermineSetup();
	else
	{
		m_nWiimoteId = m_nGuitarId = m_nNunchukId = m_nStatusId = m_nClassicId = ~0;
	}
	return true;
}


int VistaWiimoteDriver::DetermineSetup()
{
	// want normal mote?
	m_nWiimoteId = m_pMappingAspect->GetSensorId( m_pMappingAspect->GetTypeId("WIIMOTE"), 0 );
	m_nGuitarId  = m_pMappingAspect->GetSensorId( m_pMappingAspect->GetTypeId("GUITAR"), 0 );
	m_nNunchukId = m_pMappingAspect->GetSensorId( m_pMappingAspect->GetTypeId("NUNCHUK"), 0 );
	m_nStatusId  = m_pMappingAspect->GetSensorId( m_pMappingAspect->GetTypeId("STATUS"), 0 );
	m_nClassicId = m_pMappingAspect->GetSensorId( m_pMappingAspect->GetTypeId("CLASSIC"), 0 );

	return 0;
}


bool VistaWiimoteDriver::DoSensorUpdate(microtime nTs)
{
	if(!m_bConnected)
		return false;

	if( m_bParamChangeFlag )
		SetupWiiMote();

	bool bDoMoteRecord = false;//WIIUSE_USING_ACC( m_pMote[0] ) || WIIUSE_USING_IR( m_pMote[0] );

	if( wiiuse_poll(m_pMote, 1) )
	{
		// new values!
		switch( m_pMote[0]->event )
		{
		case WIIUSE_EVENT:
		{
			switch( m_pMote[0]->exp.type)
			{
				case EXP_CLASSIC:
				{
					RecordExtSensor<classic_ctrl_t>( nTs,
							                      m_nClassicId,
							                      reinterpret_cast<const classic_ctrl_t*>(
							                    		       &m_pMote[0]->exp.classic ) );
					break;
				}
				case EXP_GUITAR_HERO_3:
				{
					RecordExtSensor<guitar_hero_3_t>( nTs,
												  m_nGuitarId,
							                      reinterpret_cast<const guitar_hero_3_t*>(
							                    		       &m_pMote[0]->exp.gh3 ) );
					break;
				}
				case EXP_NUNCHUK:
				{
					RecordExtSensor<nunchuk_t>( nTs,
												 m_nNunchukId,
							                      reinterpret_cast<const nunchuk_t*>(
							                    		       &m_pMote[0]->exp.nunchuk ) );
					break;
				}
				default:
					break;
			}
			bDoMoteRecord = true;
			break;
		}
		case WIIUSE_STATUS:
		{
			RecordStateEvent( nTs );
			break;
		}
		case WIIUSE_DISCONNECT:
		case WIIUSE_UNEXPECTED_DISCONNECT:
		{
			std::cout << "DISCONNECT event.\n";
			RecordStateEvent( nTs );
			m_bConnected = false;
			m_pRumble->m_pMote = NULL;
			break;
		}
		case WIIUSE_NUNCHUK_INSERTED:
		{
			m_nState = m_nState | ST_NUNCHUK;
			std::cout << "Extension NUNCHUK INSERT event.\n";
			RecordStateEvent( nTs );
			break;
		}
		case WIIUSE_CLASSIC_CTRL_INSERTED:
		{
			m_nState = m_nState | ST_CLASSIC;
			std::cout << "Extension CLASSIC INSERT event.\n";
			RecordStateEvent( nTs );
			break;
		}
		case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
		{
			m_nState = m_nState | ST_GUITAR;
			std::cout << "Extension GUITAR INSERT event.\n";
			RecordStateEvent( nTs );
			break;
		}
		case WIIUSE_NUNCHUK_REMOVED:
		{
			m_nState = m_nState & ~ST_NUNCHUK;
			std::cout << "Extension NUNCHUK INSERT event.\n";
			RecordStateEvent( nTs );
			break;
		}
		case WIIUSE_CLASSIC_CTRL_REMOVED:
		{
			m_nState = m_nState & ~ST_CLASSIC;
			std::cout << "Extension CLASSIC INSERT event.\n";
			RecordStateEvent( nTs );
			break;
		}
		case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
		{
			m_nState = m_nState & ~ST_GUITAR;
			std::cout << "Extension GUITAR INSERT event.\n";
			RecordStateEvent( nTs );
			break;
		}
		default:
			break;
		}
	}

	if(bDoMoteRecord)
		RecordMoteEvent( nTs );

	IVistaDriverForceFeedbackAspect::IForceAlgorithm *pFF;
	if( (pFF=m_pRumble->GetForceAlgorithm()) )
	{
		VistaVector3D crPos((float)m_pMote[0]->ir.x, (float)m_pMote[0]->ir.y, (float)m_pMote[0]->ir.z);
		VistaVector3D crVel = crPos + VistaVector3D( m_pMote[0]->accel.x, m_pMote[0]->accel.y, m_pMote[0]->accel.z );
		VistaEulerAngles oAngles( m_pMote[0]->orient.roll, m_pMote[0]->orient.pitch, m_pMote[0]->orient.yaw );
		VistaQuaternion q( oAngles );

		VistaVector3D rV3;
		VistaQuaternion rQ;
		if(pFF->UpdateForce(double(nTs), crPos, crVel, q, rV3, rQ ))
		{
			m_pRumble->SetForce( rV3, rQ );
		}
	}
	return true;
}


bool VistaWiimoteDriver::RecordMoteEvent( microtime nTs )
{
	if(m_nWiimoteId == ~0)
		return false;
	// the mote event is somewhat special, as we need to distill the
	// relevant information out of a pool of the complete event
	// structure.

	// first: get some memory.
	VistaDeviceSensor *pSensor = GetSensorByIndex(m_nWiimoteId);

	MeasureStart(m_nWiimoteId, nTs);
	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the IRMan
	_wiimoteMeasure *s = reinterpret_cast<_wiimoteMeasure*>(&( *pM ).m_vecMeasures[0]);

	// now copy field-wise
	memcpy( &s->m_Acceleration, &m_pMote[0]->accel, sizeof( vec3b_t ) );
	memcpy( &s->m_Orientation, &m_pMote[0]->orient, sizeof( orient_t) );
	memcpy( &s->m_GravityForce, &m_pMote[0]->gforce, sizeof( gforce_t ));
	memcpy( &s->m_IR, &m_pMote[0]->ir, sizeof( ir_t ));
	memcpy( &s->m_Acceleration_calib, &m_pMote[0]->accel_calib, sizeof( accel_t ) );

	s->m_buttons = m_pMote[0]->btns;

	MeasureStop(m_nWiimoteId);
	pSensor->SetUpdateTimeStamp( nTs );

	return true;
}

bool VistaWiimoteDriver::RecordStateEvent( microtime nTs )
{
	if(m_nStatusId == ~0)
		return false;
	// the mote event is somewhat special, as we need to distill the
	// relevant information out of a pool of the complete event
	// structure.

	// first: get some memory.
	VistaDeviceSensor *pSensor = GetSensorByIndex(m_nStatusId);

	MeasureStart(m_nStatusId, nTs);
	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the IRMan
	_wiistateMeasure *s = reinterpret_cast<_wiistateMeasure*>(&( *pM ).m_vecMeasures[0]);

	// now copy field-wise
	s->m_nState        = m_pMote[0]->state;
	s->m_nBatteryLevel = m_pMote[0]->battery_level;
	s->m_cLEDs         = m_pMote[0]->leds;

	s->m_nExpansionState  = m_nState;

	MeasureStop(m_nStatusId);
	pSensor->SetUpdateTimeStamp(nTs);
	return true;
}

bool VistaWiimoteDriver::EnableWiiMoteSensor(unsigned int nMaxSlotsToRead,
        float nMaxHistoryAccessTimeInMsec)
{
	return EnableSensorTyped("WIIMOTE", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableNunchukSensor(unsigned int nMaxSlotsToRead,
        float nMaxHistoryAccessTimeInMsec)
{
	return EnableSensorTyped("NUNCHUK", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableClassicSensor(unsigned int nMaxSlotsToRead,
        float nMaxHistoryAccessTimeInMsec)
{
	return EnableSensorTyped("CLASSIC", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableGuitarSensor(unsigned int nMaxSlotsToRead,
        float nMaxHistoryAccessTimeInMsec)
{
	return EnableSensorTyped("GUITAR", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableStateSensor(unsigned int nMaxSlotsToRead,
        float nMaxHistoryAccessTimeInMsec)
{
	return EnableSensorTyped("STATUS", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

void VistaWiimoteDriver::SetParamChangeFlag()
{
	m_bParamChangeFlag = true;
}


bool VistaWiimoteDriver::EnableSensorTyped( const std::string &strType,
									 unsigned int nMaxSlotsToRead,
		                             float nMaxHistoryAccessTimeInMsec)
{
	if(m_pMappingAspect->GetIsType(strType) == false)
		return false;

	 VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	 IVistaMeasureTranscoderFactory *pFac
	         = VistaWiimoteDriver::GetDriverFactoryMethod()->GetTranscoderFactoryForSensor(strType);

	 pSensor->SetMeasureTranscode( pFac->CreateTranscoder() );

	 unsigned int nId = AddDeviceSensor( pSensor );
	 unsigned int nSensorType = VistaWiimoteDriver::GetDriverFactoryMethod()->GetTypeFor(strType);
	 // only inspect 1 slot in 1 msec
	 VddUtil::SetupSensorHistory( this, pSensor,
								  VistaWiimoteDriver::GetDriverFactoryMethod(),
			                     strType, nMaxSlotsToRead, (int)nMaxHistoryAccessTimeInMsec );
	 m_pMappingAspect->SetSensorId( nSensorType, 0, nId );
	 return true;
}

VistaWiimoteDriver::CWiiMoteParameters *VistaWiimoteDriver::GetParameters() const
{
	return dynamic_cast<VistaWiimoteDriver::CWiiMoteParameters*>( m_pParams->GetParameterContainer() );
}

bool VistaWiimoteDriver::GetIsConnected() const
{
	return m_bConnected;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

