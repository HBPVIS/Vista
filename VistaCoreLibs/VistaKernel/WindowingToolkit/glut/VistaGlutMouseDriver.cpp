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

#include "VistaGlutMouseDriver.h"
#include <map>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/InteractionManager/VistaDriverWindowAspect.h>

#include <VistaKernel/DisplayManager/VistaWindow.h>

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

static UVistaMouseMap S_mapMouseMap;

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

class VistaOpenSGglutMouseDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()()
	{
		return new VistaGlutMouseDriver;
	}
protected:
private:
};

namespace
{
	VistaOpenSGglutMouseDriverCreationMethod *SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaGlutMouseDriver::GetDriverCreationMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaOpenSGglutMouseDriverCreationMethod;
		SpFactory->RegisterSensorType( "",
				                       sizeof( IVistaMouseDriver::_sMouseMeasure ),
				                       100,
				                       new VistaMouseDriverTranscodeFactory,
				                       IVistaMouseDriver::GetMouseTranscodeName() );
	}
	return SpFactory;
}

// #############################################################################
class VistaOpenSGglutMouseTouchSequence :
	public VistaDriverWindowAspect::IVistaDriverWindowTouchSequence
{

public:
	VistaOpenSGglutMouseTouchSequence(VistaGlutMouseDriver *pDriver)
		: VistaDriverWindowAspect::IVistaDriverWindowTouchSequence(),
		  m_pMouseDriver(pDriver)
	{

	}

	bool AttachSequence(CWindowHandle *)
	{
		return false;
	}

	bool DetachSequence(CWindowHandle *)
	{
		return false;
	}

	virtual std::list<CWindowHandle*> GetWindowList() const
	{
		return std::list<CWindowHandle*>();
	}

	virtual bool AttachSequence(VistaWindow *pWindow)
	{

		int windowId;

		// check whether this driver is already registered with the window
		WINMAP::const_iterator cit = m_mapWindows.find(pWindow);
		if(cit == m_mapWindows.end())
		{
			// ugly workaround
			windowId = static_cast<int>(reinterpret_cast<long long>(pWindow->GetWindowId()));

			// ok, register with the window is in the statics
			// section
			S_mapMouseMap.RegisterMouseWithWindow(windowId, m_pMouseDriver);

			int nCurWindow = glutGetWindow();

			// create a sensor for this window
			VistaDeviceSensor *pSensor = new VistaDeviceSensor;
			unsigned int nIdx = m_pMouseDriver->AddDeviceSensor(pSensor);
			// create new entry in the sensor map to attach the
			// sensor id to the glut window id
			m_mapSensor[windowId] = _sSensorState(nIdx);

			// register this window with the instance variable
			m_mapWindows[pWindow] = windowId;
			glutSetWindow(windowId);

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
		return false;
	}

	virtual bool DetachSequence(VistaWindow *pWindow)
	{

		WINMAP::iterator cit = m_mapWindows.find(pWindow);
		if(cit != m_mapWindows.end())
		{
			SENMAP::iterator it = m_mapSensor.find( (*cit).second );
			if(it != m_mapSensor.end())
			{
				unsigned int nSensorIdx = (*it).second.m_nIndex;
				// remove sensor from driver
				VistaDeviceSensor *pSensor = m_pMouseDriver->GetSensorByIndex(nSensorIdx);
				m_pMouseDriver->RemDeviceSensor(pSensor);

				m_mapSensor.erase(it);
			}

			// erase from window map

			m_mapWindows.erase(cit);
			// ugly workaround
			S_mapMouseMap.UnregisterMouseFromWindow(static_cast<int>(reinterpret_cast<long long>(pWindow->GetWindowId())), m_pMouseDriver);
			return true;
		}
		return false;
	}

	bool GetWindowList(std::list<VistaWindow*> &liRet) const
	{
		for(WINMAP::const_iterator cit = m_mapWindows.begin();
			cit != m_mapWindows.end(); ++cit)
			liRet.push_back( (*cit).first );

		return true;
	}

	VistaGlutMouseDriver *m_pMouseDriver;

	typedef std::map<VistaWindow *, int> WINMAP;

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
VistaGlutMouseDriver::VistaGlutMouseDriver()
: IVistaMouseDriver(),
  m_nWheelState(0),
  m_nWheelDirState(0),
  m_pWindowAspect(new VistaDriverWindowAspect)
{
	RegisterAspect( m_pWindowAspect );
	// is deleted by aspect, later on
	m_nButtonStates[0] = m_nButtonStates[1] = m_nButtonStates[2] = 0;
	m_pWindowAspect->SetTouchSequence(new VistaOpenSGglutMouseTouchSequence(this));
}

VistaGlutMouseDriver::~VistaGlutMouseDriver()
{
	UnregisterAspect(m_pWindowAspect, false);
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutMouseDriver::DoSensorUpdate(microtime dTs)
{
	if(m_vecUpdates.empty())
		return false;

	VistaOpenSGglutMouseTouchSequence *pTs
		= static_cast<VistaOpenSGglutMouseTouchSequence*>(m_pWindowAspect->GetTouchSequence());
	// hopefully, the measure history is large enough
	for(std::vector<_state>::const_iterator cit = m_vecUpdates.begin();
		cit != m_vecUpdates.end(); ++cit)
	{
		VistaOpenSGglutMouseTouchSequence::SENMAP::iterator it = pTs->m_mapSensor.find( (*cit).m_nWinId );

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


