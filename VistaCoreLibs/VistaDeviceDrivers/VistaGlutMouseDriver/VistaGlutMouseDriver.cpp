/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#include "VistaGlutMouseDriver.h"
#include <map>
#include <functional>
#include <algorithm>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>

#if defined(USE_NATIVE_GLUT)
  #if defined(DARWIN) // we use the mac os GLUT framework on darwin
    #include <GLUT/glut.h>
  #else
    #include <GL/glut.h>
  #endif
#else
    #include <GL/freeglut.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace
{
	class UVistaMouseMap
	{
	public:
		typedef std::map<int, VistaGlutMouseDriver *> DEVMAP;
		DEVMAP m_mapDevices;

		VistaGlutMouseDriver *RetrieveMouseFromWindowId(int nWindow)
		{
			DEVMAP::const_iterator cit = m_mapDevices.find(nWindow);
			if(cit == m_mapDevices.end())
				return NULL; // ?

			return (*cit).second;
		}

		bool RegisterMouseWithWindow(int nWindowId, VistaGlutMouseDriver *pAddDriver)
		{
			VistaGlutMouseDriver *pDriver = RetrieveMouseFromWindowId(nWindowId);
			if(pDriver)
				return (pDriver == pAddDriver);


			m_mapDevices[nWindowId] = pAddDriver;

			return true;
		}

		bool UnregisterMouseFromWindow(int nWindowId, VistaGlutMouseDriver *pAddDriver)
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

	UVistaMouseMap S_mapMouseMap;
}

void VistaGlutMouseDriver::MouseFunction( int iButton, int iState, int iX, int iY)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutMouseDriver *pMouse = S_mapMouseMap.RetrieveMouseFromWindowId(nWindow);
	if(!pMouse)
		return;

	// save current state of buttons for eventually
	// incoming MotionFunctions, otherwise, the button press
	// or release might be lost.
	pMouse->m_nButtonStates[iButton] = 1-iState;

	_state s(nWindow, iX, iY,pMouse->m_nButtonStates[0],
					 pMouse->m_nButtonStates[1],
					 pMouse->m_nButtonStates[2],
					 -1, pMouse->m_nWheelDirState);

	pMouse->m_vecUpdates.push_back( s );
}


void VistaGlutMouseDriver::MotionFunction( int iX, int iY )
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutMouseDriver *pMouse = S_mapMouseMap.RetrieveMouseFromWindowId(nWindow);
	if(!pMouse)
		return;

	_state s(nWindow, iX, iY, pMouse->m_nButtonStates[0],
					 pMouse->m_nButtonStates[1],
					 pMouse->m_nButtonStates[2],
					 -1, pMouse->m_nWheelState);

	pMouse->m_vecUpdates.push_back( s );

}

void VistaGlutMouseDriver::MouseWheelFunction( int nWheelNumber, int nDirection, int nX, int nY)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutMouseDriver *pMouse = S_mapMouseMap.RetrieveMouseFromWindowId(nWindow);
	if(!pMouse)
		return;

	_state s(nWindow, nX, nY, pMouse->m_nButtonStates[0],
					 pMouse->m_nButtonStates[1],
					 pMouse->m_nButtonStates[2],
					 nWheelNumber, nDirection);
	pMouse->m_vecUpdates.push_back(s);
}


	// #############################################################################
	VistaGlutMouseDriverCreationMethod::VistaGlutMouseDriverCreationMethod(IVistaTranscoderFactoryFactory *fac)
	: IVistaDriverCreationMethod( fac )
	{
		RegisterSensorType( "",
			sizeof( IVistaMouseDriver::_sMouseMeasure ),
			100,
			fac->CreateFactoryForType("") );
	}

	IVistaDeviceDriver *VistaGlutMouseDriverCreationMethod::CreateDriver()
	{
		return new VistaGlutMouseDriver(this);
	}

	// #############################################################################
class VistaGlutMouseTouchSequence :
	public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{

public:
	VistaGlutMouseTouchSequence(VistaGlutMouseDriver *pDriver)
		: VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence(),
		  m_pMouseDriver(pDriver)
	{

	}

	bool AttachSequence( const VistaDriverAbstractWindowAspect::WindowHandle& oWindow )
	{
		// check whether this driver is already registered with the window
		WINMAP::const_iterator cit = m_mapWindows.find(oWindow);
		if(cit != m_mapWindows.end())
		{
			vstr::warnp() << "[GlutMouseDriver]: Trying to attach "
					<< "to Window which is already registered for a mouse!" << std::endl;
			return false;
		}

		int nWindowId = oWindow.GetID();

		// ok, register with the window is in the statics
		// section
		S_mapMouseMap.RegisterMouseWithWindow( nWindowId, m_pMouseDriver);

		int nCurWindow = glutGetWindow();

		// create a sensor for this window
		VistaDeviceSensor *pSensor = new VistaDeviceSensor;
		unsigned int nIdx = m_pMouseDriver->AddDeviceSensor(pSensor);
		// create new entry in the sensor map to attach the
		// sensor id to the glut window id
		m_mapSensor[nWindowId] = _sSensorState(nIdx);

		// register this window with the instance variable
		m_mapWindows[oWindow] = nWindowId;
		glutSetWindow(nWindowId);

		// it should be ok to register these functions more
		// than once. The interaction stuff from the implementation
		// layer _has_ to be initialized *after* the display stuff anyways
		// and we always set the same function points. Sad that there is
		// no way to actually get the current value of the callbacks in glut.
		// note that, however, these functions are not unregistered, as there
		// may be more than one mouse.
		glutMouseFunc(&VistaGlutMouseDriver::MouseFunction);
		glutMotionFunc(&VistaGlutMouseDriver::MotionFunction);
		glutPassiveMotionFunc(&VistaGlutMouseDriver::MotionFunction);
#if !defined(USE_NATIVE_GLUT)
		glutMouseWheelFunc( &VistaGlutMouseDriver::MouseWheelFunction);
#endif
		glutSetWindow(nCurWindow); // reset old window
		return true;		
	}

	bool DetachSequence( const VistaDriverAbstractWindowAspect::WindowHandle& oWindow )
	{
		WINMAP::iterator cit = m_mapWindows.find( oWindow );
		if( cit != m_mapWindows.end() )
		{
			SENMAP::iterator it = m_mapSensor.find( (*cit).second );
			if( it != m_mapSensor.end() )
			{
				unsigned int nSensorIdx = (*it).second.m_nIndex;
				// remove sensor from driver
				VistaDeviceSensor *pSensor = m_pMouseDriver->GetSensorByIndex(nSensorIdx);
				m_pMouseDriver->RemDeviceSensor(pSensor);
				m_pMouseDriver->GetFactory()->
						        GetTranscoderFactoryForSensor("")->
						        DestroyTranscoder( pSensor->GetMeasureTranscode() );
				delete pSensor;

				m_mapSensor.erase(it);
			}

			// erase from window map

			m_mapWindows.erase(cit);
			
			S_mapMouseMap.UnregisterMouseFromWindow( oWindow.GetID() , m_pMouseDriver );
			return true;
		}
		return false;
	}


	typedef std::map<VistaDriverAbstractWindowAspect::WindowHandle, int> WINMAP;


	class _copyIn : public std::unary_function< const WINMAP::value_type &, void>
	{
	public:
		_copyIn( std::list<VistaDriverAbstractWindowAspect::WindowHandle> &list )
		: m_list(list) {}

		std::list<VistaDriverAbstractWindowAspect::WindowHandle> &m_list;

		void operator()( const WINMAP::value_type &p )
		{
			m_list.push_back( p.first );
		}
	};


	virtual std::list<VistaDriverAbstractWindowAspect::WindowHandle> GetWindowList() const
	{
		std::list<VistaDriverAbstractWindowAspect::WindowHandle> list;

		std::for_each( m_mapWindows.begin(), m_mapWindows.end(), _copyIn(list) );
		return list;
	}



	VistaGlutMouseDriver *m_pMouseDriver;



	struct _sSensorState
	{
		_sSensorState()
			: m_nIndex(~0),
			  m_nWheelState(0),
			  m_nWheelDir(0) {}

		_sSensorState(unsigned int nIdx)
			: m_nIndex(nIdx),
			  m_nWheelState(0),
			  m_nWheelDir(0)
		{
		}

		unsigned int m_nIndex;
		int m_nWheelState,
			m_nWheelDir;

	};

	typedef std::map<int, _sSensorState> SENMAP;
	WINMAP  m_mapWindows;
	SENMAP  m_mapSensor;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutMouseDriver::VistaGlutMouseDriver(IVistaDriverCreationMethod *crm)
: IVistaMouseDriver(crm),
  m_nWheelState(0),
  m_nWheelDirState(0),
  m_pWindowAspect(new VistaDriverAbstractWindowAspect)
{
	RegisterAspect( m_pWindowAspect );
	// is deleted by aspect, later on
	m_nButtonStates[0] = m_nButtonStates[1] = m_nButtonStates[2] = 0;
	m_pWindowAspect->SetTouchSequence(new VistaGlutMouseTouchSequence(this));
}

VistaGlutMouseDriver::~VistaGlutMouseDriver()
{
	UnregisterAspect(m_pWindowAspect, false);
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutMouseDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	if(m_vecUpdates.empty())
		return false;

	VistaGlutMouseTouchSequence *pTs
		= static_cast<VistaGlutMouseTouchSequence*>(m_pWindowAspect->GetTouchSequence());
	// hopefully, the measure history is large enough
	for(std::vector<_state>::const_iterator cit = m_vecUpdates.begin();
		cit != m_vecUpdates.end(); ++cit)
	{
		VistaGlutMouseTouchSequence::SENMAP::iterator it = pTs->m_mapSensor.find( (*cit).m_nWinId );

		if(it == pTs->m_mapSensor.end())
		{
			// utter error!
			continue;
		}

		unsigned int nSensorIdx = (*it).second.m_nIndex; // get sensor index
		VistaDeviceSensor *pSensor = GetSensorByIndex(nSensorIdx);

		MeasureStart(nSensorIdx, dTs);
		UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_LEFT,   (*cit).m_nButtonStates[0]);
		UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_MIDDLE, (*cit).m_nButtonStates[1]);
		UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_RIGHT,  (*cit).m_nButtonStates[2]);
		UpdateMousePosition(nSensorIdx, (*cit).m_nX, (*cit).m_nY);
		if( (*cit).m_nWheelNumber >= 0 )
		{
			(*it).second.m_nWheelState += (*cit).m_nWheelDirection;
			(*it).second.m_nWheelDir    = (*cit).m_nWheelDirection;
		}
		UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_WHEEL_STATE,  (*it).second.m_nWheelState);
		UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_WHEEL_DIR,    (*it).second.m_nWheelDir);

		MeasureStop(nSensorIdx);

		// indicate change
		pSensor->SetUpdateTimeStamp(dTs);
	}
	if(!m_vecUpdates.empty())
	{
		m_vecUpdates.clear();
	}

	return true;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


