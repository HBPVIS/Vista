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

#ifndef _VISTAGLUTMOUSEDRIVER_H
#define _VISTAGLUTMOUSEDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaDeviceDriversBase/VistaMouseDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverWindowAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaGlutMouseDriver : public IVistaMouseDriver
{
public:
	VistaGlutMouseDriver();
	virtual ~VistaGlutMouseDriver();

	static IVistaDriverCreationMethod *GetDriverCreationMethod();
	static void MouseFunction ( int iButton, int iState, int iX, int iY);
	static void MotionFunction( int iX, int iY );
	static void MouseWheelFunction( int nWheelNumber, int nDirection, int nX, int nY);
protected:
	virtual bool DoSensorUpdate(microtime dTs);
private:

	struct _state
	{
		_state(int nGlutWinId,
			   int nX = 0,
			   int nY = 0,
			   int Bs0 = 0,
			   int Bs1 = 0,
			   int Bs2 = 0,
			   int nWheelNumber = -1,
			   int nWheelDirection = 0)
			: m_nWinId(nGlutWinId),
			  m_nX(nX),
			  m_nY(nY),
			  m_nWheelNumber(nWheelNumber),
			  m_nWheelDirection(nWheelDirection)
		{
			m_nButtonStates[0] = Bs0;
			m_nButtonStates[1] = Bs1;
			m_nButtonStates[2] = Bs2;
		}

		int m_nButtonStates[3];
		int m_nX, m_nY;
		int m_nWheelNumber,
			m_nWheelDirection,
			m_nWinId;
	};

	std::vector<_state> m_vecUpdates;
	int m_nButtonStates[3];
	int m_nWheelState,
		m_nWheelDirState;
	VistaDriverWindowAspect *m_pWindowAspect;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
