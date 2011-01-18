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

#include "VistaIRManDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceDriversOut.h>

#include "../VistaDriverConnectionAspect.h"
#include "../VistaDriverMeasureHistoryAspect.h"
#include "../VistaDeviceSensor.h"


#include <VistaInterProcComm/Connections/VistaConnectionSerial.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


class VistaIRManTranscode : public IVistaMeasureTranscode
{
public:
	VistaIRManTranscode()
	{
		m_nNumberOfScalars = 6;
	}

	static std::string GetTypeString() { return "VistaIRManTranscode"; }
	REFL_INLINEIMP(VistaIRManTranscode, IVistaMeasureTranscode);

};


class VistaIRManTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaIRManTranscode;
	}
};


class VistaIRManKeyCodeGet : public IVistaMeasureTranscode::TTranscodeValueGet<_sIRManSample>
{
public:
	VistaIRManKeyCodeGet( const std::string &sPropName,
		const std::string &sClassName,
		const std::string &sDescription)
		: IVistaMeasureTranscode::TTranscodeValueGet<_sIRManSample>(sPropName, sClassName, sDescription)
	{}

	virtual _sIRManSample GetValue(const VistaSensorMeasure *m)    const
	{
		_sIRManSample *s = (_sIRManSample*)(&((*m).m_vecMeasures[0]));
		return *s;
	}

	virtual bool GetValue(const VistaSensorMeasure *m, _sIRManSample &out) const
	{
		out = GetValue(m);
		return true;
	}
};

class VistaIRManKeyCodeIntGet : public IVistaMeasureTranscode::TTranscodeValueGet<uint64>
{
public:
	VistaIRManKeyCodeIntGet( const std::string &sPropName,
		const std::string &sClassName,
		const std::string &sDescription)
		: IVistaMeasureTranscode::TTranscodeValueGet<uint64>(sPropName, sClassName, sDescription)
	{}

	virtual uint64 GetValue(const VistaSensorMeasure *m)    const
	{
		_sIRManSample *s = (_sIRManSample*)(&((*m).m_vecMeasures[0]));

		uint64 n = 0;

		unsigned char *c = (unsigned char*)&n;
		for(int i=2; i < 8; ++i)
			c[i] = s->m_acKeyCode[i-2];

		return n;
	}

	virtual bool GetValue(const VistaSensorMeasure *m, uint64 &out) const
	{
		out = GetValue(m);
		return true;
	}
};

namespace
{
	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaIRManKeyCodeGet("KEYCODE",
								  VistaIRManTranscode::GetTypeString(),
								  "a 6-byte key code from the IR"),
		new VistaIRManKeyCodeIntGet("KEY",
								VistaIRManTranscode::GetTypeString(),
								"the 6-byte keycode as int64"),
		NULL
	};
}

class IRManAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	virtual bool operator()(VistaConnection *pConnection)
	{
		if(!pConnection->GetIsOpen())
			if(!pConnection->Open())
				return false;


		pConnection->SetIsBlocking(false);
		char c;
		while( pConnection->ReadRawBuffer( &c, 1 ) > 0 ); // semicolon on purpose

		// set back to blocking I/O
		pConnection->WriteString( "IR" );
		std::string sResponse;
		pConnection->SetIsBlocking(true);
		pConnection->ReadString( sResponse, 2 );
		if(sResponse != "OK")
		{
			// deep, deep trouble ;)
			vdderr << "IRMAN DID NOT RESPOND CORRECTLY\n";
			pConnection->Close();
			return false;
		}
		else
		{
			vdderr << "IRMAN FOUND\n";
		}
		pConnection->SetIsBlocking(false);
#if defined(WIN32)
		pConnection->WaitForIncomingData();
#endif

		return false;
	}
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaIRManDriver::VistaIRManDriver()
		: IVistaDeviceDriver(),
		  m_pConnectionAspect(new VistaDriverConnectionAspect)
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_CONNECTION_THREADED );

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	AddDeviceSensor(pSensor);

	pSensor->SetMeasureTranscode( new VistaIRManTranscode );

	m_pConnectionAspect->SetConnection( 0, NULL, "MAIN", true );
	m_pConnectionAspect->SetUpdateConnectionIndex(0);
	m_pConnectionAspect->SetAttachSequence(0, new IRManAttachSequence);

	RegisterAspect( m_pConnectionAspect );
}

VistaIRManDriver::~VistaIRManDriver()
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	RemDeviceSensor(pSensor);
	delete pSensor;

	VistaConnection *pCon = m_pConnectionAspect->GetConnection(0);
	pCon->Close();

	UnregisterAspect( m_pConnectionAspect, false );
	delete m_pConnectionAspect;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
unsigned int VistaIRManDriver::GetSensorMeasureSize() const
{
	return sizeof(_sIRManSample);
}

namespace
{
	VistaIRManCreateMethod *SpCreation = NULL;
}

IVistaDriverCreationMethod *VistaIRManDriver::GetDriverFactoryMethod()
{
	if(SpCreation == NULL)
	{
		SpCreation = new VistaIRManCreateMethod;
		SpCreation->RegisterSensorType( "",
				                        sizeof(_sIRManSample),
				                        20,
				                        new VistaIRManTranscodeFactory,
				                        VistaIRManTranscode::GetTypeString() );
	}
	return SpCreation;
}

bool VistaIRManDriver::DoSensorUpdate( microtime nTs )
{
	// first of all, indicate the beginning of a sampling
	// for the sensor with index 0 at time nTs
	m_pConnectionAspect->GetConnection(0)->SetIsBlocking(true);

	MeasureStart(0, nTs);
	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the IRMan
	_sIRManSample *s = reinterpret_cast<_sIRManSample*>(&( *pM ).m_vecMeasures[0]);

	m_pConnectionAspect->GetConnection(0)->ReadRawBuffer( s, sizeof(_sIRManSample) );

	// we are done. Indicate that to the history
	MeasureStop(0);

	m_pConnectionAspect->GetConnection(0)->SetIsBlocking(false);

#if defined(WIN32)
	m_pConnectionAspect->GetConnection(0)->WaitForIncomingData();
#endif

	GetSensorByIndex(0)->SetUpdateTimeStamp(nTs);
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


