/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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

#include "VistaVRPNDriver.h"
#include <VistaDeviceDriversBase/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <time.h>
#include <vrpn_Analog.h>
#include <vrpn_Button.h>
#include <vrpn_Tracker.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

//extern const char * vrpn_MAGIC;


class CVRPNTranscoder : public IVistaMeasureTranscode
{
	  REFL_INLINEIMP(CVRPNTranscoder, IVistaMeasureTranscode);
public:
	CVRPNTranscoder()
	{
		// inherited as protected member
		m_nNumberOfScalars = 0;
	}


	static std::string GetTypeString() { return "CVRPNTranscoder"; }
};


class CVRPNAnalogTranscoder : public CVRPNTranscoder
{
	REFL_INLINEIMP(CVRPNAnalogTranscoder, CVRPNTranscoder)
public:
	CVRPNAnalogTranscoder()
	{
		m_nNumberOfScalars = 128;
	}

	static std::string GetTypeString() { return "CVRPNAnalogTranscoder"; }
};

class CVRPNButtonTranscoder : public CVRPNTranscoder
{
	REFL_INLINEIMP(CVRPNButtonTranscoder, CVRPNTranscoder)
public:
	CVRPNButtonTranscoder()
	{
		m_nNumberOfScalars = 0;
	}

	static std::string GetTypeString() { return "CVRPNButtonTranscoder"; }
};

class CVRPNTrackerTranscoder : public CVRPNTranscoder
{
	REFL_INLINEIMP(CVRPNTrackerTranscoder, CVRPNTranscoder)
public:
	CVRPNTrackerTranscoder()
	{
		m_nNumberOfScalars = 0;
	}

	static std::string GetTypeString() { return "CVRPNTrackerTranscoder"; }
};


class CVRPNTimeStampGet : public IVistaMeasureTranscode::TTranscodeValueGet<microtime>
{
public:
	CVRPNTimeStampGet()
	: IVistaMeasureTranscode::TTranscodeValueGet<microtime>(
			   "VRPN_TIMECODE", CVRPNTranscoder::GetTypeString(),
			   "transformed microstamp from the VRPN sample")
	{

	}

	virtual microtime GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		// the timeval is always the first member in the CB structs
		struct timeval *tv = (struct timeval*)&(*pMeasure).m_vecMeasures[0];
		return double(tv->tv_sec + (tv->tv_usec / 1000000.0));
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, microtime &dtOut) const
	{
		dtOut = GetValue(pMeasure);
		return true;
	}
};


class CVRPNAnalogValueGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	CVRPNAnalogValueGet()
	: IVistaMeasureTranscode::CScalarDoubleGet("VALUE",
			                                   CVRPNAnalogTranscoder::GetTypeString(),
											   "get analog value for a sample (indexed)")
	{

	}

	virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			                     double &dValue, unsigned int nIdx ) const
	{
		vrpn_ANALOGCB *cb = (vrpn_ANALOGCB*)&(*pMeasure).m_vecMeasures[0];
		if(cb->num_channel < (int)nIdx)
			return false; // not my channel, channel request out of bounds

		if(nIdx > vrpn_CHANNEL_MAX)
			return false; // index too high

		dValue = cb->channel[nIdx];

		return true;
	}
};

class CVRPNAnalogChannelGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	CVRPNAnalogChannelGet()
	: IVistaMeasureTranscode::CUIntGet("CHANNELS", CVRPNAnalogTranscoder::GetTypeString(),
			                           "query channel from current sample")
	{

	}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		vrpn_ANALOGCB *cb = (vrpn_ANALOGCB*)&(*pMeasure).m_vecMeasures[0];
		return cb->num_channel;
	}

	virtual bool GetValue( const VistaSensorMeasure *pMeasure, unsigned int &nValue ) const
	{
		nValue = GetValue(pMeasure);
		return true;
	}
};

class CVRPNAnalogChannelBlindGet : public IVistaMeasureTranscode::TTranscodeValueGet< std::vector<double> >
{
public:
	CVRPNAnalogChannelBlindGet()
	: IVistaMeasureTranscode::TTranscodeValueGet< std::vector<double> >("VALUES",
			                             CVRPNAnalogTranscoder::GetTypeString(),
			                             "retrieve current analog value by using the channel as given in the data")
	{

	}

	virtual std::vector<double> GetValue(const VistaSensorMeasure *pMeasure)    const
	{
		// the timeval is always the first member in the CB structs
		std::vector<double> vecRet;
		GetValue(pMeasure, vecRet);
		return vecRet;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, std::vector<double> &dtOut) const
	{
		vrpn_ANALOGCB *cb = (vrpn_ANALOGCB*)&(*pMeasure).m_vecMeasures[0];
		dtOut.resize( cb->num_channel );

		// we put trust in this that vrpn_float64 is always sizeof double ;)
		memcpy( &dtOut[0], cb->channel, cb->num_channel * sizeof(double) );
		return true;
	}
};

// ##############################################################################

class CVRPNButtonValueGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	CVRPNButtonValueGet()
	: IVistaMeasureTranscode::CUIntGet("STATE",
			                           CVRPNButtonTranscoder::GetTypeString(),
			                           "return the state of the button in this sample")
	{

	}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		vrpn_BUTTONCB *cb = (vrpn_BUTTONCB*)&(*pMeasure).m_vecMeasures[0];
		return cb->state;
	}

	virtual bool GetValue( const VistaSensorMeasure *pMeasure, unsigned int &nValue ) const
	{
		nValue = GetValue(pMeasure);
		return true;
	}
};

class CVRPNButtonNumberGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	CVRPNButtonNumberGet()
	: IVistaMeasureTranscode::CUIntGet("NUMBER",
			                           CVRPNButtonTranscoder::GetTypeString(),
			                           "returns the number of the button pushed")
	{

	}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		vrpn_BUTTONCB *cb = (vrpn_BUTTONCB*)&(*pMeasure).m_vecMeasures[0];
		return cb->button;
	}

	virtual bool GetValue( const VistaSensorMeasure *pMeasure, unsigned int &nValue ) const
	{
		nValue = GetValue(pMeasure);
		return true;
	}
};

class CVRPNButtonMaskGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	CVRPNButtonMaskGet()
	: IVistaMeasureTranscode::CUIntGet("BTMASK",
			                           CVRPNButtonTranscoder::GetTypeString(),
			                           "returns a 32bit mask for button state")
	{

	}

	virtual unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		vrpn_BUTTONCB *cb = (vrpn_BUTTONCB*)&(*pMeasure).m_vecMeasures[0];
		return (1UL << cb->button)*cb->state;
	}

	virtual bool GetValue( const VistaSensorMeasure *pMeasure, unsigned int &nValue ) const
	{
		nValue = GetValue(pMeasure);
		return true;
	}
};


// ##############################################################################

class CVRPNSensorNumberGet : public IVistaMeasureTranscode::CUIntGet
{
public:
	CVRPNSensorNumberGet()
	: IVistaMeasureTranscode::CUIntGet("SENSORINDEX",
			                           CVRPNTrackerTranscoder::GetTypeString(),
			                           "retrieve the sensor index of the report")
	{

	}

	virtual unsigned int GetValue(const VistaSensorMeasure *pMeasure ) const
	{
		vrpn_TRACKERCB *cb = (vrpn_TRACKERCB*)&(*pMeasure).m_vecMeasures[0];
		return cb->sensor;
	}

	virtual bool GetValue(const VistaSensorMeasure *pMeasure, unsigned int &value ) const
	{
		value = GetValue(pMeasure);
		return true;
	}
};

class CVRPNSensorPositionGet : public IVistaMeasureTranscode::CV3Get
{
public:
	CVRPNSensorPositionGet()
	: IVistaMeasureTranscode::CV3Get( "POSITION",
			                          CVRPNTrackerTranscoder::GetTypeString(),
			                          "retrieve position value of tracker record" )
	{

	}

	virtual VistaVector3D GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		VistaVector3D vec;
		GetValue(pMeasure, vec);
		return vec;
	}

	virtual bool GetValue( const VistaSensorMeasure *pMeasure, VistaVector3D &vec ) const
	{
		vrpn_TRACKERCB *cb = (vrpn_TRACKERCB*)&(*pMeasure).m_vecMeasures[0];
		vec[0] = float(cb->pos[0]); // have to round here
		vec[1] = float(cb->pos[1]);
		vec[2] = float(cb->pos[2]);
		return true;
	}
};


class CVRPNSensorOrientationGet : public IVistaMeasureTranscode::CQuatGet
{
public:
	CVRPNSensorOrientationGet()
	: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
			                           CVRPNTrackerTranscoder::GetTypeString(),
			                           "retrieve orientation value of tracker record")
	{

	}

	virtual VistaQuaternion GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		VistaQuaternion q;
		GetValue(pMeasure, q);
		return q;
	}

	virtual bool GetValue( const VistaSensorMeasure *pMeasure, VistaQuaternion &q ) const
	{
		vrpn_TRACKERCB *cb = (vrpn_TRACKERCB*)&(*pMeasure).m_vecMeasures[0];

		// vrpn stores q = (x,y,z,w) (like in vista)
		q[Vista::X] = (float)cb->quat[0];
		q[Vista::Y] = (float)cb->quat[1];
		q[Vista::Z] = (float)cb->quat[2];
		q[Vista::W] = (float)cb->quat[3];

		return true;
	}
};


// ##############################################################################
// PROPERTY GETTERS FOR DATA ACCESS USING TRANSCODERS
// ##############################################################################

namespace
{
	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new CVRPNTimeStampGet,
		new CVRPNAnalogValueGet,
		new CVRPNAnalogChannelBlindGet,
		new CVRPNAnalogChannelGet,

		new CVRPNButtonValueGet,
		new CVRPNButtonNumberGet,
		new CVRPNButtonMaskGet,

		new CVRPNSensorNumberGet,
		new CVRPNSensorPositionGet,
		new CVRPNSensorOrientationGet,

		NULL
	};


	IVistaPropertyGetFunctor *SaParameterGetter[] =
	{
			NULL
	};

	IVistaPropertySetFunctor *SaParameterSetter[] =
	{
			NULL
	};

	class CVRPNDriverCreationMethod : public IVistaDriverCreationMethod
	{
	public:
		virtual IVistaDeviceDriver *operator()()
		{
			return new VistaVRPNDriver;
		}
	};

	CVRPNDriverCreationMethod *g_SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaVRPNDriver::GetDriverFactoryMethod()
{
	if( g_SpFactory == NULL )
	{
		g_SpFactory = new CVRPNDriverCreationMethod;
		g_SpFactory->RegisterSensorType( "ANALOG",
										 sizeof(_vrpn_ANALOGCB),
										 100, new TDefaultTranscoderFactory<CVRPNAnalogTranscoder>,
										 CVRPNAnalogTranscoder::GetTypeString() );
		g_SpFactory->RegisterSensorType( "BUTTON",
										 sizeof(_vrpn_BUTTONCB),
										 100, new TDefaultTranscoderFactory<CVRPNButtonTranscoder>,
										 CVRPNButtonTranscoder::GetTypeString()  );
		g_SpFactory->RegisterSensorType( "TRACKER",
										 sizeof(_vrpn_TRACKERCB),
										 100, new TDefaultTranscoderFactory<CVRPNTrackerTranscoder>,
										 CVRPNTrackerTranscoder::GetTypeString() );

		g_SpFactory->AddTranscoderType( CVRPNTranscoder::GetTypeString() );
	}
	return g_SpFactory;

}



// ****************************************************************************
// PARAMETER API IMPLEMENTATION
// ****************************************************************************


/**
 * helper structure to map a sensor to a connection internally.
 * We need this structure to update the sensors during the
 * VRPN callbacks. VRPN will call a callback for every single item
 * in the network / vrpn queue. The bTick parameter is set as a
 * one time-set from every sensor map entity (once true, it will
 * remain true until the next call to "CVRPNDriver::DoSensorUpdate()"
 * However, we do use the Tick/Untick API for that to make sure,
 * no one resets the flag to false by accident and trap access to
 * the boolean value.
 */
VistaVRPNDriver::_cVRPN2SensorMap::_cVRPN2SensorMap(VistaVRPNDriver *pDriver,
		                                        vrpn_BaseClass *base,
		                                        unsigned int nId,
		                                        bool *bTick)
: m_pDriver(pDriver)
, m_pVRPNHandle(base)
, m_nSensorId(nId)
, m_bNewData(bTick)
, m_nUpdateTime(0)
{

}


void VistaVRPNDriver::_cVRPN2SensorMap::Tick()
{
	*m_bNewData = true;
}

void VistaVRPNDriver::_cVRPN2SensorMap::Untick()
{
	*m_bNewData = false;
}

bool VistaVRPNDriver::_cVRPN2SensorMap::GetTick() const
{
	return *m_bNewData;
}


namespace
{
	/**
	 * helper function template to record data of different type.
	 * We do a copy-in upon every receive of the data, so all we
	 * need is the assignment varying by type (see below).
	 * We do assume here, however, that the assignment operator
	 * is proper for the structure (typically it is for VRPN)
	 */
	template<class T> void recordData( IVistaDeviceDriver *pDriver,
			                           VistaDeviceSensor *pSensor,
			                           microtime nUpdateTime,
			                           const T &data )
	{
		// we work directly on the history, as some API we need in the
		// driver is not public (IVistaDeviceDriver::MeasureStart() for
		// example, but this is just a macro to operate on the history
		// anyways... so skip this here and work directly on the api
		VistaDriverMeasureHistoryAspect *pHistory
		           = dynamic_cast<VistaDriverMeasureHistoryAspect*>(
		        		     pDriver->GetAspectById(VistaDriverMeasureHistoryAspect::GetAspectId() ));
		if(!pHistory)
			return; // should be more verbose?

		// update start
		pHistory->MeasureStart(pSensor, nUpdateTime);

		// get the current place for the decoding for sensor 0
		VistaSensorMeasure *pM = pHistory->GetCurrentSlot(pSensor);

		// for the sake of readability: interpret the memory blob
		// as a place to store a sample of structure type T
		// this is a pointer cast, should cost almost nothing
		T *s = reinterpret_cast<T*>(&( *pM ).m_vecMeasures[0]);

		// store the data
		*s = data; // copy in

		// we are done. Indicate that to the history
		pHistory->MeasureStop(pSensor);
	}


	/**
	 * static callback to handle the analog data channels
	 * @param userdata we expect that to be of type _cVRPN2SensorMap
	 */
	void VRPN_CALLBACK handle_analog_channel(void * userdata,  const vrpn_ANALOGCB info)
	{
		VistaVRPNDriver::_cVRPN2SensorMap *pMap = reinterpret_cast<VistaVRPNDriver::_cVRPN2SensorMap*>( userdata );
		if(pMap == NULL)
			return;

		VistaDeviceSensor *pSensor = pMap->m_pDriver->GetSensorByIndex( pMap->m_nSensorId );
		if(pSensor) // should work... but we better check
		{
			recordData<vrpn_ANALOGCB>( pMap->m_pDriver, pSensor, pMap->m_nUpdateTime, info );
		}
		pMap->Tick(); // set new data flag to true
	}

	/**
	 * static callback to handle the button data channels
	 * @param userdata we expect that to be of type _cVRPN2SensorMap
	 */
	void VRPN_CALLBACK handle_button_channel(void *userdata, const vrpn_BUTTONCB info )
	{
		VistaVRPNDriver::_cVRPN2SensorMap *pMap = reinterpret_cast<VistaVRPNDriver::_cVRPN2SensorMap*>( userdata );
		if(pMap == NULL)
			return; // should be more verbose?

		VistaDeviceSensor *pSensor = pMap->m_pDriver->GetSensorByIndex( pMap->m_nSensorId );
		if(pSensor) // should work... but we better check
		{
			recordData<vrpn_BUTTONCB>( pMap->m_pDriver, pSensor, pMap->m_nUpdateTime, info );
		}

		pMap->Tick(); // set new data flag to true

	}


	/**
	 * static callback to handle the tracker data channels
	 * @param userdata we expect that to be of type _cVRPN2SensorMap
	 */
	void VRPN_CALLBACK handle_tracker_channel( void *userdata, const vrpn_TRACKERCB info )
	{
		VistaVRPNDriver::_cVRPN2SensorMap *pMap = reinterpret_cast<VistaVRPNDriver::_cVRPN2SensorMap*>( userdata );
		if(pMap == NULL)
			return; // should be more verbose?

		VistaDeviceSensor *pSensor = pMap->m_pDriver->GetSensorByIndex( pMap->m_nSensorId );
		if(pSensor) // should work... but we better check
		{
			recordData<vrpn_TRACKERCB>( pMap->m_pDriver, pSensor, pMap->m_nUpdateTime, info );
		}

		pMap->Tick(); // set new data flag to true
	}


	/**
	 * a simple functor to set the current timestamp to the callback
	 * structure and to call mainloop (e.g. poll on the sensor)
	 * for the vrpn_BaseObject. This should result in a number of "pushes"
	 * to the sensor
	 */
	class _cPollVrpn : public std::unary_function<bool, VistaVRPNDriver::_cVRPN2SensorMap>
	{
	public:
		_cPollVrpn(microtime nUpdateTime)
		: m_nUpdateTime(nUpdateTime)
		{

		}


		bool operator()( VistaVRPNDriver::_cVRPN2SensorMap *mp )
		{
			// copy current timestamp to callback structure
			mp->m_nUpdateTime = m_nUpdateTime;

			// call mainloop, this may result in a number of updates
			// on the history of the sensor attached to the mp structure
			mp->m_pVRPNHandle->mainloop();

			// that is why we update the timestamp now: readers will have
			// to wait for the batch of samples to come
			// update the time-stamp for waiting readers on the sensor
			mp->m_pDriver->GetSensorByIndex(mp->m_nSensorId)->SetUpdateTimeStamp( m_nUpdateTime );
			return mp->GetTick();
		}

		microtime m_nUpdateTime;
	};
}


/**
 * a helper structure to do the set-up during CVRPNDriver::Connect().
 * the idea is simple: use the sensor mapping aspect to create a callback
 * handle (cb structure) for each sensor that the user attached before
 * calling connect. The structure will comprise the driver and the sensor
 * index and so on, possibly all information needed during update.
 * Note that the sensor name plays an important role for the VRPN driver
 * to work, as currently there is no other way to connect to the VRPN
 * service for each sensor.
 */
class _cSetupFunctor : public std::unary_function<void, unsigned int>
{
public:
	_cSetupFunctor(VistaVRPNDriver *pDriver,
			VistaDriverSensorMappingAspect *pMapping)
	: m_pDriver(pDriver)
	, m_pMapping(pMapping)
	{

	}


	/**
	 * this is the actual operator called during the connect sequence:
	 * passing the sensor type, we look for the number of registered
	 * sensors PER TYPE and create a sensor for that accordingly.
	 * @see setup_sensor()
	 */
	void operator()(unsigned int nSensorType)
	{
		// get device sensor all for each type
		for(unsigned int n=0; n < m_pMapping->GetNumRegisteredSensorsForType(nSensorType); ++n)
		{
			unsigned int nMappedId = m_pMapping->GetSensorId(nSensorType, n);

			if(nMappedId != ~0)
				setup_sensor(nMappedId, nSensorType);
		}
	}


	/**
	 * the bulky load function for the sensor creation.
	 * @param nId the driver id of the sensor to set-up
	 * @param nType the transcode type to setup (this is important
	          to identify the correct callback, as VRPN does assign
	          statically typed callbacks to each type of sensor)
	 */
	void setup_sensor( unsigned int nId, unsigned int nType)
	{
		// get sensor from driver
		VistaDeviceSensor *pSensor = m_pDriver->GetSensorByIndex(nId);
		if(pSensor == NULL)
			return; // should be more verbose?

		// create connection based on the sensor name
		std::string strConnectionName = pSensor->GetSensorName();
		if(strConnectionName.empty())
			return; // should be more verbose?


		std::string strType;
		vrpn_BaseClass *pVrpnHandle = NULL;
		VistaVRPNDriver::_cVRPN2SensorMap *pMap = new VistaVRPNDriver::_cVRPN2SensorMap( m_pDriver,
									 NULL,
									 nId,
									 &m_pDriver->GetTick() );

		// check if whe have a type for that type-id
		if( g_SpFactory->GetTypeNameFor(nType, strType ) == true )
		{
			// could resolve type name, (good)

			// the below is the factory method for types of VRPN data transports
			// VRPN allows arbitrary schemes, so this might not be the appropriate
			// way to do it... but alas... analog, button and tracker are predominantly
			// used, as other types are realized with some effort in VRPN, so most
			// programmers seem to fallback to those types.
			if( VistaAspectsComparisonStuff::StringEquals(strType, "ANALOG", false) )
			{
				vrpn_Analog_Remote *pHandle = new vrpn_Analog_Remote( strConnectionName.c_str() );
				if( pHandle && pHandle->connectionPtr() && pHandle->connectionPtr()->doing_okay() ) //&& pHandle->connectionPtr()->connected() )
				{
					// ok, seemed to have worked
					pVrpnHandle = pHandle;
					pHandle->register_change_handler(pMap, handle_analog_channel );

					std::cout << "numChannels: " << pHandle->getNumChannels() << std::endl;
				}
			}
			else if(VistaAspectsComparisonStuff::StringEquals(strType, "BUTTON", false) )
			{
				vrpn_Button_Remote *pHandle = new vrpn_Button_Remote( strConnectionName.c_str() );
				if(pHandle && pHandle->connectionPtr() && pHandle->connectionPtr()->doing_okay() )
				{
					pVrpnHandle = pHandle;
					pHandle->register_change_handler(pMap, handle_button_channel );
				}
			}
			else if(VistaAspectsComparisonStuff::StringEquals(strType, "TRACKER", false) )
			{
				vrpn_Tracker_Remote *pHandle = new vrpn_Tracker_Remote( strConnectionName.c_str() );
				if(pHandle && pHandle->connectionPtr() && pHandle->connectionPtr()->doing_okay() )
				{
					pVrpnHandle = pHandle;
					pHandle->register_change_handler(pMap, handle_tracker_channel );
				}
			}
		}

		VistaDriverInfoAspect *info
				= dynamic_cast<VistaDriverInfoAspect*>(m_pDriver->GetAspectById( VistaDriverInfoAspect::GetAspectId() ) );

		VistaPropertyList &oSensorSection = (*info).GetInfoPropsWrite().GetPropertyRef("SENSORS").GetPropertyListRef();
		std::string prefix = VistaAspectsConversionStuff::ConvertToString(nId)
		                   + std::string(":")
		                   + VistaAspectsConversionStuff::ConvertToString(nType)
						   + std::string(" (")
						   + strType + ") ";

		// note that the above does only tell that a VRPN server was found at the
		// given IP. It does however not tell anything about the fact if there
		// was a sensor installed with the prefix of the connection string,
		// we do not care for that now, so let's go...
		if( pVrpnHandle )
		{
			oSensorSection.SetStringValue(prefix + strConnectionName, "connected");
			pMap->m_pVRPNHandle = pVrpnHandle;
			m_pDriver->AddVRPNSensor( pMap ); // set map to driver
		}
		else
		{
			delete pMap;
			oSensorSection.SetStringValue(prefix + strConnectionName, "failed");
		}
	}

	VistaVRPNDriver *m_pDriver;
	VistaDriverSensorMappingAspect *m_pMapping;
};



/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaVRPNDriver::VistaVRPNDriver()
: IVistaDeviceDriver()
,  m_pInfo(new VistaDriverInfoAspect )
, m_pMappingAspect( new VistaDriverSensorMappingAspect )
, m_bNewData(false)
, m_bConnected(false)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);
	RegisterAspect( m_pInfo );
	RegisterAspect( m_pMappingAspect );

	m_pMappingAspect->RegisterType( "ANALOG",
			                        sizeof(_vrpn_ANALOGCB),
			                        100,
			                        new TDefaultTranscoderFactory<CVRPNAnalogTranscoder> );
	m_pMappingAspect->RegisterType( "BUTTON",
			                        sizeof(_vrpn_BUTTONCB),
			                        100,
			                        new TDefaultTranscoderFactory<CVRPNButtonTranscoder> );
	m_pMappingAspect->RegisterType( "TRACKER",
			                        sizeof(_vrpn_TRACKERCB),
			                        100,
			                        new TDefaultTranscoderFactory<CVRPNTrackerTranscoder> );


	// plug some information, while at it
	VistaPropertyList &props = m_pInfo->GetInfoPropsWrite();
	std::string sVRPNVersion( vrpn_cookie_size(), ' ' );
	write_vrpn_cookie( &sVRPNVersion[0], vrpn_cookie_size(), 0 );
	props.SetStringValue("VRPN_VERSION", sVRPNVersion );
	props.SetPropertyListValue("SENSORS", VistaPropertyList() );
}

VistaVRPNDriver::~VistaVRPNDriver()
{
	for( std::vector<_cVRPN2SensorMap*>::iterator it = m_vecHandles.begin();
		it != m_vecHandles.end(); ++it )
	{
		delete (*it)->m_pVRPNHandle; // should close and kill connection
		delete *it; // kill pointer
	}


	UnregisterAspect( m_pInfo, false );
	delete m_pInfo;

	UnregisterAspect( m_pMappingAspect, false );
	delete m_pMappingAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaVRPNDriver::Connect()
{
	if(m_bConnected)
		return false; // already connected

	// we assume that all sensors are already attached and ready

	std::list<unsigned int> liTypes = g_SpFactory->GetTypes();
	std::for_each( liTypes.begin(), liTypes.end(), _cSetupFunctor(this, m_pMappingAspect) );

	m_bConnected = true;
	return m_bConnected;
}




bool VistaVRPNDriver::PhysicalEnable(bool bEnable)
{
	return true;
}


bool VistaVRPNDriver::DoSensorUpdate(microtime nTs)
{
	m_bNewData = false; // will be set by side effect...

	for_each( m_vecHandles.begin(), m_vecHandles.end(), _cPollVrpn(nTs) );

	return m_bNewData; // was be changed as a side-effect when new data was coming in
}


bool VistaVRPNDriver::AddVRPNSensor( _cVRPN2SensorMap *mp )
{
	m_vecHandles.push_back(mp);
	return true;
}

bool &VistaVRPNDriver::GetTick()
{
	return m_bNewData;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

