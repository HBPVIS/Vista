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

#include "VistaKeyboardDriver.h"
#include "VistaDeviceSensor.h"
#include "VistaDriverMeasureHistoryAspect.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaKeyboardMeasureTranscode : public IVistaMeasureTranscode
{
public:
	VistaKeyboardMeasureTranscode()
	{
		// inherited from measure transcode
		m_nNumberOfScalars = 2;
	}

	virtual ~VistaKeyboardMeasureTranscode() {}

	static std::string GetTypeString() { return "VistaKeyboardMeasureTranscode"; }

	REFL_INLINEIMP(VistaKeyboardMeasureTranscode, IVistaMeasureTranscode);
};


IVistaMeasureTranscode *VistaKeyboardDriverTranscodeFactory::CreateTranscoder()
{
	return new VistaKeyboardMeasureTranscode;
}

class VistaKeyboardScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaKeyboardScalarTranscode()
		: IVistaMeasureTranscode::CScalarDoubleGet("DSCALAR",
		VistaKeyboardMeasureTranscode::GetTypeString(),
								"keyboard strokes in comp (0,1) -> (key, mod)") {}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
								 double &dScalar,
								 unsigned int nIndex ) const
	{
		if(!pMeasure || nIndex > 2)
			return false;

		IVistaKeyboardDriver::_sKeyboardMeasure *m = (IVistaKeyboardDriver::_sKeyboardMeasure*)&(*pMeasure).m_vecMeasures[0];
		if(nIndex == 0)
			dScalar = m->m_nKey;
		else
			dScalar = m->m_nModifier;

		return true;
	}
};

class VistaKeyboardKeyTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
{
public:
	VistaKeyboardKeyTranscode(const std::string &strName, int nIdx)
		: IVistaMeasureTranscode::TTranscodeValueGet<int>(strName,
		VistaKeyboardMeasureTranscode::GetTypeString(),
								"keyboard strokes as int"),
	m_nIdx(nIdx)
	{}

	virtual int GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		IVistaKeyboardDriver::_sKeyboardMeasure *m = (IVistaKeyboardDriver::_sKeyboardMeasure*)&(*pMeasure).m_vecMeasures[0];
		if(m_nIdx == 0)
			return m->m_nKey;
		else
			return m->m_nModifier;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure,
								 int &dScalar ) const
	{
		dScalar = GetValue(pMeasure);
		return true;
	}

private:
	int m_nIdx;
};


static IVistaPropertyGetFunctor *SaGetter[] =
{
	new VistaKeyboardScalarTranscode,
	new VistaKeyboardKeyTranscode("KEY", 0),
	new VistaKeyboardKeyTranscode("MODIFIER", 1),
	NULL
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaKeyboardDriver::IVistaKeyboardDriver()
: IVistaDeviceDriver(sizeof(_sKeyboardMeasure))
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);
	VistaDeviceSensor *pSen = new VistaDeviceSensor;
	pSen->SetMeasureTranscode(new VistaKeyboardMeasureTranscode);

	AddDeviceSensor(pSen);
}


IVistaKeyboardDriver::~IVistaKeyboardDriver()
{
	VistaDeviceSensor *pSen = GetSensorByIndex(0);

	VistaKeyboardMeasureTranscode *pTC = dynamic_cast<VistaKeyboardMeasureTranscode*>(pSen->GetMeasureTranscode());
	delete pTC;
	pSen->SetMeasureTranscode(NULL);

	RemDeviceSensor(pSen);
	delete pSen;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

std::string IVistaKeyboardDriver::GetKeyboardTranscodeName()
{
	return VistaKeyboardMeasureTranscode::GetTypeString();
}

bool IVistaKeyboardDriver::UpdateKey(int nKey, int nModifier)
{
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));

	if(pM == NULL)
		return false;

	_sKeyboardMeasure *m = (_sKeyboardMeasure*)&(*pM).m_vecMeasures[0];
	m->m_nKey      = nKey;
	m->m_nModifier = nModifier;

	return true;
}


void IVistaKeyboardDriver::MeasureStart(microtime dTs)
{
	IVistaDeviceDriver::MeasureStart(0, dTs);
}

void IVistaKeyboardDriver::MeasureStop()
{
	IVistaDeviceDriver::MeasureStop(0);
}


unsigned int IVistaKeyboardDriver::GetSensorMeasureSize() const
{
	return sizeof(_sKeyboardMeasure);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


