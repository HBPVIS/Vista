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

#ifndef __VISTAPHANTOMDRIVER_H
#define __VISTAPHANTOMDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaPhantomDriverConfig.h"

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverForceFeedbackAspect.h>
#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverWorkspaceAspect;
class IVistaDriverProtocolAspect;
class VistaDriverInfoAspect;
class VistaDeviceIdentificationAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a driver for the Phantom haptic device. It uses the OpenHaptics library
 * and supports multiple phantoms on the same machine. The driver collects
 * data at the full data rate and offers advanced force feedback using the
 * force feedback API and LLDx.
 */
class VISTAPHANTOMDRIVERAPI VistaPhantomDriver : public IVistaDeviceDriver
{
	friend class VistaPhantomForceFeedbackAspect;
public:

	struct _sPhantomMeasure
	{
		float m_afPosition[3],        /**< in mm */
			  m_afRotMatrix[16],      /**< column major */
			  m_afVelocity[3],        /**< in mm/s */
			  m_afForce[3],           /**< in N */
			  m_afAngularVelocity[3], /**< the gimbal's, in rad/s */
			  m_afJointAngles[3],     /**< radians */
			  m_afGimbalAngles[3],    /**< radians */
			  m_afTorque[3],          /**< in Nm */
		      m_nUpdateRate,          /**< in Hz */
			  m_afOverheatState[6];   /**< degrees Celsius? */
		long  m_nEncoderValues[6];    /**< raw values */
		int   m_nButtonState;         /**< button mask */
		bool  m_bInkwellSwitch;
	};


	VistaPhantomDriver();
	virtual ~VistaPhantomDriver();

	/**
	 * Overloaded from IVistaDeviceDriver.
	 * Tries to open the phantom device that was specified in the ProtocolAspect's name
	 * field. Revision is ignored.
	 */
	bool Connect();

	virtual unsigned int GetSensorMeasureSize() const;

	/**
	 * Returns a factory method for the creation of a driver.
	 * This code is used by the new InteractionManager and should
	 * not be used by client code. If called in the context of
	 * the base class IVistaDeviceDriver, an exception will be thrown.
	 */
	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	class VISTAPHANTOMDRIVERAPI VistaPhantomForceFeedbackAspect : public IVistaDriverForceFeedbackAspect
	{
		friend class VistaPhantomDriver;
	public:
		virtual bool SetForce( const VistaVector3D   & force,
							   const VistaQuaternion &angularForce );

		virtual bool SetForcesEnabled(bool bEnabled);
		virtual bool GetForcesEnabled() const;


		virtual int GetNumInputDOF() const;
		virtual int GetNumOutputDOF() const;

        virtual float GetMaximumStiffness() const;
        virtual float GetMaximumForce() const;


	private:
		VistaPhantomForceFeedbackAspect( VistaPhantomDriver *pDriver );
		virtual ~VistaPhantomForceFeedbackAspect();
		VistaPhantomDriver *m_pParent;

		int m_nInputDOF,
			m_nOutputDOF;
        float m_nMaxForce,
              m_nMaxStiffness;

		VistaVector3D   m_v3Force;
		VistaQuaternion m_qAngularForce;
	};

	std::string GetDeviceString() const;
	void SetDeviceString(const std::string &strDevice);
protected:
	bool         PhysicalEnable(bool bEnable);
	virtual bool DoSensorUpdate(microtime dTs);
private:
	VistaDriverWorkspaceAspect *m_pWorkSpace;
	//IVistaDriverProtocolAspect  *m_pProtocol;
	VistaDriverInfoAspect      *m_pInfo;
	VistaDeviceIdentificationAspect *m_pIdentification;
	VistaPhantomForceFeedbackAspect *m_pForceFeedBack;


	struct _sPhantomPrivate;
	_sPhantomPrivate *m_pPrivate;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //__VISTAPHANTOMDRIVER_H

