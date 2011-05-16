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

#include "VistaMIDIDriver.h"
#include "../VistaDeviceSensor.h"
#include "../VistaDriverMeasureHistoryAspect.h"
#include "../VistaDriverConnectionAspect.h"

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaMIDIDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
  virtual bool operator()(VistaConnection *pCon)
	{
	  pCon->Close();
	  return !pCon->GetIsOpen();
	}
};

class VistaMIDIAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
  virtual bool operator()(VistaConnection *pCon)
	{
	  if(!pCon->GetIsOpen())
		  return pCon->Open();

	  return true;
	}
};


class VistaMIDIDriverTranscode : public IVistaMeasureTranscode
{
public:
	VistaMIDIDriverTranscode() {
		// inherited as protected member
		m_nNumberOfScalars = 4;
	}

	~VistaMIDIDriverTranscode() {}
	static std::string GetTypeString() { return "VistaMIDIDriverTranscode"; }

	REFL_INLINEIMP(VistaMIDIDriverTranscode, IVistaMeasureTranscode);
};

class VistaMIDIDriverTypeGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	VistaMIDIDriverTypeGet()
	: IVistaMeasureTranscode::CUIntGet(
		"TYPE",
		VistaMIDIDriverTranscode::GetTypeString(),
		"The type of MIDI message (CC, PC, ...)" )
		{}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);

		// the first byte of a midi message is the status byte
		// of which the first 4 bit contain the type of the message
		return (m->status & 0xF0) >> 4;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, unsigned int &nType) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);
		nType = (m->status & 0xF0) >> 4;
		return true;
	}
};

class VistaMIDIDriverChannelGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	VistaMIDIDriverChannelGet()
	: IVistaMeasureTranscode::CUIntGet(
		"CHANNEL",
		VistaMIDIDriverTranscode::GetTypeString(),
		"The MIDI channel on which this message was sent (1-16)" )
		{}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);

		// the first byte of a midi message is the status byte
		// of which the last 4 bit contain the midi channel.
		// values are 0-15. midi channels are numbered 1-16, thus +1.
		return (m->status & 0x0F) + 1;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, unsigned int &nChannel) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);
		nChannel = (m->status & 0x0F) + 1;
		return true;
	}
};

class VistaMIDIDriverControllerGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	VistaMIDIDriverControllerGet()
	: IVistaMeasureTranscode::CUIntGet(
		"CONTROLLER",
		VistaMIDIDriverTranscode::GetTypeString(),
		"The CC controller number (0-119). The values 120-127 are reserved messages." )
		{}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);

		// the lower 7 bit of the the first data byte (second message byte)
		// of a midi message contain the controller number for a CC message.
		return (m->data0 & 0x7F);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, unsigned int &nController) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);
		nController = (m->data0 & 0x7F);
		return true;
	}
};

class VistaMIDIDriverValueGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	VistaMIDIDriverValueGet()
	: IVistaMeasureTranscode::CUIntGet(
		"VALUE",
		VistaMIDIDriverTranscode::GetTypeString(),
		"The CC control value (0-127)." )
		{}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);

		// the lower 7 bit of the the second data byte (third message byte)
		// of a midi message contain the control value for a CC message.
		return (m->data1 & 0x7F);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, unsigned int &nValue) const
	{
		_sMIDIMeasure *m = (_sMIDIMeasure*)&(pMeasure->m_vecMeasures[0]);
		nValue = (m->data1 & 0x7F);
		return true;
	}
};

static IVistaPropertyGetFunctor *SaGetter[] =
{
	new VistaMIDIDriverTypeGet,
	new VistaMIDIDriverChannelGet,
	new VistaMIDIDriverControllerGet,
	new VistaMIDIDriverValueGet
};

class VistaMIDIDriverTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaMIDIDriverTranscode;
	}
};


class VistaMIDIDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()()
	{
		return new VistaMIDIDriver;
	}
};

namespace
{
	VistaMIDIDriverCreationMethod *g_SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaMIDIDriver::GetDriverFactoryMethod()
{
	if( g_SpFactory == NULL )
	{
		g_SpFactory = new VistaMIDIDriverCreationMethod;

		// register one unnamed sensor type
		/** @todo : better estimate for update frequency! */
		g_SpFactory->RegisterSensorType( "",
										 sizeof(_sMIDIMeasure),
										 100, new VistaMIDIDriverTranscodeFactory,
										 VistaMIDIDriverTranscode::GetTypeString());
	}
	return g_SpFactory;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMIDIDriver::VistaMIDIDriver() :
  IVistaDeviceDriver(),
  m_pConnection(new VistaDriverConnectionAspect)
{
  SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);

  m_pConnection->SetConnection(0, NULL, "MAIN", true);
  RegisterAspect(m_pConnection);

  m_pConnection->SetDetachSequence(0, new VistaMIDIDetachSequence);
  m_pConnection->SetAttachSequence(0, new VistaMIDIAttachSequence);

  VistaDeviceSensor *pSen = new VistaDeviceSensor();
  pSen->SetMeasureHistorySize(10);
  pSen->SetMeasureTranscode( new VistaMIDIDriverTranscode );
  AddDeviceSensor( pSen );
}


VistaMIDIDriver::~VistaMIDIDriver()
{
  VistaDeviceSensor *pSen = GetSensorByIndex(0);
  RemDeviceSensor(pSen);

  VistaMIDIDriverTranscode *pTC =
	dynamic_cast<VistaMIDIDriverTranscode*>(pSen->GetMeasureTranscode());

  delete pTC;
  pSen->SetMeasureTranscode(NULL);

  delete pSen;
  delete m_pConnection;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaMIDIDriver::DoSensorUpdate(microtime dTs)
{
  int bytes_read = 0;
  unsigned char bMIDIEvent[3];

  _sMIDIMeasure sMIDIMeasure;

  VistaDeviceSensor *pSensor = GetSensorByIndex(0);
  VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);


  bool oldstate = m_pConnection->GetConnection(0)->GetIsBlocking();
  m_pConnection->GetConnection(0)->SetIsBlocking(true);

  while( m_pConnection->GetConnection(0)->HasPendingData() )
  {
	MeasureStart( 0, dTs );

	bytes_read = m_pConnection->GetConnection(0)->
	  ReadRawBuffer( bMIDIEvent, 3 );

	sMIDIMeasure.status = bMIDIEvent[0];
	sMIDIMeasure.data0  = bMIDIEvent[1];
	sMIDIMeasure.data1  = bMIDIEvent[2];

	std::memcpy( &(*pM).m_vecMeasures[0], &sMIDIMeasure, sizeof(_sMIDIMeasure) );

	MeasureStop( 0 );
  }

  m_pConnection->GetConnection(0)->SetIsBlocking(oldstate);
  pSensor->SetUpdateTimeStamp(dTs);

  return true;
}

unsigned int VistaMIDIDriver::GetSensorMeasureSize() const
{
  return sizeof( _sMIDIMeasure) ;
}

