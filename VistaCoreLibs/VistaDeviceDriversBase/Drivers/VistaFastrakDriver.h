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

#ifndef _VISTAFASTRAKDRIVER_H
#define _VISTAFASTRAKDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "../VistaDeviceDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;
class VistaDriverSensorMappingAspect;
class IVistaFastrakCommandSet;
class VistaFastrackProtocolAspect;
class VistaFastrackReferenceFrameAspect;
class VistaDriverInfoAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a driver for devices supporting some version of the FastRak protocol.
 * It was implemented on top of the Patriot and Liberty trackers, so it should
 * work for those. It was never tested with ISxxx trackers (ultrasonics), but
 * if one has the time, it should be adaptable to their protocol variant as well.
 * A side note: there was not much time developing this driver, so it might
 * have flaws, not work for you, be too slow, whatever... if you feel the
 * prickeling in your fingers and have the device ready: go ahead and fix it.
 *
 * The internal driver logic expects the connection attached to be a serial
 * line connection, as it twiddles with timeouts needed for serial lines. So
 * it might not work very good with USB connections whatsoever.
 *
 * One idea was to encapsulate the protocol variant in IFastrakCommandSet
 * specializations. This idea may be immature. But should be the way to go when
 * trying to move to another FastRak protocol variant.
 */
class VISTADEVICEDRIVERSAPI VistaFastrakDriver : public IVistaDeviceDriver
{
	friend class VistaFastrakAttachSequence;
public:
	struct _sFastrakSample
	{
		float m_vPos[3], // pos   x,y,z
			  m_vOri[4]; // quat, x,y,z, w
		int   m_nDistortionLevel,
			  m_nTimeStamp,
			  m_nFrameCount;
	};


	struct _sFastrakButtonSample : public _sFastrakSample
	{
		bool m_bBtPress;
	};


	VistaFastrakDriver();
	virtual ~VistaFastrakDriver();

	virtual bool Connect();

	virtual unsigned int GetSensorMeasureSize() const;
	static IVistaDriverCreationMethod *GetDriverFactoryMethod();


	void SetCommandSet( IVistaFastrakCommandSet *pSet );
	IVistaFastrakCommandSet *GetCommandSet() const;
protected:
	virtual bool PhysicalEnable(bool bEnable);
	virtual bool DoSensorUpdate(microtime dTs);
private:
	VistaDriverConnectionAspect    *m_pConnection;
	VistaDriverSensorMappingAspect *m_pSensorMap;
	VistaFastrackProtocolAspect    *m_pProtocol;
	VistaDriverInfoAspect          *m_pInfoAspect;
	VistaFastrackReferenceFrameAspect *m_pRefFrame;


	IVistaFastrakCommandSet         *m_pCommandSet;

	unsigned int m_nWandType,
		         m_nBodyType;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAFASTRAKDRIVER_H

