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

#if !defined(_VISTADISPLAYMANAGER_H)
#define _VISTADISPLAYMANAGER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <VistaBase/VistaVectorMath.h>

#include <vector>
#include <map>
#include <list>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaDisplay;
class VistaDisplaySystem;
class VistaVirtualPlatform;
class VistaViewport;
class VistaWindow;
class VistaProjection;
class VistaDisplayEntity;
class IVistaDisplayBridge;
class VistaPropertyList;

class Vista2DBitmap;
class Vista2DText ;
class Vista2DLine;
class Vista2DRectangle;
class IVistaSceneOverlay;


// prototypes
class   VistaDisplayManager;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaDisplayManager & );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaDisplayManager manages definitions of display systems like monitors,
 * workbenches and CAVEs. Right now only one display system can be set.
 */
class VISTAKERNELAPI VistaDisplayManager : public VistaEventHandler
{
public:
	VistaDisplayManager (IVistaDisplayBridge *pDisplayBridge);

	virtual ~VistaDisplayManager();

	IVistaDisplayBridge * GetDisplayBridge () const;

	/**
	 * Create a display system according to the data within the
	 * given ini section. As a side effect, a suitable reference
	 * frame is created, which can be retrieved directly from
	 * the newly create display system.
	 */
	bool CreateDisplaySystems   (const std::string &strIniSection,
								 const std::string &strIniFile);

	/**
	 * Destroy all display systems.
	 */
	bool ShutdownDisplaySystems();

	/**
	 * Update the display manager and/or the underlying display systems.
	 */
	virtual bool Update();

	/**
	 * Retrieve display entities by name.
	 */
	VistaDisplaySystem *GetDisplaySystemByName(const std::string &strName) const;
	VistaDisplay *GetDisplayByName(const std::string &strName) const;
	VistaWindow *GetWindowByName(const std::string &strName) const;
	VistaViewport *GetViewportByName(const std::string &strName) const;
	VistaProjection *GetProjectionByName(const std::string &strName) const;

	VistaDisplayEntity *GetDisplayEntityByName(const std::string &strName) const;

	/**
	 * Retrieve display systems.
	 */
	int GetNumberOfDisplaySystems() const;
	VistaDisplaySystem *GetDisplaySystem(int iIndex = 0) const;

	/**
	 * Retrieve displays.
	 */
	int GetNumberOfDisplays() const;
	VistaDisplay *GetDisplay(int iIndex = 0) const;

	/**
	 * Retrieve display systems.
	 */
	std::map<std::string, VistaDisplaySystem *> GetDisplaySystems() const;
	const std::map<std::string, VistaDisplaySystem *> &GetDisplaySystemsConstRef() const;

	/**
	 * Retrieve windows.
	 */
	std::map<std::string, VistaWindow *> GetWindows() const;
	const std::map<std::string, VistaWindow *> &GetWindowsConstRef() const;

	/**
	 * Retrieve viewports.
	 */
	std::map<std::string, VistaViewport *> GetViewports() const;
	const std::map<std::string, VistaViewport *> &GetViewportsConstRef() const;

	/**
	 * Retrieve projections.
	 */
	std::map<std::string, VistaProjection *> GetProjections() const;
	const std::map<std::string, VistaProjection *> &GetProjectionsConstRef() const;

	/**
	 * Create/destroy display systems.
	 */
	virtual VistaDisplaySystem *CreateDisplaySystem(VistaVirtualPlatform *pReferenceFrame,
													 const VistaPropertyList &refProps);
	virtual bool DestroyDisplaySystem(VistaDisplaySystem *pDisplaySystem);

	/**
	 * Create/destroy displays.
	 */
	virtual VistaDisplay *CreateDisplay(const VistaPropertyList &refProps);
	virtual bool DestroyDisplay(VistaDisplay *pDisplay);

	/**
	 * Create/destroy windows.
	 */
	virtual VistaWindow *CreateVistaWindow(VistaDisplay *pParent,
									   const VistaPropertyList &refProps);
	virtual bool DestroyVistaWindow(VistaWindow *pWindow);

	/**
	 * Create/destroy viewports.
	 */
	virtual VistaViewport *CreateViewport(VistaDisplaySystem *pDisplaySystem,
										   VistaWindow *pWindow,
										   const VistaPropertyList &refProps);
	virtual bool DestroyViewport(VistaViewport *pViewport);

	/**
	 * Create/destroy projections.
	 */
	virtual VistaProjection *CreateProjection(VistaViewport *pViewport,
											   const VistaPropertyList &refProps);
//    virtual VistaProjection *CreateProjection(VistaWindow *pWindow,
//                                               const PropertyList &refProps);
	virtual bool DestroyProjection(VistaProjection *pProjection);

	virtual void Debug ( std::ostream & out ) const;

	/**
	 * Helper methods to keep track of renamed display entities.
	 */
	bool RenameDisplaySystem(const std::string &strOldName,
							 const std::string &strNewName);
	bool RenameViewport(const std::string &strOldName,
						const std::string &strNewName);
	bool RenameProjection(const std::string &strOldName,
						  const std::string &strNewName);
	bool RenameWindow(const std::string &strOldName,
					  const std::string &strNewName);
	bool RenameDisplay(const std::string &strOldName,
					   const std::string &strNewName);


	bool MakeScreenshot(const std::string &sWindowName,
			const std::string &strFilenamePrefix) const;

	Vista2DText* Add2DText(const std::string &strViewportName = "");
	 Vista2DBitmap*	Add2DBitmap	(const std::string &strViewportName = "") ;
	 Vista2DLine*	Add2DLine	(const std::string &strViewportName ="") ;
	 Vista2DRectangle*	Add2DRectangle	(const std::string &strViewportName ="") ;

	 bool AddSceneOverlay(IVistaSceneOverlay *, const std::string & = "");
	 bool RemSceneOverlay(IVistaSceneOverlay *, const std::string & = "");

	virtual void HandleEvent(VistaEvent *pEvent);

	static bool RegisterEventTypes(VistaEventManager *);
protected:


private:
	/**
	 * backward link to vista's main object
	 */
	IVistaDisplayBridge                * m_pBridge;
	std::vector<VistaDisplaySystem *>   m_vecDisplaySystems;
	std::vector<VistaDisplay *>         m_vecDisplays;
	std::vector<VistaReferenceFrame*>   m_vecRefFrames;

	// provide data structures for mapping from name to display entity
	std::map<std::string, VistaDisplaySystem *>    m_mapDisplaySystems;
	std::map<std::string, VistaDisplay *>          m_mapDisplays;
	std::map<std::string, VistaWindow *>           m_mapWindows;
	std::map<std::string, VistaViewport *>         m_mapViewports;
	std::map<std::string, VistaProjection *>       m_mapProjections;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTADISPLAYMANAGER_NEW_H)
