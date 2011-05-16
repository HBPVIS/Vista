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

#include "VistaDisplayManager.h"
#include "VistaDisplaySystem.h"
#include "VistaDisplayEntity.h"
#include "VistaDisplay.h"
#include "VistaProjection.h"
#include "VistaWindow.h"
#include "VistaViewport.h"
#include "VistaDisplayBridge.h"
#include "VistaVirtualPlatform.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaDisplayEvent.h>


#include <VistaTools/VistaProfiler.h>
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include "../VistaKernelOut.h"

#include <set>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

// forward declaration for helper method
static int FillPropertyList(VistaPropertyList &refProps, 
					  const std::string &strIniSection,
					  const std::string &strIniFile,
					  bool bConvertKeyToUpper,
					  bool bSetNameToSectionName);
static VistaWindow *CreateWindowFromSection(const std::string &strIniSection, 
									  const std::string &strIniFile,
									  VistaDisplayManager *pDisplayManager);

static VistaVirtualPlatform *CreateRefFrameFromSection(const std::string &strIniSection,
												 const std::string &strIniFile);

bool VistaDisplayManager::RegisterEventTypes(VistaEventManager *pEventMgr)
{
	
	//if(pEventMgr->GetEventTypeMapping(VistaEvent::VET_DISPLAY) == -1)
	//{
		VistaEventManager::EVENTTYPE eTp = pEventMgr->RegisterEventType("VET_DISPLAY");
		//pEventMgr->MapEventType(eTp, VistaEvent::VET_DISPLAY);
		VistaDisplayEvent::SetTypeId(eTp);

		for(int n = VistaDisplayEvent::VDE_UPDATE; n < VistaDisplayEvent::VDE_UPPER_BOUND; ++n)
			pEventMgr->RegisterEventId(eTp, VistaDisplayEvent::GetIdString(n));

		return true;
	//}

	//return false;
}
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaDisplayManager::VistaDisplayManager (IVistaDisplayBridge *pDisplayBridge)
: VistaEventHandler(),
  m_pBridge(pDisplayBridge)
{
}

VistaDisplayManager::~VistaDisplayManager()
{
#ifdef DEBUG
	vkernout << " [VistaDisplayManager] >> DESTRUCTOR <<" << endl;
#endif

	ShutdownDisplaySystems();

	/** @todo  <ms> - find a better place to do this... */
	delete m_pBridge;

	for(std::vector<VistaReferenceFrame*>::iterator it = m_vecRefFrames.begin();
		it != m_vecRefFrames.end(); ++it)
	{
		delete *it;
	}
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetVistaSystem                                              */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayBridge                                            */
/*                                                                            */
/*============================================================================*/
IVistaDisplayBridge * VistaDisplayManager::GetDisplayBridge () const
{
	return m_pBridge;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystems                                           */
/*                                                                            */
/*============================================================================*/
std::map<std::string, VistaDisplaySystem *> VistaDisplayManager::
												GetDisplaySystems() const
{
	return m_mapDisplaySystems;
}

const std::map<std::string, VistaDisplaySystem *> &VistaDisplayManager::
												GetDisplaySystemsConstRef() const
{
	return m_mapDisplaySystems;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindows                                                  */
/*                                                                            */
/*============================================================================*/
std::map<std::string, VistaWindow *> VistaDisplayManager::GetWindows() const
{
	return m_mapWindows;
}

const std::map<std::string, VistaWindow *> &VistaDisplayManager::GetWindowsConstRef() const
{
	return m_mapWindows;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewports                                                */
/*                                                                            */
/*============================================================================*/
std::map<std::string, VistaViewport *> VistaDisplayManager::GetViewports() const
{
	return m_mapViewports;
}

const std::map<std::string, VistaViewport *> &VistaDisplayManager::GetViewportsConstRef() const
{
	return m_mapViewports;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetProjections                                              */
/*                                                                            */
/*============================================================================*/
std::map<std::string, VistaProjection *> VistaDisplayManager::GetProjections() const
{
	return m_mapProjections;
}

const std::map<std::string, VistaProjection *> &VistaDisplayManager::GetProjectionsConstRef() const
{
	return m_mapProjections;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateDisplaySystem                                         */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::CreateDisplaySystems(const std::string &strIniSection,
												const std::string &strIniFile)
{
	vkernout << "------------------------------------------------------------------------------" << endl;
	vkernout << " [VistaDisplayManager] - creating display systems from ini file information..." << endl;
	vkernout << "                         ini section: " << strIniSection << endl;
	vkernout << "                         ini file:    " << strIniFile << endl;

	list<string> liDispSysSects;
	VistaProfiler oProf;
	oProf.GetTheProfileList(strIniSection, "DISPLAYSYSTEMS", liDispSysSects, strIniFile);

	if(liDispSysSects.empty())
	{
		// utter a sanity warning here
		vkernout << "[VistaDisplayManager]\n"
			 << "<WARNING>\n"
			 << "\tNo displays given in DISPLAYSYSTEMS or no DISPLAYSYSTEMS section at all.\n"
			 << "\tMost probably, there will be no window...\n"
			 << "</WARNING>\n\n";
		return true;
	}

	list<string>::iterator itDispSysSect;

	for (itDispSysSect=liDispSysSects.begin(); itDispSysSect!=liDispSysSects.end(); ++ itDispSysSect)
	{
		VistaPropertyList oProps;
		string strDisplaySystem = *itDispSysSect;
		vkernout << " [VistaDisplayManager] - creating display system from section '"
			<< strDisplaySystem << "'..." << endl;

		if (FillPropertyList(oProps, strDisplaySystem, strIniFile, true, true))
		{
			VistaVirtualPlatform *pReferenceFrame = NULL;

			if (oProps.HasProperty("REFERENCE_FRAME"))
			{
				string strSection = oProps.GetStringValue("REFERENCE_FRAME");

				pReferenceFrame = CreateRefFrameFromSection(strSection, strIniFile);
			}
			if (!pReferenceFrame)
				pReferenceFrame = new VistaVirtualPlatform;
			
			// create the display system
			VistaDisplaySystem *pDisplaySystem = CreateDisplaySystem(pReferenceFrame, oProps);
			
			if (!pDisplaySystem)
			{
				vkernout << " [VistaDisplayManager] - ERROR - unable to create display system..." << endl;
				delete pReferenceFrame;
				continue;
			}
			
		m_vecRefFrames.push_back(pReferenceFrame);

			vkernout << " [VistaDisplayManager] - display system created..." << endl;
			
			// create its viewports
			list<string> liStrings;
			oProps.GetStringListValue("VIEWPORTS", liStrings);
			list<string>::iterator itString;
			for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
			{
				vkernout << " [VistaDisplayManager] - working on viewport from section '" 
					<< *itString << "'..." << endl;
				
				VistaPropertyList oViewportProps;
				if (!FillPropertyList(oViewportProps, *itString, strIniFile, true, true))
				{
					vkernout << " [VistaDisplayManager] - ERROR - unable to find viewport section..." << endl;
					vkernout << "                                 skipping viewport..." << endl;
					continue;
				}
				
				if (!oViewportProps.HasProperty("WINDOW"))
				{
					vkernout << " [VistaDisplayManager] - ERROR - unable to create viewport..." << endl;
					vkernout << "                                 cannot determine window section..." << endl;
					continue;
				}
				
				if (!oViewportProps.HasProperty("PROJECTION"))
				{
					vkernout << " [VistaDisplayManager] - ERROR - skipping viewport..." << endl;
					vkernout << "                                 no projection section given..." << endl;
					continue;
				}
				
				// now, find out, whether we already know about the window...
				string strWindowSection = oViewportProps.GetStringValue("WINDOW");
				vkernout << " [VistaDisplayManager] - working on window from section '" 
					<< strWindowSection << "'..." << endl;
				
				VistaWindow *pWindow = CreateWindowFromSection(strWindowSection, 
					strIniFile, this);
				if (!pWindow)
					continue;

				vkernout << " [VistaDisplayManager] - creating viewport..." << endl;
				VistaViewport *pViewport = CreateViewport(pDisplaySystem, pWindow, oViewportProps);
				if (!pViewport)
				{
					vkernout << " [VistaDisplayManager] - ERROR - unable to create viewport..." << endl;
					continue;
				}
				
				// finally, create the corresponding projection...
				string strProjSection = oViewportProps.GetStringValue("PROJECTION");
				vkernout << " [VistaDisplayManager] - creating projection from section '"
					<< strProjSection << "'..." << endl;
				VistaPropertyList oProjectionProps;
				if (!FillPropertyList(oProjectionProps, strProjSection, strIniFile, true, true))
				{
					vkernout << " [VistaDisplayManager] -  ERROR - unable to create projection..." << endl;
					vkernout << "                                  cannot find corresponding section..." << endl;
					continue;
				}
				
				VistaProjection *pProjection = CreateProjection(pViewport, oProjectionProps);
				if (!pProjection)
				{
					vkernout << " [VistaDisplayManager] - ERROR - unable to create projection..." << endl;
				}

				vkernout << endl;
			}
			
		}
		else
		{
			vkernout << " [VistaDisplayManager] - ERROR - unable to create display system..." << endl;
			vkernout << "                                 unable to find section '" << strDisplaySystem << "'..." << endl;
			continue;
		}
	}

	vkernout << " [VistaDisplayManager] - display system creation finished..." << endl << endl;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ShutdownDisplaySystems                                      */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::ShutdownDisplaySystems()
{
	vkernout << " [VistaDisplayManager] - shutting down display systems..." << endl;

	while (!m_vecDisplaySystems.empty())
	{
		VistaDisplaySystem *pDisplaySystem = m_vecDisplaySystems[0];

		while (pDisplaySystem->GetNumberOfViewports())
		{
			VistaViewport *pViewport = pDisplaySystem->GetViewport(0);

			VistaProjection *pProjection = pViewport->GetProjection();

			if (!DestroyProjection(pProjection))
				return false;

			if (!DestroyViewport(pViewport))
				return false;
		}
	
		if (!DestroyDisplaySystem(pDisplaySystem))
			return false;
	}

	while (!m_vecDisplays.empty())
	{
		VistaDisplay *pDisplay = m_vecDisplays[0];

		while (pDisplay->GetNumberOfWindows())
		{
			VistaWindow *pWindow = pDisplay->GetWindow(0);

			if (!DestroyVistaWindow(pWindow))
				return false;
		}

		if (!DestroyDisplay(pDisplay))
			return false;
	}

	// there might still be some windows left...
	while (!m_mapWindows.empty())
	{
		VistaWindow *pWindow = (*(m_mapWindows.begin())).second;

		if (!DestroyVistaWindow(pWindow))
			return false;
	}

	vkernout << " [VistaDisplayManager] - display system shutdown complete..." << endl;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Update                                                      */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::Update()
{
	return m_pBridge->UpdateDisplaySystems(this);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystemByName                                      */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem *VistaDisplayManager::GetDisplaySystemByName(const std::string &strName) const
{
	map<string, VistaDisplaySystem *>::const_iterator it = m_mapDisplaySystems.find(strName);

	if (it != m_mapDisplaySystems.end())
		return (*it).second;

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayByName                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplay *VistaDisplayManager::GetDisplayByName(const std::string &strName) const
{
	map<string, VistaDisplay *>::const_iterator it = m_mapDisplays.find(strName);

	if (it != m_mapDisplays.end())
		return (*it).second;

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowByName                                             */
/*                                                                            */
/*============================================================================*/
VistaWindow *VistaDisplayManager::GetWindowByName(const std::string &strName) const
{
	map<string, VistaWindow *>::const_iterator it = m_mapWindows.find(strName);

	if (it != m_mapWindows.end())
		return (*it).second;

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportByName                                           */
/*                                                                            */
/*============================================================================*/
VistaViewport *VistaDisplayManager::GetViewportByName(const std::string &strName) const
{
	map<string, VistaViewport *>::const_iterator it = m_mapViewports.find(strName);

	if (it != m_mapViewports.end())
		return (*it).second;

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetProjectionByName                                         */
/*                                                                            */
/*============================================================================*/
VistaProjection *VistaDisplayManager::GetProjectionByName(const std::string &strName) const
{
	map<string, VistaProjection *>::const_iterator it = m_mapProjections.find(strName);

	if (it != m_mapProjections.end())
		return (*it).second;

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayEntityByName                                      */
/*                                                                            */
/*============================================================================*/
VistaDisplayEntity *VistaDisplayManager::GetDisplayEntityByName(const std::string &strName) const
{
	
	VistaDisplayEntity *pDisEntity = NULL;
	
	pDisEntity = (VistaDisplayEntity *)GetDisplaySystemByName(strName);
	if( pDisEntity != NULL ) return pDisEntity;

	pDisEntity = (VistaDisplayEntity *)GetViewportByName(strName);
	if( pDisEntity != NULL ) return pDisEntity;

	pDisEntity = (VistaDisplayEntity *)GetDisplayByName(strName);
	if( pDisEntity != NULL ) return pDisEntity;

	pDisEntity = (VistaDisplayEntity *)GetWindowByName(strName);
	if( pDisEntity != NULL ) return pDisEntity;

	pDisEntity = (VistaDisplayEntity *)GetProjectionByName(strName);
	if( pDisEntity != NULL ) return pDisEntity;

	return NULL;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNumberOfDisplaySystems                                   */
/*                                                                            */
/*============================================================================*/
int VistaDisplayManager::GetNumberOfDisplaySystems() const
{
	return m_vecDisplaySystems.size();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystem                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem *VistaDisplayManager::GetDisplaySystem(int iIndex) const
{
	if (0<=iIndex && iIndex<int(m_vecDisplaySystems.size()))
		return m_vecDisplaySystems[iIndex];
	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNumberOfDisplays                                         */
/*                                                                            */
/*============================================================================*/
int VistaDisplayManager::GetNumberOfDisplays() const
{
	return m_vecDisplays.size();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplay                                                  */
/*                                                                            */
/*============================================================================*/
VistaDisplay *VistaDisplayManager::GetDisplay(int iIndex) const
{
	if (0<=iIndex && iIndex<int(m_vecDisplays.size()))
		return m_vecDisplays[iIndex];
	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateDisplaySystem                                         */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem *VistaDisplayManager::CreateDisplaySystem(VistaVirtualPlatform *pReferenceFrame,
															   const VistaPropertyList &refProps)
{
	if (!m_pBridge)
		return NULL;

	VistaDisplaySystem *pDisplaySystem = m_pBridge->CreateDisplaySystem(pReferenceFrame, this, refProps);

	if (pDisplaySystem)
	{
		// check, whether the display system name is unique...
		string strDisplaySystemName = pDisplaySystem->GetNameForNameable();

		if (strDisplaySystemName.empty())
		{
			strDisplaySystemName = "DISPLAY_SYSTEM_"+pDisplaySystem->GetNameableIdAsString();
			pDisplaySystem->SetNameForNameable(strDisplaySystemName);

			vkernout << " [VistaDisplayManager] - WARNING - trying to create unnamed display system..." << endl;
			vkernout << "                                   setting name to '" << strDisplaySystemName << "'..." << endl;

		}

		if (m_mapDisplaySystems.find(strDisplaySystemName) != m_mapDisplaySystems.end())
		{
			vkernout << " [VistaDisplayManager] - WARNING - unable to create display system..." << endl;
			vkernout << "                                   name '" << strDisplaySystemName << "' not unique..." << endl;

			m_pBridge->DestroyDisplaySystem(pDisplaySystem);
			return false;
		}
		m_mapDisplaySystems[strDisplaySystemName] = pDisplaySystem;

		// memorize display system
		m_vecDisplaySystems.push_back(pDisplaySystem);
	}

	return pDisplaySystem;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyDisplaySystem                                        */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::DestroyDisplaySystem(VistaDisplaySystem *pDisplaySystem)
{
	if (!m_pBridge)
		return false;

	if (!pDisplaySystem)
	{
		if (pDisplaySystem->GetNumberOfViewports()>0)
		{
			vkernout << " [VistaDisplayManager] - WARNING - unable to destroy display systems..." << endl;
			vkernout << "                                   disp. sys. still has viewports..." << endl;
			return false;
		}
	}

	// find display system in display system vector
	std::vector<VistaDisplaySystem *>::iterator itDS;
	for (itDS=m_vecDisplaySystems.begin(); itDS!=m_vecDisplaySystems.end(); ++itDS)
	{
		if (pDisplaySystem == *itDS)
		{
			m_vecDisplaySystems.erase(itDS);
			break;
		}
	}

	map<string, VistaDisplaySystem *>::iterator itMap = m_mapDisplaySystems.find(pDisplaySystem->GetNameForNameable());
	if (itMap != m_mapDisplaySystems.end())
		m_mapDisplaySystems.erase(itMap);

	// and have it destroyed
	return m_pBridge->DestroyDisplaySystem(pDisplaySystem);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateDisplay                                               */
/*                                                                            */
/*============================================================================*/
VistaDisplay *VistaDisplayManager::CreateDisplay(const VistaPropertyList &refProps)
{
	if (!m_pBridge)
		return NULL;

	VistaDisplay *pDisplay = m_pBridge->CreateDisplay(this, refProps);

	if (pDisplay)
	{
		// check, whether the display name is unique...
		string strDisplayName = pDisplay->GetNameForNameable();
		
		if (strDisplayName.empty())
		{
			strDisplayName = string("DISPLAY_")+pDisplay->GetNameableIdAsString();
			pDisplay->SetNameForNameable(strDisplayName);

			vkernout << " [VistaDisplayManager] - WARNING - trying to create unnamed display..." << endl;
			vkernout << "                                   setting name to '" << strDisplayName << "'..." << endl;

		}
		
		if (m_mapDisplays.find(strDisplayName) != m_mapDisplays.end())
		{
			vkernout << " [VistaDisplayManager] - WARNING - unable to create display..." << endl;
			vkernout << "                                   name '" << strDisplayName << "' not unique..." << endl;

			m_pBridge->DestroyDisplay(pDisplay);
			return false;
		}
		m_mapDisplays[strDisplayName] = pDisplay;

		// memorize display 
		m_vecDisplays.push_back(pDisplay);
	}

	return pDisplay;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyDisplay                                              */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::DestroyDisplay(VistaDisplay *pDisplay)
{
	if (!m_pBridge)
		return false;

	if (!pDisplay)
		return true;

	// check, whether we don't have any dependent objects left...
	if (pDisplay->GetNumberOfWindows()>0)
	{
		vkernout << " [VistaDisplayManager] - WARNING - unable to destroy display..." << endl;
		vkernout << "                                   there are still windows on the display..." << endl;
		return false;
	}

	// find display in display vector
	std::vector<VistaDisplay *>::iterator itD;
	for (itD=m_vecDisplays.begin(); itD!=m_vecDisplays.end(); ++itD)
	{
		if (pDisplay == *itD)
		{
			m_vecDisplays.erase(itD);
			break;
		}
	}
	
	map<string, VistaDisplay *>::iterator itMap = m_mapDisplays.find(pDisplay->GetNameForNameable());
	if (itMap != m_mapDisplays.end())
		m_mapDisplays.erase(itMap);

	// and have it destroyed
	return m_pBridge->DestroyDisplay(pDisplay);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateVistaWindow                                           */
/*                                                                            */
/*============================================================================*/
VistaWindow *VistaDisplayManager::CreateVistaWindow(VistaDisplay *pParent, 
													  const VistaPropertyList &refProps)
{
	if (!m_pBridge)
		return NULL;

	VistaWindow *pWindow = m_pBridge->CreateVistaWindow(pParent, refProps);

	if (pWindow)
	{
		// check, whether the window name is unique...
		string strWindowName = pWindow->GetNameForNameable();
		vkernout << "window @ " << pWindow << " created.\n";

		if (strWindowName.empty())
		{
			strWindowName = "WINDOW_"+pWindow->GetNameableIdAsString();
			pWindow->SetNameForNameable(strWindowName);

			vkernout << " [VistaDisplayManager] - WARNING - trying to create unnamed window..." << endl;
			vkernout << "                                   setting name to '" << strWindowName << "'..." << endl;

		}

		if (m_mapWindows.find(strWindowName) != m_mapWindows.end())
		{
			vkernout << " [VistaDisplayManager] - WARNING - unable to create window..." << endl;
			vkernout << "                                   name '" << strWindowName << "' not unique..." << endl;

			m_pBridge->DestroyVistaWindow(pWindow);
			return false;
		}
		m_mapWindows[strWindowName] = pWindow;

		if (pParent)
		{
			vector<VistaWindow *> &refWindows = pParent->GetWindows();
			refWindows.push_back(pWindow);
		}
	}

	return pWindow;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyVistaWindow                                          */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::DestroyVistaWindow(VistaWindow *pWindow)
{
	if (!m_pBridge)
		return false;
	
	if (!pWindow)
		return true;    // well, there's no window there, right?!

	if (pWindow->GetNumberOfViewports()>0)
	{
		vkernout << " [VistaDisplayManager] - WARNING - unable to destroy window..." << endl;
		vkernout << "                                   there are still viewports in window..." << endl;
		return false;
	}

	// remove references to window
	VistaDisplay *pParent = pWindow->GetDisplay();
	if (pParent)
	{
		vector<VistaWindow *> &vecWindows = pParent->GetWindows();
		vector<VistaWindow *>::iterator itW;
		for (itW=vecWindows.begin(); itW!=vecWindows.end(); ++itW)
		{
			if (pWindow==*itW)
			{
				vecWindows.erase(itW);
				break;
			}
		}
	}
		
	map<string, VistaWindow *>::iterator itMap = m_mapWindows.find(pWindow->GetNameForNameable());
	if (itMap != m_mapWindows.end())
		m_mapWindows.erase(itMap);

	vkernout << "Destroying window @ " << pWindow << endl;
	return m_pBridge->DestroyVistaWindow(pWindow);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateViewport                                              */
/*                                                                            */
/*============================================================================*/
VistaViewport *VistaDisplayManager::CreateViewport(VistaDisplaySystem *pDisplaySystem,
													 VistaWindow *pWindow,
													 const VistaPropertyList &refProps)
{
	if (!m_pBridge)
		return NULL;

	VistaViewport *pViewport = m_pBridge->CreateViewport(pDisplaySystem, pWindow, refProps);

	if (pViewport)
	{
		// check, whether the viewport name is unique...
		string strViewportName = pViewport->GetNameForNameable();

		if (strViewportName.empty())
		{
			strViewportName = "VIEWPORT_"+pViewport->GetNameableIdAsString();
			pViewport->SetNameForNameable(strViewportName);

			vkernout << " [VistaDisplayManager] - WARNING - trying to create unnamed viewport..." << endl;
			vkernout << "                                   setting name to '" << strViewportName << "'..." << endl;

		}

		if (m_mapViewports.find(strViewportName) != m_mapViewports.end())
		{
			vkernout << " [VistaDisplayManager] - WARNING - unable to create viewport..." << endl;
			vkernout << "                                   name '" << strViewportName << "' not unique..." << endl;

			m_pBridge->DestroyViewport(pViewport);
			return false;
		}
		m_mapViewports[strViewportName] = pViewport;

		if (pDisplaySystem)
		{
			vector<VistaViewport*> &refViewports = pDisplaySystem->GetViewports();
			refViewports.push_back(pViewport);
		}

		if (pWindow)
		{
			vector<VistaViewport*> &refViewports = pWindow->GetViewports();
			refViewports.push_back(pViewport);
		}
	}

	return pViewport;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyViewport                                             */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::DestroyViewport(VistaViewport *pViewport)
{
	if (!m_pBridge)
		return false;

	if (!pViewport)
		return true;

	if (pViewport->GetProjection())
	{
		vkernout << " [VistaDisplayManager] - WARNING - unable to destroy viewport..." << endl;
		vkernout << "                                   there is still a projection..." << endl;
		return false;
	}

	VistaDisplaySystem *pDS = pViewport->GetDisplaySystem();
	if (pDS)
	{
		vector<VistaViewport *> &refViewports = pDS->GetViewports();
		vector<VistaViewport *>::iterator itVp;
		for (itVp=refViewports.begin(); itVp!=refViewports.end(); ++itVp)
		{
			if (pViewport == *itVp)
			{
				refViewports.erase(itVp);
				break;
			}
		}
	}

	VistaWindow *pW = pViewport->GetWindow();
	if (pW)
	{
		vector<VistaViewport *> &refViewports = pW->GetViewports();
		vector<VistaViewport *>::iterator itVp;
		for (itVp=refViewports.begin(); itVp!=refViewports.end(); ++itVp)
		{
			if (pViewport == *itVp)
			{
				refViewports.erase(itVp);
				break;
			}
		}
	}

	map<string, VistaViewport *>::iterator itMap = m_mapViewports.find(pViewport->GetNameForNameable());
	if (itMap != m_mapViewports.end())
		m_mapViewports.erase(itMap);

	return m_pBridge->DestroyViewport(pViewport);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProjection                                            */
/*                                                                            */
/*============================================================================*/
VistaProjection *VistaDisplayManager::CreateProjection(VistaViewport *pViewport,
														 const VistaPropertyList &refProps)
{
	if (!m_pBridge)
		return NULL;

	if (pViewport && pViewport->GetProjection())
	{
		vkernout << " [VistaDisplayManager] - WARNING - unable to create projection..." << endl;
		vkernout << "                                   parent viewport already has a projection..." << endl;
		return NULL;
	}

	VistaProjection *pProjection = m_pBridge->CreateProjection(pViewport, refProps);

	if (pProjection)
	{
		// check, whether the projection name is unique...
		string strProjectionName = pProjection->GetNameForNameable();

		if (strProjectionName.empty())
		{
			strProjectionName = "PROJECTION_"+pProjection->GetNameableIdAsString();
			pProjection->SetNameForNameable(strProjectionName);

			vkernout << " [VistaDisplayManager] - WARNING - trying to create unnamed projection..." << endl;
			vkernout << "                                   setting name to '" << strProjectionName << "'..." << endl;

		}

		if (m_mapProjections.find(strProjectionName) != m_mapProjections.end())
		{
			vkernout << " [VistaDisplayManager] - WARNING - unable to create projection..." << endl;
			vkernout << "                                   name '" << strProjectionName << "' not unique..." << endl;

			m_pBridge->DestroyProjection(pProjection);
			return false;
		}
		m_mapProjections[strProjectionName] = pProjection;

		if (pViewport)
		{
			pViewport->SetProjection(pProjection);
		}
	}

	return pProjection;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyProjection                                           */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::DestroyProjection(VistaProjection *pProjection)
{
	if (!m_pBridge)
		return false;
	
	if (!pProjection)
		return true;

	VistaViewport *pVp = pProjection->GetViewport();
	if (pVp)
	{
		pVp->SetProjection(NULL);
	}

	map<string, VistaProjection *>::iterator itMap = m_mapProjections.find(pProjection->GetNameForNameable());
	if (itMap != m_mapProjections.end())
		m_mapProjections.erase(itMap);


	return m_pBridge->DestroyProjection(pProjection);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   RenameXXX                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaDisplayManager::RenameDisplaySystem(const std::string &strOldName, 
											   const std::string &strNewName)
{
	if (m_mapDisplaySystems.find(strNewName) != m_mapDisplaySystems.end())
		return false;

	map<string, VistaDisplaySystem *>::iterator it = m_mapDisplaySystems.find(strOldName);

	if (it != m_mapDisplaySystems.end())
	{
		VistaDisplaySystem *pTemp = (*it).second;
		m_mapDisplaySystems.erase(it);

		m_mapDisplaySystems[strNewName] = pTemp;
	}

	return true;
}

bool VistaDisplayManager::RenameViewport(const std::string &strOldName, 
										  const std::string &strNewName)
{
	if (m_mapViewports.find(strNewName) != m_mapViewports.end())
		return false;

	map<string, VistaViewport *>::iterator it = m_mapViewports.find(strOldName);

	if (it != m_mapViewports.end())
	{
		VistaViewport *pTemp = (*it).second;
		m_mapViewports.erase(it);

		m_mapViewports[strNewName] = pTemp;
	}

	return true;
}

bool VistaDisplayManager::RenameProjection(const std::string &strOldName, 
											const std::string &strNewName)
{
	if (m_mapProjections.find(strNewName) != m_mapProjections.end())
		return false;

	map<string, VistaProjection *>::iterator it = m_mapProjections.find(strOldName);

	if (it != m_mapProjections.end())
	{
		VistaProjection *pTemp = (*it).second;
		m_mapProjections.erase(it);

		m_mapProjections[strNewName] = pTemp;
	}

	return true;
}

bool VistaDisplayManager::RenameWindow(const std::string &strOldName, 
											   const std::string &strNewName)
{
	if (m_mapWindows.find(strNewName) != m_mapWindows.end())
		return false;

	map<string, VistaWindow *>::iterator it = m_mapWindows.find(strOldName);

	if (it != m_mapWindows.end())
	{
		VistaWindow *pTemp = (*it).second;
		m_mapWindows.erase(it);

		m_mapWindows[strNewName] = pTemp;
	}

	return true;
}


bool VistaDisplayManager::RenameDisplay(const std::string &strOldName, 
											   const std::string &strNewName)
{
	if (m_mapDisplays.find(strNewName) != m_mapDisplays.end())
		return false;

	map<string, VistaDisplay *>::iterator it = m_mapDisplays.find(strOldName);

	if (it != m_mapDisplays.end())
	{
		VistaDisplay *pTemp = (*it).second;
		m_mapDisplays.erase(it);

		m_mapDisplays[strNewName] = pTemp;
	}

	return true;
}

bool VistaDisplayManager::MakeScreenshot(const std::string &sWindowName,
					const std::string &strFilenamePrefix) const
{
	VistaWindow *pWin = GetWindowByName(sWindowName);
	if(pWin)
	{
		return m_pBridge->MakeScreenshot(*pWin, strFilenamePrefix);
	}
	else
		return false;
}

bool VistaDisplayManager::AddSceneOverlay(IVistaSceneOverlay *pDraw, 
										const std::string &strViewportName)
{
	if(strViewportName.empty())
	{
		const std::map<std::string, VistaViewport*> &mp = GetViewportsConstRef();
		if(mp.size() == 1)
		{
			return m_pBridge->AddSceneOverlay(pDraw, (*mp.begin()).first);
		}
		return false;
	}
	else
		return m_pBridge->AddSceneOverlay(pDraw, strViewportName);
}

bool VistaDisplayManager::RemSceneOverlay(IVistaSceneOverlay *pDraw,
										const std::string &strVpName)
{
	if(strVpName.empty())
	{
		const std::map<std::string, VistaViewport*> &mp = GetViewportsConstRef();
		if(mp.size() == 1)
		{
			return m_pBridge->RemSceneOverlay(pDraw, (*mp.begin()).first);
		}
		return false;
	}
	else
		return m_pBridge->RemSceneOverlay(pDraw, strVpName);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Add2DText                                                   */
/*                                                                            */
/*============================================================================*/

Vista2DText* VistaDisplayManager::Add2DText(const std::string &strViewportName)
{
	if(strViewportName.empty())
	{
		const std::map<std::string, VistaViewport*> &mp = GetViewportsConstRef();
		if(mp.size() == 1)
		{
			return m_pBridge->Add2DText((*mp.begin()).first);
		}
		vkernerr << "[VistaDisplayMgr] Warning: Can not add 2D Text without given viewport name, when more than one viewport is registered." << std::endl;
		return NULL;
	}
	else
		return m_pBridge->Add2DText(strViewportName);
}

Vista2DBitmap*	VistaDisplayManager::Add2DBitmap	(const std::string &strViewportName )
{
	if(strViewportName.empty())
	{
		const std::map<std::string, VistaViewport*> &mp = GetViewportsConstRef();
		if(mp.size() == 1)
		{
			return m_pBridge->Add2DBitmap((*mp.begin()).first);
		}
		return NULL;
	}
	else
		return m_pBridge->Add2DBitmap(strViewportName);
}

Vista2DLine*	VistaDisplayManager::Add2DLine	(const std::string &strViewportName )
{
	if(strViewportName.empty())
	{
		const std::map<std::string, VistaViewport*> &mp = GetViewportsConstRef();
		if(mp.size() == 1)
		{
			return m_pBridge->Add2DLine((*mp.begin()).first);
		}
		return NULL;
	}
	else
		return m_pBridge->Add2DLine(strViewportName);
}

Vista2DRectangle*	VistaDisplayManager::Add2DRectangle	(const std::string &strViewportName )
{
	if(strViewportName.empty())
	{
		const std::map<std::string, VistaViewport*> &mp = GetViewportsConstRef();
		if(mp.size() == 1)
		{
			return m_pBridge->Add2DRectangle((*mp.begin()).first);
		}
		return NULL;
	}
	else
		return m_pBridge->Add2DRectangle(strViewportName);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaDisplayManager::Debug ( std::ostream & out ) const
{
	unsigned int iDispSysIndex;
	set<VistaDisplay *> setDisplays;
	set<VistaWindow *> setWindows;

	for (iDispSysIndex=0; iDispSysIndex<m_vecDisplaySystems.size(); ++iDispSysIndex)
	{
		VistaDisplaySystem *pDispSys = m_vecDisplaySystems[iDispSysIndex];
		out << (*pDispSys) << endl;

		unsigned int iViewportIndex;
		for (iViewportIndex=0; iViewportIndex<pDispSys->GetNumberOfViewports(); ++iViewportIndex)
		{
			VistaViewport *pViewport = pDispSys->GetViewport(iViewportIndex);
			if (pViewport)
			{
				out << (*pViewport) << endl;
				
				VistaProjection *pProjection = pViewport->GetProjection();
				if (pProjection)
					out << (*pProjection) << endl;

				if (pViewport->GetWindow())
				{
					setWindows.insert(pViewport->GetWindow());
				}
			}
		}
	}

	set<VistaWindow *>::iterator itWin;
	for (itWin = setWindows.begin(); itWin != setWindows.end(); ++itWin)
	{
		out << (*(*itWin)) << endl;

		if ((*itWin)->GetDisplay())
		{
			setDisplays.insert((*itWin)->GetDisplay());
		}
	}

	set<VistaDisplay *>::iterator itDisp;
	for (itDisp = setDisplays.begin(); itDisp != setDisplays.end(); ++itDisp)
	{
		out << (*(*itDisp)) << endl;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ProcessEvent                                                */
/*                                                                            */
/*============================================================================*/
void VistaDisplayManager::HandleEvent(VistaEvent *pEvent)
{
	// we are registered for display type-events!
	VistaDisplayEvent *pDisp = dynamic_cast<VistaDisplayEvent*>(pEvent);
	if(pDisp)
	{
		switch(pDisp->GetId())
		{
		case VistaDisplayEvent::VDE_POSITIONCHANGE:
			{
				// notify ;) (window only)
				pDisp->GetViewport()->GetWindow()->GetWindowProperties()->Notify(
					VistaWindow::VistaWindowProperties::MSG_POSITION_CHANGE);
				break;
			}
		case VistaDisplayEvent::VDE_RATIOCHANGE:
			{
				VistaViewport *pVp = pDisp->GetViewport();
				pVp->GetViewportProperties()->Notify( VistaViewport::VistaViewportProperties::MSG_SIZE_CHANGE );
				break;
			}
		default:
			break;
		}
	}
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( std::ostream & out, const VistaDisplayManager & refDisplayManager )
{
	refDisplayManager.Debug ( out );
	return out;
}

/*============================================================================*/
/*  LOCAL VARS / FUNCTIONS                                                    */
/*============================================================================*/
int FillPropertyList(VistaPropertyList &refProps, 
					  const std::string &strIniSection,
					  const std::string &strIniFile,
					  bool bConvertKeyToUpper,
					  bool bSetNameToSectionName)
{
	VistaProfiler oProf;

	if (!oProf.GetTheProfileSection(strIniSection, strIniFile))
		return 0;

	refProps.clear();

	list<string> liKeys;
	oProf.GetTheProfileSectionEntries(strIniSection, liKeys, strIniFile);
	list<string>::iterator itKey;
	for (itKey = liKeys.begin(); itKey!=liKeys.end(); ++itKey)
	{
		string strKey = *itKey;
		string strValue;
		oProf.GetTheProfileString(strIniSection, *itKey, "", strValue, strIniFile);

		if (bConvertKeyToUpper)
			strKey = VistaAspectsConversionStuff::ConvertToUpper(strKey);

		refProps.SetStringValue(strKey, strValue);
	}

	if (bSetNameToSectionName && !refProps.HasProperty("NAME"))
		refProps.SetStringValue("NAME", strIniSection);

	return refProps.size();
}

VistaWindow *CreateWindowFromSection(const std::string &strIniSection, 
									  const std::string &strIniFile,
									  VistaDisplayManager *pDisplayManager)
{
	VistaPropertyList oWindowProps;
	if (!FillPropertyList(oWindowProps, strIniSection, strIniFile, true, true))
	{
		vkernout << " [VistaDisplayManager] - ERROR - unable to find window section..." << endl;
		return NULL;
	}

	string strWindowName = oWindowProps.GetStringValue("NAME");
	VistaWindow *pWindow = pDisplayManager->GetWindowByName(strWindowName);
	if (!pWindow)
	{
		// we don't know about the window, so we have to create it...
		// but first, we have to check for its display...
		VistaDisplay *pDisplay = NULL;
		if (oWindowProps.HasProperty("DISPLAY"))
		{
			string strDisplaySection = oWindowProps.GetStringValue("DISPLAY");
			vkernout << " [VistaDisplaySection] - working on display from section '"
				<< strDisplaySection << "'..." << endl;
			VistaPropertyList oDisplayProps;

			if (!FillPropertyList(oDisplayProps, strDisplaySection, strIniFile, true, true))
			{
				vkernout << " [VistaDisplaySection] - ERROR - unable to read display data..." << endl;
				return NULL;
			}

			string strDisplayName = oDisplayProps.GetStringValue("NAME");
			pDisplay = pDisplayManager->GetDisplayByName(strDisplayName);
			if (!pDisplay)
			{
				vkernout << " [VistaDisplaySection] - creating display..." << endl;
				pDisplay = pDisplayManager->CreateDisplay(oDisplayProps);
			}
		}

		vkernout << " [VistaDisplayManager] - creating window..." << endl;
		pWindow = pDisplayManager->CreateVistaWindow(pDisplay, oWindowProps);
	}

	return pWindow;
}

VistaVirtualPlatform *CreateRefFrameFromSection(const std::string &strIniSection,
												 const std::string &strIniFile)
{
	VistaPropertyList oFrameProps;
	if (!FillPropertyList(oFrameProps, strIniSection, strIniFile, true, true))
	{
		vkernout << " [VistaDisplayManager] - ERROR - unable to find reference frame section..." << endl;
		return NULL;
	}

	vkernout << " [VistaDisplayManager] - creating virtual platform from section '"
		<< strIniSection << "'..." << endl;

	VistaVirtualPlatform *pPlatform = new VistaVirtualPlatform();

	if (oFrameProps.HasProperty("TRANSLATION"))
	{
		list<double> liTrans;
		string strValue = oFrameProps.GetStringValue("TRANSLATION");
		if (VistaAspectsConversionStuff::ConvertToList(strValue, liTrans) == 3)
		{
			list<double>::iterator itTrans = liTrans.begin();
			VistaVector3D v3Trans;
			v3Trans[0] = (float) *(itTrans++);
			v3Trans[1] = (float) *(itTrans++);
			v3Trans[2] = (float) *(itTrans);

#ifdef DEBUG
			vkernout << " [VistaDisplayManager] - setting ref frame translation to " << v3Trans << endl;
#endif
			pPlatform->SetTranslation(v3Trans);
		}
	}

	if (oFrameProps.HasProperty("ROTATION"))
	{
		list<double> liRot;
		string strValue = oFrameProps.GetStringValue("ROTATION");
		if (VistaAspectsConversionStuff::ConvertToList(strValue, liRot) == 4)
		{
			list<double>::iterator itRot = liRot.begin();
			VistaQuaternion qRot;
			qRot[0] = (float) *(itRot++);
			qRot[1] = (float) *(itRot++);
			qRot[2] = (float) *(itRot++);
			qRot[3] = (float) *itRot;

#ifdef DEBUG
			vkernout << " [VistaDisplayManager] - setting ref frame rotation to " << qRot << endl;
#endif

			pPlatform->SetRotation(qRot);
		}
		if (VistaAspectsConversionStuff::ConvertToList(strValue, liRot) == 3)
		{
			list<double>::iterator itRot = liRot.begin();
			VistaQuaternion qRot;
            float rx,ry,rz;
            rx = (Vista::Pi/180.0f)*(float) *(itRot++);
			ry = (Vista::Pi/180.0f)*(float) *(itRot++);
			rz = (Vista::Pi/180.0f)*(float) *itRot;
            qRot = VistaQuaternion(VistaEulerAngles(rx,ry,rz));

#ifdef DEBUG
			vkernout << " [VistaDisplayManager] - setting ref frame rotation to " << qRot << endl;
#endif

			pPlatform->SetRotation(qRot);
		}
	}

	if (oFrameProps.HasProperty("SCALE"))
	{
		float fScale = (float) oFrameProps.GetDoubleValue("SCALE");

#ifdef DEBUG
		vkernout << " [VistaDisplayManager] - setting ref frame scale to " << fScale << endl;
#endif

		if (fScale > 0)
			pPlatform->SetScale(fScale);
	}

	if (oFrameProps.HasProperty("NAME"))
	{
		pPlatform->SetNameForNameable(oFrameProps.GetStringValue("NAME"));
	}

	return pPlatform;
}




