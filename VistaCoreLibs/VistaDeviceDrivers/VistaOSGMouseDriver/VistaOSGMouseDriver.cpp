/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#include "VistaOSGMouseDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/DisplayManager/OpenSceneGraphWindowImp/VistaOSGWindowingToolkit.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>

#include <osgGA/EventQueue>
#include <osgGA/GUIEventAdapter>
#include <osgViewer/GraphicsWindow>

#include <algorithm>
#include <iterator>
#include <assert.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

// #############################################################################
VistaOSGMouseDriverCreationMethod::VistaOSGMouseDriverCreationMethod(IVistaTranscoderFactoryFactory *fac)
: IVistaDriverCreationMethod( fac )
{
	RegisterSensorType( "",
		sizeof( IVistaMouseDriver::_sMouseMeasure ),
		100,
		fac->CreateFactoryForType("") );
}

IVistaDeviceDriver *VistaOSGMouseDriverCreationMethod::CreateDriver()
{
	return new VistaOSGMouseDriver(this);
}

	// #############################################################################


class VistaOSGMouseTouchSequence :
	public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{
public:
	typedef std::list<VistaDriverAbstractWindowAspect::WindowHandle> WindowList;
public:
	VistaOSGMouseTouchSequence(VistaOSGMouseDriver *pDriver)
		: VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence()
		, m_pMouseDriver( pDriver )
	{
	}

	bool AttachSequence( const VistaDriverAbstractWindowAspect::WindowHandle& oHandle )
	{
		// check whether this driver is already registered with the window
		WindowList::const_iterator itWin = std::find( m_vecWindows.begin(), m_vecWindows.end(), oHandle );
		if( itWin != m_vecWindows.end() )
			return false;

		if( m_pMouseDriver->RegisterWindow( oHandle.GetWindow() ) == false )
			return false;

		m_vecWindows.push_back( oHandle );
		return true;
	}

	bool DetachSequence( const VistaDriverAbstractWindowAspect::WindowHandle& oHandle )
	{
		WindowList::iterator itWin = std::find( m_vecWindows.begin(), m_vecWindows.end(), oHandle );
		if( itWin == m_vecWindows.end() )
			return false;
		m_vecWindows.erase( itWin );
		return true;
	}
	
	virtual std::list<VistaDriverAbstractWindowAspect::WindowHandle> GetWindowList() const
	{
		std::list<VistaDriverAbstractWindowAspect::WindowHandle> liWins;

		std::copy( m_vecWindows.begin(), m_vecWindows.end(), std::back_inserter( liWins ) );
		return liWins;
	}

	WindowList& GetWindows()
	{
		return m_vecWindows;
	}

private:
	WindowList m_vecWindows;
	VistaOSGMouseDriver* m_pMouseDriver;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOSGMouseDriver::VistaOSGMouseDriver(IVistaDriverCreationMethod *crm)
: IVistaMouseDriver(crm)
, m_pWindowAspect(new VistaDriverAbstractWindowAspect)
, m_pWindowingToolkit( NULL )
, m_bGrabCursorChanged( true )
{
	RegisterAspect( m_pWindowAspect );
	m_pWindowAspect->SetTouchSequence(new VistaOSGMouseTouchSequence(this));
}

VistaOSGMouseDriver::~VistaOSGMouseDriver()
{
	UnregisterAspect( m_pWindowAspect, false );
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOSGMouseDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	if( m_pWindowingToolkit == NULL || m_mapWindows.empty() )
		return false;

	for( WindowMap::iterator itWin = m_mapWindows.begin();
			itWin != m_mapWindows.end(); ++itWin )
	{
		osgGA::EventQueue::Events& oEvents = m_pWindowingToolkit->GetEventsForWindow( (*itWin).first );
		osgGA::EventQueue::Events::iterator itEvent;
		for( itEvent = oEvents.begin();
			itEvent != oEvents.end();
			++itEvent )
		{
			VistaDeviceSensor* pSensor = GetSensorByIndex( (*itWin).second.m_nSensorIndex );

			osgGA::GUIEventAdapter* pEvent = itEvent->get();
			if( pEvent->getHandled() )
				continue;
			switch( pEvent->getEventType() )
			{				
				case osgGA::GUIEventAdapter::PUSH:
				{
					switch( pEvent->getButton() )
					{
						case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
						{
							(*itWin).second.m_nButtonStates[0] = true;
							break;
						}
						case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
						{
							(*itWin).second.m_nButtonStates[1] = true;
							break;
						}
						case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
						{
							(*itWin).second.m_nButtonStates[2] = true;
							break;
						}
						default:
							VISTA_THROW( "OSGMpouseDriver: Unknown button press", -1 );
					}
					break;
				}
				case osgGA::GUIEventAdapter::RELEASE:
				{
					switch( pEvent->getButton() )
					{
						case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
						{
							(*itWin).second.m_nButtonStates[BT_LEFT] = false;
							break;
						}
						case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
						{
							(*itWin).second.m_nButtonStates[BT_RIGHT] = false;
							break;
						}
						case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
						{
							(*itWin).second.m_nButtonStates[BT_MIDDLE] = false;
							break;
						}
						default:
							VISTA_THROW( "OSGMpouseDriver: Unknown button press", -1 );
					}					
					break;
				}
				case osgGA::GUIEventAdapter::SCROLL:
				{
					if( pEvent->getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP )
					{
						(*itWin).second.m_nWheelDirection = +1;
						++(*itWin).second.m_nWheelNumber;
					}
					else
					{
						(*itWin).second.m_nWheelDirection = -1;
						--(*itWin).second.m_nWheelNumber;
					}
					break;
				}
				case osgGA::GUIEventAdapter::MOVE:
				case osgGA::GUIEventAdapter::DRAG:
				{
					break;
				}
				default:
					continue;
			}

			int nPosX = pEvent->getX();
			int nPosY = pEvent->getY();
			
			if( GetParameters()->GetCaptureCursor() )
			{
				osgViewer::GraphicsWindow* pWin = m_pWindowingToolkit->GetOsgWindowForWindow( (*itWin).first );
				assert( pWin );

				int nX, nY, nWidth, nHeight;
				pWin->getWindowRectangle( nX, nY, nWidth, nHeight );
				int nCenterX = nWidth / 2;
				int nCenterY = nHeight / 2;				
				
				int nDeltaX = nPosX - nCenterX;
				int nDeltaY = nPosY - nCenterY;		
				
				if( nDeltaX != 0 || nDeltaY != 0 )
				{
					pWin->requestWarpPointer( nCenterX, nCenterY );
				}

				if( m_bGrabCursorChanged )
				{
					// @todo: react to dynamic changes
					m_bGrabCursorChanged = false;
					nPosX = 0;
					nPosY = 0;
				}
			}

			MeasureStart( (*itWin).second.m_nSensorIndex, dTs);
			UpdateMousePosition( (*itWin).second.m_nSensorIndex, nPosX, nPosY );
			UpdateMouseButton( (*itWin).second.m_nSensorIndex,
					BT_WHEEL_DIR, (*itWin).second.m_nWheelDirection );
			UpdateMouseButton( (*itWin).second.m_nSensorIndex,
					BT_WHEEL_STATE, (*itWin).second.m_nWheelNumber );
			UpdateMouseButton( (*itWin).second.m_nSensorIndex,
					BT_LEFT, (double)(*itWin).second.m_nButtonStates[BT_LEFT] );
			UpdateMouseButton( (*itWin).second.m_nSensorIndex,
					BT_RIGHT, (double)(*itWin).second.m_nButtonStates[BT_RIGHT] );
			UpdateMouseButton( (*itWin).second.m_nSensorIndex,
					BT_MIDDLE, (double)(*itWin).second.m_nButtonStates[BT_MIDDLE] );
			MeasureStop( (*itWin).second.m_nSensorIndex );
			pEvent->setHandled( true );
			pSensor->SetUpdateTimeStamp( dTs );
		}
	}

	return false;




	//VistaOSGMouseTouchSequence *pTs
	//	= static_cast<VistaOSGMouseTouchSequence*>(m_pWindowAspect->GetTouchSequence());
	//// hopefully, the measure history is large enough
	//for(std::vector<_state>::const_iterator cit = m_vecUpdates.begin();
	//	cit != m_vecUpdates.end(); ++cit)
	//{
	//	VistaOSGMouseTouchSequence::SENMAP::iterator it = pTs->m_mapSensor.find( (*cit).m_nWinId );

	//	if(it == pTs->m_mapSensor.end())
	//	{
	//		// utter error!
	//		continue;
	//	}

	//	unsigned int nSensorIdx = (*it).second.m_nIndex; // get sensor index
	//	VistaDeviceSensor *pSensor = GetSensorByIndex(nSensorIdx);

	//	MeasureStart(nSensorIdx, dTs);
	//	UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_LEFT,   (*cit).m_nButtonStates[0]);
	//	UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_MIDDLE, (*cit).m_nButtonStates[1]);
	//	UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_RIGHT,  (*cit).m_nButtonStates[2]);
	//	UpdateMousePosition(nSensorIdx, (*cit).m_nX, (*cit).m_nY);
	//	if( (*cit).m_nWheelNumber >= 0 )
	//	{
	//		(*it).second.m_nWheelState += (*cit).m_nWheelDirection;
	//		(*it).second.m_nWheelDir    = (*cit).m_nWheelDirection;
	//	}
	//	UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_WHEEL_STATE,  (*it).second.m_nWheelState);
	//	UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_WHEEL_DIR,    (*it).second.m_nWheelDir);

	//	MeasureStop(nSensorIdx);

	//	// indicate change
	//	pSensor->SetUpdateTimeStamp(dTs);
	//}
	//if(!m_vecUpdates.empty())
	//{
	//	m_vecUpdates.clear();
	//}
}

bool VistaOSGMouseDriver::RegisterWindow( VistaWindow* pWindow )
{
	if( m_pWindowingToolkit == NULL )
	{
		m_pWindowingToolkit = dynamic_cast<VistaOSGWindowingToolkit*>(
					pWindow->GetDisplayBridge()->GetWindowingToolkit() );
		if( m_pWindowingToolkit == NULL )
		{
			VISTA_THROW( "VistaOSGKeyboardDriver requires OSGWindowingToolkit", -1 );
		}
	}

	WindowInfo oInfo;
	oInfo.m_nWheelNumber = 0;
	oInfo.m_nWheelDirection = 0;
	oInfo.m_nButtonStates[0] = false;
	oInfo.m_nButtonStates[1] = false;
	oInfo.m_nButtonStates[2] = false;

	// create a sensor for this window
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	oInfo.m_nSensorIndex = AddDeviceSensor( pSensor );
	
	m_mapWindows[pWindow] = oInfo;
	return true;
}

bool VistaOSGMouseDriver::UnregisterWindow( VistaWindow* pWindow )
{
	WindowMap::iterator itWin = m_mapWindows.find( pWindow );
	if( itWin == m_mapWindows.end() )
		return false;
	m_mapWindows.erase( itWin );
	return true;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


