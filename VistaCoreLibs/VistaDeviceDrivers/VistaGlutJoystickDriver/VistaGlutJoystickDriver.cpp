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
// $Id: VistaGlutJoystickDriver.cpp 4783 2009-10-02 22:33:37Z ingoassenmacher $

#include "VistaGlutJoystickDriver.h"
#include <map>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>

#include <GL/glut.h>
#include <algorithm>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VistaGlutJoystickDriverCreationMethod::VistaGlutJoystickDriverCreationMethod( IVistaTranscoderFactoryFactory *fac )
: IVistaDriverCreationMethod(fac)
{
	RegisterSensorType( "",  sizeof(VistaGlutJoystickDriver::_sJoyMeasure),
					   100,
					   fac->CreateFactoryForType("") );

}

IVistaDeviceDriver *VistaGlutJoystickDriverCreationMethod::CreateDriver()
{
	return new VistaGlutJoystickDriver(this);
}




class UVistaJoystickMap
{
public:
	typedef std::map<int, VistaGlutJoystickDriver *> DEVMAP;
	DEVMAP m_mapDevices;

	VistaGlutJoystickDriver *RetrieveJoystickFromWindowId(int nWindow) const
	{
		DEVMAP::const_iterator cit = m_mapDevices.find(nWindow);
		if(cit == m_mapDevices.end())
			return NULL; // ?

		return (*cit).second;
	}

	bool RegisterJoystickWithWindow(int nWindowId, VistaGlutJoystickDriver *pAddDriver)
	{
		VistaGlutJoystickDriver *pDriver = RetrieveJoystickFromWindowId(nWindowId);
		if(pDriver)
			return (pDriver == pAddDriver);


		m_mapDevices[nWindowId] = pAddDriver;

		return true;
	}

	bool UnregisterJoystickFromWindow(int nWindowId, VistaGlutJoystickDriver *pAddDriver)
	{
		DEVMAP::iterator it = m_mapDevices.find(nWindowId);
		if(it == m_mapDevices.end())
			return true;

		if((*it).second != pAddDriver)
			return false;

		m_mapDevices.erase(it);

		return true;
	}
};

static UVistaJoystickMap S_mapJoystickMap;

bool VistaGlutJoystickDriver::_sJoySample::operator==(const _sJoySample &oOther) const
{
	return ( (oOther.m_nBtMask == m_nBtMask)
			&& (oOther.m_nAxis1 == m_nAxis1)
			&& (oOther.m_nAxis2 == m_nAxis2)
			&& (oOther.m_nAxis3 == m_nAxis3) );
}

VistaGlutJoystickDriver::_sJoySample &
		VistaGlutJoystickDriver::_sJoySample::operator=(const _sJoySample &oOther)
{
	m_nBtMask = oOther.m_nBtMask;
	m_nAxis1  = oOther.m_nAxis1;
	m_nAxis2  = oOther.m_nAxis2;
	m_nAxis3  = oOther.m_nAxis3;
	return *this;
}

void VistaGlutJoystickDriver::JoystickFunction(unsigned int ui1, int n1, int n2, int n3)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutJoystickDriver *pJoystick = S_mapJoystickMap.RetrieveJoystickFromWindowId(nWindow);
	if(!pJoystick)
		return;

	static VistaGlutJoystickDriver::_sJoySample SOld;

	VistaGlutJoystickDriver::_sJoySample s(ui1, n1, n2, n3);
	if( !( s == SOld ))
	{
		pJoystick->m_vecJoyVec.push_back( s );
		SOld = s;
	}
}

/*============================================================================*/
/*============================================================================*/

class VistaGlutJoystickTouchSequence :
	public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{

public:
	VistaGlutJoystickTouchSequence(VistaGlutJoystickDriver *pDriver)
		: VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence(),
		  m_pJoystickDriver(pDriver),
		  m_nPollInterval(10) // default to 10ms
	{
	}

	bool AttachSequence(VistaDriverAbstractWindowAspect::WindowHandle *pWindow)
	{
		int wid = pWindow->GetID();

		// check whether this driver is already registered with the window
		WINMAP::const_iterator cit = m_mapWindows.find(pWindow);
		if(cit == m_mapWindows.end())
		{
			// ok, register with the window is in the statics
			// section
			S_mapJoystickMap.RegisterJoystickWithWindow(wid, m_pJoystickDriver);

			// register this window with the instance variable
			m_mapWindows[pWindow] = wid;

			// set the "current" window in glut
			int nCurWindow = glutGetWindow();
			glutSetWindow( wid );

			// we register the static callbacks functions every time a Joystick
			// is attached to a window (so this may happen multiple times, even
			// when there is only one instance of Joystick. The important thing
			// is that we always set the same static function, so nothing really
			// terrible should happen.
			glutJoystickFunc(&VistaGlutJoystickDriver::JoystickFunction, m_nPollInterval);

			glutSetWindow(nCurWindow); // reset cur window id
			return true;
		}
		return false;
	}

	bool DetachSequence(VistaDriverAbstractWindowAspect::WindowHandle *pWindow)
	{
		WINMAP::iterator cit = m_mapWindows.find(pWindow);
		if(cit != m_mapWindows.end())
		{
			// erase from map
			m_mapWindows.erase(cit);
			int wid = pWindow->GetID();
			S_mapJoystickMap.UnregisterJoystickFromWindow(wid, m_pJoystickDriver);
			return true;
		}
		return false;
	}

	typedef std::map<VistaDriverAbstractWindowAspect::WindowHandle *, int> WINMAP;


	class _copyIn : public std::unary_function< const WINMAP::value_type &, void>
	{
	public:
		_copyIn( std::list<VistaDriverAbstractWindowAspect::WindowHandle*> &list )
		: m_list(list) {}

		std::list<VistaDriverAbstractWindowAspect::WindowHandle*> &m_list;

		void operator()( const WINMAP::value_type &p )
		{
			m_list.push_back( p.first );
		}
	};

	virtual std::list<VistaDriverAbstractWindowAspect::WindowHandle*> GetWindowList() const
	{
		std::list<VistaDriverAbstractWindowAspect::WindowHandle*> list;

		std::for_each( m_mapWindows.begin(), m_mapWindows.end(), _copyIn(list) );
		return list;
	}

	VistaGlutJoystickDriver *m_pJoystickDriver;


	WINMAP  m_mapWindows;
	int     m_nPollInterval;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutJoystickDriver::VistaGlutJoystickDriver(IVistaDriverCreationMethod *fac)
: IVistaDeviceDriver(fac),
  m_pWindowAspect(new VistaDriverAbstractWindowAspect)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);

	// is deleted by aspect, later on
	m_pWindowAspect->SetTouchSequence(new VistaGlutJoystickTouchSequence(this));
	RegisterAspect( m_pWindowAspect );


	VistaDeviceSensor *pSen = new VistaDeviceSensor;
	pSen->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );
	pSen->SetTypeHint( GetFactory()->GetTypeFor( "" ) );
	AddDeviceSensor(pSen);
}

VistaGlutJoystickDriver::~VistaGlutJoystickDriver()
{
	VistaDeviceSensor *pSen = GetSensorByIndex(0);
	m_pHistoryAspect->UnregisterSensor(pSen);
	RemDeviceSensor(pSen);
	GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder( pSen->GetMeasureTranscode() );
	pSen->SetMeasureTranscode(NULL);

	delete pSen;

	UnregisterAspect(m_pWindowAspect, false);
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutJoystickDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);

	for(std::vector< _sJoySample >::const_iterator cit = m_vecJoyVec.begin();
		cit != m_vecJoyVec.end(); ++cit)
	{

		VistaSensorMeasure *pM = MeasureStart(0, dTs, true);
		if(pM == NULL)
		{
			MeasureStop(0);
			continue; // do not leave the loop here, maybe the update on the other axes works
		}
		// MEASURE HERE PLZ

		_sJoyMeasure *m = (*pM).getWrite<_sJoyMeasure>();

		m->m_nVals[0] = (*cit).m_nBtMask;
		m->m_nVals[1] = (*cit).m_nAxis1;
		m->m_nVals[2] = (*cit).m_nAxis2;
		m->m_nVals[3] = (*cit).m_nAxis3;

		MeasureStop(0);

	}

	if( !m_vecJoyVec.empty() )
	{
		pSensor->SetUpdateTimeStamp(dTs); // indicate change
		m_vecJoyVec.clear();
	}
	return (m_vecJoyVec.empty() ? false : true);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

