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

#include <DTrack.hpp>

#include "VistaDTrackSDKDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverThreadAspect.h>

class VistaDTrackSDKMarkerTranscode : public IVistaMeasureTranscode
{
public:
	VistaDTrackSDKMarkerTranscode()
	{
		m_nNumberOfScalars = 0;
	}

	static std::string GetTypeString() { return "VistaDTrackSDKMarkerTranscode"; }
	REFL_INLINEIMP(VistaDTrackSDKMarkerTranscode, IVistaMeasureTranscode);
};

class VistaDTrackSDKBodyTranscode : public IVistaMeasureTranscode
{
public:
	VistaDTrackSDKBodyTranscode()
	{
		m_nNumberOfScalars = 0;
	}

	static std::string GetTypeString() { return "VistaDTrackSDKBodyTranscode"; }
	REFL_INLINEIMP(VistaDTrackSDKBodyTranscode, IVistaMeasureTranscode);
};


class VistaDTrackSDKQualityTranscode : public IVistaMeasureTranscode::CDoubleGet
{
public:
	VistaDTrackSDKQualityTranscode(const std::string &strTranscoderClassName,
			                    const std::string &strDescription )
	: IVistaMeasureTranscode::CDoubleGet("QUALITY", strTranscoderClassName, strDescription )
	{

	}

	double GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		// for quality, all measures have the same layout
		dtrack_marker_type *m = (dtrack_marker_type*)&(*pMeasure).m_vecMeasures[0];
		return double(m->quality);
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, double &q) const
	{
		q = GetValue(pMeasure);
		return true;
	}
};

class VistaDTrackSDKIdTranscode : public IVistaMeasureTranscode::CUIntGet
{
public:
	VistaDTrackSDKIdTranscode(const std::string &strTranscoderClassName,
				                    const std::string &strDescription )
		: IVistaMeasureTranscode::CUIntGet("ID", strTranscoderClassName, strDescription )
		{

		}

		unsigned int GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			// for quality, all measures have the same layout
			dtrack_marker_type *m = (dtrack_marker_type*)&(*pMeasure).m_vecMeasures[0];
			return (unsigned int)m->id;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, unsigned int &id) const
		{
			id = GetValue(pMeasure);
			return true;
		}
};

class VistaDTrackSDKPosTranscode : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaDTrackSDKPosTranscode(const std::string &strTranscodeClassName,
			                    const std::string &strDescription )
		: IVistaMeasureTranscode::CV3Get("POSITION",
				strTranscodeClassName, strDescription) {}

	virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		// maker and body structs have the same layout
		dtrack_marker_type *m = (dtrack_marker_type*)&(*pMeasure).m_vecMeasures[0];

		return VistaVector3D(
					float( m->loc[0]),
					float( m->loc[1]),
					float( m->loc[2]));
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
	{
		v3Pos = GetValue(pMeasure);
		return true;
	}
};

class VistaDTrackSDKBodyOrientationTranscode : public IVistaMeasureTranscode::CQuatGet
{
public:
	VistaDTrackSDKBodyOrientationTranscode()
		: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
				VistaDTrackSDKBodyTranscode::GetTypeString(),
								"DTrack SDK 3D orientation (right-handed)") {}

	virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		dtrack_body_type *m = (dtrack_body_type*)&(*pMeasure).m_vecMeasures[0];
		VistaTransformMatrix mt (float(m->rot[0]), float(m->rot[3]), float(m->rot[6]), 0,
								  float(m->rot[1]), float(m->rot[4]), float(m->rot[7]), 0,
								  float(m->rot[2]), float(m->rot[5]), float(m->rot[8]), 0,
								  0    , 0    , 0    , 1);
		VistaQuaternion q(mt);
		q.Normalize();
		return q;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &qRot) const
	{
		qRot = GetValue(pMeasure);
		return true;
	}
};

namespace
{
	IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaDTrackSDKPosTranscode(VistaDTrackSDKMarkerTranscode::GetTypeString(),
				"dtrack SDK MARKER 3D position values"),
		new VistaDTrackSDKPosTranscode(VistaDTrackSDKBodyTranscode::GetTypeString(),
				"dtrack SDK BODY 3D position values"),
		new VistaDTrackSDKBodyOrientationTranscode,
		new VistaDTrackSDKIdTranscode(VistaDTrackSDKMarkerTranscode::GetTypeString(),
				"dtrack SDK MARKER 3D id"),
		new VistaDTrackSDKIdTranscode(VistaDTrackSDKBodyTranscode::GetTypeString(),
				"dtrack SDK BODY 3D id"),
		new VistaDTrackSDKQualityTranscode(VistaDTrackSDKBodyTranscode::GetTypeString(),
				"dtrack SDK BODY quality"),
		new VistaDTrackSDKQualityTranscode(VistaDTrackSDKMarkerTranscode::GetTypeString(),
				"dtrack SDK MARKER quality"),
		NULL
	};
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace
{
	VistaDTrackSDKDriverCreateMethod *SpFactory = NULL;
}


IVistaDeviceDriver *VistaDTrackSDKDriverCreateMethod::operator()()
{
	return new VistaDTrackSDKDriver;
}

class VistaDTrackSDKMarkerTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDTrackSDKMarkerTranscode;
	}
};

class VistaDTrackSDKBodyTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDTrackSDKBodyTranscode;
	}
};

IVistaDriverCreationMethod *VistaDTrackSDKDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaDTrackSDKDriverCreateMethod;

		// we assume an update rate of 20Hz at max. Devices we inspected reported
		// an update rate of about 17Hz, which seem reasonable.
		SpFactory->RegisterSensorType( "BODY", sizeof(dtrack_body_type),
			                           60,
									   new VistaDTrackSDKBodyTranscodeFactory,
									   VistaDTrackSDKBodyTranscode::GetTypeString() );
		SpFactory->RegisterSensorType( "MARKER", sizeof(dtrack_marker_type),
			                           60,
									   new VistaDTrackSDKMarkerTranscodeFactory,
									   VistaDTrackSDKMarkerTranscode::GetTypeString() );

	}

	return SpFactory;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*VistaDTrackSDKParameterContainerCreate : 
	public VistaDriverGenericParameterAspect::IContainerCreate
{
public:
	IParameterContainer *CreateContainer();
	bool DeleteContainer( IParameterContainer *pContainer );
};
*/
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaDTrackSDKDriver::VistaDTrackSDKDriver()
	: IVistaDeviceDriver()
	, m_pDriver(NULL)
	, m_pThread(NULL)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( GetDriverFactoryMethod()->GetTypeFor("BODY") );
	pSensor->SetSensorName("BODY0");
	AddDeviceSensor( pSensor );
	pSensor->SetMeasureTranscode( new VistaDTrackSDKBodyTranscode );

	pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( GetDriverFactoryMethod()->GetTypeFor("MARKER") );
	AddDeviceSensor( pSensor );
	pSensor->SetSensorName("MARKER0");
	pSensor->SetMeasureTranscode( new VistaDTrackSDKMarkerTranscode );

//	m_pParameters = new VistaGenericParameterAspect

	m_pThread = new VistaDriverThreadAspect(this);
	RegisterAspect(m_pThread);
}

VistaDTrackSDKDriver::~VistaDTrackSDKDriver()
{
	UnregisterAspect(m_pThread,false);
	delete m_pThread;

	VistaDeviceSensor *pBodySensor = GetSensorByIndex(0);
	VistaDeviceSensor *pMarkerSensor = GetSensorByIndex(1);

	RemDeviceSensor( pBodySensor );
	RemDeviceSensor( pMarkerSensor );
	delete pBodySensor;
	delete pMarkerSensor;

	delete m_pDriver;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
unsigned int VistaDTrackSDKDriver::GetSensorMeasureSize() const
{
	return sizeof(dtrack_body_type); // default size
}

bool VistaDTrackSDKDriver::PhysicalEnable(bool bEnabled)
{
	if(!m_pDriver)
		return true;

	if( m_pDriver->cmd_cameras(bEnabled) )
		return true;

	m_pDriver->udperror(); // print error
	return false;
}

bool VistaDTrackSDKDriver::Connect()
{
	if(m_pDriver)
		return false; // do not connect on state connected

	m_pDriver = new DTrack(5000, "dtrack", 5001);

#ifdef DEBUG
	std::cout << "[VistaDTrackSDKDriver] Connect(): instantiated driver." 
			  << std::endl;
#endif
	
	if(m_pDriver->valid())
	{
#ifdef DEBUG
		std::cout << "[VistaDTrackSDKDriver] DTrack driver is valid, i.e. UDP socket could be created." 
				  << std::endl;
#endif
		return true;
	}
	else
		m_pDriver->udperror();

	std::cerr << "[VistaDTrackSDKDriver] failed to create UDP socket to tracker, aborting Connect()!" 
			  << std::endl;

	delete m_pDriver;
	m_pDriver = NULL; // user may try re-connect

	return false;
}

bool VistaDTrackSDKDriver::UpdateBodySensor( microtime dTs, int nIdx )
{
	VistaDeviceSensor *pBodySensor = GetSensorByIndex(0);
	MeasureStart( nIdx, dTs );
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pBodySensor);

	if(pM == NULL)
	{
		MeasureStop(0);
		return false;
	}

	dtrack_body_type *m = (dtrack_body_type*)&(*pM).m_vecMeasures[0];

	*m = m_pDriver->get_body(nIdx);

	MeasureStop(nIdx);

	pBodySensor->SetUpdateTimeStamp( dTs );

	return true;
}

bool VistaDTrackSDKDriver::UpdateMarker( microtime dTs, int nIdx )
{
	VistaDeviceSensor *pMarkerSensor = GetSensorByIndex(1);
	MeasureStart( nIdx, dTs );
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pMarkerSensor);

	if(pM == NULL)
	{
		MeasureStop(1);
		return false;
	}

	dtrack_body_type *m = (dtrack_body_type*)&(*pM).m_vecMeasures[0];
	*m = m_pDriver->get_body(nIdx);


	MeasureStop(nIdx);

	pMarkerSensor->SetUpdateTimeStamp( dTs );

	return true;
}


bool VistaDTrackSDKDriver::DoSensorUpdate(microtime dTs)
{
	if(m_pDriver->receive())
	{
		UpdateBodySensor(dTs, 0);
		UpdateMarker(dTs, 1);
	}
	else
	{
		m_pDriver->udperror();
		m_pDriver->parseerror();
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

