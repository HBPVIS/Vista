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

#ifndef _VISTAHIDDRIVER_H__
#define _VISTAHIDDRIVER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"
#include "../VistaDeviceDriver.h"
#include <string>
#include <vector>

#ifdef WIN32
#include <Windows.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
#ifdef LINUX
struct input_event ;
#endif
#ifdef WIN32
class VistaDriverThreadAspect;
#endif

class VistaDriverConnectionAspect;
class VistaDriverSensorMappingAspect;
class VistaDeviceIdentificationAspect;
class VistaHIDDriverForceFeedbackAspect;
class VistaHIDDetachSequence;

struct _sHIDMeasure;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver working on top of the event subsystem of the Linux kernel.
 * or on top of the HID layer under windows.
 * @todo add documentation for this driver, PLEAZE
 */
class VISTADEVICEDRIVERSAPI VistaHIDDriver : public IVistaDeviceDriver
{
public:
	VistaHIDDriver();
	virtual ~VistaHIDDriver();

	unsigned int GetSensorMeasureSize() const;

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	static bool ScanAvailableHIDDevices(std::list<std::string> &liResult);

	bool Connect();

protected:
	bool DoSensorUpdate(microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);
private:
	bool InitDriver(int nVendor, int nDevId);

#ifdef LINUX
	VistaDriverConnectionAspect        *m_pConnection;
	VistaHIDDriverForceFeedbackAspect  *m_pForce;
	std::vector<input_event> m_vEvents ;
#endif
#ifdef WIN32
	bool StartWinRead();

	VistaDriverThreadAspect			*m_pThreadAspect;

	HANDLE m_hDevice;
	HANDLE m_hReadEvent;
	BYTE m_readBuffer[512];
	OVERLAPPED m_readOverlap;
#endif

	VistaDriverSensorMappingAspect     *m_pSensorMappingAsp;
	VistaDeviceIdentificationAspect    *m_pIdentAsp;

	_sHIDMeasure *m_pMeasure;
};

/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMOUSEDRIVER_H
