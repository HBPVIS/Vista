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

VistaDisplay *VistaWindow::GetDisplay() const
{
	return m_pDisplay;
}

std::string VistaWindow::GetDisplayName() const
{
	string sDisplayName;

	if(m_pDisplay)
	{
		sDisplayName = m_pDisplay->GetNameForNameable();
	}
	return sDisplayName;
}
 
unsigned int VistaWindow::GetNumberOfViewports() const
{
	return (unsigned int)m_vecViewports.size();
}

std::list<std::string> VistaWindow::GetViewportNames() const
{
	list<string> liNames;

	for( int i = 0; i < (int)m_vecViewports.size(); ++i )
	{
		liNames.push_back(m_vecViewports[i]->GetNameForNameable());
	}
	return liNames;
}

VistaViewport *VistaWindow::GetViewport(unsigned int iIndex) const
{
	if (iIndex<m_vecViewports.size())
		return m_vecViewports[iIndex];

	return NULL;
}

std::vector<VistaViewport *> &VistaWindow::GetViewports()
{
	return m_vecViewports;
}

void VistaWindow::Debug(std::ostream &out) const
{
	VistaDisplayEntity::Debug(out);

	out << " [VistaWindow] - title:    " << GetWindowProperties()->GetTitle() << std::endl;

	int x, y;
	GetWindowProperties()->GetPosition(x, y);
	out << " [VistaWindow] - position: " << x << " / " << y << std::endl;

	GetWindowProperties()->GetSize(x, y);
	out << " [VistaWindow] - size:     " << x << " / " << y << std::endl;

	out << " [VistaWindow] - stereo:   " << (GetWindowProperties()->GetStereo()?"yes":"no") << std::endl;
	out << " [VistaWindow] - fullscreen: " << (GetWindowProperties()->GetFullScreen()?"enabled":"disabled") << std::endl;


	out << " [VistaWindow] - display name: ";
	if (m_pDisplay)
		out << m_pDisplay->GetNameForNameable() << std::endl;
	else
	out << "*none* (no display given)" << std::endl;

	out << " [VistaWindow] - viewports:    " << m_vecViewports.size() << std::endl;
	
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
		out << std::endl;
	}
}

VistaDisplayEntity::IVistaDisplayEntityProperties *VistaWindow::CreateProperties()
{
	return new VistaWindowProperties(this, GetDisplayBridge());
}

VistaWindow::VistaWindowProperties *VistaWindow::GetWindowProperties() const
{
	return static_cast<VistaWindowProperties*>(GetProperties());
}

namespace {
	const std::string sSReflectionType("VistaWindow");
	
	IVistaPropertyGetFunctor *aCgFunctors[] =
	{
		new TVistaPropertyGet<bool, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_BOOL>
							("STEREO", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetStereo),
		new TVistaPropertyGet<bool, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_BOOL>
							("USE_ACCUM_BUFFER", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetUseAccumBuffer),
		new TVistaPropertyGet<bool, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_BOOL>
							("USE_STENCIL_BUFFER", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetUseStencilBuffer),
		new TVistaProperty2RefGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>
							("POSITION", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetPosition),	
		new TVistaProperty2RefGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>
							("SIZE", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetSize),
		new TVistaPropertyGet<bool, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_BOOL>
							("FULLSCREEN", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetFullScreen),
		new TVistaPropertyGet<std::string, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_STRING>
							("TITLE", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetTitle),
		new TVistaPropertyGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>(
							"VSYNC", sSReflectionType,
							&VistaWindow::VistaWindowProperties::GetVSyncEnabled),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaWindow, VistaProperty::PROPT_STRING>
							("DISPLAY_NAME", sSReflectionType,
							 &VistaWindow::GetDisplayName),
		new TVistaDisplayEntityParentPropertyGet<unsigned int, VistaWindow, VistaProperty::PROPT_DOUBLE>
							("NUMBER_OF_VIEWPORTS", sSReflectionType,
							 &VistaWindow::GetNumberOfViewports),	
		new TVistaDisplayEntityParentPropertyGet<std::list<std::string>, VistaWindow, VistaProperty::PROPT_LIST>
							("VIEWPORT_NAMES", sSReflectionType,
							 &VistaWindow::GetViewportNames),
		new TVistaPropertyGet<int, VistaWindow::VistaWindowProperties, VistaProperty::PROPT_INT>
							("WINDOW_ID", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::GetWindowId),
		NULL

	};

	IVistaPropertySetFunctor *aCsFunctors[] =
	{
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>
							("STEREO", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetStereo),
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>
							("USE_ACCUM_BUFFER", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetUseAccumBuffer),
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>
							("USE_STENCIL_BUFFER", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetUseStencilBuffer),
		new TVistaProperty2ValSet<int, VistaWindow::VistaWindowProperties>
							("POSITION", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetPosition),
		new TVistaProperty2ValSet<int, VistaWindow::VistaWindowProperties>
							("SIZE", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetSize),
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>
							("FULLSCREEN", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetFullScreen),
		new TVistaPropertySet<const std::string &, std::string, VistaWindow::VistaWindowProperties>
							("TITLE", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetTitle),
		new TVistaPropertySet<bool, bool, VistaWindow::VistaWindowProperties>(
							"VSYNC", sSReflectionType,
							&VistaWindow::VistaWindowProperties::SetVSyncEnabled),
		new TVistaPropertySet<const string &, string,VistaWindow::VistaWindowProperties>
							("NAME", sSReflectionType,
							 &VistaWindow::VistaWindowProperties::SetName),

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
	if(GetStereo() == bStereo)
	{
		return false;
	}
	else
	{
		GetDisplayBridge()->SetWindowStereo(bStereo, pW);
		Notify( MSG_STEREO_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetUseAccumBuffer                                       */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::GetUseAccumBuffer() const
{
	return GetDisplayBridge()->GetWindowAccumBufferEnabled(
							static_cast<VistaWindow*>(GetParent() ) );
}

bool VistaWindow::VistaWindowProperties::SetUseAccumBuffer(const bool bSet)
{
	VistaWindow *pW = static_cast<VistaWindow*>( GetParent() );	
	if( GetUseAccumBuffer() == bSet )
	{
		return false;
	}
	else
	{
		GetDisplayBridge()->SetWindowAccumBufferEnabled( bSet, pW);
		Notify( MSG_ACCUM_BUFFER_CHANGE );
		return true;
	}
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetUseStencilBuffer                                     */
/*                                                                            */
/*============================================================================*/
bool VistaWindow::VistaWindowProperties::GetUseStencilBuffer() const
{
	return GetDisplayBridge()->GetWindowStencilBufferEnabled(
							static_cast<VistaWindow*>(GetParent() ) );
}

bool VistaWindow::VistaWindowProperties::SetUseStencilBuffer(const bool bSet)
{
	VistaWindow *pW = static_cast<VistaWindow*>( GetParent() );	
	if( GetUseStencilBuffer() == bSet )
	{
		return false;
	}
	else
	{
		GetDisplayBridge()->SetWindowStencilBufferEnabled( bSet, pW);
		Notify( MSG_STENCIL_BUFFER_CHANGE );
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

	if((iCurXPos == x) && (iCurYPos == y))
	{
		return false;
	}
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

bool VistaWindow::VistaWindowProperties::SetSize( const int w, const int h )
{
	int iCurW = 0;
	int iCurH = 0;
	GetDisplayBridge()->GetWindowSize( iCurW, iCurH, static_cast<VistaWindow*>( GetParent() ) );
	if( ( iCurW == w ) && ( iCurH == h ) )
	{
		return false;
	}
	else
	{
		GetDisplayBridge()->SetWindowSize( w, h, static_cast<VistaWindow*>( GetParent() ) );

		Notify( MSG_SIZE_CHANGE );

		std::vector<VistaViewport*>& vecViewports = static_cast<VistaWindow*>( GetParent() )->GetViewports();
		for( std::vector<VistaViewport*>::iterator itViewport = vecViewports.begin();
				itViewport != vecViewports.end(); ++itViewport )
		{
			(*itViewport)->GetViewportProperties()->Notify( 
						VistaViewport::VistaViewportProperties::MSG_SIZE_CHANGE );
			(*itViewport)->GetViewportProperties()->Notify( 
						VistaViewport::VistaViewportProperties::MSG_POSITION_CHANGE );
		}
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

bool VistaWindow::VistaWindowProperties::SetTitle(const std::string& sTitle)
{
	VistaWindow *pWindow = static_cast<VistaWindow*>(GetParent());	
	if( GetTitle() == sTitle )
	{
		return false;
	}
	else
	{
		GetDisplayBridge()->SetWindowTitle(sTitle, pWindow);
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
/*  NAME      :   GetWindowId                                                 */
/*                                                                            */
/*============================================================================*/
int VistaWindow::VistaWindowProperties::GetWindowId() const
{
	VistaWindow *pWindow = static_cast<VistaWindow*>(GetParent());	
	return GetDisplayBridge()->GetWindowId( pWindow );
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
