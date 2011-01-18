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

#ifndef __VISTAWIIMOTEDRIVER_H
#define __VISTAWIIMOTEDRIVER_H

#if defined(WIN32)
#pragma warning(disable: 4786)
#endif

//#if !defined(WIN32)

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaWiimoteDriverConfig.h"

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>

#include <wiiuse.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class IVistaDriverProtocolAspect;
class VistaDriverSensorMappingAspect;
class VistaDriverGenericParameterAspect;
class VistaDriverThreadAspect;
class CWiimoteRumble;
class VistaMutex;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAWIIMOTEDRIVERAPI VistaWiimoteDriver : public IVistaDeviceDriver
{
public:

	VistaWiimoteDriver();
	~VistaWiimoteDriver();

	bool Connect();

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	class VistaWiimoteUpdate;


	class VISTAWIIMOTEDRIVERAPI CWiiMoteParameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		CWiiMoteParameters( VistaWiimoteDriver* pDriver );

		enum
		{
			MSG_MOTEID_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
			MSG_ACCELREPORT_CHG,
			MSG_IRREPORT_CHG,
			MSG_SMOOTHALPHA_CHG,
			MSG_ACCELTHRESHOLD_CHG,
			MSG_WAITTIMEOUT_CHG,
			MSG_LAST
		};

		std::string GetWiiMoteId() const;
		bool SetWiiMoteId( const std::string &strWiiMoteId );

		bool GetDoesAccelerationReports() const;
		bool SetDoesAccelerationReports( bool bDoesIt );

		bool GetDoesIRReports() const;
		bool SetDoesIRReports( bool bDoesIt );

		float GetSmoothAlpha() const;
		bool  SetSmoothAlpha( float nAlpha );

		int GetAccelerationThreshold() const;
		bool SetAccelerationThreshold( int nThreshold );

		int GetWaitTimeout() const;
		bool SetWaitTimeout( int nTimeoutInSecs );

	private:
		VistaWiimoteDriver* m_pDriver;

		std::string m_strWiiMoteId;
		bool m_bDoesAcc,
		     m_bDoesIR;

		float m_nSmoothAlpha;
		int   m_nAccelThreshold,
		      m_nWaitTimeout;

		VistaMutex*	m_pLock;
	};

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


	CWiiMoteParameters *GetParameters() const;
	bool GetIsConnected() const;

	bool EnableWiiMoteSensor(unsigned int nMaxSlotsToRead,
                             float nMaxHistoryAccessTimeInMsec);
	bool EnableNunchukSensor(unsigned int nMaxSlotsToRead,
                             float nMaxHistoryAccessTimeInMsec);
	bool EnableClassicSensor(unsigned int nMaxSlotsToRead,
                             float nMaxHistoryAccessTimeInMsec);
	bool EnableGuitarSensor(unsigned int nMaxSlotsToRead,
                           float nMaxHistoryAccessTimeInMsec);
	bool EnableStateSensor(unsigned int nMaxSlotsToRead,
				           float nMaxHistoryAccessTimeInMsec);

	void SetParamChangeFlag();

protected:
	virtual bool DoSensorUpdate(microtime nTs);
	virtual bool PhysicalEnable(bool bEnable);

private:
	bool RecordMoteEvent( microtime nTs );
	bool RecordStateEvent( microtime nTs );
	bool SetupWiiMote();
	bool EnableSensorTyped( const std::string &strType, unsigned int nMaxSlotsToRead,
            float nMaxHistoryAccessTimeInMsec );


	template<class T> bool RecordExtSensor( microtime nTs,
			                        unsigned int nSensorIdx,
			                        const T *msg )
	{
		if( nSensorIdx == ~0 )
			return false;

		VistaDeviceSensor *pSensor = GetSensorByIndex(nSensorIdx);

		MeasureStart(nSensorIdx, nTs);
		// get the current place for the decoding for sensor 0
		VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

		// for the sake of readability: interpret the memory blob
		// as a place to store a sample for the IRMan
		T *s = reinterpret_cast<T*>(&( *pM ).m_vecMeasures[0]);

		memcpy( s, msg, sizeof( T ) );

		// we are done. Indicate that to the history
		MeasureStop(nSensorIdx);

		pSensor->SetUpdateTimeStamp(nTs);

		return true;
	}

	int DetermineSetup();

    wiimote **m_pMote;

	VistaDriverInfoAspect             *m_pInfo;
	IVistaDriverProtocolAspect         *m_pProtocol;
	VistaDriverSensorMappingAspect    *m_pMappingAspect;
	VistaDriverGenericParameterAspect *m_pParams;
	VistaDriverThreadAspect		   *m_pThreadAspect;
	CWiimoteRumble                     *m_pRumble;

	unsigned int m_nWiimoteId,
	             m_nNunchukId,
	             m_nStatusId,
	             m_nClassicId,
	             m_nGuitarId;

	bool m_bConnected;
	bool m_bParamChangeFlag;

	int m_nState;	
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

//#endif // LINUX

#endif //__VISTAJSWJOYSTICK_H

