/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>

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



//IVistaDriverCreationMethod *VistaMIDIDriver::GetDriverFactoryMethod()
//{
//	if( g_SpFactory == NULL )
//	{
//		g_SpFactory = new VistaMIDIDriverCreationMethod;
//
//		// register one unnamed sensor type
//		/** @todo : better estimate for update frequency! */
//		g_SpFactory->RegisterSensorType( "",
//										 sizeof(_sMIDIMeasure),
//										 100, new VistaMIDIDriverTranscodeFactory,
//										 VistaMIDIDriverTranscode::GetTypeString());
//	}
//	return g_SpFactory;
//}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMIDIDriver::VistaMIDIDriver(IVistaDriverCreationMethod *crm) :
  IVistaDeviceDriver(crm),
  m_pConnection(new VistaDriverConnectionAspect)
{
  SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);

  m_pConnection->SetConnection(0, NULL, "MAIN", true);
  RegisterAspect(m_pConnection);

  m_pConnection->SetDetachSequence(0, new VistaMIDIDetachSequence);
  m_pConnection->SetAttachSequence(0, new VistaMIDIAttachSequence);

  VistaDeviceSensor *pSen = new VistaDeviceSensor();
  pSen->SetMeasureHistorySize(10);
  pSen->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );
  pSen->SetTypeHint( GetFactory()->GetTypeFor( "" ) );
  AddDeviceSensor( pSen );
}


VistaMIDIDriver::~VistaMIDIDriver()
{
  VistaDeviceSensor *pSen = GetSensorByIndex(0);
  RemDeviceSensor(pSen);

  delete pSen->GetMeasureTranscode();

  /*
  VistaMIDIDriverTranscode *pTC =
	dynamic_cast<VistaMIDIDriverTranscode*>(pSen->GetMeasureTranscode());

  delete pTC;
  */
  pSen->SetMeasureTranscode(NULL);

  delete pSen;
  delete m_pConnection;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaMIDIDriver::DoSensorUpdate(VistaType::microtime dTs)
{
  unsigned char bMIDIEvent[3];

  VistaMIDIMeasures::sMIDIMeasure sMIDIMeasure;

  VistaDeviceSensor *pSensor = GetSensorByIndex(0);
  VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);


  bool oldstate = m_pConnection->GetConnection(0)->GetIsBlocking();
  m_pConnection->GetConnection(0)->SetIsBlocking(true);

  while( m_pConnection->GetConnection(0)->HasPendingData() )
  {
	MeasureStart( 0, dTs );

	m_pConnection->GetConnection(0)->ReadRawBuffer( bMIDIEvent, 3 );

	sMIDIMeasure.status = bMIDIEvent[0];
	sMIDIMeasure.data0  = bMIDIEvent[1];
	sMIDIMeasure.data1  = bMIDIEvent[2];

	std::memcpy( &(*pM).m_vecMeasures[0], &sMIDIMeasure, sizeof(VistaMIDIMeasures::sMIDIMeasure) );

	MeasureStop( 0 );
  }

  m_pConnection->GetConnection(0)->SetIsBlocking(oldstate);
  pSensor->SetUpdateTimeStamp(dTs);

  return true;
}

