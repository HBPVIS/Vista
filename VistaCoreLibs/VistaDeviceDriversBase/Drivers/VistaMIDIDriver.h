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

#ifndef _VISTAMIDIDRIVER_H__
#define _VISTAMIDIDRIVER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"
#include "../VistaDeviceDriver.h"
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverConnectionAspect ;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the memory layout of a MIDI measure.
 */
struct _sMIDIMeasure
{
  unsigned char status;
  unsigned char data0;
  unsigned char data1;
};

/**
 * standard Linux midi driver. This driver is not known to work under windows.
 *
 *
 * @todo update documentation, especially on setting up the connection
 * @todo check windows implementation
 * @todo see whether this driver can benefit from the new Connect() API
 */
class VISTADEVICEDRIVERSAPI VistaMIDIDriver : public IVistaDeviceDriver
{
public:
	VistaMIDIDriver();
	virtual ~VistaMIDIDriver();

	unsigned int GetSensorMeasureSize() const;

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

protected:
	bool DoSensorUpdate(microtime dTs);

private:
	// the file descriptor for the device file
	VistaDriverConnectionAspect* m_pConnection ;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTAMOUSEDRIVER_H
