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
// $Id: VistaPickEvent.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include "VistaPickEvent.h"
#include <iostream>
#include <VistaKernel/VistaKernelOut.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

int VistaPickEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaPickEvent::VistaPickEvent()
: VistaEvent()
{
	SetType(VistaPickEvent::GetTypeId());
	SetId(VEID_NONE);
	m_pNode			=	NULL;
	m_pPickable		=	NULL;
	m_pContactData	=	NULL;
	m_iNumberOfContacts = 0;
}

VistaPickEvent::~VistaPickEvent()
{
#ifdef DEBUG
	vkernout << " [VistaPickEvent] >> DESTRUCTOR <<" << endl;
#endif
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetPickedEvent                                              */
/*                                                                            */
/*============================================================================*/
void VistaPickEvent::SetPickedEvent(IVistaNode *pNode, VistaPickManager::SContactData *pData,
									 VistaPickable *pPickable)
{
	SetId(VPE_PICKED);
	m_pNode = pNode;
	m_pContactData = pData;
	m_pPickable = pPickable;
	m_iNumberOfContacts = -1;
//	SetInputDevice(NULL);
	m_pPickQuery = pData->pPickQuery;
	//std::list<VistaInputDevice::VISTA_BUTTON_ID> liBts;
	//pData->pInputDevice->GetPressedButtons(liBts);
	//SetButtonIds(liBts);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetTouchedEvent                                             */
/*                                                                            */
/*============================================================================*/
void VistaPickEvent::SetTouchedEvent(IVistaNode *pNode, int iNumberOfContacts,
									  VistaPickManager::SContactData *pData,
									  VistaPickable *pPickable)
{
	SetId(VPE_TOUCHED);
	m_pNode = pNode;
	m_iNumberOfContacts = iNumberOfContacts;
	m_pPickable = pPickable;
	m_pContactData = pData;
//	SetInputDevice(NULL);
	m_pPickQuery = pData->pPickQuery;
	//std::list<VistaInputDevice::VISTA_BUTTON_ID> liBts;
	//SetButtonIds(liBts); // clear buttons
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetName                                                     */
/*                                                                            */
/*============================================================================*/
std::string VistaPickEvent::GetName() const
{
	return VistaEvent::GetName() + "::VistaPickEvent";
}

std::string VistaPickEvent::GetIdString(int nId) 
{
	switch (nId)
	{
	case VPE_PICKED:
		return "VPE_PICKED";
	case VPE_TOUCHED:
		return "VPE_TOUCHED";
	default:
		return VistaEvent::GetIdString(nId);
	}
}

int VistaPickEvent::GetTypeId()
{
	return VistaPickEvent::m_nEventId;
}

void VistaPickEvent::SetTypeId(int nId)
{
	if(VistaPickEvent::m_nEventId == VistaEvent::VET_INVALID)
		VistaPickEvent::m_nEventId = nId;
}

VistaPickManager::CPickQuery   *VistaPickEvent::GetPickQuery() const
{
	return m_pPickQuery;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaPickEvent::Debug(std::ostream & out) const
{
	VistaEvent::Debug(out);
	out << " [ViPiEv]   EventId:  ";
	switch (GetId())
	{
//	case VPE_INVALID:
//		out << "VPE_INVALID" << endl;
//		break;
	case VPE_PICKED:
		out << "VPE_PICKED" << endl;
		out << " [ViPiEv]   Node:     " << m_pNode << endl;
		out << " [ViPiEv]   Contact Data: ";
		if (m_pContactData)
		{
			if (m_pContactData->fDistance >= 0)
			{
				out << "pos: " << m_pContactData->v3Position 
					<< " - dist: " << m_pContactData->fDistance << endl;
			}
			else
			{
				out << "NO CONTACT" << endl;
			}
			//out << "                          device: " << m_pContactData->pInputDevice << endl;
		}
		else
		{
			out << "NULL (WARNING!!!)" << endl;
		}
		break;
	case VPE_TOUCHED:
		out << "VPE_TOUCHED" << endl;
		out << " [ViPiEv]   Node:     " << m_pNode << endl;
		out << " [ViPiEv]   Contacts: " << m_iNumberOfContacts << endl;
		out << " [ViPiEv]   Contact Data: ";
		if (m_pContactData)
		{
			if (m_pContactData->fDistance >= 0)
			{
				out << "pos: " << m_pContactData->v3Position 
					<< " - dist: " << m_pContactData->fDistance << endl;
			}
			else
			{
				out << "NO CONTACT" << endl;
			}
			//out << "                          device: " << m_pContactData->pInputDevice << endl;
		}
		else
		{
			out << "NULL (WARNING!!!)" << endl;
		}
		break;
	default:
		out << "UNKNOWN" << endl;
		break;
	}
}


