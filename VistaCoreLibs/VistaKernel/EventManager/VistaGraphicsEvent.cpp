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
// $Id: VistaGraphicsEvent.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include <VistaKernel/EventManager/VistaGraphicsEvent.h>
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>
#include <VistaKernel/VistaKernelOut.h>

#include <iostream>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
int VistaGraphicsEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaGraphicsEvent::VistaGraphicsEvent() :
m_vViewDir()
{
	SetType(VistaGraphicsEvent::GetTypeId());
	SetId(VGE_INVALID);
}

VistaGraphicsEvent::~VistaGraphicsEvent()
{
#ifdef DEBUG
	vkernout << " [VistaGraphicsEvent] >> DESTRUCTOR <<" << endl;
#endif
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetLightDirectionEvent                                      */
/*                                                                            */
/*============================================================================*/
bool VistaGraphicsEvent::SetLightDirectionEvent(const VistaVector3D &vViewDir)
{
	m_vViewDir = vViewDir;

	SetId(VGE_LIGHT_DIRECTION);

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewDirection                                            */
/*                                                                            */
/*============================================================================*/
const VistaVector3D & VistaGraphicsEvent::GetViewDirection() const
{
	return m_vViewDir;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetName                                                     */
/*                                                                            */
/*============================================================================*/
std::string VistaGraphicsEvent::GetName() const
{
	return VistaEvent::GetName() + "::VistaGraphicsEvent";
}


std::string VistaGraphicsEvent::GetIdString(int nId) 
{
	switch (nId)
	{
	case VGE_LIGHT_DIRECTION:
		return "VGE_LIGHT_DIRECTION";
	case VGE_UPPER_BOUND:
		return "VGE_UPPER_BOUND";
	default:
		return VistaEvent::GetIdString(nId);
	}
}

int VistaGraphicsEvent::GetTypeId()
{
	return VistaGraphicsEvent::m_nEventId;
}

void VistaGraphicsEvent::SetTypeId(int nId)
{
	if(VistaGraphicsEvent::m_nEventId == VistaEvent::VET_INVALID)
		VistaGraphicsEvent::m_nEventId = nId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaGraphicsEvent::Debug(std::ostream & out) const
{
	VistaEvent::Debug(out);
	out << " [ViGrEv]   EventId: "
		<< GetIdString(GetId())
		<< endl;
}

int VistaGraphicsEvent::Serialize(IVistaSerializer &out) const
{
	int iSize = VistaEvent::Serialize(out);
	iSize += out.WriteFloat32(m_vViewDir[0]);
	iSize += out.WriteFloat32(m_vViewDir[1]);
	iSize += out.WriteFloat32(m_vViewDir[2]);

	return iSize;
}

int VistaGraphicsEvent::DeSerialize(IVistaDeSerializer &in)
{
	int iSize = VistaEvent::DeSerialize(in);
	iSize += in.ReadFloat32(m_vViewDir[0]);
	iSize += in.ReadFloat32(m_vViewDir[1]);
	iSize += in.ReadFloat32(m_vViewDir[2]);

	return iSize;
}

