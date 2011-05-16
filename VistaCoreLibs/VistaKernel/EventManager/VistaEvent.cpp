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

#include <VistaKernel/EventManager/VistaEvent.h>
#include <iostream>
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

int VistaEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaEvent::~VistaEvent()
{
#ifdef DEBUG
//    cout << " [VistaEvent] >> DESTRUCTOR <<" << endl;
#endif
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetName                                                     */
/*                                                                            */
/*============================================================================*/
std::string VistaEvent::GetName() const
{
	return std::string("VistaEvent");
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaEvent::Debug(std::ostream & out) const
{
	out << " [ViEv]     Name:      " << GetName() << endl;
	out << " [ViEv]     TypeId:    " << m_iType << endl;

	int iPrecision = out.precision(12);
	out << " [ViEv]     TimeStamp: " << m_dTime << endl;
	out.precision(iPrecision);

	out << " [ViEv]     Handled:   " << m_bHandled << endl;
}


int VistaEvent::Serialize(IVistaSerializer &out) const
{
	int iSize=0;
	iSize += out.WriteInt32(GetType());
	iSize += out.WriteInt32(GetId());
	iSize += out.WriteBool(IsHandled());
	iSize += out.WriteDouble(GetTime());
  //  vkernout << "VistaEvent::Serialize(): written " << iSize << " bytes.\n";
	return iSize;
}

int VistaEvent::DeSerialize(IVistaDeSerializer &in)
{
	int iSize=0;
	iSize += in.ReadInt32(m_iType);
	iSize += in.ReadInt32(m_iId);
	iSize += in.ReadBool(m_bHandled);
	iSize += in.ReadDouble(m_dTime);
	//std::cout << "VistaEvent::DeSerialize(): read " << iSize << " bytes.\n";

	return iSize;
}


std::string VistaEvent::GetSignature() const
{
	return GetName();
}


void VistaEvent::SetType(int iType)
{
	m_iType = iType;
	assert(m_iType >= 0);
}

int VistaEvent::GetType() const
{
	return m_iType;
}

int VistaEvent::GetCount() const
{
	return m_nCnt;
}


std::string VistaEvent::GetIdString(int nId) 
{
	switch(nId)
	{
	case VEID_NONE:
		return "VEID_NONE";
	case VEID_LAST:
		return "VEID_LAST";
	default:
		return "<unknown>";
	}
}

int VistaEvent::GetTypeId()
{
	return m_nEventId;
}

void VistaEvent::SetTypeId(int nId)
{
	if(m_nEventId == VistaEvent::VET_INVALID)
		m_nEventId = nId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( ostream & out, const VistaEvent & object )
{
	object.Debug ( out );
	return out;
}

