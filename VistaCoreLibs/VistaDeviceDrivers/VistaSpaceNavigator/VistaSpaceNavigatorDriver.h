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

#ifndef __VISTASPACENAVIGATORDRIVER_H
#define __VISTASPACENAVIGATORDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaSpaceNavigatorConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

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
 * apparently a driver for the space navigator, working on the event layer
 * of the linux kernel. This driver will not operate on windows or non-linux
 * systems.
 *
 * @todo document me
 */
class VISTASPACENAVIGATORAPI VistaSpaceNavigator : public IVistaDeviceDriver
{
public:
	VistaSpaceNavigator();
	virtual ~VistaSpaceNavigator();


	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	struct VISTASPACENAVIGATORAPI _sMeasure
	{
		_sMeasure()
			: m_nRotationX(0),
				m_nRotationY(0),
				m_nRotationZ(0),
				m_nRotationAngle(0),

				m_nPositionX(0),
				m_nPositionY(0),
				m_nPositionZ(0),
				m_nLength(0)
		{
			m_nKeys[0] = m_nKeys[1] = 0;
		}

		double m_nRotationX,
			   m_nRotationY,
			   m_nRotationZ,
			   m_nRotationAngle;
		double m_nPositionX,
			   m_nPositionY,
			   m_nPositionZ,
			   m_nLength;

		long   m_nKeys[2];
	};
	virtual unsigned int GetSensorMeasureSize() const;
protected:
	virtual bool DoSensorUpdate(microtime dTs);
private:
	VistaDriverConnectionAspect *m_pConAsp;
};


/**
 * creates a VistaSpaceNavigator driver
 */
class VISTASPACENAVIGATORAPI VistaSpaceNavigatorCreationMethod : public IVistaDriverCreationMethod
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
#endif //__VISTA3DCSPACENAVIGATORDRIVER_H
