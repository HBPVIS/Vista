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

#ifndef _VISTAIRMANDRIVER_H
#define _VISTAIRMANDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"
#include "../VistaDeviceDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A driver for an infrared receiver called IRMan and compatible devices.
 * Decodes keystrokes and collects them as byte-sequences as output of the
 * drivers's sensors. The device is simple in nature, it only needs a proper
 * connection setup (and yes... the device expects that to be a serial connection
 * setup at 8,n,1, 9600 baud.
 * The standard implementation offers to deliver the collected 6byte code
 * or the same code as 64bit value, which might be easier to process.
 * The driver exports a single sensor at index 0.
 *
 * @todo check whether this driver can benefit from the new Connect() API.
 */
class VISTADEVICEDRIVERSAPI VistaIRManDriver : public IVistaDeviceDriver
{
public:
	VistaIRManDriver();
	virtual ~VistaIRManDriver();

	unsigned int GetSensorMeasureSize() const;

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

protected:
	bool DoSensorUpdate( microtime nTs );
private:
	VistaDriverConnectionAspect *m_pConnectionAspect;
};

/**
 * the creation method for the IRMan driver.
 */
class VistaIRManCreateMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()() { return new VistaIRManDriver; }
};

/**
 * the memory layout for an IRMan sample, 6 bytes as collected by the device.
 */
struct _sIRManSample
{
	unsigned char m_acKeyCode[6];
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAIRMANDRIVER_H

