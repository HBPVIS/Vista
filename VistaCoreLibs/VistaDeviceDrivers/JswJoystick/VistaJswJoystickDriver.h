/*============================================================================*/
/*                    ViSTA VR toolkit - Jsw Joystick driver                  */
/*               Copyright (c) 1997-2011 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id$

#ifndef __VISTAJSWJOYSTICKDRIVER_H
#define __VISTAJSWJOYSTICKDRIVER_H

#if defined(WIN32)
#pragma warning(disable: 4786)
#endif

#if !defined(WIN32)

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <jsw.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverWorkspaceAspect;
class IVistaDriverProtocolAspect;
class VistaDriverInfoAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaJswJoystickDriver : public IVistaDeviceDriver
{
public:
	struct _sJoyMeasure
	{
		time_t m_nAxesTs[16];
		int    m_nAxes[16];
		time_t m_nButtonsTs[32];
		bool   m_nButtons[32];
	};

	VistaJswJoystickDriver();
	~VistaJswJoystickDriver();

	bool Connect();


	virtual unsigned int GetSensorMeasureSize() const;
	static IVistaDriverCreationMethod *GetDriverFactoryMethod();
protected:
	bool DoSensorUpdate(microtime nTs);
private:
	js_data_struct 				  m_jsd;
	bool 						  m_bOpened;
	_sJoyMeasure                  m_nState;

	VistaDriverWorkspaceAspect  *m_pWorkspace;
	IVistaDriverProtocolAspect   *m_pProtocol;
	VistaDriverInfoAspect       *m_pInfo;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif // LINUX

#endif //__VISTAJSWJOYSTICK_H

