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
// $Id: VistaDriverMeasureHistoryAspect.cpp 23585 2011-09-28 07:44:46Z dr165799 $

#include "VistaDriverMeasureHistoryAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"
#include <cassert>

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverMeasureHistoryAspect::m_nAspectId  = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverMeasureHistoryAspect::VistaDriverMeasureHistoryAspect()
: IVistaDeviceDriver::IVistaDeviceDriverAspect()
{
	if(VistaDriverMeasureHistoryAspect::GetAspectId() == -1) // unregistered
		VistaDriverMeasureHistoryAspect::SetAspectId(
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("HISTORY"));

	SetId(VistaDriverMeasureHistoryAspect::GetAspectId());
}

VistaDriverMeasureHistoryAspect::~VistaDriverMeasureHistoryAspect()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverMeasureHistoryAspect::GetAspectId()
{
	return VistaDriverMeasureHistoryAspect::m_nAspectId;
}

void VistaDriverMeasureHistoryAspect::SetAspectId(int nId)
{
	assert(m_nAspectId == -1);
	m_nAspectId = nId;

}

bool VistaDriverMeasureHistoryAspect::RegisterSensor(VistaDeviceSensor *pSen)
{
	_sL l(&pSen->GetDriverMeasures());
	m_mapHistories[pSen] = l;
	return true;
}

bool VistaDriverMeasureHistoryAspect::UnregisterSensor(VistaDeviceSensor *pSen)
{
	HISTORY::iterator it = m_mapHistories.find(pSen);
	if(it == m_mapHistories.end())
		return true; // not registered

	m_mapHistories.erase(it);
	return true;
}


void VistaDriverMeasureHistoryAspect::MeasureStart(VistaDeviceSensor*pSen,
													VistaType::microtime dTs)
{
	HISTORY::const_iterator cit = m_mapHistories.find(pSen);
	if(cit == m_mapHistories.end())
		return; // not registered

	// statistics
	VistaType::microtime t = VistaTimeUtils::GetStandardTimer().GetMicroTime();
	pSen->GetWindowTimer().RecordTime();
	//

	// create an entry
	_sL l(&pSen->GetDriverMeasures());

	// fill it with 'local' information
	(*l.m_nIt).m_nMeasureTs = dTs;
	(*l.m_nIt).m_nMeasureIdx = pSen->GetMeasureIndex();

	// copy into the history map for this sensor
	m_mapHistories[pSen] = l; // replace
}

void  VistaDriverMeasureHistoryAspect::MeasureStop(VistaDeviceSensor *pSen)
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it != m_mapHistories.end())
	{
		// advance in measure index for sensor
		pSen->AdvanceMeasure();
	}
}


bool VistaDriverMeasureHistoryAspect::SetHistorySize(VistaDeviceSensor *pSen,
							unsigned int nUserSize,
							unsigned int nDriverSize,
							unsigned int nSlotLength)
{
	HISTORY::iterator it = m_mapHistories.find(pSen);
	if(it != m_mapHistories.end())
	{
		(*it).second.m_pMeasures->m_rbHistory.Reset();

		(*it).second.m_pMeasures->m_rbHistory = TVistaRingBuffer<VistaSensorMeasure>(nUserSize+nDriverSize);

		for(unsigned int n=0; n < nUserSize+nDriverSize; ++n)
		{
			VistaSensorMeasure m(0, 0, nSlotLength);
			(*it).second.m_pMeasures->m_rbHistory.Add(m);
		}

		(*it).second.m_pMeasures->m_rbHistory.Reset();
		(*it).second.m_pMeasures->m_nClientReadSize = nUserSize;
		(*it).second.m_pMeasures->m_nDriverWriteSize = nDriverSize;

		pSen->GetWindowTimer().ResetAveraging();
		return true;
	}
	return false;
}

unsigned int VistaDriverMeasureHistoryAspect::GetHistorySize(VistaDeviceSensor *pSen) const
{
	return pSen->GetMeasureHistorySize();
}

VistaSensorMeasure *VistaDriverMeasureHistoryAspect::GetCurrentSlot(VistaDeviceSensor*pSen) const
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it == m_mapHistories.end())
		return NULL;

	return &(*(*it).second.m_pMeasures->m_rbHistory.GetCurrent());
}

bool VistaDriverMeasureHistoryAspect::GetIsRegistered(VistaDeviceSensor *pSen) const
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it == m_mapHistories.end())
		return false;
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


