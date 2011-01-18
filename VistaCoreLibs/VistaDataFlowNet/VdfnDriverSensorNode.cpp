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

#include "VdfnDriverSensorNode.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaBase/VistaTimer.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnDriverSensorNode::VdfnDriverSensorNode( VistaDeviceSensor *pSensor )
: IVdfnNode(),
  m_pSensor(pSensor),
  m_pHistoryPort( new HistoryPort ),
  m_nOldUpdateIndex(0),
  m_nLastUpdateIdx(0)
{
	RegisterOutPort( "history",  m_pHistoryPort );
	m_pHistoryPort->SetValue( new VdfnHistoryPortData( m_pSensor->GetMeasures(),
								  m_pSensor->GetMeasureTranscode(),
								  m_pSensor),
								  0 );
}

VdfnDriverSensorNode::~VdfnDriverSensorNode()
{
}

bool VdfnDriverSensorNode::GetIsValid() const
{
	return (m_pSensor != NULL);
}

bool VdfnDriverSensorNode::DoEvalNode()
{
	// check for change in sensor source

	// first: fix the current situation (driver might concurrently
	// deliver new measures)
	VistaDeviceSensor *pSensor = m_pHistoryPort->GetValue()->m_pSensor;
	IVistaDeviceDriver *pDriver = pSensor->GetParent();

	unsigned int nMIndex = pSensor->GetMeasureIndex();

	// determine difference to last update
	// |now - last| but no more than our desired read size (skimming)
	unsigned int nNewMeasureCount
		= std::min<unsigned int>( nMIndex - m_nOldUpdateIndex,
								 m_pHistoryPort->GetValue()->m_oHistory.m_nClientReadSize);

	// write to clients of the history port (as this is convenient)
	m_pHistoryPort->GetValue()->m_nNewMeasures     = nNewMeasureCount;
	m_pHistoryPort->GetValue()->m_nRealNewMeasures = nMIndex - m_nOldUpdateIndex;
	m_pHistoryPort->GetValue()->m_nUpdateIndex     = nMIndex;

	// this is a race: but the value is for statistics and highly fluctuating
	m_pHistoryPort->GetValue()->m_nAvgDriverUpdTime = pDriver->GetAverageUpdateTime();
	m_pHistoryPort->GetValue()->m_nAvgUpdFreq = pDriver->GetAverageUpdateFrequency();


	// indicate change on the port by timestamp
	m_pHistoryPort->SetUpdateTimeStamp( GetUpdateTimeStamp() );

	// and revision
	m_pHistoryPort->IncUpdateCounter();

	// note the current index
	m_nOldUpdateIndex = nMIndex;
	return true;
}

unsigned int VdfnDriverSensorNode::CalcUpdateNeededScore() const
{
	// update on new -> swap <- count (indicated by GetDataCount())
	uint32 nUpd = m_pSensor->GetDataCount();

	if( m_nLastUpdateIdx != nUpd ) // poll: different to old value?
	{
		// yes
		m_nLastUpdateIdx = nUpd; // mark the spot
	}
	return m_nLastUpdateIdx; // new state -> update
}

