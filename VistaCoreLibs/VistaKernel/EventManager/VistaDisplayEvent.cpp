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

#include <VistaKernel/EventManager/VistaDisplayEvent.h>
#include <iostream>
#include "../VistaKernelOut.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

int VistaDisplayEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaDisplayEvent::VistaDisplayEvent()
{
	SetType(VistaDisplayEvent::GetTypeId());

	SetId(VDE_INVALID);
	m_pViewport = NULL;
}

VistaDisplayEvent::~VistaDisplayEvent()
{
#ifdef DEBUG
	vkernout << " [VistaDisplayEvent] >> DESTRUCTOR <<" << endl;
#endif
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetId                                                       */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayEvent::SetId(int iId)
{
	if (iId > VDE_INVALID && iId < VDE_UPPER_BOUND)
	{
		VistaEvent::SetId(iId);
		return true;
	}
	VistaEvent::SetId(VDE_INVALID);
	return false;
}

void VistaDisplayEvent::SetViewport(VistaViewport *pPort)
{
	m_pViewport = pPort;
}

VistaViewport *VistaDisplayEvent::GetViewport() const
{
	return m_pViewport;
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetName                                                     */
/*                                                                            */
/*============================================================================*/
std::string VistaDisplayEvent::GetName() const
{
	return VistaEvent::GetName() + "::VistaDisplayEvent";
}

std::string VistaDisplayEvent::GetIdString(int nId) 
{
	switch (nId)
	{
	case VDE_INVALID:
		return  "VDE_INVALID";
	case VDE_UPDATE:
		return "VDE_UPDATE";
	case VDE_RATIOCHANGE:
		return "VDE_RATIOCHANGED";
	case VDE_POSITIONCHANGE:
		return "VDE_POSITIONCHANGE";
	default:
		return VistaEvent::GetIdString(nId);
	}
}

int VistaDisplayEvent::GetTypeId()
{
	return VistaDisplayEvent::m_nEventId;
}

void VistaDisplayEvent::SetTypeId(int nId)
{
	if(VistaDisplayEvent::m_nEventId == VistaEvent::VET_INVALID)
		VistaDisplayEvent::m_nEventId = nId;
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaDisplayEvent::Debug(std::ostream & out) const
{
	VistaEvent::Debug(out);
	out << " [ViDiEv]   EventId: "
		<< GetIdString(GetId())
		<< endl;
}


