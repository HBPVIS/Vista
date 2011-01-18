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

#if defined(WIN32)

#ifndef __VISTADIRECTXGAMEPAD_H
#define __VISTADIRECTXGAMEPAD_H


#pragma warning( disable : 4996 ) // disable deprecated warning

#include "VistaDirectXGamepadConfig.h"

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverForceFeedbackAspect.h>

#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <dinput.h>
#include <dinputd.h>
#include <XInput.h> // XInput API


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverAbstractWindowAspect;
class IVistaDriverProtocolAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * DirectX compatible joystick driver. Supports joysticks with up to two axes.
 * Needs a DirectX SDK installed in order to be compiled. The DirectInput
 * API was frozen somewhat in DirectX 8, so older revisions seem to be fine.
 * The Code supports the usage of DirectX specific force feedback effects,
 * but one needs to know that the DirectX API is going to be used, which is
 * ok in case you want to design custom effects.
 * The driver <b>needs</b> a properly configured and working
 * VistaDriverAbstractWindowAspect in order to work. As it is a windows device, it
 * needs a message port for processing. It gets it by using the window API
 * of the window aspect. The aspect has to be set <i>before</i> a call to connect.
 * The measure struct directly stores a copy of the structure delivered by DirectX
 * so in case you need more information, get the sensor and cast the slot pointer
 * to DIJOYSTATE2 type.
 * @todo add support for workspace aspect
 *
 */
class VISTADIRECTXGAMEPADAPI VistaDirectXGamepad : public IVistaDeviceDriver
{
	friend class VistaDirectXForceFeedbackAspect;
public:
	VistaDirectXGamepad();
	~VistaDirectXGamepad();

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// FORCE FEEDBACK API
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	class VISTADIRECTXGAMEPADAPI VistaDirectXForceFeedbackAspect
				  : public IVistaDriverForceFeedbackAspect
	{
		friend class VistaDirectXGamepad;
	public:
		class VISTADIRECTXGAMEPADAPI CEffect
		{
			friend class VistaDirectXForceFeedbackAspect;
		public:

			bool SetParameters( DIEFFECT &params);
			bool GetParameters( DIEFFECT &params );


			LPDIRECTINPUTEFFECT m_pEffect;
		private:
			CEffect();
			~CEffect();
		};

		CEffect* CreateEffect(REFGUID effectType);
		bool     DeleteEffect(CEffect *);

		bool Start(CEffect *pEffect, int nIterations);
		bool Stop(CEffect *pEffect);

		virtual bool SetForce( const VistaVector3D   & force,
							   const VistaQuaternion & qIgnored);

		bool     SetCurrentEffect( CEffect * );
		CEffect *GetCurrentEffect() const;


		virtual int GetNumInputDOF() const;
		virtual int GetNumOutputDOF() const;

		virtual bool SetForcesEnabled(bool bEnabled);
		virtual bool GetForcesEnabled() const;

        virtual float GetMaximumStiffness() const;
        virtual float GetMaximumForce() const;
	private:
		VistaDirectXForceFeedbackAspect(VistaDirectXGamepad *pPad);
		virtual ~VistaDirectXForceFeedbackAspect();

		VistaDirectXGamepad *m_pParent;
		std::vector<CEffect*> m_vecEffects;
		CEffect              *m_pCurrentEffect;
	};

	enum eDriver
	{
		TP_UNKNOWN=-1,
		TP_XINPUT=0, /**< use this for XINPUT style processing */
		TP_DIRECTX   /**< use this for DirectInput style processing */
	};


	bool Connect();

	unsigned int GetSensorMeasureSize() const;
	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	// ########################################################################


	enum ePadType
	{
		TP_UNKNWON   = -1,
		TP_RUMBLEPAD =  0,
		TP_XBOX360   =  1
	};

	ePadType GetPadType() const;

	unsigned int GetNumberOfButtons() const;

	enum
	{
		AXIS_X1 = 0,
		AXIS_Y1,
		AXIS_Z1,
		AXIS_Z2,
		AXIS_X2,
		AXIS_Y2,
		AXIS_POV = 6
	};

	bool GetRanges( int &xMin, int &yMin, int &zMin,
					int &xMax, int &yMax, int &zMax) const;

	/**
	 * @param strJoyName the name of the joystick to search and use.
	          When no name is set, the <b>default</b> joystick as set in
	          the windows settings is used for processing. Otherwise,
	          a case-sensitive search during connect is done.
	 */
	void SetJoystickName( const std::string &strJoyName );

	/**
	 * @return the name set by SetJoystickName()
	 */
	std::string GetJoystickName() const;

	VistaDirectXForceFeedbackAspect *GetDirectXForceFeedbackAspect() const;

	virtual bool RegisterAspect(IVistaDeviceDriverAspect *pAspect);

	/**
	 * Unregisters an aspect with this driver. Can be used to delete the aspect
	 * while unregistering it.
	 * @param pAspect the aspect instance to unregister.
	 * @param bDelete true iff delete is to be called on the pointer after the
					  un-registration process.
	 * @return false iff the aspect was not registered with the driver instance
	 */
	virtual bool UnregisterAspect(IVistaDeviceDriverAspect *pAspect,
						  bool bDelete = true);

protected:
	bool DoSensorUpdate(microtime dTs);

protected:
private:
	friend class GamepadAttach;
	bool InitInput();
	bool InitDirectXInput();
	bool UpdateDirectXInput(microtime dTs);

	bool InitXInput();

	eDriver              m_eDriver;
	LPDIRECTINPUT8       m_pDI;
	LPDIRECTINPUTDEVICE8 m_pJoystick;
	DIJOYSTATE2          m_oldState;
	unsigned int         m_nNumberOfButtons;

	// ViSTA specific
	VistaDriverAbstractWindowAspect *m_pWindowAspect;
	IVistaDriverProtocolAspect       *m_pProtocol;

	// FORCE FEEDBACK
	IVistaDriverForceFeedbackAspect  *m_pForceAspect;
	std::string                      m_strJoyName;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //__VISTASYSTEM_H

#endif // WIN32
