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
// $Id: VdfnDriverSensorNode.h 22143 2011-07-01 15:07:00Z dr165799 $

#ifndef _VDFNDRIVERSENSORNODE_H
#define _VDFNDRIVERSENSORNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnHistoryPort.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaDLVTypes.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDeviceSensor;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the driver sensor source node is a stub node to get information from the
 * VistaDeviceDrivers layer into the DataFlowNet layer. It will forward a pointer
 * to the complete history of a sensor into the net upon change of the history.
 * In order to operate, it needs to be a attached to a VistaDeviceSensor.
 * Note that this node decides to forward <b>a pointer</b> to the history, which,
 * as you can see here, is perfectly allright with the type system of the DFN,
 * however, pointer passing may be a problem in case of side effects.
 *
 * @outport{history,VdfnHistoryPortData*,a pointer to the history for further processing}
 */
class VISTADFNAPI VdfnDriverSensorNode : public IVdfnNode
{
public:
	/**
	 * @param pSensor a non-NULL pointer to the sensor to propagate the history from
	 */
	VdfnDriverSensorNode( VistaDeviceSensor *pSensor );
	~VdfnDriverSensorNode();

	/**
	 * @return true when a valid sensor was attached to this node.
	 */
	bool GetIsValid() const;

	/**
	 * this, well... ehr... ok... is a slight hack. It indicates that
	 * in a clustered setup, when using master/slave distribution, this
	 * node will only be evaluated on the master node, and not on a slave
	 * node. Additionally, the value of the outports of this node will be determined
	 * by the computation on the master, not on the slaves.
	 */
	virtual bool GetIsMasterSim() const { return true; }
protected:
	/**
	 * calculates the new number of measures and forwards the history pointer to
	 * the network.
	 * @return true
	 */
	bool DoEvalNode();

	/**
	 * determines the dirty state as a consequence of the state of the history.
	 */
	virtual unsigned int    CalcUpdateNeededScore() const;
private:
	VistaDeviceSensor *m_pSensor;
	HistoryPort        *m_pHistoryPort;

	mutable VistaType::uint32	m_nLastUpdateIdx; // is updated in CalcUpdateNeededScore()
	unsigned int    m_nOldUpdateIndex;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNDRIVERSENSORNODE_H
