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
/*
 * $Id$
 */

#include "TimeObserver.h"
#include "DemoEvent.h"
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <iostream>

using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

TimeObserver::TimeObserver(VistaEventManager *pEventManager, int iEventType)
{
	cout << "[TimeObserver] I will listen to predraw graphics events" << endl;
	cout << "               and fire a demo event every 2 sec." << endl;
	m_dNextTime = 0;
	m_pEventManager = pEventManager;
	m_iEventType = iEventType;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Notify                                                      */
/*                                                                            */
/*============================================================================*/
void TimeObserver::Notify(const VistaEvent *pEvent)
{
	if (pEvent->GetType() == VistaSystemEvent::GetTypeId())
	{
		if ( ((VistaSystemEvent *)pEvent)->GetId() == VistaSystemEvent::VSE_PREGRAPHICS )
		{
			if (pEvent->GetTime() > m_dNextTime)
			{
				std::cout << "[TimeObserver] Got notified";
				DemoEvent oEvent;
				m_pEventManager->ProcessEvent(&oEvent);

				m_dNextTime = pEvent->GetTime() + 2.0;
				std::cout << " - Year" << std::endl;
			}
		}
	}
}

