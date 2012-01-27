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

#include "VistaGlutKeyboardDriver.h"

#include <VistaKernel/VistaKernelOut.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/GlutWindowImp/VistaGlutWindowingToolkit.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/InteractionManager/VistaDriverWindowAspect.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

#if defined(DARWIN) // we use the mac os GLUT framework on darwin
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <map>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
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

static UVistaKeyboardMap S_mapKeyboardMap;


// do translation of key presses here
void VistaGlutKeyboardDriver::SetKeyValue(VistaGlutKeyboardDriver *pKeyboard,
												 unsigned char nKeyValue,
												 bool bIsUp,
												 int nModifier)
{
	int nCurrentKey = 0;
	switch(nKeyValue)
	{
		case 27: //ASCII Escape
			nCurrentKey = VISTA_KEY_ESC;
			break;
		case 13: //ASCII Return
			nCurrentKey = VISTA_KEY_ENTER;
			break;
		case 9: //ASCII Tab ???
			nCurrentKey = VISTA_KEY_TAB;
			break;
		case 8: //ASCII Backspace
			nCurrentKey = VISTA_KEY_BACKSPACE;
			break;
		default:
			nCurrentKey = int(nKeyValue);
			break;
	}
	//pKeyboard->m_nCurrentGlutKey = nCurrentKey;
	//pKeyboard->m_bNeedsUpdate = true;

	pKeyboard->m_vecKeyVec.push_back( _sKeyHlp(nCurrentKey, nModifier, bIsUp ) );
}

void VistaGlutKeyboardDriver::SetSpecialKeyValue(VistaGlutKeyboardDriver *pKeyboard,
														int nKeyValue,
														bool bIsUp,
														int nModifier)
{
	int nCurrentKey = 0;
	switch(nKeyValue)
	{
	case GLUT_KEY_F1:
		nCurrentKey = VISTA_KEY_F1;
		break;
	case GLUT_KEY_F2:
		nCurrentKey = VISTA_KEY_F2;
		break;
	case GLUT_KEY_F3:
		nCurrentKey = VISTA_KEY_F3;
		break;
	case GLUT_KEY_F4:
		nCurrentKey = VISTA_KEY_F4;
		break;
	case GLUT_KEY_F5:
		nCurrentKey = VISTA_KEY_F5;
		break;
	case GLUT_KEY_F6:
		nCurrentKey = VISTA_KEY_F6;
		break;
	case GLUT_KEY_F7:
		nCurrentKey = VISTA_KEY_F7;
		break;
	case GLUT_KEY_F8:
		nCurrentKey = VISTA_KEY_F8;
		break;
	case GLUT_KEY_F9:
		nCurrentKey = VISTA_KEY_F9;
		break;
	case GLUT_KEY_F10:
		nCurrentKey = VISTA_KEY_F10;
		break;
	case GLUT_KEY_F11:
		nCurrentKey = VISTA_KEY_F11;
		break;
	case GLUT_KEY_F12:
		nCurrentKey = VISTA_KEY_F12;
		break;
	case GLUT_KEY_LEFT:
		nCurrentKey = VISTA_KEY_LEFTARROW;
		break;
	case GLUT_KEY_RIGHT:
		nCurrentKey = VISTA_KEY_RIGHTARROW;
		break;
	case GLUT_KEY_UP:
		nCurrentKey = VISTA_KEY_UPARROW;
		break;
	case GLUT_KEY_DOWN:
		nCurrentKey = VISTA_KEY_DOWNARROW;
		break;
	case GLUT_KEY_PAGE_UP:
		nCurrentKey = VISTA_KEY_PAGEUP;
		break;
	case GLUT_KEY_PAGE_DOWN:
		nCurrentKey = VISTA_KEY_PAGEDOWN;
		break;
	case GLUT_KEY_HOME:
		nCurrentKey = VISTA_KEY_HOME;
		break;
	case GLUT_KEY_END:
		nCurrentKey = VISTA_KEY_END;
		break;

	default:
		nCurrentKey = nKeyValue;
		break;
	}
	//pKeyboard->m_nCurrentSpecialKey = nCurrentKey;
	//pKeyboard->m_bIsUpSpecial = true;

	pKeyboard->m_vecKeyVec.push_back( _sKeyHlp(nCurrentKey, nModifier, bIsUp ));
}


void VistaGlutKeyboardDriver::KeyDownFunction(unsigned char ucKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	//pKeyboard->m_bIsUpKey = false;
	SetKeyValue(pKeyboard, ucKey, false, glutGetModifiers());
}

void VistaGlutKeyboardDriver::KeyUpFunction(unsigned char ucKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	//pKeyboard->m_bIsUpKey = true;
	SetKeyValue(pKeyboard, ucKey, true, glutGetModifiers());
}

void VistaGlutKeyboardDriver::SpecialKeyDownFunction(int nCurrentKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	//pKeyboard->m_bIsUpKey = false;
	SetSpecialKeyValue(pKeyboard, nCurrentKey, false, glutGetModifiers());
}

void VistaGlutKeyboardDriver::SpecialKeyUpFunction(int nCurrentKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return; // ?

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	//pKeyboard->m_bIsUpKey = true;
	SetSpecialKeyValue(pKeyboard, nCurrentKey, true, glutGetModifiers());
}

class VistaOpenSGglutKeyboardDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()()
	{
		return new VistaGlutKeyboardDriver;
	}
protected:
private:
};


namespace
{
	VistaOpenSGglutKeyboardDriverCreationMethod *SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaGlutKeyboardDriver::GetDriverCreationMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaOpenSGglutKeyboardDriverCreationMethod;
		SpFactory->RegisterSensorType( "",
				                       sizeof( IVistaKeyboardDriver::_sKeyboardMeasure ),
				                       20,
				                       new VistaKeyboardDriverTranscodeFactory,
				                       IVistaKeyboardDriver::GetKeyboardTranscodeName() );
	}
	return SpFactory;
}

/*============================================================================*/
/*============================================================================*/

class VistaOpenSGglutKeybTouchSequence :
	public VistaDriverWindowAspect::IVistaDriverWindowTouchSequence
{

public:
	VistaOpenSGglutKeybTouchSequence(VistaGlutKeyboardDriver *pDriver)
		: VistaDriverWindowAspect::IVistaDriverWindowTouchSequence(),
		  m_pKeyboardDriver(pDriver)
	{
	}

	bool AttachSequence(WindowHandle *)
	{
		return false;
	}

	bool DetachSequence(WindowHandle *)
	{
		return false;
	}

	virtual std::list<WindowHandle*> GetWindowList() const
	{
		return std::list<WindowHandle*>();
	}

	virtual bool AttachSequence(VistaWindow *pWindow)
	{
		VistaGlutWindowingToolkit* pGlutWTA = dynamic_cast<VistaGlutWindowingToolkit*>(
						pWindow->GetProperties()->GetDisplayBridge()->GetWindowingToolkit() );
		if( pGlutWTA == NULL )
		{
			vstr::errf() << "[GlutKeyboardDriver]: Trying to use "
					<< "Glut mousse driver, but WindowingToolkit is not GLUT!" << std::endl;
			return false;
		}
		// check whether this driver is already registered with the window
		WINMAP::const_iterator cit = m_mapWindows.find(pWindow);
		if(cit != m_mapWindows.end())
		{
			vstr::errf() << "[GlutKeyboardDriver]: Trying to attach "
					<< "to Window [" << pWindow->GetNameForNameable()
					<< "], which is already registered for a mouse!" << std::endl;
			return false;
		}			

		int iWindowID = pGlutWTA->GetWindowId( pWindow );

		S_mapKeyboardMap.RegisterKeyboardWithWindow(iWindowID, m_pKeyboardDriver);

		// register this window with the instance variable
		m_mapWindows[pWindow] = iWindowID;

		// set the "current" window in glut
		int nCurWindow = glutGetWindow();
		glutSetWindow( iWindowID );

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

	virtual bool DetachSequence(VistaWindow *pWindow)
	{

		WINMAP::iterator cit = m_mapWindows.find(pWindow);
		if(cit != m_mapWindows.end())
		{
			// erase from map
			m_mapWindows.erase(cit);
			VistaGlutWindowingToolkit* pGlutWTA = dynamic_cast<VistaGlutWindowingToolkit*>(
						pWindow->GetProperties()->GetDisplayBridge()->GetWindowingToolkit() );
			if( pGlutWTA == NULL )
			{
				vstr::errf() << "[GlutKeyboardDriver]: Trying to use "
						<< "Glut mousse driver, but WindowingToolkit is not GLUT!" << std::endl;
				return false;
			}

			int iWindowID = pGlutWTA->GetWindowId( pWindow );
			S_mapKeyboardMap.UnregisterKeyboardFromWindow( iWindowID, m_pKeyboardDriver );
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

	VistaGlutKeyboardDriver *m_pKeyboardDriver;

	typedef std::map<VistaWindow *, int> WINMAP;
	WINMAP  m_mapWindows;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutKeyboardDriver::VistaGlutKeyboardDriver()
: IVistaKeyboardDriver(),
 m_bLastFrameValue(false),
 m_pWindowAspect(new VistaDriverWindowAspect)
{
	RegisterAspect(m_pWindowAspect);
	// is deleted by aspect, later on
	m_pWindowAspect->SetTouchSequence(new VistaOpenSGglutKeybTouchSequence(this));
}

VistaGlutKeyboardDriver::~VistaGlutKeyboardDriver()
{
	UnregisterAspect(m_pWindowAspect, false);
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutKeyboardDriver::DoSensorUpdate(microtime dTs)
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


