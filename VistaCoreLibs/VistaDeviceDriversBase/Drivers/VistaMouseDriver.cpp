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

#include "VistaMouseDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaMouseDriverMeasureTranscode : public IVistaMeasureTranscode
{
public:
	VistaMouseDriverMeasureTranscode();
	virtual ~VistaMouseDriverMeasureTranscode();

	static std::string GetTypeString();

	REFL_DECLARE
};

VistaMouseDriverMeasureTranscode::VistaMouseDriverMeasureTranscode()
{
	// inherited as protected member
	m_nNumberOfScalars = IVistaMouseDriver::BT_LAST;
}

VistaMouseDriverMeasureTranscode::~VistaMouseDriverMeasureTranscode()
{}

std::string VistaMouseDriverMeasureTranscode::GetTypeString()
{
	return "VistaMouseDriverMeasureTranscode";
}

REFL_IMPLEMENT(VistaMouseDriverMeasureTranscode, IVistaMeasureTranscode);


// ######################################################################
// TRANSCODER FACTORY
// ######################################################################

IVistaMeasureTranscode *VistaMouseDriverTranscodeFactory::CreateTranscoder()
{
	return new VistaMouseDriverMeasureTranscode;
}

void VistaMouseDriverTranscodeFactory::DestroyTranscoder( IVistaMeasureTranscode *trans )
{
	delete trans;
}


class VistaMousePosTranscode : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaMousePosTranscode()
		: IVistaMeasureTranscode::CV3Get("POSITION",
		VistaMouseDriverMeasureTranscode::GetTypeString(),
								"mouse pixel coords in comp (0,1) -> (x,y)") {}

	virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		IVistaMouseDriver::_sMouseMeasure *m = (IVistaMouseDriver::_sMouseMeasure*)&(*pMeasure).m_vecMeasures[0];

		return VistaVector3D(float(m->m_nX),
							  float(m->m_nY), 0);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
	{
		v3Pos = GetValue(pMeasure);
		return true;
	}
};

class VistaMouseScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaMouseScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("DSCALAR",
		VistaMouseDriverMeasureTranscode::GetTypeString(),
								"mouse buttons in comp (0,3) -> (l,m,r,w)") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex >= (IVistaMouseDriver::BT_LAST))
			return false;

		IVistaMouseDriver::_sMouseMeasure *m = (IVistaMouseDriver::_sMouseMeasure*)&(*pMeasure).m_vecMeasures[0];

		dScalar = m->m_nButtons[nIndex];
		return true;
	}
};

class VistaMouseWheelStateTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
public:
	VistaMouseWheelStateTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<int>("WHEEL_STATE",
		VistaMouseDriverMeasureTranscode::GetTypeString(),
								"wheel state as an int") {}

	virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		IVistaMouseDriver::_sMouseMeasure *m = (IVistaMouseDriver::_sMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
		return m->m_nButtons[IVistaMouseDriver::BT_WHEEL_STATE];
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nState) const
	{
		nState = GetValue(pMeasure);
		return true;
	}
};

class VistaMouseWheelDirTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
public:
	VistaMouseWheelDirTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<int>("WHEEL_DIR",
		VistaMouseDriverMeasureTranscode::GetTypeString(),
								"wheel dir (-1 up, 1 down)") {}

	virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		IVistaMouseDriver::_sMouseMeasure *m = (IVistaMouseDriver::_sMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
		return m->m_nButtons[IVistaMouseDriver::BT_WHEEL_DIR];
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nState) const
	{
		nState = GetValue(pMeasure);
		return true;
	}
};

class VistaMousePixelPosTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
public:
	VistaMousePixelPosTranscode(int nPosId,
		const std::string &strLabel,
		const std::string &strDesc)
		: IVistaMeasureTranscode::TTranscodeValueGet<int>(strLabel,
		VistaMouseDriverMeasureTranscode::GetTypeString(), strDesc),
		m_nPosId(nPosId)
	{}

	virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		IVistaMouseDriver::_sMouseMeasure *m = (IVistaMouseDriver::_sMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
		return m_nPosId ? m->m_nY : m->m_nX;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nState) const
	{
		nState = GetValue(pMeasure);
		return true;
	}

	int m_nPosId;
};


class VistaMouseButtonTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
{
public:
	VistaMouseButtonTranscode(IVistaMouseDriver::eBt Bt,
		const std::string &strLabel,
		const std::string &strDesc)
		: IVistaMeasureTranscode::TTranscodeValueGet<bool>(strLabel,
		VistaMouseDriverMeasureTranscode::GetTypeString(), strDesc),
		m_bt(Bt)
	{}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		IVistaMouseDriver::_sMouseMeasure *m = (IVistaMouseDriver::_sMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
		return m->m_nButtons[m_bt] ? true : false;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &nState) const
	{
		nState = GetValue(pMeasure);
		return true;
	}

	IVistaMouseDriver::eBt m_bt;
};

static IVistaPropertyGetFunctor *SapGetter[] =
{
	new VistaMousePosTranscode,
	new VistaMouseScalarTranscode,
	new VistaMouseWheelStateTranscode,
	new VistaMouseWheelDirTranscode,
	new VistaMousePixelPosTranscode(0, "X_POS", "x pos in pixel space"),
	new VistaMousePixelPosTranscode(1, "Y_POS", "y pos in pixel space"),
	new VistaMouseButtonTranscode(IVistaMouseDriver::BT_LEFT, "LEFT_BUTTON", "left button state (true: pressed)"),
	new VistaMouseButtonTranscode(IVistaMouseDriver::BT_RIGHT, "RIGHT_BUTTON", "right button state (true: pressed)"),
	new VistaMouseButtonTranscode(IVistaMouseDriver::BT_MIDDLE, "MIDDLE_BUTTON", "middle button state (true: pressed)"),
	NULL
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaMouseDriver::IVistaMouseDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);
}


IVistaMouseDriver::~IVistaMouseDriver()
{
	// as a kind of "service" delete all attached sensors
	// and remove from history.
	for(unsigned int n=0; n < GetNumberOfSensors(); ++n)
	{
		VistaDeviceSensor *pSen = GetSensorByIndex(n);
		RemDeviceSensor(pSen);
		GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder( pSen->GetMeasureTranscode() );
		pSen->SetMeasureTranscode(NULL);
		delete pSen;
	}
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
unsigned int IVistaMouseDriver::AddDeviceSensor(VistaDeviceSensor *pSensor)
{
	// add to parent driver
	if( pSensor->GetTypeHint() == ~0 )
	{
		pSensor->SetTypeHint( GetFactory()->GetTypeFor( "" ) );
	}
	if(!pSensor->GetMeasureTranscode())
	{
		pSensor->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );
	}
	unsigned int nIdx = IVistaDeviceDriver::AddDeviceSensor(pSensor);	
	return nIdx;
}

std::string IVistaMouseDriver::GetMouseTranscodeName()
{
	return VistaMouseDriverMeasureTranscode::GetTypeString();
}

bool IVistaMouseDriver::UpdateMousePosition(unsigned int nIdx, int x, int y)
{
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(nIdx));

	if(pM == NULL)
		return false;

	_sMouseMeasure *m = (_sMouseMeasure*)(&(*pM).m_vecMeasures[0]);
	m->m_nX = x;
	m->m_nY = y;
	return true;
}

bool IVistaMouseDriver::UpdateMouseButton(unsigned int nIdx, eBt nMouseBt,
										  double nPressed)
{
	if(nMouseBt >= BT_LAST)
		return false;

	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(nIdx));
	if(pM == NULL)
		return false;
	_sMouseMeasure *m = (_sMouseMeasure*)(&(*pM).m_vecMeasures[0]);
	m->m_nButtons[nMouseBt] = int(nPressed);

	return true;
}

