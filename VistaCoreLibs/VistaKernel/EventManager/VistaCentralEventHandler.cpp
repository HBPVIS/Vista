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

#include <VistaKernel/EventManager/VistaCentralEventHandler.h>

// avoid include file dependencies - we don't like excessive compile times, right?! (ms)
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaDisplayEvent.h>
#include <VistaKernel/EventManager/VistaGraphicsEvent.h>
//#include <VistaKernel/EventManager/VistaInputEvent.h>
#include <VistaKernel/EventManager/VistaPickEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaCommandEvent.h>
#include <VistaKernel/VistaKernelOut.h>

//#include <VistaKernel/InteractionManager/VistaKeyboard.h>

#include <VistaKernel/EventManager/VistaEventManager.h>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#ifdef DEBUG
//#define EXCESSIVE_DEBUG		// makes the system pretty talkative...
#endif

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaCentralEventHandler::VistaCentralEventHandler(VistaEventManager *pMgr)
: m_pEvMgr(pMgr)
{
	m_pEvMgr->AddEventHandler(this, VistaEventManager::NVET_ALL, 
								VistaEventManager::NVET_ALL, 
								VistaEventManager::PRIO_MID);
}

VistaCentralEventHandler::~VistaCentralEventHandler()
{
#ifdef DEBUG
	vkernout << " [ViEvHandler] >> DESTRUCTOR <<" << endl;
#endif
	m_pEvMgr->RemEventHandler(this, VistaEventManager::NVET_ALL,
							  VistaEventManager::NVET_ALL);
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaCentralEventHandler::HandleEvent(VistaEvent *pEvent)
{
	GeneralEventHandler(pEvent);

	if (pEvent->IsHandled())
		return;

	int nTypeID = pEvent->GetType();
	if(nTypeID == VistaSystemEvent::GetTypeId())
		DispatchSystemEvent(static_cast<VistaSystemEvent*>(pEvent));
	//else if(nTypeID == VistaInputEvent::GetTypeId())
	//	DispatchInputEvent(static_cast<VistaInputEvent*>(pEvent));
	else if(nTypeID == VistaPickEvent::GetTypeId())
		DispatchPickEvent(static_cast<VistaPickEvent*>(pEvent));
	else if(nTypeID == VistaCommandEvent::GetTypeId())
		DispatchCommandEvent(static_cast<VistaCommandEvent*>(pEvent));
	else if(nTypeID == VistaDisplayEvent::GetTypeId())
		DispatchDisplayEvent(static_cast<VistaDisplayEvent*>(pEvent));
	else if(nTypeID == VistaGraphicsEvent::GetTypeId())
		DispatchGraphicsEvent(static_cast<VistaGraphicsEvent*>(pEvent));
	else
		ExternalEventHandler(pEvent);
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GeneralEventHandler                                         */
/*                                                                            */
/*============================================================================*/
void VistaCentralEventHandler::GeneralEventHandler(VistaEvent *pEvent)
{
#ifdef EXCESSIVE_DEBUG
	vkernout << "ViCeEvHa::GeneralEventHandler - received event type " << *pEvent->GetType() << endl;
#endif
}

//============================================================================
// GRAPHICS Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Order of Calls:
// 1. GraphicsEventHandler
//
// No special Handlers
//============================================================================
void VistaCentralEventHandler::DispatchGraphicsEvent(VistaGraphicsEvent *pEvent)
{
	if (pEvent->GetId() == VistaGraphicsEvent::VGE_INVALID)
		return;

	GraphicsEventHandler(pEvent);
}

void VistaCentralEventHandler::GraphicsEventHandler(VistaGraphicsEvent *pEvent)
{
#ifdef EXCESSIVE_DEBUG
	vkernout << "ViCeEvHa::GraphicsEventHandler - received event type " << *pEvent->GetType() << endl;
#endif
}

//============================================================================
// SYSTEM Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Call Order:
// 1. SystemEventHandler
// 2. Special Handlers (init, quit, exit, predraw, postdraw)
//============================================================================
void VistaCentralEventHandler::DispatchSystemEvent(VistaSystemEvent *pEvent)
{
	if (pEvent->GetId() == VistaSystemEvent::VSE_INVALID)
		return;

	// call user-defined general system event handler
	SystemEventHandler(pEvent);

	if (pEvent->IsHandled())
		return;

	// if the event is not handled yet, call the specialized handlers
	switch (pEvent->GetId())
	{
	case VistaSystemEvent::VSE_INIT:
		InitVistaEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_QUIT:
		QuitVistaEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_EXIT:
		ExitVistaEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_PREGRAPHICS:
		PreDrawGraphicsEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_POSTGRAPHICS:
		PostDrawGraphicsEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_POSTAPPLICATIONLOOP:
		PostAppEventHandler(pEvent);
		break;
	case VistaSystemEvent::VSE_PREAPPLICATIONLOOP:
		PreAppEventHandler(pEvent);
		break;
	default:
		break;
	}
}

void VistaCentralEventHandler::SystemEventHandler(VistaSystemEvent *pEvent)
{
#ifdef EXCESSIVE_DEBUG
	vkernout << "ViCeEvHa::SystemEventHandler - received event type " << *pEvent->GetType() << endl;
#endif
}

void VistaCentralEventHandler::CommandEventHandler(VistaCommandEvent *pEvent)
{
#ifdef EXCESSIVE_DEBUG
	vkernout << "ViCeEvHa::CommandEventHandler - received event type " << *pEvent->GetType() << endl;
#endif
}

void VistaCentralEventHandler::DispatchCommandEvent(VistaCommandEvent *pEvent)
{
	if (pEvent->GetId() >= VistaCommandEvent::VEIDC_LAST)
		return;

	CommandEventHandler(pEvent);
}

void VistaCentralEventHandler::InitVistaEvent(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	vkernout << "ViCeEvHa::InitVistaEvent - received event:" << endl << *pEvent;
#endif

	vkernout << "WARNING: Application didn't define a system event handler for initialization!" << endl;
}

void VistaCentralEventHandler::QuitVistaEvent(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	vkernout << "ViCeEvHa::QuitVistaEvent - received event:" << endl << *pEvent;
#endif
}

void VistaCentralEventHandler::ExitVistaEvent(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	vkernout << "ViCeEvHa::ExitVistaEvent - received event:" << endl << *pEvent;
#endif
}

void VistaCentralEventHandler::PreDrawGraphicsEvent(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	// well, better forget the following...
	// cout << "ViCeEvHa::PreDrawGraphicsEvent - received event:" << endl << *pEvent;
#endif
}

void VistaCentralEventHandler::PostDrawGraphicsEvent(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	// well, better forget the following...
	// cout << "ViCeEvHa::PostDrawGraphicsEvent - received event:" << endl << *pEvent;
#endif
}

void VistaCentralEventHandler::PreAppEventHandler(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	// well, better forget the following...
	// cout << "ViCeEvHa::PostDrawGraphicsEvent - received event:" << endl << *pEvent;
#endif
}

void VistaCentralEventHandler::PostAppEventHandler(VistaSystemEvent *pEvent)
{
#ifdef DEBUG
	// well, better forget the following...
	// cout << "ViCeEvHa::PostDrawGraphicsEvent - received event:" << endl << *pEvent;
#endif
}


//============================================================================
// DISPLAY Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Call Order:
// 1. DisplayEventHandler
// 
// no special handlers
//============================================================================
void VistaCentralEventHandler::DispatchDisplayEvent(VistaDisplayEvent *pEvent)
{
	if (pEvent->GetId() == VistaDisplayEvent::VDE_INVALID)
		return;

	DisplayEventHandler(pEvent);
}

void VistaCentralEventHandler::DisplayEventHandler(VistaDisplayEvent *pEvent)
{
#ifdef DEBUG
	vkernout << "ViCeEvHa::DisplayEventHandler - received event:" << endl << *pEvent;
#endif
}

//============================================================================
// INPUT Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Order of Calls:
// 1. InputEventHandler
// 2. Special Event Handler(s) (Keyboard)
//============================================================================
//void VistaCentralEventHandler::DispatchInputEvent(VistaInputEvent *pEvent)
//{
//	// make sure, we have a valid event
//	if (pEvent->GetId() == VistaInputEvent::VIE_INVALID)
//		return;
//
//	// call user-defined general input event handler
//	InputEventHandler(pEvent);
//
//	if (pEvent->IsHandled())
//		return;
//
//	switch(pEvent->GetInputDevice()->WhoAmI())
//	{
//	case VID_KEYBOARD:
//
//		if ( pEvent->GetId() == VistaInputEvent::VIE_BUTTON_PRESSED &&
//			 !ProcessKeyPress(((VistaKeyboard*)(pEvent->GetInputDevice()))->GetCurrentKey() ) )
//			return;
//		break;
//	default:
//		return;
//	}
//	pEvent->SetHandled(true);
//}

//void VistaCentralEventHandler::InputEventHandler (VistaInputEvent *pEvent)
//{
////#ifdef DEBUG
//#ifdef EXCESSIVE_DEBUG
//	cout << "ViCeEvHa::InputEventHandler - received event:" << endl << *pEvent;
//#endif
//}

bool VistaCentralEventHandler::ProcessKeyPress(int keyCode)
{
#ifdef DEBUG
	vkernout << "ViCeEvHa::ProcessKeyPress - doing nothing..." << endl;
#endif

	return false;
}


//============================================================================
// PICK Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Order of Calls:
// 1. PickEventHandler
//
// no special handlers
//============================================================================
void VistaCentralEventHandler::DispatchPickEvent(VistaPickEvent *pEvent)
{
    if (pEvent->GetId() == VistaPickEvent::VEID_NONE)
		return;

	// call the user event handler
	PickEventHandler(pEvent);
}

void VistaCentralEventHandler::PickEventHandler(VistaPickEvent *pEvent)
{
#ifdef EXCESSIVE_DEBUG
	vkernout << "ViCeEvHa::PickEventHandler - received event:" << endl << *pEvent;
#endif
}


//============================================================================
// EXTERNAL Event Handling, i.e. user-defineable(??) callback
//============================================================================
void VistaCentralEventHandler::ExternalEventHandler(VistaEvent *pEvent)
{
#ifdef EXCESSIVE_DEBUG
	vkernout << "ViCeEvHa::ExternalEventHandler - received event:" << endl << *pEvent;
#endif
}

