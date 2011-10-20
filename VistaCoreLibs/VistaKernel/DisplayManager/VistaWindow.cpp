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
// $Id: VistaWindow.cpp 22128 2011-07-01 11:30:05Z dr165799 $

#include "VistaWindow.h"
#include "VistaDisplayManager.h"
#include "VistaDisplay.h"
#include "VistaDisplayBridge.h"
#include "VistaViewport.h"
#include "VistaDisplaySystem.h"

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
VistaWindow::VistaWindow(VistaDisplay *pDisplay,
						   IVistaDisplayEntityData *pData,
						   IVistaDisplayBridge *pBridge)
	: VistaDisplayEntity(pData, pBridge), m_pDisplay(pDisplay)
{
	
}

VistaWindow::~VistaWindow()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowId                                                 */
/*                                                                            */
/*============================================================================*/
int VistaWindow::GetWindowId() const
{
	return GetDisplayBridge()->GetWindowId(this);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplay                                                  */
/*                                                                            */
/*============================================================================*/
VistaDisplay *VistaWindow::GetDisplay() const
{
	return m_pDisplay;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayName                                              */
/*                                                                            */
/*============================================================================*/
std::string VistaWindow::GetDisplayName() const
{
	string sDisplayName;

	if(m_pDisplay)
	{
		sDisplayName = m_pDisplay->GetNameForNameable();
	}
	return sDisplayName;
}

 
/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNumberOfViewports                                        */
/*                                                                            */
/*============================================================================*/
unsigned int VistaWindow::GetNumberOfViewports() const
{
	return m_vecViewports.size();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportNames                                            */
/*                                                                            */
/*============================================================================*/
std::list<std::string> VistaWindow::GetViewportNames() const
{
	list<string> liNames;

	int i, iViewportCount = m_vecViewports.size();
	for (i=0; i<iViewportCount; ++i)
	{
		liNames.push_back(m_vecViewports[i]->GetNameForNameable());
	}
	return liNames;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewport                                                 */
/*                                                                            */
/*============================================================================*/
VistaViewport *VistaWindow::GetViewport(unsigned int iIndex) const
{
	if (iIndex<m_vecViewports.size())
		return m_vecViewports[iIndex];

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewports                                                */
/*                                                                            */
/*============================================================================*/
std::vector<VistaViewport *> &VistaWindow::GetViewports()
{
	return m_vecViewports;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaWindow::Debug(std::ostream &out) const
{
	VistaDisplayEntity::Debug(out);

	out << " [VistaWindow] - title:    " << GetWindowProperties()->GetTitle() << endl;

	int x, y;
	GetWindowProperties()->GetPosition(x, y);
	out << " [VistaWindow] - position: " << x << " / " << y << endl;

	GetWindowProperties()->GetSize(x, y);
	out << " [VistaWindow] - size:     " << x << " / " << y << endl;

	out << " [VistaWindow] - stereo:   " << (GetWindowProperties()->GetStereo()?"yes":"no") << endl;
	out << " [VistaWindow] - fullscreen: " << (GetWindowProperties()->GetFullScreen()?"enabled":"disabled") << endl;


	out << " [VistaWindow] - display name: ";
	if (m_pDisplay)
		out << m_pDisplay->GetNameForNameable() << endl;
	else
	out << "*none* (no display given)" << endl;

	out << " [VistaWindow] - viewports:    " << m_vecViewports.size() << endl;
	
	if (m_vecViewports.size())
	{
		out << " [VistaWindow] - vp names:     ";
		unsigned int i;
		for (i=0; i<m_vecViewports.size(); ++i)
		{
			if (i>0)
				out << ", ";
			out << m_vecViewports[i]->GetNameForNameable();
		}
		out << endl;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProperties                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplayEntity::IVistaDisplayEntityProperties *VistaWindow::CreateProperties()
{
	return new VistaWindowProperties(this, GetDisplayBridge());
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowProperties                                         */
/*                                                                            */
/*============================================================================*/
VistaWindow::VistaWindowProperties *VistaWindow::GetWindowProperties() const
{
	return static_cast<VistaWindowProperties*>(GetProperties());
}

namespace {
	const std::string sSReflectionType("VistaWindow");
	
	IVistaPropertyGetFunctor *aCgFunctors[] =
	{
		new TVistaPropertyGet<bool, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_BOOL>(
							"STEREO", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetStereo),
		new TVistaProperty2RefGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>(
							"POSITION", sSReflectionType,
							&VistaWindow::VistaWindowProperties::GetPosition),	
		new TVistaProperty2RefGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>(
							"SIZE", sSReflectionType,
							&VistaWindow::VistaWindowProperties::GetSize),
		new TVistaPropertyGet<bool, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_BOOL>(
							"FULL_SCREEN", sSReflectionType,
							&VistaWindow::VistaWindowProperties::GetFullScreen),
		new TVistaPropertyGet<std::string, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_STRING>(
							"TITLE", sSReflectionType,
							&VistaWindow::VistaWindowProperties::GetTitle),
		new TVistaPropertyGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>(
							"VSYNC", sSReflectionType,
							&VistaWindow::VistaWindowProperties::GetVSyncEnabled),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaWindow, VistaProperty::PROPT_STRING>(
							"DISPLAY_NAME", sSReflectionType,
							&VistaWindow::GetDisplayName),
		new TVistaDisplayEntityParentPropertyGet<unsigned int, VistaWindow, VistaProperty::PROPT_DOUBLE>(
							"NUMBER_OF_VIEWPORTS", sSReflectionType,
							&VistaWindow::GetNumberOfViewports),	
		new TVistaDisplayEntityParentPropertyGet<std::list<std::string>, VistaWindow, VistaProperty::PROPT_LIST>(
							"VIEWPORT_NAMES", sSReflectionType,
							&VistaWindow::GetViewportNames),
		NULL

	};

	IVistaPropertySetFunctor *aCsFunctors[] =
	{
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>(
							"STEREO", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetStereo,
							&VistaAspectsConversionStuff::ConvertToBool),
		new TVistaProperty2ValSet<int, VistaWindow::VistaWindowProperties>(
							"POSITION", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetPosition,
							&VistaAspectsConversionStuff::ConvertStringTo2Int),
		new TVistaProperty2ValSet<int, VistaWindow::VistaWindowProperties>(
							"SIZE", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetSize,
							&VistaAspectsConversionStuff::ConvertStringTo2Int),
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>(
							"FULL_SCREEN", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetFullScreen,
							&VistaAspectsConversionStuff::ConvertToBool),
		new TVistaPropertySet<const std::string &, std::string, VistaWindow::VistaWindowProperties>(
							"TITLE", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetTitle,
							&VistaAspectsConversionStuff::ConvertToString),
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>(
							"VSYNC", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetVSyncEnabled,
							&VistaAspectsConversionStuff::ConvertToBool),
		new TVistaPropertySet<const string &, string,VistaWindow::VistaWindowProperties>(
							"NAME", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetName,
							&VistaAspectsConversionStuff::ConvertToString),
		NULL
	};
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetName                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::SetName(const std::string &sName)
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());
	string strOldName = pW->GetNameForNameable();

	if(strOldName != "")
	{
		if (!pW->GetDisplay()->GetDisplayManager()->RenameWindow(strOldName, sName))
			return false;
	}
	return IVistaDisplayEntityProperties::SetName(sName);

}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetStereo                                               */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::GetStereo() const
{
	return GetDisplayBridge()->GetWindowStereo(static_cast<VistaWindow*>(GetParent()));
}

bool VistaWindow::VistaWindowProperties::SetStereo(const bool bStereo)
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	if(GetStereo() == bStereo) return false;
	else{
		GetDisplayBridge()->SetWindowStereo(bStereo, pW);
		Notify(MSG_STEREO_CHANGE);
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetPosition                                             */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::GetPosition(int& x, int& y) const
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	GetDisplayBridge()->GetWindowPosition(x, y, pW);

	return true;
}
bool VistaWindow::VistaWindowProperties::SetPosition(const int x, const int y)
{
	int iCurXPos, iCurYPos;
	 
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	GetPosition(iCurXPos, iCurYPos);

	if((iCurXPos == x) && (iCurYPos == y)) return false;
	else
	{
		GetDisplayBridge()->SetWindowPosition(x, y, pW);
		Notify( MSG_POSITION_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetSize                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::GetSize(int& w, int& h) const
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	GetDisplayBridge()->GetWindowSize( w, h, pW );

	return true;
}

bool VistaWindow::VistaWindowProperties::SetSize(const int w, const int h)
{
	int iCurW, iCurH;
	 
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	GetSize(iCurW, iCurH);

	if((iCurW == w) && (iCurH == h)) return false;
	else
	{
		GetDisplayBridge()->SetWindowSize(w, h, pW);
		Notify( MSG_SIZE_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetFullScreen                                           */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::GetFullScreen() const
{
	return GetDisplayBridge()->GetFullScreen(static_cast<VistaWindow*>(GetParent()));
}

bool VistaWindow::VistaWindowProperties::SetFullScreen(bool bFullScreen)
{
	if( bFullScreen != GetFullScreen() )
	{
		GetDisplayBridge()->SetFullScreen(bFullScreen, static_cast<VistaWindow*>(GetParent()));
		Notify( MSG_FULLSCREEN_CHANGE );
		return true;
	}
	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetTitle                                                */
/*                                                                            */
/*============================================================================*/
std::string VistaWindow::VistaWindowProperties::GetTitle() const
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	return GetDisplayBridge()->GetWindowTitle(pW);
}

bool VistaWindow::VistaWindowProperties::SetTitle(const std::string& strTitle)
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());	
	if(GetTitle() == strTitle) return false;
	else{
		GetDisplayBridge()->SetWindowTitle(strTitle, pW);
		Notify( MSG_TITLE_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetReflectionableType                                       */
/*                                                                            */
/*============================================================================*/
string VistaWindow::VistaWindowProperties::GetReflectionableType() const
{
	return sSReflectionType;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   AddToBaseTypeList                                           */
/*                                                                            */
/*============================================================================*/
int VistaWindow::VistaWindowProperties::AddToBaseTypeList(list<string> &rBtList) const
{
	int nSize = IVistaDisplayEntityProperties::AddToBaseTypeList(rBtList);
	rBtList.push_back(sSReflectionType);
	return nSize + 1;
}


VistaWindow::VistaWindowProperties::~VistaWindowProperties()
{
}

VistaWindow::VistaWindowProperties::VistaWindowProperties(
	VistaWindow *pParent,
	IVistaDisplayBridge *pBridge)
  : IVistaDisplayEntityProperties(pParent, pBridge)
{
}

bool VistaWindow::VistaWindowProperties::SetVSyncEnabled( bool bVSync )
{
	if( GetDisplayBridge()->SetWindowVSync( bVSync, static_cast<VistaWindow*>(GetParent()) ) )
	{
		Notify( MSG_VSYNC_CHANGE );
		return true;
	}
	return false;
}

int VistaWindow::VistaWindowProperties::GetVSyncEnabled() const
{
	VistaWindow *pW = static_cast<VistaWindow*>(GetParent());
	return GetDisplayBridge()->GetWindowVSync( pW );
}
