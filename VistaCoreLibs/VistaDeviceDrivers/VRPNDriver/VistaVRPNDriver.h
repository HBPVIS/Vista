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

#ifndef __VISTAVRPNRIVER_H
#define __VISTAVRPNRIVER_H

#if defined(WIN32)
#pragma warning(disable: 4786)
#endif

//#if !defined(WIN32)

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaVRPNDriverConfig.h"

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>


#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class VistaDriverSensorMappingAspect;

class vrpn_BaseClass;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VRPNDRIVERAPI VistaVRPNDriver : public IVistaDeviceDriver
{
public:
	class _cVRPN2SensorMap
	{
	public:
		_cVRPN2SensorMap( VistaVRPNDriver *pDriver,
				          vrpn_BaseClass *,
				          unsigned int id,
				          bool *bTick);

		void Tick();
		void Untick();

		bool GetTick() const;

		VistaVRPNDriver*	m_pDriver;
		vrpn_BaseClass*		m_pVRPNHandle;
		unsigned int		m_nSensorId;
		microtime			m_nUpdateTime;

	private:
		bool            *m_bNewData;
	};


	VistaVRPNDriver();
	~VistaVRPNDriver();

	bool Connect();

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();


	bool AddVRPNSensor( _cVRPN2SensorMap *mp );
	bool &GetTick();

protected:
	virtual bool DoSensorUpdate(microtime nTs);
	virtual bool PhysicalEnable(bool bEnable);

private:
	VistaDriverInfoAspect             *m_pInfo;
	VistaDriverSensorMappingAspect    *m_pMappingAspect;

	std::vector<_cVRPN2SensorMap*>       m_vecHandles;

	bool m_bNewData;
	bool m_bConnected;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

//#endif // LINUX

#endif //__VISTAVRPNDRIVER_H

