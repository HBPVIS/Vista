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

#ifndef __VISTADTRACKSDKDRIVERDRIVER_H
#define __VISTADTRACKSDKDRIVERDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDTrackSDKDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class DTrack;
class VistaDriverThreadAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver for the simple SpaceNav device vom 3DC. It uses the HID
 * event layer on Linux and the 3DC specific driver shipped with windows.
 * On windows, the driver raises a dependency to the installed DCOM drivers,
 * even on compile time.
 * There is currently only support for a two-button device having one puck
 * and no device backwards communication.
 *
 */
class VISTADTRACKSDKDRIVERAPI VistaDTrackSDKDriver : public IVistaDeviceDriver
{
public:
	VistaDTrackSDKDriver();
	virtual ~VistaDTrackSDKDriver();


	bool Connect();

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	virtual unsigned int GetSensorMeasureSize() const;

protected:
	virtual bool PhysicalEnable(bool bEnabled);
	virtual bool DoSensorUpdate(microtime dTs);
private:
	DTrack *m_pDriver;

	VistaDriverThreadAspect *m_pThread;

	bool UpdateBodySensor( microtime dTs, int nIdx = 0 );
	bool UpdateMarker( microtime dTs, int nIdx = 0 );
};


/**
 * creates the 3DC space nav driver
 */
class VISTADTRACKSDKDRIVERAPI VistaDTrackSDKDriverCreateMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTADTRACKSDKDRIVERDRIVER_H
