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
// $Id: VistaDTrackDriver.h 22143 2011-07-01 15:07:00Z dr165799 $

#ifndef _VISTADTRACKDRIVER_H
#define _VISTADTRACKDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"
#include "../VistaDeviceDriver.h"
#include <vector>
#include <map>
#include <string>

//#include "VistaCommandTransform.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;
class VistaByteBufferDeSerializer;
class ILineDecode;
class VistaDriverSensorMappingAspect;
class VistaDTrackProtocolAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a driver for A.R.T. optical tracking systems. It supports almost
 * all available features (2009), protocol 1 and 2 of the A.R.T. protocol,
 * measure devices and single markers.
 * It can be a beast to setup, especially, as the A.R.T. device is capable of
 * producing almost any kind of weird mapping from the bodies on the device
 * into the application. So this driver heavily relies on a proper setup mapping
 * aspect. It does, by itself, not create any sensors, but the sensors have
 * to be given from the outside to the driver. The update code merely decodes
 * incoming packets, filters sensors that are present and checks whether the
 * user has wanted them or not (indicated by: a sensor for an A.R.T. sensor id
 * is present in the set of sensors, e.g., the sensor mapping).
 * The driver is capable of attaching to a stream, avoiding to open the line
 * again. As the transfer protocol is UDP, this can be used to sniff on an active
 * transmission, or to put up a logging client.
 *
 * @todo the decoding can be somewhat slow (it uses stream classes...)
 */
class VISTADEVICEDRIVERSAPI VistaDTrackDriver : public IVistaDeviceDriver
{
public:
	VistaDTrackDriver();
	virtual ~VistaDTrackDriver();

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	virtual bool GetAttachOnly() const;
	virtual void SetAttachOnly(bool bAttach);

protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);


private:
	VistaDriverConnectionAspect     *m_pConnection;
	VistaDTrackProtocolAspect       *m_pProtocol;
	VistaDriverSensorMappingAspect  *m_pSensors;
	VistaByteBufferDeSerializer     *m_pDeSerializer,
									 *m_pLine;
	std::vector<VistaType::ubyte8> m_vecPacketBuffer;


	typedef std::map<std::string, ILineDecode*> DECODEMAP;
	DECODEMAP m_mapDecoder;
	unsigned int m_nGlobalType,
				 m_nMarkerType;

	bool m_bAttachOnly;
};


/**
 * memory layout of the global type sensor
 */
struct _sGlobalMeasure
{
	int   m_nFrameCount;
	float m_nTimeStamp;
};


/**
 * memory layout of the typical body measure
 */
struct _sBodyMeasure
{
	double  m_nId;
	double m_nQuality;
	double  m_nPos[3],
			m_nEuler[3],
			m_anRot[9];
};


/**
 * memory layout of the stick measure
 */
struct _sStickMeasure
{
	double  m_nId;
	double m_nQuality;
	double m_nButtonState;
	double  m_nPos[3],
			m_nEuler[3],
			m_anRot[9];
};

/**
 *  memory layout of the marker measure
 */
struct _sMarkerMeasure
{
	double m_nId;
	double m_nQuality,
		  m_nPos[3];
};

/**
 * memory layout of the measure device's measure
 */
struct _sMeasureMeasure
{
	double m_nId;
	double m_nQuality;
	double m_nButtonState;
	double m_nPos[3],
		   m_anRot[9];
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADTRACKDRIVER_H
