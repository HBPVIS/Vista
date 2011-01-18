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

#ifndef _VISTAPHANTOMSERVERDRIVER_H
#define _VISTAPHANTOMSERVERDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"
#include "../VistaDeviceDriver.h"
#include "../VistaDriverForceFeedbackAspect.h"
#include <VistaBase/VistaVectorMath.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverConnectionAspect;
class VistaDriverWorkspaceAspect;
class VistaByteBufferSerializer;
class VistaConnection;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a driver for remote phantom access using the VistaPhantomServer implementation
 * which constructs a TCP/IP line between the host where the phantom is attached
 * and the host where the data is needed. It is not necessarily superceeded by
 * the phantom driver which is available as a plugin to ViSTA, as it realizes
 * the transport remotely, which is not possible using the plugin.
 * However, this driver, by nature has more latency and omits data in order not
 * to clutter up the TCP/IP connection. It supports force feedback rendering
 * using the ForceFeedbackAspect.
 *
 *
 */
class VISTADEVICEDRIVERSAPI VistaPhantomServerDriver : public IVistaDeviceDriver
{
	friend class VistaPhantomServerControlAttachSequence;
	friend class VistaPhantomServerForceFeedbackAspect;
public:
	VistaPhantomServerDriver();
	virtual ~VistaPhantomServerDriver();

	virtual unsigned int GetSensorMeasureSize() const;

	/**
	 * Returns a factory method for the creation of a driver.
	 * This code is used by the new InteractionManager and should
	 * not be used by client code. If called in the context of
	 * the base class IVistaDeviceDriver, an exception will be thrown.
	 */
	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	class VISTADEVICEDRIVERSAPI VistaPhantomServerForceFeedbackAspect : public IVistaDriverForceFeedbackAspect
	{
		friend class VistaPhantomServerDriver;
	public:
		virtual bool SetForce( const VistaVector3D   & force,
							   const VistaQuaternion &angularForce );

		bool SetConstraint( const VistaVector3D		& contactPoint,
									const VistaVector3D	& normal,
									const float stiffness,
									const VistaVector3D	& internalForce) const;

		virtual int GetNumInputDOF() const;
		virtual int GetNumOutputDOF() const;

        virtual float GetMaximumStiffness() const;
        virtual float GetMaximumForce() const;

		bool SetForcesEnabled(bool bEnabled);
		bool GetForcesEnabled() const;

		bool SetDynamicFrictionEnabled( bool bEnabled );
		bool GetDynamicFrictionEnabled() const;
	private:
		VistaPhantomServerForceFeedbackAspect( VistaPhantomServerDriver *pDriver );
		virtual ~VistaPhantomServerForceFeedbackAspect();

		VistaPhantomServerDriver *m_pParent;
		VistaConnection *GetChannel(int nId) const;
		VistaByteBufferSerializer *m_pSerializer;


		float m_nDefaultStiffness;
		bool  m_bForcesEnabled;
		bool  m_bDynamicFriction;
	};

	void	AttachMaximalBoundary() const;
	void	DetachBoundary() const;
	void	ResetEncoders() const;

	void	GetMaxWorkspace(VistaVector3D & min, VistaVector3D & max) const;
	void	GetMaxUsableWorkspace(VistaVector3D & min, VistaVector3D & max) const;

protected:
	bool PhysicalEnable(bool bEnable);
	virtual bool DoSensorUpdate(microtime dTs);
private:
	VistaDriverConnectionAspect *m_pConAspect;
	VistaDriverWorkspaceAspect *m_pWorkSpace;

	// MEMBERS
	int		m_inputDOF;
	int		m_outputDOF;
	float	m_maxForce;
	float	m_maxStiffness;

	VistaVector3D		m_maxWorkspaceMin, m_maxWorkspaceMax;
	VistaVector3D		m_maxUsableWorkspaceMin, m_maxUsableWorkspaceMax;
};


/**
 * the memory layout of a phantom measure
 */
struct _sPhantomMeasure
{

	float m_afPosition[3],
		  m_afRotMatrix[9],
		  m_afPosSCP[3],
		  m_afVelocity[3],
		  m_afForce[3];
	int   m_nButtonState;
	float m_nUpdateRate,
		  m_afOverheatState[6];
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPHANTOMSERVERDRIVER_H

