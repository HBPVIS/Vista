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
// $Id: VistaDirectXGamepad.h 5557 2010-10-19 07:54:07Z dominikrausch $

#ifndef __VISTAWIIMOTESENSORMEASURES_H
#define __VISTAWIIMOTESENSORMEASURES_H

#include "wiiuse.h"

struct _wiimoteMeasure
	{
		vec3b_t  m_Acceleration;
		orient_t m_Orientation;
		gforce_t m_GravityForce;

		ir_t    m_IR;
		accel_t m_Acceleration_calib;

		unsigned short m_buttons;
	};


	enum eState
	{
		ST_NOEXP   = 0,
		ST_NUNCHUK =  1,
		ST_GUITAR  =  2,
		ST_CLASSIC =  4
	};


	struct _wiistateMeasure
	{
		int   m_nState;          /**< see wiiuse docu */
		int   m_nExpansionState; /**< see eState */
		float m_nBatteryLevel;   /**< between 0 and 1 */
		byte  m_cLEDs;           /**< bit-mask */
	};
	
	


#endif //__VISTAWIIMOTESENSORMEASURES_H

