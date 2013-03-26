/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#include "VistaViewport.h"
#include "VistaDisplaySystem.h"
#include "VistaProjection.h"
#include "VistaWindow.h"
#include "VistaDisplayBridge.h"
#include "VistaDisplayManager.h"

#include <VistaAspects/VistaAspectsUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaViewport::VistaViewport(VistaDisplaySystem *pDisplaySystem, 
							   VistaWindow *pWindow,
							   IVistaDisplayEntityData *pData,
							   IVistaDisplayBridge *pBridge)
: VistaDisplayEntity(pData, pBridge),
  m_pDisplaySystem(pDisplaySystem),
  m_pWindow(pWindow),
  m_pProjection(NULL)
{
}

VistaViewport::~VistaViewport()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystem                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem *VistaViewport::GetDisplaySystem() const
{
	return m_pDisplaySystem;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystemName                                        */
/*                                                                            */
/*============================================================================*/
string VistaViewport::GetDisplaySystemName() const
{
	string sDisplaySName;

	if(m_pDisplaySystem)
	{
		sDisplaySName = m_pDisplaySystem->GetNameForNameable();
	}
	return sDisplaySName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindow                                                   */
/*                                                                            */
/*============================================================================*/
VistaWindow *VistaViewport::GetWindow() const
{
	return m_pWindow;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowName                                               */
/*                                                                            */
/*============================================================================*/
string VistaViewport::GetWindowName() const
{
	string sWindowName;

	if(m_pWindow)
	{
		sWindowName = m_pWindow->GetNameForNameable();
	}
	return sWindowName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetProjection                                           */
/*                                                                            */
/*============================================================================*/
VistaProjection *VistaViewport::GetProjection() const
{
	return m_pProjection;
}

void VistaViewport::SetProjection(VistaProjection *pProjection)
{
	m_pProjection = pProjection;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetProjectionName                                           */
/*                                                                            */
/*============================================================================*/
string VistaViewport::GetProjectionName() const
{
	string sProjectionName;

	if(m_pProjection)
	{
		sProjectionName = m_pProjection->GetNameForNameable();
	}
	return sProjectionName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaViewport::Debug(std::ostream &out) const
{
	//VistaDisplayEntity::Debug(out);

	out << vstr::indent << "[VistaViewport]      - name                   : " 
		<< GetNameForNameable() << std::endl;

	int x, y;
	GetViewportProperties()->GetPosition(x, y);
	out << vstr::indent << "[VistaViewport]      - position               : " 
		<< x << " / " << y << std::endl;

	GetViewportProperties()->GetSize(x, y);
	out << vstr::indent << "[VistaViewport]      - size                   : "
		<< x << " / " << y << std::endl;

	out << vstr::indent << "[VistaViewport]      - display system name    : ";
	if (m_pDisplaySystem)
		out << m_pDisplaySystem->GetNameForNameable() << std::endl;
	else
		out << "*none* (no display system given)" << std::endl;

	out << vstr::indent << "[VistaViewport]      - window name            : ";
	if (m_pWindow)
		out << m_pWindow->GetNameForNameable() << std::endl;
	else
		out <<"*none* (no window given)" << std::endl;

	out << vstr::indent << "[VistaViewport]      - projection name        : ";
	if (m_pProjection)
		out << m_pProjection->GetNameForNameable() << std::endl;
	else
		out << "*none* (no projection given)" << std::endl;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProperties                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplayEntity::IVistaDisplayEntityProperties *VistaViewport::CreateProperties()
{
	return new VistaViewportProperties(this, GetDisplayBridge());
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportProperties                                       */
/*                                                                            */
/*============================================================================*/
VistaViewport::VistaViewportProperties *VistaViewport::GetViewportProperties() const
{
	return static_cast<VistaViewportProperties*>(GetProperties());
}

namespace {
	const std::string sSReflectionType("VistaViewport");


	IVistaPropertyGetFunctor *aCgFunctors[] =
	{

		new TVistaProperty2RefGet<int, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_INT>
		("POSITION", sSReflectionType,
		 &VistaViewport::VistaViewportProperties::GetPosition),	
		new TVistaProperty2RefGet<int, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_INT>
		("SIZE", sSReflectionType,
		 &VistaViewport::VistaViewportProperties::GetSize),	
		 new TVistaPropertyGet<bool, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_BOOL>
		("PASSIVE_BACKGROUND", sSReflectionType,
		&VistaViewport::VistaViewportProperties::GetHasPassiveBackground),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaViewport, VistaProperty::PROPT_STRING>
		("DISPLAY_SYSTEM_NAME", sSReflectionType,
		 &VistaViewport::GetDisplaySystemName),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaViewport, VistaProperty::PROPT_STRING>
		("WINDOW_NAME", sSReflectionType,
		 &VistaViewport::GetWindowName),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaViewport, VistaProperty::PROPT_STRING>
		("PROJECTION_NAME", sSReflectionType,
		 &VistaViewport::GetProjectionName),
		NULL
	};

	IVistaPropertySetFunctor *aCsFunctors[] =
	{	
		new TVistaProperty2ValSet<int, VistaViewport::VistaViewportProperties>
		("POSITION", sSReflectionType,
		 &VistaViewport::VistaViewportProperties::SetPosition),
		new TVistaProperty2ValSet<int, VistaViewport::VistaViewportProperties>
		("SIZE", sSReflectionType,
		 &VistaViewport::VistaViewportProperties::SetSize),
		 new TVistaPropertySet<bool, bool, VistaViewport::VistaViewportProperties>
		("PASSIVE_BACKGROUND", sSReflectionType,
		 &VistaViewport::VistaViewportProperties::SetHasPassiveBackground),
		new TVistaPropertySet<const string &, string,VistaViewport::VistaViewportProperties>
		("NAME", sSReflectionType,
		 &VistaViewport::VistaViewportProperties::SetName),
		NULL
	};
}

bool VistaViewport::VistaViewportProperties::SetName(const std::string &sName)
{
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	string strOldName = pV->GetNameForNameable();

	if(strOldName != "")
	{
		if (!pV->GetDisplaySystem()->GetDisplayManager()->RenameViewport(strOldName, sName))
			return false;
	}
	return IVistaDisplayEntityProperties::SetName(sName);

}

bool VistaViewport::VistaViewportProperties::GetPosition(int& x, int& y) const
{
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportPosition(x, y, pV);

	return true;
}

bool VistaViewport::VistaViewportProperties::SetPosition(const int x, const int y)
{
	int iCurXPos, iCurYPos;
	 
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportPosition(iCurXPos, iCurYPos, pV);

	if((iCurXPos == x) && (iCurYPos == y))
		return false;
	else
	{
		GetDisplayBridge()->SetViewportPosition(x, y, pV);
		Notify( MSG_POSITION_CHANGE );
		return true;
	}
}

bool VistaViewport::VistaViewportProperties::GetSize(int& w, int& h) const
{
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportSize(w, h, pV);

	return true;
}

bool VistaViewport::VistaViewportProperties::SetSize(const int w, const int h)
{
	int iCurW, iCurH;
	 
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportSize(iCurW, iCurH, pV);

	if((iCurW == w) && (iCurH == h))
		return false;
	else
	{
		GetDisplayBridge()->SetViewportSize(w, h, pV);
		Notify( MSG_SIZE_CHANGE );
		return true;
	}
}


bool VistaViewport::VistaViewportProperties::GetHasPassiveBackground() const
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportHasPassiveBackground( pViewport );
}

bool VistaViewport::VistaViewportProperties::SetHasPassiveBackground( const bool bSet )
{
	if( bSet == GetHasPassiveBackground() )
		return false;
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	GetDisplayBridge()->SetViewportHasPassiveBackground( bSet, pViewport );
	Notify( MSG_PASSIVE_BACKGROUND_CHANGE );
	return true;
}




string VistaViewport::VistaViewportProperties::GetReflectionableType() const
{
	return sSReflectionType;
}

int VistaViewport::VistaViewportProperties::AddToBaseTypeList(list<string> &rBtList) const
{
	int nSize = IVistaDisplayEntityProperties::AddToBaseTypeList(rBtList);
	rBtList.push_back(sSReflectionType);
	return nSize + 1;
}


VistaViewport::VistaViewportProperties::VistaViewportProperties(
	VistaDisplayEntity *pParent,
	IVistaDisplayBridge *pDisplayBridge)
  : IVistaDisplayEntityProperties(pParent, pDisplayBridge)
{
}

VistaViewport::VistaViewportProperties::~VistaViewportProperties()
{
}


