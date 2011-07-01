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

#include "VistaGlutKeyboardDriver.h"
#include <map>
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

VistaGlutKeyboardDriverCreationMethod::VistaGlutKeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory *fac)
: IVistaDriverCreationMethod(fac)
{
	RegisterSensorType( "",
		sizeof( IVistaKeyboardDriver::_sKeyboardMeasure ),
		20,
		fac->CreateFactoryForType("") );
}

IVistaDeviceDriver *VistaGlutKeyboardDriverCreationMethod::CreateDriver()
{
	return new VistaGlutKeyboardDriver(this);
}


namespace
{
	class UVistaKeyboardMap
	{
	public:
		typedef std::map<int, VistaGlutKeyboardDriver *> DEVMAP;
		DEVMAP m_mapDevices;

		VistaGlutKeyboardDriver *RetrieveKeyboardFromWindowId(int nWindow)
		{
			DEVMAP::const_iterator cit = m_mapDevices.find(nWindow);
			if(cit == m_mapDevices.end())
				return NULL; // ?

			return (*cit).second;
		}

		bool RegisterKeyboardWithWindow(int nWindowId, VistaGlutKeyboardDriver *pAddDriver)
		{
			VistaGlutKeyboardDriver *pDriver = RetrieveKeyboardFromWindowId(nWindowId);
			if(pDriver)
				return (pDriver == pAddDriver);


			m_mapDevices[nWindowId] = pAddDriver;

			return true;
		}

		bool UnregisterKeyboardFromWindow(int nWindowId, VistaGlutKeyboardDriver *pAddDriver)
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

	UVistaKeyboardMap S_mapKeyboardMap;
}

// do translation of key presses here
void VistaGlutKeyboardDriver::SetKeyValue(VistaGlutKeyboardDriver *pKeyboard,
												 unsigned char nKeyValue,
												 bool bIsUp,
												 int nModifier)
{
	int nCurrentKey = int(nKeyValue);
	pKeyboard->m_vecKeyVec.push_back( _sKeyHlp(nCurrentKey, nModifier, bIsUp ) );
}

void VistaGlutKeyboardDriver::SetSpecialKeyValue(VistaGlutKeyboardDriver *pKeyboard,
														int nKeyValue,
														bool bIsUp,
														int nModifier)
{
	int nCurrentKey = int(nKeyValue);
	pKeyboard->m_vecKeyVec.push_back( _sKeyHlp(nCurrentKey, nModifier, bIsUp ));
}


void VistaGlutKeyboardDriver::KeyDownFunction(unsigned char ucKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetKeyValue(pKeyboard, ucKey, false, glutGetModifiers());
}

void VistaGlutKeyboardDriver::KeyUpFunction(unsigned char ucKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetKeyValue(pKeyboard, ucKey, true, glutGetModifiers());
}

void VistaGlutKeyboardDriver::SpecialKeyDownFunction(int nCurrentKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetSpecialKeyValue(pKeyboard, nCurrentKey, false, glutGetModifiers());
}

void VistaGlutKeyboardDriver::SpecialKeyUpFunction(int nCurrentKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetSpecialKeyValue(pKeyboard, nCurrentKey, true, glutGetModifiers());
}


/*============================================================================*/
/*============================================================================*/

class VistaGlutKeybTouchSequence :
	public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{

public:
	VistaGlutKeybTouchSequence(VistaGlutKeyboardDriver *pDriver)
		: VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence(),
		  m_pKeyboardDriver(pDriver)
	{
	}

	bool AttachSequence(VistaDriverAbstractWindowAspect::WindowHandle *pWindow)
	{
		// check whether this driver is already registered with the window
		WINMAP::const_iterator cit = m_mapWindows.find(pWindow);
		if(cit == m_mapWindows.end())
		{
			// ugly workaround
			int windowId = pWindow->GetID();

			// ok, register with the window is in the statics
			// section
			if(S_mapKeyboardMap.RegisterKeyboardWithWindow(windowId, m_pKeyboardDriver))
			{
				// register this window with the instance variable
				m_mapWindows[pWindow] = windowId;

				// set the "current" window in glut
				int nCurWindow = glutGetWindow();
				glutSetWindow( windowId );

				// we register the static callbacks functions every time a keyboard
				// is attached to a window (so this may happen multiple times, even
				// when there is only one instance of keyboard. The important thing
				// is that we always set the same static function, so nothing really
				// terrible should happen.
				glutKeyboardFunc(&VistaGlutKeyboardDriver::KeyDownFunction);
				glutSpecialFunc(&VistaGlutKeyboardDriver::SpecialKeyDownFunction);
				glutKeyboardUpFunc(&VistaGlutKeyboardDriver::KeyUpFunction);
				glutSpecialUpFunc(&VistaGlutKeyboardDriver::SpecialKeyUpFunction);

				glutSetWindow(nCurWindow); // reset cur window id
				return true;
			}
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
			return S_mapKeyboardMap.UnregisterKeyboardFromWindow( pWindow->GetID(), m_pKeyboardDriver );
		}
		return false;
	}

	typedef std::map<VistaDriverAbstractWindowAspect::WindowHandle*, int> WINMAP;

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

	VistaGlutKeyboardDriver *m_pKeyboardDriver;

	WINMAP  m_mapWindows;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutKeyboardDriver::VistaGlutKeyboardDriver(IVistaDriverCreationMethod *crm)
: IVistaKeyboardDriver(crm),
 m_bLastFrameValue(false),
 m_pWindowAspect(new VistaDriverAbstractWindowAspect)
{
	RegisterAspect(m_pWindowAspect);
	// is deleted by aspect, later on
	m_pWindowAspect->SetTouchSequence(new VistaGlutKeybTouchSequence(this));
}

VistaGlutKeyboardDriver::~VistaGlutKeyboardDriver()
{
	UnregisterAspect(m_pWindowAspect, false);
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutKeyboardDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	if(m_vecKeyVec.empty()) // no key pressed
	{
		if(m_bLastFrameValue) // last frame had a value, so set back to 0
		{
			// the behavior we want is to get the outvalue on KEY to be 0
			// after an up-key, that is we want a single "-value" in the history
			// followed by a "0"
			m_bLastFrameValue = false;
			VistaDeviceSensor *pSensor = GetSensorByIndex(0);
			MeasureStart(dTs);
			UpdateKey( 0, 0 );
			MeasureStop();
			pSensor->SetUpdateTimeStamp(dTs);
			return true;
		}
		return false;
	}

	VistaDeviceSensor *pSensor = GetSensorByIndex(0);

	for(std::vector< _sKeyHlp >::const_iterator cit = m_vecKeyVec.begin();
		cit != m_vecKeyVec.end(); ++cit)
	{
		MeasureStart(dTs);
		UpdateKey(( (*cit).m_bUpKey == true ? -(*cit).m_nKey : (*cit).m_nKey ), (*cit).m_nModifier);
		MeasureStop();

		// toggle to true only last-neg-frame value
		// forces a "-value" is followed by "0" iff no other
		// key is pressed afterwards.
		m_bLastFrameValue |= ((*cit).m_bUpKey == true);

	}
	pSensor->SetUpdateTimeStamp(dTs);

	m_vecKeyVec.clear();


	return true;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
