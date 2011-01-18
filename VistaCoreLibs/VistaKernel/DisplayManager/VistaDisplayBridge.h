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
// $Id$

#if !defined _VISTADISPLAYBRIDGE_H
#define _VISTADISPLAYBRIDGE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>
#include <list>
#include <VistaBase/VistaVectorMath.h>

#include <VistaKernel/VistaKernelConfig.h>
/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;
class IVistaDisplayEntityData;
class VistaDisplaySystem;
class VistaDisplay;
class VistaWindow;
class VistaViewport;
class VistaProjection;
class VistaPropertyList;
class VistaVirtualPlatform;
class IVistaObserveable;

class Vista2DText;
class Vista2DBitmap;
class Vista2DLine;
class Vista2DRectangle;
class Vista2DDrawingObject;

class IVistaSceneOverlay;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * IVistaDisplayBridge is ...
 */
class VISTAKERNELAPI IVistaDisplayBridge
{
	friend class VistaDisplaySystem;
	friend class VistaDisplay;
	friend class VistaWindow;
	friend class VistaViewport;
	friend class VistaProjection;
	friend class VistaDisplayManager;

public:
	virtual ~IVistaDisplayBridge();

	/**
	 * Create a display bridge.
	 */
	IVistaDisplayBridge();


	/**
	 * Update display systems. All necessary information is to be retrieved
	 * from the registered display manager.
	 */
	virtual bool UpdateDisplaySystems(VistaDisplayManager *pDisplayManager);

	/**
	 * Methods for display system management.
	 */
	virtual bool CreateDisplaysFromIniFile(const std::string &strDisplayIniFile,
		const std::string &strDisplaySection) { return false; }


	virtual VistaDisplaySystem *CreateDisplaySystem(VistaVirtualPlatform *pReferenceFrame,
													 VistaDisplayManager *pDisplayManager,
													 const VistaPropertyList &refProps) = 0;
	virtual bool DestroyDisplaySystem   (VistaDisplaySystem *pDisplaySystem) = 0;
	virtual void DebugDisplaySystem     (std::ostream &out,
										 const VistaDisplaySystem *pTarget) = 0;
	virtual void SetViewerPosition      (const VistaVector3D &v3Pos,
										 VistaDisplaySystem *pTarget) = 0;
	virtual void GetViewerPosition      (VistaVector3D &v3Pos,
										 const VistaDisplaySystem *pTarget) = 0;
	virtual void SetViewerOrientation   (const VistaQuaternion &qOrient,
										 VistaDisplaySystem *pTarget) = 0;
	virtual void GetViewerOrientation   (VistaQuaternion &qOrient,
										 const VistaDisplaySystem *pTarget) = 0;
	virtual void SetEyeOffsets          (const VistaVector3D &v3LeftOffset,
										 const VistaVector3D &v3RightOffset,
										 VistaDisplaySystem *pTarget) = 0;
	virtual void GetEyeOffsets          (VistaVector3D &v3LeftOffset,
										 VistaVector3D &v3RightOffset,
										 const VistaDisplaySystem *pTarget) = 0;
	virtual void SetLocalViewer         (bool bLocalViewer,
										 VistaDisplaySystem *pTarget) = 0;
	virtual bool GetLocalViewer         (const VistaDisplaySystem *pTarget) = 0;
	virtual void SetHMDModeActive        ( bool bSet,
										 VistaDisplaySystem *pTarget ) = 0;
	virtual bool GetHMDModeActive        ( const VistaDisplaySystem *pTarget ) = 0;
	virtual void ObserverUpdate         (IVistaObserveable *pObserveable,
										 int msg,
										 int ticket,
										 VistaDisplaySystem *pTarget);

	/**
	 * Methods for display management.
	 */
	virtual VistaDisplay *CreateDisplay(VistaDisplayManager *pDisplayManager,
										 const VistaPropertyList &refProps) = 0;
	virtual bool DestroyDisplay         (VistaDisplay *pDisplay) = 0;
//    virtual std::string GetDisplayString(const VistaDisplay *pTarget) = 0;
	virtual void DebugDisplay           (std::ostream &out,
										 const VistaDisplay *pTarget) = 0;

	/**
	 * Methods for window management.
	 */
	virtual VistaWindow *CreateVistaWindow  (VistaDisplay *pDisplay,
											  const VistaPropertyList &refProps) = 0;
	virtual bool DestroyVistaWindow     (VistaWindow *pWindow) = 0;
	virtual void SetWindowStereo        (bool bStereo,
										 VistaWindow *pTarget) = 0;
	virtual bool GetWindowStereo        (const VistaWindow *pTarget) = 0;
	virtual void SetWindowPosition      (int x, int y,
										 VistaWindow *pTarget) = 0;
	virtual void GetWindowPosition      (int &x, int &y,
										 const VistaWindow *pTarget) = 0;
	virtual void SetWindowSize          (int w, int h,
										 VistaWindow *pTarget) = 0;
	virtual void GetWindowSize          (int &w, int &h,
										 const VistaWindow *pTarget) = 0;
	virtual void SetFullScreen          (bool bFullScreen,
										 VistaWindow *pTarget) = 0;
	virtual bool GetFullScreen          (const VistaWindow *pTarget) = 0;
	virtual void SetWindowTitle         (const std::string &strTitle,
										 VistaWindow *pTarget) = 0;
	virtual std::string GetWindowTitle  (const VistaWindow *pTarget) = 0;
	virtual void *GetWindowId           (const VistaWindow *pTarget) = 0;
	virtual void DebugVistaWindow       (std::ostream &out,
										 const VistaWindow *pTarget) = 0;

	/**
	 * Methods for viewport management.
	 */
	virtual VistaViewport *CreateViewport(VistaDisplaySystem *pDisplaySystem,
										   VistaWindow *pWindow,
										   const VistaPropertyList &refProps) = 0;
	virtual bool DestroyViewport        (VistaViewport *pViewport) = 0;
	virtual void SetViewportPosition    (int x, int y,
										 VistaViewport *pTarget) = 0;
	virtual void GetViewportPosition    (int &x, int &y,
										 const VistaViewport *pTarget) = 0;
	virtual void SetViewportSize        (int w, int h,
										 VistaViewport *pTarget) = 0;
	virtual void GetViewportSize        (int &w, int &h,
										 const VistaViewport *pTarget) = 0;
	virtual void DebugViewport          (std::ostream &out,
										 const VistaViewport *pTarget) = 0;

	/**
	 * Methods for projection management.
	 */
	virtual VistaProjection *CreateProjection(VistaViewport *pViewport,
											   const VistaPropertyList &refProps) = 0;
//    virtual VistaProjection *CreateProjection(VistaWindow *pWindow,
//                                               const PropertyList &refProps) = 0;
	virtual bool DestroyProjection      (VistaProjection *pProjection) = 0;
	virtual void SetProjectionPlane     (const VistaVector3D &v3MidPoint,
										 const VistaVector3D &v3NormalVector,
										 const VistaVector3D &v3UpVector,
										 VistaProjection *pTarget) = 0;
	virtual void GetProjectionPlane     (VistaVector3D &v3MidPoint,
										 VistaVector3D &v3NormalVector,
										 VistaVector3D &v3UpVector,
										 const VistaProjection *pTarget) = 0;
	virtual void SetProjPlaneExtents    (double dLeft, double dRight, double dBottom, double dTop,
										 VistaProjection *pTarget) = 0;
	virtual void GetProjPlaneExtents    (double &dLeft, double &dRight, double &dBottom, double &dTop,
										 const VistaProjection *pTarget) = 0;
	virtual void SetProjClippingRange   (double dNear, double dFar,
										 VistaProjection *pTarget) = 0;
	virtual void GetProjClippingRange   (double &dNear, double &dFar,
										 const VistaProjection *pTarget) = 0;
	virtual void SetProjStereoMode      (int iMode,
										 VistaProjection *pTarget) = 0;
	virtual int  GetProjStereoMode      (const VistaProjection *pTarget) = 0;
	virtual void DebugProjection        (std::ostream &out,
										 const VistaProjection *pTarget) = 0;

	/**
	 * Methods for the creation of new display entities.
	 */
	virtual VistaDisplaySystem *NewDisplaySystem   (VistaVirtualPlatform *pReferenceFrame,
													 IVistaDisplayEntityData *pData,
													 VistaDisplayManager *pDisplayManager);
	virtual VistaDisplay       *NewDisplay         (IVistaDisplayEntityData *pData,
													 VistaDisplayManager *pDisplayManager);
	virtual VistaWindow        *NewWindow          (VistaDisplay *pDisplay,
													 IVistaDisplayEntityData *pData);
	virtual VistaViewport      *NewViewport        (VistaDisplaySystem *pDisplaySystem,
													 VistaWindow *pWindow,
													 IVistaDisplayEntityData *pData);
	virtual VistaProjection    *NewProjection      (VistaViewport *pViewport,
													 IVistaDisplayEntityData *pData);


	virtual bool MakeScreenshot(const VistaWindow &vp,
				const std::string &strFilenamePrefix,
				const bool &bNoScreenshotOnClients = true ) const = 0;

	virtual Vista2DText*	Add2DText(const std::string &strWindowName = "") = 0 ;
	virtual Vista2DBitmap*	Add2DBitmap	(const std::string &strWindowName = "") = 0;
	virtual Vista2DLine*	Add2DLine	(const std::string &strWindowName ="") = 0;
	virtual Vista2DRectangle*	Add2DRectangle	(const std::string &strWindowName ="") = 0;

	virtual bool Get2DOverlay(const std::string &strWindowName,
							  std::list<Vista2DDrawingObject*>&) = 0;

	virtual bool AddSceneOverlay(IVistaSceneOverlay *pDraw, const std::string &strVpName = "") = 0;
	virtual bool RemSceneOverlay(IVistaSceneOverlay *pDraw, const std::string &strVpName = "") = 0;

	/// returns an RBG or RGBA image with 8 bits per channel
	/// when you no longer need the raw-buffer, use free to clear up the space
	virtual bool DoLoadBitmap(const std::string &strNewFName,
							  unsigned char** pBitmapData,
							  int& nWidth,
							  int& nHeight,
							  bool& bAlpha ) = 0;

	virtual bool Delete2DDrawingObject(Vista2DDrawingObject* p2DObject) = 0;

	virtual bool GetShowCursor() const = 0;
	virtual void SetShowCursor(bool bShowCursor) = 0;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTADISPLAYBRIDGE_H)
