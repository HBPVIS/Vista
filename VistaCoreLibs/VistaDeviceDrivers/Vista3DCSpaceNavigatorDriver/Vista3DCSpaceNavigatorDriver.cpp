/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

// include header here

#include <cassert>

#if defined(WIN32)

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atlstr.h>

using namespace ATL;

#import "progid:TDxInput.Device.1" no_namespace

#include <windows.h>
#include <float.h>

#else

#include <linux/input.h>
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <cassert>
#endif // defined WIN32


#if !defined(_isnan)
#define _isnan(A) (A != A)
#endif

#include "Vista3DCSpaceNavigatorDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>

#include <VistaBase/VistaTimerImp.h>

#include <VistaBase/VistaTimerImp.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
Vista3DCSpaceNavigatorCreateMethod::Vista3DCSpaceNavigatorCreateMethod( IVistaTranscoderFactoryFactory *fac )
: IVistaDriverCreationMethod(fac)
{

	// we assume an update rate of 20Hz at max. Devices we inspected reported
	// an update rate of about 17Hz, which seem reasonable.
	RegisterSensorType( "", sizeof(Vista3DCSpaceNavigator::_sMeasure),
								   20,
								   fac->CreateFactoryForType("") );
}

IVistaDeviceDriver *Vista3DCSpaceNavigatorCreateMethod::CreateDriver()
{
	return new Vista3DCSpaceNavigator(this);
}



#if defined(WIN32)
struct Vista3DCSpaceNavigator::_sPrivate
{
	_sPrivate()
		: m_nKeyStates(0),
		  m_bLastZero(false)
	{
	}

	CComPtr<ISensor>   m_3DSensor;
	CComPtr<IKeyboard> m_3DKeyboard;
	__int64            m_nKeyStates;
	_sMeasure          m_measure;
	bool               m_bLastZero;
};

#else
struct Vista3DCSpaceNavigator::_sPrivate
{
	_sPrivate()
		: m_iFD(-1),
		  m_bLastZero(false),
		  m_rx(0), m_ry(0), m_rz(0),
		  m_dTs(IVistaTimerImp::GetSingleton()->GetSystemTime()) // should think about this... has a side-effect

	{
	}

	int  m_iFD;
	bool m_bLastZero;
	int  m_rx, m_ry, m_rz;
	double m_dTs;
	_sMeasure m_measure;
};
#endif


Vista3DCSpaceNavigator::Vista3DCSpaceNavigator(IVistaDriverCreationMethod *crm)
	: IVistaDeviceDriver(crm),
	  m_pPrivate(new _sPrivate),
	  m_pWorkspace(new VistaDriverWorkspaceAspect)
{
	m_nOldKeys[0] = 0;
	m_nOldKeys[1] = 0;

	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	AddDeviceSensor( pSensor );
	pSensor->SetTypeHint( GetFactory()->GetTypeFor("") );
	pSensor->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );


	VistaBoundingBox bb;

	bb.m_v3Min[0] = bb.m_v3Min[1] = bb.m_v3Min[2] = -400;
	bb.m_v3Max[0] = bb.m_v3Max[1] = bb.m_v3Max[2] =  400;

	m_pWorkspace->SetWorkspace( "KNOB", bb );

	RegisterAspect( m_pWorkspace );
}

Vista3DCSpaceNavigator::~Vista3DCSpaceNavigator()
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	IVistaMeasureTranscode *pTr = pSensor->GetMeasureTranscode();
	pSensor->SetMeasureTranscode(NULL);
	GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(pTr);

	RemDeviceSensor( pSensor );
	delete pSensor;

#if defined(WIN32)
	CComPtr<ISimpleDevice> _3DxDevice;

	// Release the sensor and keyboard interfaces
	if (m_pPrivate->m_3DSensor)
	{
		m_pPrivate->m_3DSensor->get_Device((IDispatch**)&_3DxDevice);
		m_pPrivate->m_3DSensor.Release();
	}

	if (m_pPrivate->m_3DKeyboard)
		m_pPrivate->m_3DKeyboard.Release();

	if (_3DxDevice)
	{
		// Disconnect it from the driver
		_3DxDevice->Disconnect();
		_3DxDevice.Release();
	}

	CoUninitialize();
#else
	if( m_pPrivate->m_iFD != -1)
	{
		// turn off led!
		input_event out_event;
		memset(&out_event, 0, sizeof(input_event));
		out_event.type = EV_LED;
		out_event.code = LED_MISC;
		out_event.value = 0;

		int nRet = write( m_pPrivate->m_iFD, &out_event, sizeof(input_event) );
		assert(nRet == sizeof(input_event));

		close(m_pPrivate->m_iFD);
	}
#endif
	delete m_pPrivate;

	UnregisterAspect( m_pWorkspace, false );
	delete m_pWorkspace;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool Vista3DCSpaceNavigator::Connect()
{
#if defined(WIN32)
	m_nOldKeys[0]=m_nOldKeys[1] = -1;
	HRESULT hr;
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED );

	CComPtr<IUnknown> _3DxDevice;

	// Create the device object
	hr = _3DxDevice.CoCreateInstance(__uuidof(Device));
	if (SUCCEEDED(hr))
	{
		CComPtr<ISimpleDevice> _3DxSimpleDevice;

		hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
		if (SUCCEEDED(hr))
		{
			// Get the interfaces to the sensor and the keyboard;
			m_pPrivate->m_3DSensor   = _3DxSimpleDevice->Sensor;
			m_pPrivate->m_3DKeyboard = _3DxSimpleDevice->Keyboard;

			// Associate a configuration with this device
			//_3DxSimpleDevice->LoadPreferences(_T("ViSTA"));
			// Connect to the driver
			_3DxSimpleDevice->Connect();
			return true;
		}
	}
	else
		return false;
#else
	// enumerate /dev/event* devices, check product/vendor id, pick first device
	std::string sDir = "/dev/input/";
	DIR *devdir = opendir((char*)sDir.data());
	if( devdir == NULL )
		return false; // we can not work like that...

	dirent *entry;

	while( (entry = readdir(devdir)) )
	{
		// enumerate all event* files in /dev/input
		if( strncmp( entry->d_name, "event", 5 ) == 0 )
		{
			int fd = open( (sDir + (entry->d_name)).c_str(), O_RDWR );
			if(fd == -1)
			{
				std::cerr << "[3DCSpaceNav] could not open [" << (sDir + (entry->d_name)) << "]. permissions?\n";
				continue;
			}

			input_id device_info;
			// suck out device information
			if(ioctl( fd, EVIOCGID, &device_info)) {
				perror("[3DCSpaceNav] failed to get device information");
				std::cout << "[3DCSpaceNav] file: " << (sDir + (entry->d_name)) << std::endl;
				close( fd );
				continue;
			}

			// check specifically for
			// Logitech (0x046d)
			// 3DConnexion Space Navigator 3D Mouse (0xc626)
			if( device_info.vendor == 0x046d &&
				(device_info.product == 0xc626 || /* SpaceNavigator */
				 device_info.product == 0xc628 )) /* SpaceNavigator for Notebooks */
			{
				std::cout << "[3DCSpaceNav] SpaceNavigator device detected on event interface "
					<< sDir + entry->d_name << ". keeping fileconnection open."
					<< std::endl;

				m_pPrivate->m_iFD = fd;

				// turn on led!
				input_event out_event;
				memset(&out_event, 0, sizeof(input_event));
				out_event.type = EV_LED;
				out_event.code = LED_MISC;
				out_event.value = 1;
				int nRet = write( m_pPrivate->m_iFD, &out_event, sizeof(input_event) );
				assert(nRet == sizeof(input_event));
				closedir(devdir); // close stream for directory
				return true;

			}
			else
			{
				printf ("[3DCSpaceNav] Unknown product. vendor id is 0x%04x, product id is 0x%04x.\n", device_info.vendor, device_info.product);
			}
			close( fd );
		}
	}

	closedir(devdir); // close stream
#endif
	return false;
}

bool Vista3DCSpaceNavigator::DoSensorUpdate(VistaType::microtime dTs)
{
	bool bNewState = false;

#if defined(WIN32)

	if (m_pPrivate->m_3DKeyboard)
	{
		// Check if any change to the keyboard state
		try {
			// note: the device seems to have trouble when both buttons
			// are pressed... hmm, or I misinterpreted the API?
			// anyhow, state cluttering can happen and the device reports
			// buttons set which are not pressed... if you do it slowly,
			// there does not seem to be a problem...
			for (long i=1; i<=2; i++)
			{
				//__int64 mask = (__int64)1<<(i-1);
				VARIANT_BOOL isPressed;
				isPressed = m_pPrivate->m_3DKeyboard->IsKeyDown(i);
				if (isPressed == VARIANT_TRUE)
				{
					m_pPrivate->m_measure.m_nKeys[i-1] = 1;
				}
				else
				{
					// assume up then
					if(m_pPrivate->m_3DKeyboard->IsKeyUp(i))
					{
						m_pPrivate->m_measure.m_nKeys[i-1] = 0;
					}
				}

				if(m_pPrivate->m_measure.m_nKeys[i-1] != m_nOldKeys[i-1])
				{
					bNewState = true;
					m_nOldKeys[i-1] = m_pPrivate->m_measure.m_nKeys[i-1];
				}
			}
		}
		catch (...) {
			// Some sort of exception handling
			return false;
		}
	}
	if (m_pPrivate->m_3DSensor)
	{
		try {

			CComPtr<IAngleAxis> pRotation   = m_pPrivate->m_3DSensor->Rotation;
			CComPtr<IVector3D> pTranslation = m_pPrivate->m_3DSensor->Translation;

			// do measure here, now!
			pTranslation->get_X(&m_pPrivate->m_measure.m_nPositionX);
			pTranslation->get_Y(&m_pPrivate->m_measure.m_nPositionY);
			pTranslation->get_Z(&m_pPrivate->m_measure.m_nPositionZ);
			pTranslation->get_Length(&m_pPrivate->m_measure.m_nLength);

			pRotation->get_X(&m_pPrivate->m_measure.m_nRotationX);
			pRotation->get_Y(&m_pPrivate->m_measure.m_nRotationY);
			pRotation->get_Z(&m_pPrivate->m_measure.m_nRotationZ);
			pRotation->get_Angle(&m_pPrivate->m_measure.m_nRotationAngle);

			// we want to avoid a continuous report of 0 values
			// in each sampling, so we test for the first 0 values here
			// report the first encountered (checking for bLastZero)
			// and reset to non-zero flag on a first new value then
	         if (pRotation->Angle == 0. && pTranslation->Length == 0.)
		     {
				 // we assume these values to be non-ill behaved, 0==0
				 if(!m_pPrivate->m_bLastZero)
				 {
					 // last value was no zero, set values was ok
					 bNewState = true;
					 m_pPrivate->m_bLastZero = true;
				 }
			 }
			 else
			 {
				 bNewState = true;
				 m_pPrivate->m_bLastZero = false;
			 }

			pRotation.Release();
			pTranslation.Release();
		}
		catch (...)
		{
			// Some sort of exception handling
			return false;
		}
	}
#else
	if( m_pPrivate->m_iFD == -1 )
		return false;

	int bytes_read = 0;
	input_event inputev;

	struct pollfd fds;
	fds.fd = m_pPrivate->m_iFD;
	fds.events  = POLLIN;
	fds.revents = 0;

	poll(&fds, 1, 0);

	bool pending = (fds.revents & POLLIN);

	// read all raw input events from the event layer
	while( pending )
	{
		int nRet = read( m_pPrivate->m_iFD, &inputev, sizeof(struct input_event) );
		assert(nRet == sizeof(input_event));

		switch( inputev.type )
		{
			case EV_KEY:
			{
				m_pPrivate->m_measure.m_nKeys[inputev.code-256] = inputev.value;
				m_pPrivate->m_bLastZero = false;
				bNewState = true;
				break;
			}
			case EV_ABS:
			case EV_REL:
			{
				switch( inputev.code )
				{
					// the windows implementation maps x->x, y->y, z->z
					// which the transcode hands over unchanged.
					// we swap/invert the axes here, to have the same measure
					// content, get the same results from the unchanged transcode.
				case REL_X:
					m_pPrivate->m_measure.m_nPositionX = inputev.value;
					break;
				case REL_Y:
					m_pPrivate->m_measure.m_nPositionZ = inputev.value;
					break;
				case REL_Z:
					m_pPrivate->m_measure.m_nPositionY = -inputev.value;
					break;
				case REL_RX:
					m_pPrivate->m_rx = inputev.value;
					break;
				case REL_RY:
					m_pPrivate->m_rz = inputev.value;
					break;
				case REL_RZ:
					m_pPrivate->m_ry = -inputev.value;
					break;
				}
				m_pPrivate->m_bLastZero = false;
				bNewState = true;
				break;
			}
			default:
				break;
		}

		m_pPrivate->m_dTs = IVistaTimerImp::GetSingleton()->GetSystemTime();
		poll(&fds, 1, 0);
		pending = (fds.revents & POLLIN);
	}

	// simulate 0 measure if no event arrived for 60ms
	//
	// we have to do this because the device from 3dconnexion is
	// broken, in the respect that it reports in its HID descriptor
	// to deliver relative values, but delivers absolute values actually.
	// the linux kernel input subsystem swallows the 0 for relative event
	// types, thus we emulate the measure after a threshold of 60ms.
	// the device reports event with  ~20Hz, so...
	if( (bNewState == false) && m_pPrivate->m_dTs < (IVistaTimerImp::GetSingleton()->GetSystemTime() - 0.06f) )
	{
		if(!m_pPrivate->m_bLastZero)
		{
			m_pPrivate->m_measure.m_nPositionX = 0;
			m_pPrivate->m_measure.m_nPositionY = 0;
			m_pPrivate->m_measure.m_nPositionZ = 0;
			m_pPrivate->m_rx  = 0;
			m_pPrivate->m_ry  = 0;
			m_pPrivate->m_rz  = 0;
			m_pPrivate->m_dTs = 0;
			bNewState = true;
			m_pPrivate->m_bLastZero = true;
		}
	}

	// transform the values
	// into the format which the windows implementation gives, and
	// which is expected by the transcodes.
	// the linux event layer (as well as libspnav..) gives the raw axis
	// values (-400;400) for the rotation axes. they have to be transformed
	// into a normalized rotation axis and angle.
	if( bNewState )
	{
		// scale the angles like this:
		// -400 -> -PI/2
		//  400 ->  PI/2
		// -> 90 degrees in both directions
		const float pi_half = Vista::Pi/2.0f;
		float rx = m_pPrivate->m_rx*pi_half/400.0f;
		float ry = m_pPrivate->m_ry*pi_half/400.0f;
		float rz = m_pPrivate->m_rz*pi_half/400.0f;
		if( rx < -pi_half ) rx = -pi_half;
		if( rx >  pi_half ) rx =  pi_half;
		if( ry < -pi_half ) ry = -pi_half;
		if( ry >  pi_half ) ry =  pi_half;
		if( rz < -pi_half ) rz = -pi_half;
		if( rz >  pi_half ) rz =  pi_half;

		VistaQuaternion xrot( VistaAxisAndAngle( VistaVector3D(1,0,0), rx ) );
		VistaQuaternion yrot( VistaAxisAndAngle( VistaVector3D(0,1,0), ry ) );
		VistaQuaternion zrot( VistaAxisAndAngle( VistaVector3D(0,0,1), rz ) );

		VistaQuaternion q = xrot*yrot*zrot;
		q.Normalize();
		VistaAxisAndAngle aaa( q.GetAxisAndAngle() );

		m_pPrivate->m_measure.m_nRotationX = aaa.m_v3Axis[0];
		m_pPrivate->m_measure.m_nRotationY = aaa.m_v3Axis[1];
		m_pPrivate->m_measure.m_nRotationZ = aaa.m_v3Axis[2];
		m_pPrivate->m_measure.m_nRotationAngle = aaa.m_fAngle*180.0f/Vista::Pi;
	}

#endif
	if(bNewState)
	{
		MeasureStart( 0, dTs );
		VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));

		if(pM == NULL)
		{
			MeasureStop(0);
			return false;
		}

		_sMeasure *m = (_sMeasure*)&(*pM).m_vecMeasures[0];
		memcpy( m, &m_pPrivate->m_measure, sizeof(_sMeasure) );

		MeasureStop(0);

		GetSensorByIndex(0)->SetUpdateTimeStamp( dTs );
	}


	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


