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

#ifndef _VISTAOPENSGDISPLAYBRIDGE_H
#define _VISTAOPENSGDISPLAYBRIDGE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaDisplayEntity.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>

/** @todo move 2d draw obj to dispmgr folder */
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>

#include <VistaKernel/OpenSG/VistaOpenSGTextForeground.h>
#include <VistaKernel/OpenSG/VistaOpenSGGLOverlayForeground.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGPassiveWindow.h>
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGComponentTransform.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGProjectionCameraDecorator.h>
#include <OpenSG/OSGPerspectiveCamera.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGImage.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaOpenSGNodeData;
class VistaDisplayManager;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGDisplayBridge : public IVistaDisplayBridge
{
public:

	// forward declarations of local classes
	class DisplaySystemData;
	class DisplayData;
	class WindowData;
	class ViewportData;
	class ProjectionData;

	class VistaOpenSG2DBitmap;

	// CONSTRUCTOR / DESTRUCTOR
	VistaOpenSGDisplayBridge(	osg::RenderAction *pRenderAction,
								osg::NodePtr pRealRoot);
	virtual ~VistaOpenSGDisplayBridge();

	virtual bool CreateDisplaysFromIniFile(const std::string &strDisplayIniFile,
		const std::string &strDisplaySection);

	osg::RenderAction * GetRenderAction() const;

	// render to all windows in the display manager
	void Draw();

	/**
	 * Update display systems. All necessary information is to be retrieved
	 * from the registered display manager.
	 */
	bool UpdateDisplaySystems ( VistaDisplayManager * pDisplayManager );
	void UpdateDisplaySystem  ( VistaDisplaySystem  * pDisplaySystem  );
	VistaDisplaySystem * CreateDisplaySystem( VistaVirtualPlatform * pReferenceFrame,
												VistaDisplayManager  * pDisplayManager,
												const VistaPropertyList & refProps );

	bool DestroyDisplaySystem( VistaDisplaySystem * pDisplaySystem );
	void DebugDisplaySystem( std::ostream & out, const VistaDisplaySystem * pTarget );
	void SetViewerPosition( const VistaVector3D & v3Pos, VistaDisplaySystem * pTarget );
	void GetViewerPosition( VistaVector3D & v3Pos, const VistaDisplaySystem * pTarget );
	void SetViewerOrientation( const VistaQuaternion & qOrient, VistaDisplaySystem * pTarget );
	void GetViewerOrientation( VistaQuaternion & qOrient, const VistaDisplaySystem * pTarget );
	void SetEyeOffsets( const VistaVector3D & v3LeftOffset, const VistaVector3D & v3RightOffset,
						VistaDisplaySystem * pTarget );
	void GetEyeOffsets( VistaVector3D & v3LeftOffset, VistaVector3D & v3RightOffset,
						const VistaDisplaySystem * pTarget );
	void SetLocalViewer( bool bLocalViewer, VistaDisplaySystem * pTarget );
	bool GetLocalViewer( const VistaDisplaySystem * pTarget );
	void SetHMDModeActive( bool bSet, VistaDisplaySystem * pTarget );
	bool GetHMDModeActive( const VistaDisplaySystem * pTarget );

	virtual void ObserverUpdate( IVistaObserveable   * pObserveable, int msg, int ticket,
								 VistaDisplaySystem * pTarget );

	/**
	 * Methods for display management.
	 */
	VistaDisplay * CreateDisplay( VistaDisplayManager * pDisplayManager, const VistaPropertyList & refProps );
	bool DestroyDisplay( VistaDisplay * pTarget );
	void DebugDisplay( std::ostream & out, const VistaDisplay * pTarget );

	/**
	 * Methods for window management.
	 */
	VistaWindow * CreateVistaWindow( VistaDisplay * pDisplay, const VistaPropertyList & refProps );
	bool DestroyVistaWindow( VistaWindow * pTarget );
	void SetWindowStereo( bool bStereo, VistaWindow * pTarget );
	bool GetWindowStereo( const VistaWindow * pTarget );
	void SetWindowPosition( int x, int y, VistaWindow * pTarget );
	void GetWindowPosition( int & x, int & y, const VistaWindow * pTarget );
	void SetWindowSize( int w, int h, VistaWindow * pTarget );
	void GetWindowSize( int & w, int & h, const VistaWindow * pTarget );

	// testing
	void SetFullScreen( bool bFullScreen, VistaWindow * pTarget );
	bool GetFullScreen( const VistaWindow * pTarget );

	void SetWindowTitle( const std::string & strTitle, VistaWindow * pTarget );
	std::string GetWindowTitle( const VistaWindow * pTarget );
	void * GetWindowId( const VistaWindow * pTarget );
	void DebugVistaWindow( std::ostream & out, const VistaWindow * pTarget );

	/**
	 * Methods for viewport management.
	 */
	VistaViewport * CreateViewport
	( VistaDisplaySystem * pDisplaySystem, VistaWindow * pWindow, const VistaPropertyList & refProps );
	bool DestroyViewport( VistaViewport * pTarget );
	void SetViewportPosition( int x, int y, VistaViewport * pTarget );
	void GetViewportPosition( int & x, int & y, const VistaViewport * pTarget );
	void SetViewportSize( int w, int h, VistaViewport * pTarget );
	void GetViewportSize( int & w, int & h, const VistaViewport * pTarget );
	void DebugViewport( std::ostream & out, const VistaViewport * pTarget );

	/**
	 * Methods for projection management.
	 */
	VistaProjection * CreateProjection( VistaViewport * pViewport, const VistaPropertyList & refProps );
	bool DestroyProjection( VistaProjection * pTarget );
	void SetProjectionPlane( const VistaVector3D & v3MidPoint, const VistaVector3D & v3NormalVector,
							 const VistaVector3D & v3UpVector, VistaProjection * pTarget );
	void GetProjectionPlane( VistaVector3D & v3MidPoint, VistaVector3D & v3NormalVector,
							 VistaVector3D & v3UpVector, const VistaProjection * pTarget );
	void SetProjPlaneExtents( double dLeft, double dRight, double dBottom, double dTop,
							  VistaProjection * pTarget );
	void GetProjPlaneExtents( double & dLeft, double & dRight, double & dBottom, double & dTop,
							  const VistaProjection * pTarget );
	void SetProjClippingRange( double   dNear, double   dFar,       VistaProjection * pTarget );
	void GetProjClippingRange( double & dNear, double & dFar, const VistaProjection * pTarget );
	void SetProjStereoMode( int iMode, VistaProjection * pTarget );
	int  GetProjStereoMode( const VistaProjection * pTarget );
	void DebugProjection( std::ostream & out, const VistaProjection * pTarget );

	bool SetActionFunction(void (*)());

	void SetDisplayManager(VistaDisplayManager *);
	VistaDisplayManager *GetDisplayManager() const;


	// PIXEL SPACE FUNCTIONALITY
	virtual bool MakeScreenshot(const VistaWindow &vp,
				const std::string &strFilenamePrefix,
				const bool &bNoScreenshotOnClients = true) const;

	virtual Vista2DText*	Add2DText(const std::string &strVpName = "") ;
	virtual Vista2DBitmap*	Add2DBitmap	(const std::string &strVpName = "") ;
	virtual Vista2DLine*	Add2DLine	(const std::string &strVpName ="") ;
	virtual Vista2DRectangle*	Add2DRectangle	(const std::string &strVpName ="") ;
	virtual bool Get2DOverlay(const std::string &strVpName, std::list<Vista2DDrawingObject*>&) ;


	virtual bool AddSceneOverlay(IVistaSceneOverlay *pDraw, const std::string &strVpName = "");
	virtual bool RemSceneOverlay(IVistaSceneOverlay *pDraw, const std::string &strVpName = "");

	/// returns an RBG or RGBA image with 8 bits per channel
	/// when you no longer need the raw-buffer, use free to clear up the space
	virtual bool DoLoadBitmap(const std::string &strNewFName, unsigned char** pBitmapData, int& nWidth, int& nHeight, bool& bAlpha ) ;

	virtual bool Delete2DDrawingObject(Vista2DDrawingObject* p2DObject) ;

	// cursor stuff
	virtual bool GetShowCursor() const {return m_bShowCursor; }
	void SetShowCursor(bool bShowCursor);

	// ###############################################################
	// LOCAL CLASSES
	// ###############################################################

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CDisplaySystemData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI DisplaySystemData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		DisplaySystemData();
		virtual ~DisplaySystemData();

	VistaVector3D   GetLeftEyeOffset()     const;
	VistaVector3D   GetRightEyeOffset()    const;

	void SetPlatformTransformation( const VistaTransformMatrix & matrix );

	void SetPlatformTranslation( const VistaVector3D & v3Pos );
	VistaVector3D GetPlatformTranslation() const;

	void SetPlatformOrientation( const VistaQuaternion & qOri );
	VistaQuaternion GetPlatformOrientation() const;

	void SetCameraPlatformTransformation( const VistaTransformMatrix & matrix );
	VistaTransformMatrix GetCameraPlatformTransformation() const;

	void SetCameraPlatformTranslation( const VistaVector3D & v3Pos );
	VistaVector3D GetCameraPlatformTranslation() const;

	void SetCameraPlatformOrientation( const VistaQuaternion & qOri );
	VistaQuaternion GetCameraPlatformOrientation() const;


	void SetHMDModeActive( const bool &bSet );
	bool GetHMDModeActive() const;

	private:
		bool                m_bLocalViewer;
		VistaVector3D		m_v3InitialViewerPosition;
		VistaQuaternion    m_qInitialViewerOrientation;
		VistaVector3D      m_v3LeftEyeOffset;
		VistaVector3D      m_v3RightEyeOffset;
		osg::NodeRefPtr	    m_ptrPlatformBeacon;
		osg::NodeRefPtr		m_ptrCameraBeacon;
		bool				m_bHMDModeActive;
	};

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CDisplayData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI DisplayData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		DisplayData();
		DisplayData( const std::string & sDisplayName );

		std::string GetDisplayName() const;
	private:
		std::string m_sDisplayName;
	};

 	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CWindowData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI WindowData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		WindowData();
		virtual ~WindowData();

		int GetWindowId()                const;
		std::string GetTitle()               const;
		bool GetDrawBorder()                 const;
		bool GetFullScreen()                 const;
		bool GetStereo()                     const;
		int	GetOldWindowSize()			const;
		osg::WindowPtr GetOpenSGWindow() const;
	private:
		int                    m_iWindowId;
		std::string            m_strTitle;
		bool                   m_bDrawBorder;
		bool                   m_bFullScreen;
		bool                   m_bStereo; // just in case someone asks ;-)
		int*					m_pOldWindowSize;
	osg::PassiveWindowRefPtr  m_ptrWindow;
	};


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CViewportData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI ViewportData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		ViewportData();
		~ViewportData();

		bool GetStereo() const;

		osg::ViewportPtr GetOpenSGViewport()      const ;
		osg::ViewportPtr GetOpenSGRightViewport() const ;
		osg::VistaOpenSGTextForegroundPtr GetTextForeground() const ;
		osg::VistaOpenSGGLOverlayForegroundPtr GetOverlayForeground() const;

		/**
		 * @todo think about a concept to wrap this in the DisplayManager layer
		 */
		void ReplaceViewport(osg::ViewportPtr pNewPort,
							 bool bCopyData = false,
							 bool bRight = false);

	private:
		bool             m_bStereo; // just in case someone asks ;-)
		osg::ViewportPtr m_Viewport;
		osg::ViewportPtr m_RightViewport;
		osg::VistaOpenSGTextForegroundPtr m_TextForeground;
		osg::VistaOpenSGGLOverlayForegroundPtr m_overlays;
		osg::ImageForegroundPtr m_oBitmaps;

		std::list<Vista2DDrawingObject*> m_liOverlays;
	};


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CProjectionData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI ProjectionData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:

		ProjectionData();
		virtual ~ProjectionData();

		VistaVector3D GetLeftEye()      const;
		VistaVector3D GetRightEye()     const;
		VistaVector3D GetMidPoint()     const;
		VistaVector3D GetNormalVector() const;
		VistaVector3D GetUpVector()     const;
		double         GetLeft()         const;
		double         GetRight()        const;
		double         GetBottom()       const;
		double         GetTop()          const;
		int            GetStereoMode()   const;

	void SetCameraTransformation( const VistaVector3D & v3CamPos,
								  const VistaQuaternion & qCamOri );

	void GetCameraTransformation( VistaVector3D & v3CamPos,
								  VistaQuaternion & qCamOri ) const;
	void SetCameraTranslation( const VistaVector3D & v3CamPos );
	VistaVector3D GetCameraTranslation() const;
	void SetCameraOrientation( const VistaQuaternion & qCamOri );
	VistaQuaternion GetCameraOrientation() const;

	void SetProjectionPlane();
	void SetEyes( const VistaVector3D & v3LeftOffset, const VistaVector3D & v3RightOffset );

	osg::ProjectionCameraDecoratorPtr GetCamera () const;
	private:
		osg::NodeRefPtr                      m_ptrBeacon;
		osg::NodeRefPtr                      m_ptrEyeBeacon;
		osg::NodeRefPtr                      m_ptrRightEyeBeacon;
		osg::ProjectionCameraDecoratorRefPtr m_ptrCamDeco;
		osg::ProjectionCameraDecoratorRefPtr m_ptrRightCamDeco;
		osg::PerspectiveCameraRefPtr         m_ptrPerspCam;
		osg::PerspectiveCameraRefPtr         m_ptrRightPerspCam;

		VistaVector3D m_v3LeftEye, m_v3RightEye; // left and right eye...
		VistaVector3D m_v3MidPoint, m_v3NormalVector, m_v3UpVector;
		double         m_dLeft, m_dRight, m_dBottom, m_dTop;

		int m_iStereoMode;
	};

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VistaOpenSG2DBitmap
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI VistaOpenSG2DBitmap : public Vista2DBitmap
	{
	public:

		VistaOpenSG2DBitmap(IVistaDisplayBridge *pBr,
			const std::string &sVpName);

		~VistaOpenSG2DBitmap();

		// ###############################################
		// OVERLOADED BITMAP2D API
		// ###############################################

		bool SetBitmap(const std::string &strNewFName);
		bool GetDimensions(int & nWidth, int & nHeight);
		bool SetPosition(float fPosX, float fPosY);

		osg::ImagePtr m_oImage;
		std::string m_sVpName;
	};


	bool ReplaceForegroundImage(const std::string &sVpName,
								osg::ImagePtr oOld,
								osg::ImagePtr oNew,
								float xPos, float yPos);


private:

	static void ReshapeFunction(int width, int height);

	void (*m_pActionFunction)();

	bool m_bShowCursor;

	osg::RenderAction * m_pRenderAction;

	osg::NodePtr m_pRealRoot;
	VistaDisplayManager *m_pDMgr;
	static VistaOpenSGDisplayBridge *g_DispBridge;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAOPENSGDISPLAYBRIDGE_H)

