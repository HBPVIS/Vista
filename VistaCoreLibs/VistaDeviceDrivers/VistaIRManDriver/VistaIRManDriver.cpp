/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <VistaBase/VistaStreamUtils.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VistaIRManDriverCreationMethod::VistaIRManDriverCreationMethod(IVistaTranscoderFactoryFactory *fac)
: IVistaDriverCreationMethod(fac)
{
	RegisterSensorType( "",
		sizeof( VistaIRManDriver::_sIRManSample ),
		200, /* 9600kbaud/s -> (9600 / 8) / 6 -> ~200 */
		fac->CreateFactoryForType("") );
}

IVistaDeviceDriver *VistaIRManDriverCreationMethod::CreateDriver()
{
	return new VistaIRManDriver(this);
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaIRManDriver::VistaIRManDriver(IVistaDriverCreationMethod *crm)
		: IVistaDeviceDriver(crm),
		  m_pConnectionAspect(new VistaDriverConnectionAspect)
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_CONNECTION_THREADED );

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	AddDeviceSensor(pSensor);

	pSensor->SetMeasureTranscode( crm->GetTranscoderFactoryForSensor("")->CreateTranscoder() );

	m_pConnectionAspect->SetConnection( 0, NULL, "MAIN", true );
	m_pConnectionAspect->SetUpdateConnectionIndex(0);

	RegisterAspect( m_pConnectionAspect );
}

VistaIRManDriver::~VistaIRManDriver()
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	RemDeviceSensor(pSensor);
	GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(pSensor->GetMeasureTranscode());
	pSensor->SetMeasureTranscode(NULL);
	delete pSensor;

	VistaConnection *pCon = m_pConnectionAspect->GetConnection(0);
	pCon->Close();

	UnregisterAspect( m_pConnectionAspect, false );
	delete m_pConnectionAspect;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/



bool VistaIRManDriver::Connect()
{
	VistaConnection *pConnection = m_pConnectionAspect->GetConnection(0);
	if(!pConnection)
	{
		vstr::warnp() << "[IRManDriver]: no connection connection given" << std::endl;
		return false;
	}

	if(!pConnection->GetIsOpen())
	{
		if(!pConnection->Open())
		{
			vstr::warnp() << "[IRManDriver]: Could not open connection" << std::endl;
			return false;
		}
	}

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
		vstr::warnp() << "[IRManDriver]: IRMan did not respond correctly" << std::endl;
		pConnection->Close();
		return false;
	}
	else
	{
		vstr::outi() << "[IRManDriver]: IRMan found" << std::endl;
	}
	pConnection->SetIsBlocking(false);
#if defined(WIN32)
	// activate the event to wait for a read on the connection
	// maybe this needs to be fixed on the serial connection under win32?
	pConnection->WaitForIncomingData();
#endif

	return true;
}

bool VistaIRManDriver::DoSensorUpdate( VistaType::microtime nTs )
{
	VistaConnection *pConnection = m_pConnectionAspect->GetConnection(0);
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);

	// first of all, indicate the beginning of a sampling
	// for the sensor with index 0 at time nTs
	pConnection->SetIsBlocking(true);

	VistaSensorMeasure *pM = MeasureStart(0, nTs, true);
	if(pM == NULL)
	{
		MeasureStop(0);
		vstr::warnp() << "[IRManDriver]: could not get memory to write to during call to update()" << std::endl;
		return false;
	}

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the IRMan
	_sIRManSample *s = (*pM).getWrite<_sIRManSample>();

	pConnection->ReadRawBuffer( s, sizeof(_sIRManSample) );

	// we are done. Indicate that to the history
	MeasureStop(0);

	pConnection->SetIsBlocking(false);

#if defined(WIN32)
	// we need to do that under windows to reset the event that is waiting
	// for input on the connection (maybe that is worth a fix for the serial connection
	// under win32?)
	pConnection->WaitForIncomingData();
#endif

	pSensor->SetUpdateTimeStamp(nTs);
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


