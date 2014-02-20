/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(disable: 4231)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#endif

#include "VistaOpenSGDisplayBridge.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaDisplay.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#ifdef VISTA_WITH_GLUT
#include <VistaKernel/DisplayManager/GlutWindowImp/VistaGlutWindowingToolkit.h>
#endif
#ifdef VISTA_WITH_OSG
#include <VistaKernel/DisplayManager/OpenSceneGraphWindowImp/VistaOSGWindowingToolkit.h>
#endif

#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGTextForeground.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSolidBackground.h>
#include <OpenSG/OSGPassiveBackground.h>
#include <OpenSG/OSGFileGrabForeground.h>
#include <OpenSG/OSGStereoBufferViewport.h>
#include <OpenSG/OSGPerspectiveCamera.h>
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGDrawAction.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  DISPLAY SYSTEM HELPER CLASS IMPLEMENTATION                                */
/*============================================================================*/


// #############################################################################
// LOCAL CLASSES
// #############################################################################

// ##########################
// CDisplaySystemData
// ##########################


VistaOpenSGDisplayBridge::DisplaySystemData::DisplaySystemData()
: IVistaDisplayEntityData(),
m_v3LeftEyeOffset(),
m_v3RightEyeOffset(),
m_bLocalViewer(false),
m_bHMDModeActive(false)
{
	m_ptrPlatformBeacon = osg::makeCoredNode<osg::Transform>();
	osg::setName(m_ptrPlatformBeacon, "PlatformBeacon");

	m_ptrCameraBeacon = osg::makeCoredNode<osg::Transform>();
	osg::setName(m_ptrCameraBeacon, "CameraBeacon");
}

VistaOpenSGDisplayBridge::DisplaySystemData::~DisplaySystemData()
{
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetPlatformTransformation(
	const VistaTransformMatrix & matrix )
{
	osg::Matrix m;
	m[0][0] = matrix[0][0]; m[0][1] = matrix[1][0]; m[0][2] = matrix[2][0]; m[0][3] = matrix[3][0];
	m[1][0] = matrix[0][1]; m[1][1] = matrix[1][1]; m[1][2] = matrix[2][1]; m[1][3] = matrix[3][1];
	m[2][0] = matrix[0][2]; m[2][1] = matrix[1][2]; m[2][2] = matrix[2][2]; m[2][3] = matrix[3][2];
	m[3][0] = matrix[0][3]; m[3][1] = matrix[1][3]; m[3][2] = matrix[2][3]; m[3][3] = matrix[3][3];

	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP  (t, osg::Transform::MatrixFieldMask);

}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetPlatformTranslation( const VistaVector3D & v3Pos )
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
	beginEditCP(t, osg::Transform::MatrixFieldMask);
	osg::Pnt3f &trans = (osg::Pnt3f&) t->getMatrix() [3];
	trans[0] = v3Pos[0];
	trans[1] = v3Pos[1];
	trans[2] = v3Pos[2];
	endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaVector3D VistaOpenSGDisplayBridge::DisplaySystemData::GetPlatformTranslation() const
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
	osg::Pnt3f &trans = (osg::Pnt3f&) t->getMatrix() [3];

	return VistaVector3D(trans[0], trans[1], trans[2]);
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetPlatformOrientation( const VistaQuaternion & qOri )
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
	osg::Matrix m( t->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	VistaAxisAndAngle aaa = qOri.GetAxisAndAngle();
	ori = osg::Quaternion( osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]), aaa.m_fAngle );
	m.setTransform( trans, ori, scale, scale_ori );

	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaQuaternion VistaOpenSGDisplayBridge::DisplaySystemData::GetPlatformOrientation() const
{
	osg::Matrix m( osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore())->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	/** @todo: remove conversion to/from quaternion */
	float ax, ay, az, ang;
	ori.getValueAsAxisRad(ax,ay,az,ang);
	return VistaQuaternion( VistaAxisAndAngle(VistaVector3D(ax,ay,az), ang) );
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetCameraPlatformTransformation(
	const VistaTransformMatrix & matrix )
{
	osg::Matrix m;
	m[0][0] = matrix[0][0]; m[0][1] = matrix[1][0]; m[0][2] = matrix[2][0]; m[0][3] = matrix[3][0];
	m[1][0] = matrix[0][1]; m[1][1] = matrix[1][1]; m[1][2] = matrix[2][1]; m[1][3] = matrix[3][1];
	m[2][0] = matrix[0][2]; m[2][1] = matrix[1][2]; m[2][2] = matrix[2][2]; m[2][3] = matrix[3][2];
	m[3][0] = matrix[0][3]; m[3][1] = matrix[1][3]; m[3][2] = matrix[2][3]; m[3][3] = matrix[3][3];

	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());

	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP  (t, osg::Transform::MatrixFieldMask);
}


VistaTransformMatrix  VistaOpenSGDisplayBridge::DisplaySystemData::GetCameraPlatformTransformation() const
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
	osg::Matrix m( t->getMatrix() );
	VistaTransformMatrix oTransMat;
	oTransMat[0][0] = m[0][0]; oTransMat[0][1] = m[1][0]; oTransMat[0][2] = m[2][0]; oTransMat[0][3] = m[3][0];
	oTransMat[1][0] = m[0][1]; oTransMat[1][1] = m[1][1]; oTransMat[1][2] = m[2][1]; oTransMat[1][3] = m[3][1];
	oTransMat[2][0] = m[0][2]; oTransMat[2][1] = m[1][2]; oTransMat[2][2] = m[2][2]; oTransMat[2][3] = m[3][2];
	oTransMat[3][0] = m[0][3]; oTransMat[3][1] = m[1][3]; oTransMat[3][2] = m[2][3]; oTransMat[3][3] = m[3][3];

	return oTransMat;
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetCameraPlatformTranslation( const VistaVector3D & v3Pos )
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
	beginEditCP(t, osg::Transform::MatrixFieldMask);
	osg::Pnt3f &trans = (osg::Pnt3f&) t->getMatrix() [3];
	trans[0] = v3Pos[0];
	trans[1] = v3Pos[1];
	trans[2] = v3Pos[2];
	endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaVector3D VistaOpenSGDisplayBridge::DisplaySystemData::GetCameraPlatformTranslation() const
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
	osg::Pnt3f &trans = (osg::Pnt3f&) t->getMatrix() [3];

	return VistaVector3D( trans[0], trans[1], trans[2] );
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetCameraPlatformOrientation( const VistaQuaternion & qOri )
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
	osg::Matrix m( t->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	VistaAxisAndAngle aaa = qOri.GetAxisAndAngle();
	ori = osg::Quaternion( osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]), aaa.m_fAngle );
	m.setTransform( trans, ori, scale, scale_ori );

	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP  (t, osg::Transform::MatrixFieldMask);
}

VistaQuaternion VistaOpenSGDisplayBridge::DisplaySystemData::GetCameraPlatformOrientation() const
{
	osg::Matrix m( osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore())->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	float ax, ay, az, ang;
	ori.getValueAsAxisRad(ax,ay,az,ang);
	return VistaQuaternion( VistaAxisAndAngle(VistaVector3D(ax,ay,az), ang) );
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetHMDModeActive( const bool bSet )
{
	m_bHMDModeActive = bSet;
}

bool VistaOpenSGDisplayBridge::DisplaySystemData::GetHMDModeActive() const
{
	return m_bHMDModeActive;
}


VistaVector3D   VistaOpenSGDisplayBridge::DisplaySystemData::GetLeftEyeOffset()     const
{
	return m_v3LeftEyeOffset;
}

VistaVector3D   VistaOpenSGDisplayBridge::DisplaySystemData::GetRightEyeOffset()    const
{
	return m_v3RightEyeOffset;
}

// ##########################
// CDisplayData
// ##########################


VistaOpenSGDisplayBridge::DisplayData::DisplayData()
: m_sDisplayName("")
{
}

VistaOpenSGDisplayBridge::DisplayData::DisplayData( const string & sDisplayName )
: m_sDisplayName(sDisplayName)
{
}

std::string VistaOpenSGDisplayBridge::DisplayData::GetDisplayName() const
{
	return m_sDisplayName;
}

// ##########################
// CWindowData
// ##########################

// WindowData::WindowObserver;

class VistaOpenSGDisplayBridge::WindowData::WindowObserver : public IVistaObserver
{
public:
	WindowObserver( VistaWindow* pWindow, VistaOpenSGDisplayBridge* pBridge )
	: m_pWindow( pWindow )
	, m_pBridge( pBridge )
	{
		pWindow->GetWindowProperties()->AttachObserver( this );
	}
	~WindowObserver()
	{
		if( m_pWindow )
			m_pWindow->GetWindowProperties()->DetachObserver( this );
	}

	virtual bool Observes( IVistaObserveable* pObserveable ) 
	{
		if( m_pWindow == NULL )
			return false;
		return pObserveable == m_pWindow->GetWindowProperties();
	}
	virtual void Observe( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		// no thanks
	}
	virtual void ObserveableDelete( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		m_pWindow = NULL;
	}
	virtual void ObserverUpdate( IVistaObserveable* pObserveable, int nMsg, int nTicket ) 
	{
		if( nMsg == VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE )
			m_pBridge->OnWindowSizeUpdate( m_pWindow );
	}
	virtual void ReleaseObserveable( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		if( m_pWindow )
			m_pWindow->GetWindowProperties()->DetachObserver( this );
		m_pWindow = NULL;
	}

	virtual bool ObserveableDeleteRequest( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		return true;
	}
private:
	VistaWindow*				m_pWindow;
	 VistaOpenSGDisplayBridge*	m_pBridge;
};



VistaOpenSGDisplayBridge::WindowData::WindowData()
//: m_sTitle(""),
//m_bStereo(false),
//m_bStencilBufferEnabled( false ),
//m_bAccumBufferEnabled( false ),
//m_bDrawBorder(true),
//m_bFullScreen(false),
//m_ptrWindow(osg::NullFC),
//m_iOrigPosX( 0 ),
//m_iOrigPosY( 0 ),
//m_iOrigSizeX( 0 ),
//m_iOrigSizeY( 0 ),
//m_iCurrentSizeX(0),
//m_iCurrentSizeY(0)
: m_ptrWindow( osg::NullFC )
, m_pObserver( NULL )
, m_ptrFileGrabber( osg::NullFC )
, m_ptrFileGrabberImage( osg::NullFC )
{
}

VistaOpenSGDisplayBridge::WindowData::~WindowData()
{
	delete m_pObserver;
}

void VistaOpenSGDisplayBridge::WindowData::ObserveWindow( VistaWindow* pWindow,
													VistaOpenSGDisplayBridge* pBridge )
{
	if( m_pObserver )
		delete m_pObserver;
	m_pObserver = new WindowObserver( pWindow, pBridge );
}
//
//std::string VistaOpenSGDisplayBridge::WindowData::GetTitle() const
//{
//	return m_sTitle;
//}
//
//
//bool VistaOpenSGDisplayBridge::WindowData::GetDrawBorder() const
//{
//	return m_bDrawBorder;
//}
//
//bool VistaOpenSGDisplayBridge::WindowData::GetFullScreen() const
//{
//	return m_bFullScreen;
//}
//
//bool VistaOpenSGDisplayBridge::WindowData::GetStereo() const
//{
//	return m_bStereo;
//}
//
//bool VistaOpenSGDisplayBridge::WindowData::GetAccumBufferEnabled() const
//{
//	return m_bAccumBufferEnabled;
//}
//
//bool VistaOpenSGDisplayBridge::WindowData::GetStencilBufferEnabled() const
//{
//	return m_bStencilBufferEnabled;
//}
//

osg::WindowPtr VistaOpenSGDisplayBridge::WindowData::GetOpenSGWindow() const
{
	return m_ptrWindow;
}

osg::FileGrabForegroundPtr VistaOpenSGDisplayBridge::WindowData::GetFileGrabForeground()
{
	if( m_ptrFileGrabber == osg::NullFC )
	{
		// create a foreground with screenshot functionality
		m_ptrFileGrabber = osg::FileGrabForeground::create();

		// the FileGrabForeground has a bug, leaving the internal Image alive on deletion...
		// thus we handle the iamge buffer ourselfes
		m_ptrFileGrabberImage = osg::Image::create();
		beginEditCP( m_ptrFileGrabberImage );
			m_ptrFileGrabberImage->set( osg::Image::OSG_RGB_PF, 1 );
		endEditCP( m_ptrFileGrabberImage );

		beginEditCP( m_ptrFileGrabber, osg::FileGrabForeground::ImageFieldMask );
		{
			m_ptrFileGrabber->setImage( m_ptrFileGrabberImage );
		}
		endEditCP( m_ptrFileGrabber, osg::FileGrabForeground::ImageFieldMask );

		
		// add the grabber to the next best viewport
		m_ptrWindow->getPort(0)->getMFForegrounds()->push_back( m_ptrFileGrabber );
	}

	return m_ptrFileGrabber;
}

// ##########################
// CViewportData
// ##########################

VistaOpenSGDisplayBridge::ViewportData::ViewportData()
: IVistaDisplayEntityData(),
m_Viewport(osg::NullFC),
m_RightViewport(osg::NullFC),
m_TextForeground(osg::NullFC),
m_pOverlays(osg::NullFC),
m_oBitmaps(osg::NullFC),
m_pSolidBackground(osg::NullFC),
m_pPassiveBackground(osg::NullFC),
m_bHasPassiveBackground(false)
{
}

VistaOpenSGDisplayBridge::ViewportData::~ViewportData()
{
	if(m_Viewport      != osg::NullFC)
		subRefCP(m_Viewport);
	if(m_RightViewport != osg::NullFC)
		subRefCP(m_RightViewport);

	if(m_pOverlays != osg::NullFC)
		subRefCP(m_pOverlays);

	if(m_TextForeground != osg::NullFC)
		subRefCP(m_TextForeground);

	if(m_oBitmaps != osg::NullFC)
		subRefCP(m_oBitmaps);

	if(m_pSolidBackground != osg::NullFC)
		subRefCP(m_pSolidBackground);

	if(m_pPassiveBackground != osg::NullFC)
		subRefCP(m_pPassiveBackground);
}

bool VistaOpenSGDisplayBridge::ViewportData::GetStereo() const
{
	return m_bStereo;
}

osg::ViewportPtr VistaOpenSGDisplayBridge::ViewportData::GetOpenSGViewport() const
{
	return m_Viewport;
}

osg::ViewportPtr VistaOpenSGDisplayBridge::ViewportData::GetOpenSGRightViewport() const
{
	return m_RightViewport;
}

osg::VistaOpenSGTextForegroundPtr VistaOpenSGDisplayBridge::ViewportData::GetTextForeground() const
{
	return m_TextForeground ;
}

osg::VistaOpenSGGLOverlayForegroundPtr VistaOpenSGDisplayBridge::ViewportData::GetOverlayForeground() const
{
	return m_pOverlays;
}

osg::SolidBackgroundPtr VistaOpenSGDisplayBridge::ViewportData::GetSolidBackground() const
{
	return m_pSolidBackground;
}

osg::PassiveBackgroundPtr VistaOpenSGDisplayBridge::ViewportData::GetPassiveBackground() const
{
	return m_pPassiveBackground;
}


void VistaOpenSGDisplayBridge::ViewportData::ReplaceViewport(
									osg::ViewportPtr pNewPort, bool bCopyData,	 bool bRight )
{
	osg::ViewportPtr pOldPort;
	if(bRight)
	{
		pOldPort = m_RightViewport;
		m_RightViewport = pNewPort;
	}
	else
	{
		pOldPort = m_Viewport;
		m_Viewport = pNewPort;
	}

	if(bCopyData)
	{
		beginEditCP(pNewPort);
		{
			pNewPort->setLeft(pOldPort->getLeft());
			pNewPort->setBottom(pOldPort->getBottom());
			pNewPort->setRight(pOldPort->getRight());
			pNewPort->setTop(pOldPort->getTop());
			pNewPort->setBackground(pOldPort->getBackground());
			pNewPort->setCamera(pOldPort->getCamera());
			pNewPort->setRoot(pOldPort->getRoot());
			for(osg::UInt32 i = 0; i < pOldPort->getForegrounds().size(); ++i)
			{
				pNewPort->getForegrounds().push_back(pOldPort->getForegrounds()[i]);
			}
		}
		endEditCP(pNewPort);
	}

	osg::WindowPtr pParent = pOldPort->getParent();
	beginEditCP(pParent, osg::Window::PortFieldMask);
	pParent->replacePortBy(pOldPort, pNewPort);
	endEditCP(pParent, osg::Window::PortFieldMask);

	// release old port
	subRefCP(pOldPort);
}


// ##########################
// CProjectionData
// ##########################

VistaOpenSGDisplayBridge::ProjectionData::ProjectionData()
	: m_dLeft(-1), m_dRight(1), m_dBottom(-1), m_dTop(1)
	, m_iStereoMode(VistaProjection::VistaProjectionProperties::SM_MONO)
	, m_v3NormalVector(0,0,1), m_v3UpVector(0,1,0)
{
	m_ptrBeacon = osg::makeCoredNode<osg::Transform>();
	setName(m_ptrBeacon, "Beacon");

	m_ptrEyeBeacon = osg::makeCoredNode<osg::Transform>();
	setName(m_ptrEyeBeacon, "EyeBeacon");

	m_ptrRightEyeBeacon = osg::makeCoredNode<osg::Transform>();
	setName(m_ptrRightEyeBeacon, "RightEyeBeacon");

	m_ptrCamDeco      = osg::ProjectionCameraDecorator::create();
	m_ptrRightCamDeco = osg::ProjectionCameraDecorator::create();
}

VistaOpenSGDisplayBridge::ProjectionData::~ProjectionData()
{
}


VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetLeftEye() const
{
	return m_v3LeftEye;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetRightEye() const
{
	return m_v3RightEye;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetMidPoint() const
{
	return m_v3MidPoint;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetNormalVector() const
{
	return m_v3NormalVector;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetUpVector() const
{
	return m_v3UpVector;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetLeft() const
{
	return m_dLeft;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetRight() const
{
	return m_dRight;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetBottom() const
{
	return m_dBottom;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetTop() const
{
	return m_dTop;
}

int VistaOpenSGDisplayBridge::ProjectionData::GetStereoMode() const
{
	return m_iStereoMode;
}
osg::ProjectionCameraDecoratorPtr VistaOpenSGDisplayBridge::ProjectionData::GetCamera() const
{
	return m_ptrCamDeco;
}


void VistaOpenSGDisplayBridge::ProjectionData::SetCameraTransformation( 
							const VistaVector3D & v3CamPos, const VistaQuaternion & qCamOri )
{
	VistaTransformMatrix matrix(qCamOri, v3CamPos);


	osg::Matrix m;
	m[0][0] = matrix[0][0]; m[0][1] = matrix[1][0]; m[0][2] = matrix[2][0]; m[0][3] = matrix[3][0];
	m[1][0] = matrix[0][1]; m[1][1] = matrix[1][1]; m[1][2] = matrix[2][1]; m[1][3] = matrix[3][1];
	m[2][0] = matrix[0][2]; m[2][1] = matrix[1][2]; m[2][2] = matrix[2][2]; m[2][3] = matrix[3][2];
	m[3][0] = matrix[0][3]; m[3][1] = matrix[1][3]; m[3][2] = matrix[2][3]; m[3][3] = matrix[3][3];

	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP(t, osg::Transform::MatrixFieldMask);

	//osg::Vec3f trans, scale;
	//osg::Quaternion ori, scale_ori;
	//m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	//trans = osg::Vec3f( v3CamPos[0], v3CamPos[1], v3CamPos[2] );
	//VistaAxisAndAngle aaa = qCamOri.GetAxisAndAngle();
	//ori = osg::Quaternion( osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]), aaa.m_fAngle );
	//m.setTransform( trans, ori, scale, scale_ori );

	//osg::TransformPtr t = osg::Transform::create();
	//beginEditCP(t);
	//t->setMatrix(m);
	//endEditCP(t);

	//beginEditCP(m_Beacon, osg::Node::CoreFieldMask );
	//m_Beacon->setCore(t);
	//endEditCP(m_Beacon, osg::Node::CoreFieldMask );
}

void VistaOpenSGDisplayBridge::ProjectionData::GetCameraTransformation(
							VistaVector3D & v3CamPos, VistaQuaternion & qCamOri ) const
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
	osg::Matrix m( t->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	/// @todo use the faster householder decomposition?
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	v3CamPos[0] = trans[0];
	v3CamPos[1] = trans[1];
	v3CamPos[2] = trans[2];

	float ax, ay, az, ang;
	ori.getValueAsAxisRad(ax,ay,az,ang);
	qCamOri = VistaQuaternion( VistaAxisAndAngle(VistaVector3D(ax,ay,az), ang) );
}

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraTranslation(
							const VistaVector3D & v3CamPos )
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
	osg::Matrix m( t->getMatrix() );

	m.setTranslate(v3CamPos[0], v3CamPos[1], v3CamPos[2]);

	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetCameraTranslation() const
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
	osg::Pnt3f &p = (osg::Pnt3f&) t->getMatrix() [3];
	return VistaVector3D(p[0], p[1], p[2]);
}

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraOrientation(
							const VistaQuaternion & qCamOri )
{
	osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());

	osg::Matrix m( t->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	VistaAxisAndAngle aaa = qCamOri.GetAxisAndAngle();
	ori = osg::Quaternion( osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]), aaa.m_fAngle );
	m.setTransform( trans, ori, scale, scale_ori );

	beginEditCP(t, osg::Transform::MatrixFieldMask);
	t->setMatrix(m);
	endEditCP(t, osg::Transform::MatrixFieldMask);

	//osg::Matrix invmat;
	//invmat.setRotate( ori );
	//invmat.invert();

	//osg::TransformPtr eye = osg::TransformPtr::dcast(m_ptrEyeBeacon->getCore());
	//beginEditCP(eye, osg::Transform::MatrixFieldMask);
	//eye->setMatrix(invmat);
	//endEditCP(eye, osg::Transform::MatrixFieldMask);

	//if( m_ptrRightCamDeco )
	//{
	//	osg::TransformPtr righteye = osg::TransformPtr::dcast(m_ptrRightEyeBeacon->getCore());
	//	beginEditCP(righteye, osg::Transform::MatrixFieldMask);
	//	righteye->setMatrix(invmat);
	//	endEditCP(righteye, osg::Transform::MatrixFieldMask);
	//}

}

VistaQuaternion VistaOpenSGDisplayBridge::ProjectionData::GetCameraOrientation() const
{
	osg::Matrix m( osg::TransformPtr::dcast(m_ptrBeacon->getCore())->getMatrix() );
	osg::Vec3f trans, scale;
	osg::Quaternion ori, scale_ori;
	m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
	float ax, ay, az, ang;
	ori.getValueAsAxisRad(ax,ay,az,ang);
	return VistaQuaternion( VistaAxisAndAngle(VistaVector3D(ax,ay,az), ang) );
}

void VistaOpenSGDisplayBridge::ProjectionData::SetProjectionPlane()
{
	m_v3UpVector.Normalize();
	m_v3NormalVector.Normalize();
	VistaVector3D v3Right = m_v3UpVector.Cross(m_v3NormalVector);

	// compute corners of projection plane in WCS
	VistaVector3D v3Edges[4] = {
		m_v3MidPoint + m_v3UpVector * (float)m_dBottom + v3Right * (float)m_dLeft  ,  // bottom left
		m_v3MidPoint + m_v3UpVector * (float)m_dBottom + v3Right * (float)m_dRight ,  // bottom right
		m_v3MidPoint + m_v3UpVector * (float)m_dTop    + v3Right * (float)m_dRight ,  // top right
		m_v3MidPoint + m_v3UpVector * (float)m_dTop    + v3Right * (float)m_dLeft  }; // top left

		// transfer to OpenSG
		osg::Pnt3f pProjPlaneEdges[4];
		for(int i=0; i<4; ++i)
			for(int j=0; j<3; ++j)
				pProjPlaneEdges[i][j] = v3Edges[i][j];

		beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);
		m_ptrCamDeco->getSurface().clear();
		// push the points defining the projection surface's edges
		// MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
		m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[0]);
		m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[1]);
		m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[2]);
		m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[3]);
		endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);

		beginEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);
		m_ptrRightCamDeco->getSurface().clear();
		// push the points defining the projection surface's edges
		// MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
		m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[0]);
		m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[1]);
		m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[2]);
		m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[3]);
		endEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);
}

void VistaOpenSGDisplayBridge::ProjectionData::SetEyes
( const VistaVector3D & v3LeftOffset, const VistaVector3D & v3RightOffset )
{
	m_v3LeftEye  = v3LeftOffset;
	m_v3RightEye = v3RightOffset;

	switch( m_iStereoMode )
	{
	case VistaProjection::VistaProjectionProperties::SM_MONO:
		{
			beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
			m_ptrCamDeco->setUser(m_ptrEyeBeacon);
			endEditCP  (m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
		}
		break;
	case VistaProjection::VistaProjectionProperties::SM_LEFT_EYE:
		{
			osg::Matrix m = osg::Matrix::identity();
			m.setTranslate(m_v3LeftEye[0],m_v3LeftEye[1],m_v3LeftEye[2]);

			osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrEyeBeacon->getCore());
			// this should never happen, well...
			if(t == osg::NullFC)
			{
				t = osg::Transform::create();
				beginEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);
				m_ptrEyeBeacon->setCore(t);
				endEditCP  (m_ptrEyeBeacon, osg::Node::CoreFieldMask);
			}

			beginEditCP(t, osg::Transform::MatrixFieldMask);
			t->setMatrix(m);
			endEditCP  (t, osg::Transform::MatrixFieldMask);

			beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
			m_ptrCamDeco->setUser(m_ptrEyeBeacon);
			endEditCP  (m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
		}
		break;
	case VistaProjection::VistaProjectionProperties::SM_RIGHT_EYE:
		{
			osg::Matrix m = osg::Matrix::identity();
			m.setTranslate(m_v3RightEye[0],m_v3RightEye[1],m_v3RightEye[2]);

			osg::TransformPtr t = osg::Transform::create();
			beginEditCP(t, osg::Transform::MatrixFieldMask);
			t->setMatrix(m);
			endEditCP  (t, osg::Transform::MatrixFieldMask);

			beginEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);
			m_ptrEyeBeacon->setCore(t);
			endEditCP  (m_ptrEyeBeacon, osg::Node::CoreFieldMask);

			beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
			m_ptrCamDeco->setUser(m_ptrEyeBeacon);
			endEditCP  (m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
		}
		break;
	case VistaProjection::VistaProjectionProperties::SM_FULL_STEREO:
		{
			// left eye
			osg::Matrix m = osg::Matrix::identity();
			m.setTranslate(m_v3LeftEye[0],m_v3LeftEye[1],m_v3LeftEye[2]);
			osg::TransformPtr t = osg::Transform::create();

			beginEditCP(t, osg::Transform::MatrixFieldMask);
			t->setMatrix(m);
			endEditCP  (t, osg::Transform::MatrixFieldMask);

			beginEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);
			m_ptrEyeBeacon->setCore(t);
			endEditCP  (m_ptrEyeBeacon, osg::Node::CoreFieldMask);

			beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
			m_ptrCamDeco->setUser(m_ptrEyeBeacon);
			endEditCP  (m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);

			// right eye
			osg::Matrix mr = osg::Matrix::identity();
			mr.setTranslate(m_v3RightEye[0],m_v3RightEye[1],m_v3RightEye[2]);
			osg::TransformPtr tr = osg::Transform::create();

			beginEditCP(tr, osg::Transform::MatrixFieldMask);
			tr->setMatrix(mr);
			endEditCP  (tr, osg::Transform::MatrixFieldMask);

			beginEditCP(m_ptrRightEyeBeacon, osg::Node::CoreFieldMask);
			m_ptrRightEyeBeacon->setCore(tr);
			endEditCP  (m_ptrRightEyeBeacon, osg::Node::CoreFieldMask);

			beginEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
			m_ptrRightCamDeco->setUser(m_ptrRightEyeBeacon);
			endEditCP  (m_ptrRightCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);

			// av006ce: since the ProjectionCameraDecorator
			//          inherits from the StereoCameraDecorator we
			//          could also use the following code instead
			//          of the EyeBeacons. But we would be restricted
			//          to keep the users eyes on the X-axis (literally)
			// pData->m_Camera->setEyeSeparation( -0.03 );
			// pData->m_Camera->setLeftEye( true  );
			// pData->m_RightCamera->setEyeSeparation(  0.03 );
			// pData->m_RightCamera->setLeftEye( false );
		}
		break;
	default:
		{
			vstr::warnp() << "Unknown stereo mode -> defaulting to mono mode" << std::endl;
			beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
			m_ptrCamDeco->setUser(m_ptrBeacon);
			endEditCP  (m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
		}
		break;
	}
}


// ##########################
// VistaOpenSG2DBitmap
// ##########################


VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::VistaOpenSG2DBitmap(IVistaDisplayBridge *pBr,
																	  const std::string &sVpName)
																	  : Vista2DBitmap(pBr),
																	  m_sVpName(sVpName),
																	  m_oImage(osg::NullFC)
{
}

VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::~VistaOpenSG2DBitmap()
{
	if(m_oImage != osg::NullFC)
		subRefCP(m_oImage);
}

bool VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::SetBitmap(const std::string &strNewFName)
{
	osg::ImagePtr oImage = osg::Image::create();
	addRefCP(oImage);

	std::string sF =
		VistaFileSystemDirectory::GetCurrentWorkingDirectory()
		+ VistaFileSystemDirectory::GetOSSpecificSeparator()
		+ strNewFName;

	if(oImage->read(sF.c_str()) == true)
	{
		VistaOpenSGDisplayBridge *pBr = static_cast<VistaOpenSGDisplayBridge*>(m_pDispBridge);
		if(pBr->ReplaceForegroundImage(m_sVpName, m_oImage, oImage, m_fXPos, m_fYPos) == true)
		{
			subRefCP(m_oImage);
			m_oImage = oImage;
		}
		else
			subRefCP(oImage); // kill
		return true;
	}
	else
		subRefCP(oImage); // kill

	return false;
}


bool VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::GetDimensions(int & nWidth, int & nHeight)
{
	if(m_oImage != osg::NullFC)
	{
		nWidth = int(m_oImage->getWidth());
		nHeight = int(m_oImage->getHeight());
		return true;
	}
	return false;
}

bool VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::SetPosition(float fPosX, float fPosY)
{
	// set position in members
	Vista2DDrawingObject::SetPosition(fPosX, fPosY);

	VistaOpenSGDisplayBridge *pBr = static_cast<VistaOpenSGDisplayBridge*>(m_pDispBridge);

	// refcnt the image so it will not get deleted
	addRefCP(m_oImage);
	bool bRet = pBr->ReplaceForegroundImage(m_sVpName, m_oImage, m_oImage, m_fXPos, m_fYPos);
	// remove damgling refcount, should be ok
	subRefCP(m_oImage);

	return bRet;
}


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaOpenSGDisplayBridge::VistaOpenSGDisplayBridge(	osg::RenderAction *pRenderAction,
														osg::NodePtr pRealRoot)
: m_bShowCursor(true)
, m_pWindowingToolkit( NULL )
, m_pDisplayManager( NULL )
{
	m_pRealRoot = pRealRoot;
	if(m_pRealRoot != osg::NullFC)
		addRefCP(m_pRealRoot);


	m_pRenderAction = pRenderAction;
}

VistaOpenSGDisplayBridge::~VistaOpenSGDisplayBridge()
{
	delete m_pWindowingToolkit;

	subRefCP(m_pRealRoot);
}


void VistaOpenSGDisplayBridge::SetDisplayManager(VistaDisplayManager *pDisplayManager)
{
	m_pDisplayManager = pDisplayManager;
}

VistaDisplayManager *VistaOpenSGDisplayBridge::GetDisplayManager() const
{
	return m_pDisplayManager;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaOpenSGDisplayBridge::DrawFrame()
{
	// render to all windows in the display manager
	const std::map<std::string, VistaWindow*>& mapWindows = m_pDisplayManager->GetWindowsConstRef();
	std::map<std::string, VistaWindow*>::const_reverse_iterator itWindow = mapWindows.rbegin();
	for( ; itWindow != mapWindows.rend(); ++itWindow )
	{
		m_pWindowingToolkit->BindWindow( (*itWindow).second );
		WindowData* pData = static_cast<WindowData*>( (*itWindow).second->GetData() );
		pData->m_ptrWindow->render(m_pRenderAction);
		m_pWindowingToolkit->UnbindWindow( (*itWindow).second );
	}

	// after rendering, we have to deactivate all filegrab foregrounds that
	// listed for screenshot during last frame
	for( std::vector<WindowData*>::iterator itWin = m_vecFileGrabWindows.begin();
			itWin != m_vecFileGrabWindows.end(); ++itWin )
	{
		osg::FileGrabForegroundPtr ptrGrabber = (*itWin)->GetFileGrabForeground();

		beginEditCP( ptrGrabber, osg::FileGrabForeground::ActiveFieldMask );
		{
			ptrGrabber->setActive( false );
		}
		endEditCP( ptrGrabber, osg::FileGrabForeground::ActiveFieldMask );

	}
	
	return true;
}

bool VistaOpenSGDisplayBridge::DisplayFrame()
{
	// swap all windows in the display manager
	m_pWindowingToolkit->DisplayAllWindows();
	return true;
}

bool VistaOpenSGDisplayBridge::SetDisplayUpdateCallback( IVistaExplicitCallbackInterface* pCallback )
{
	m_pWindowingToolkit->SetWindowUpdateCallback( pCallback );
	return true;
}


bool VistaOpenSGDisplayBridge::UpdateDisplaySystems( VistaDisplayManager* pDisplayManager )
{
	return true;//the OpenSG implementation doesn't need any DisplaySystem updates
}

void VistaOpenSGDisplayBridge::UpdateDisplaySystem( VistaDisplaySystem* pDisplaySystem )
{
	//the OpenSG implementation doesn't need any DisplaySystem updates
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserverUpdate                                              */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::ObserverUpdate( IVistaObserveable *pObserveable,
								int msg, int ticket, VistaDisplaySystem *pTarget)
{
	if (ticket == VistaDisplaySystem::FRAME_CHANGE)
	{
		((DisplaySystemData*)pTarget->GetData())->SetPlatformTransformation( pTarget->GetReferenceFrame()->GetMatrix() );

		if( !pTarget->GetDisplaySystemProperties()->GetLocalViewer() )
		{
			switch(msg)
			{
			case VistaVirtualPlatform::MSG_TRANSLATION_CHANGE:
				pTarget->GetDisplaySystemProperties()->Notify(
					VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_POSITION_CHANGE
					);
				break;
			case VistaVirtualPlatform::MSG_ROTATION_CHANGE:
				pTarget->GetDisplaySystemProperties()->Notify(
					VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_ORIENTATION_CHANGE
					);
				break;

			default:
				break;
			}
		}
	}
}

VistaDisplaySystem * VistaOpenSGDisplayBridge::CreateDisplaySystem(
								VistaVirtualPlatform * pReferenceFrame,
								VistaDisplayManager * pDisplayManager,
								const VistaPropertyList & refProps )
{
	// create new data container
	DisplaySystemData * pData = new DisplaySystemData;

	// get the real OpenSG root node to attach display system beacons to it
	// this means that display systems and cameras will be hidden for pure ViSTA applications

	beginEditCP(m_pRealRoot);
	m_pRealRoot->addChild(pData->m_ptrPlatformBeacon); // append beacon to root node
	endEditCP(m_pRealRoot);

	beginEditCP(pData->m_ptrPlatformBeacon);
	pData->m_ptrPlatformBeacon->addChild(pData->m_ptrCameraBeacon);
	endEditCP(pData->m_ptrPlatformBeacon);

	VistaDisplaySystem * pDisplaySystem = NewDisplaySystem(pReferenceFrame, pData, pDisplayManager);

	if( !pDisplaySystem )
	{
		vstr::errp() << " [VistaOpenSGDisplayBridge] - Unable to create display system" << std::endl;
		delete pData;
		return NULL;
	}

	pDisplaySystem->Observe(pReferenceFrame, VistaDisplaySystem::FRAME_CHANGE);
	pDisplaySystem->GetProperties()->SetPropertiesByList(refProps);

	// signal one platform update before the first draw is called
	pDisplaySystem->ObserverUpdate( pDisplaySystem->GetReferenceFrame(), 0, VistaDisplaySystem::FRAME_CHANGE );

	return pDisplaySystem;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyDisplaySystem                                        */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyDisplaySystem( VistaDisplaySystem * pDisplaySystem )
{
	if (!pDisplaySystem)
		return true;

	// release reference frame
	VistaVirtualPlatform * pReferenceFrame = pDisplaySystem->GetReferenceFrame();
	pDisplaySystem->ReleaseObserveable(pReferenceFrame, IVistaObserveable::TICKET_NONE);

	// destroy containers
	DisplaySystemData * pData = (DisplaySystemData *)pDisplaySystem->GetData();

	beginEditCP(pData->m_ptrPlatformBeacon);
	pData->m_ptrPlatformBeacon->subChild(pData->m_ptrCameraBeacon); // remove beacon from root node
	endEditCP(pData->m_ptrPlatformBeacon);

	beginEditCP(m_pRealRoot);
	m_pRealRoot->subChild(pData->m_ptrPlatformBeacon); // remove beacon from root node
	endEditCP(m_pRealRoot);

	delete pData;
	delete pDisplaySystem;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugDisplaySystem                                          */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugDisplaySystem( std::ostream & out,
												  VistaDisplaySystem * pTarget)
{
	pTarget->Debug(out);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetViewerPosition                                       */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewerPosition( const VistaVector3D & v3Pos,
												 VistaDisplaySystem* pTarget)
{
	if (!pTarget)
		return;

	DisplaySystemData * pData = (DisplaySystemData *)pTarget->GetData();

	if( pData->GetHMDModeActive() )
	{
		pData->SetCameraPlatformTranslation( v3Pos );
	}
	else
	{
		int i, iViewportCount = pTarget->GetNumberOfViewports();
		if( iViewportCount == 0 )
		{
			// we are in the initialization phase of ViSTA
			// so we store the initial viewer position for the creation
			// of subsequent projections in this display system
			pData->m_v3InitialViewerPosition = v3Pos;
		}
		else
		{
			VistaVector3D v3Tmp(v3Pos);
			if( pData->m_bLocalViewer )
			{
				//pData->SetPlatformTranslation( v3Pos );
				v3Tmp = pTarget->GetReferenceFrame()->TransformPositionToFrame( v3Pos );
			}

			// well, let's iterate over all viewports/projections
			for( i = 0; i < iViewportCount; ++i )
			{
				ProjectionData * pPData = (ProjectionData *)pTarget->GetViewport(i)->GetProjection()->GetData();
				pPData->SetCameraTranslation( v3Tmp );
			}
		}
	}
}

void VistaOpenSGDisplayBridge::GetViewerPosition( VistaVector3D & v3Pos,
												 const VistaDisplaySystem * pTarget)
{
	if (!pTarget)
		return;

	DisplaySystemData * pData = (DisplaySystemData*)pTarget->GetData();

	if( pData->GetHMDModeActive() )
	{
		// Currently: Ignores Viewer Position dada in Projection::m_Beacon
		// Should be Unity/Zero
// 		if( true )
// 		{
		v3Pos = pData->GetCameraPlatformTranslation();
// 		}
// 		else
// 		{
// 			if( pTarget->GetNumberOfViewports() > 0 )
// 			{
// 				CProjectionData * pPData = (CProjectionData *)pTarget->GetViewport(0)->GetProjection()->GetData();
// 				v3Pos = pPData->GetCameraTranslation();
// 				v3Pos = pData->GetCameraPlatformTransformation() * v3Pos;
// 				if( !pData->m_bLocalViewer )
// 				{
// 					v3Pos = pTarget->GetReferenceFrame()->TransformPositionFromFrame(v3Pos);
// 				}
// 			}
// 		}
		if( !pData->m_bLocalViewer )
			v3Pos = pTarget->GetReferenceFrame()->TransformPositionFromFrame(v3Pos);
	}
	else
	{
		//CProjectionData * pPData = (CProjectionData *)pTarget->GetViewport(0)->GetProjection()->GetData();

		ProjectionData * pPData = NULL;
		VistaViewport * pViewport = pTarget->GetViewport(0);
		if(!pViewport) return;
		pPData = (ProjectionData *)pViewport->GetProjection()->GetData();


		// take the first viewports/projections position
		if( pTarget->GetNumberOfViewports() > 0 )
		{
			//if( pData->m_bLocalViewer )
			//	v3Pos = pData->GetPlatformTranslation();
			//else

			v3Pos = pPData->GetCameraTranslation();
			if( !pData->m_bLocalViewer )
				v3Pos = pTarget->GetReferenceFrame()->TransformPositionFromFrame(v3Pos);
		}
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetViewerOrientation                                    */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewerOrientation(const VistaQuaternion & qOrient,
													VistaDisplaySystem * pTarget)
{
	if (!pTarget)
		return;

	DisplaySystemData * pData = (DisplaySystemData*)pTarget->GetData();

	if( pData->GetHMDModeActive() )
	{
		pData->SetCameraPlatformOrientation( qOrient );
	}
	else
	{
		int i, iViewportCount = pTarget->GetNumberOfViewports();
		if( iViewportCount == 0 )
		{
			// we are in the initialization phase of ViSTA
			// so we store the initial viewer orientation for the creation
			// of subsequent projections in this display system
			pData->m_qInitialViewerOrientation = qOrient;
		}
		else
		{
			VistaQuaternion q(qOrient);

			if( pData->m_bLocalViewer )
			{
				q = pTarget->GetReferenceFrame()->TransformOrientationToFrame(qOrient);
			}
			// well, let's iterate over all viewports/projections
			for( i = 0; i < iViewportCount; ++i )
			{
				ProjectionData * pPData = (ProjectionData *)pTarget->GetViewport(i)->GetProjection()->GetData();
				pPData->SetCameraOrientation( qOrient );
			}
		}
	}
}

void VistaOpenSGDisplayBridge::GetViewerOrientation(VistaQuaternion & qOrient,
													const VistaDisplaySystem * pTarget)
{
	if (!pTarget)
		return;

	DisplaySystemData * pData = (DisplaySystemData*)pTarget->GetData();

	if( pData->GetHMDModeActive() )
	{
		// Currently: Ignores Viewer Position dada in Projection::m_Beacon
		// Should be Unity/Zero
// 		if( true )
// 		{
		qOrient = pData->GetCameraPlatformOrientation();
// 		}
// 		else
// 		{
// 			if( pTarget->GetNumberOfViewports() > 0 )
// 			{
// 				CProjectionData * pPData = (CProjectionData *)pTarget->GetViewport(0)->GetProjection()->GetData();
// 				qOrient = pPData->GetCameraOrientation();
// 				qOrient = pData->GetCameraPlatformTransformation() * qOrient;
// 				if( !pData->m_bLocalViewer )
// 				{
// 					qOrient = pTarget->GetReferenceFrame()->TransformOrientationFromFrame(qOrient);
// 				}
// 			}
// 		}
		if( !pData->m_bLocalViewer )
		{
			qOrient = pTarget->GetReferenceFrame()->TransformOrientationFromFrame(qOrient);
		}
	}
	else
	{
		// take the first viewports/projections orientation
		if( pTarget->GetNumberOfViewports() > 0 )
		{
			ProjectionData * pPData = (ProjectionData *)pTarget->GetViewport(0)->GetProjection()->GetData();
			qOrient = pPData->GetCameraOrientation();
			// in case we are not a local viewer, we transform the orientation
			// back to the world reference frame using the reference frame
			// otherwise the camera orientation is about to be enough
			if( !pData->m_bLocalViewer )
			{
				qOrient = pTarget->GetReferenceFrame()->TransformOrientationFromFrame(qOrient);
			}
		}
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetEyeOffsets                                           */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetEyeOffsets( const VistaVector3D & v3LeftOffset,
											 const VistaVector3D & v3RightOffset,
 VistaDisplaySystem * pTarget)
{
	DisplaySystemData * pData = (DisplaySystemData*)pTarget->GetData();
	pData->m_v3LeftEyeOffset  = v3LeftOffset;
	pData->m_v3RightEyeOffset = v3RightOffset;

	int i, iViewportCount = pTarget->GetNumberOfViewports();
	// well, let's iterate over all viewports/projections
	for( i = 0; i < iViewportCount; ++i )
	{
		ProjectionData * pPData = (ProjectionData *)pTarget->GetViewport(i)->GetProjection()->GetData();
		pPData->SetEyes( v3LeftOffset, v3RightOffset );
	}
}

void VistaOpenSGDisplayBridge::GetEyeOffsets
( VistaVector3D & v3LeftOffset, VistaVector3D & v3RightOffset,
 const VistaDisplaySystem * pTarget)
{
	if (!pTarget)
		return;

	DisplaySystemData *pData = (DisplaySystemData *)pTarget->GetData();
	v3LeftOffset  = pData->m_v3LeftEyeOffset;
	v3RightOffset = pData->m_v3RightEyeOffset;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetLocalViewer                                          */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetLocalViewer
( bool bLocalViewer, VistaDisplaySystem * pTarget )
{
	DisplaySystemData *pData = (DisplaySystemData *)pTarget->GetData();
	pData->m_bLocalViewer = bLocalViewer;
}

bool VistaOpenSGDisplayBridge::GetLocalViewer( const VistaDisplaySystem * pTarget )
{
	DisplaySystemData *pData = (DisplaySystemData *)pTarget->GetData();
	return pData->m_bLocalViewer;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetHMDModeActive                                        */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetHMDModeActive
( bool bSet, VistaDisplaySystem * pTarget )
{
	DisplaySystemData *pData = (DisplaySystemData *)pTarget->GetData();
	pData->m_bHMDModeActive = bSet;
}

bool VistaOpenSGDisplayBridge::GetHMDModeActive( const VistaDisplaySystem * pTarget )
{
	DisplaySystemData *pData = (DisplaySystemData *)pTarget->GetData();
	return pData->m_bHMDModeActive;
}

/**
* Methods for display management.
*/
VistaDisplay * VistaOpenSGDisplayBridge::CreateDisplay
( VistaDisplayManager * pDisplayManager, const VistaPropertyList & refProps )
{
	if (!refProps.HasProperty("DISPLAY_STRING"))
	{
		vstr::errp() << " [VistaOpenSGDisplayBridge - Unable to create display - no display string given" << std::endl;
		return NULL;
	}

	string strName;
	refProps.GetValue( "DISPLAY_STRING", strName );

	// create new data container
	DisplayData *pData = new DisplayData;

	pData->m_sDisplayName = strName;

	VistaDisplay *pDisplay = NewDisplay(pData, pDisplayManager);
	if (!pDisplay)
	{
		vstr::errp() << " [VistaOpenSGDisplayBridge] - unable to create display" << std::endl;
		delete pData;
		return NULL;
	}
	//pDisplay->SetPropertiesByList(refProps);
	pDisplay->GetProperties()->SetPropertiesByList(refProps);

	return pDisplay;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyDisplay                                              */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyDisplay( VistaDisplay * pDisplay )
{
	if (!pDisplay)
		return true;

	// destroy the data container but don't change the display strings
	// in order to do so, we would need some form of reference counting...
	IVistaDisplayEntityData * pData = pDisplay->GetData();

	delete pData;
	delete pDisplay;

	return true;
}

void VistaOpenSGDisplayBridge::DebugDisplay( std::ostream & out, const VistaDisplay * pTarget)
{
	pTarget->Debug(out);
}

/**
* Methods for window management.
*/
IVistaWindowingToolkit* VistaOpenSGDisplayBridge::GetWindowingToolkit() const
{
	return m_pWindowingToolkit;
}

IVistaWindowingToolkit* VistaOpenSGDisplayBridge::CreateWindowingToolkit( const std::string& sName )
{
	if( m_pWindowingToolkit != NULL )
	{
		vstr::warnp() << "Creating new WIndowingtoolkit, but one already"
				<< " exists - deleting old one!" << std::endl;
		delete m_pWindowingToolkit;
		m_pWindowingToolkit = NULL;
	}

#ifdef VISTA_WITH_GLUT
	if( VistaAspectsComparisonStuff::StringEquals( sName, "GLUT", false ) )
	{
		// compare the string once and store the result as enum
		m_pWindowingToolkit = new VistaGlutWindowingToolkit();
		return m_pWindowingToolkit;
	}
#endif
#ifdef VISTA_WITH_OSG
	if( VistaAspectsComparisonStuff::StringEquals( sName, "OSG", false ) )
	{
		// compare the string once and store the result as enum
		m_pWindowingToolkit = new VistaOSGWindowingToolkit();
		return m_pWindowingToolkit;
	}
#endif

	
	vstr::errp() << "[VistaOpenSGSystemClassFactory::CreateWindowingToolkit] " 
			<< "Toolkit type [" << sName << "] is unknown." << std::endl;
	return NULL;
}

VistaWindow* VistaOpenSGDisplayBridge::CreateVistaWindow( VistaDisplay* pDisplay,
														 const VistaPropertyList& oProps )
{
	// create new data container
	WindowData* pData = new WindowData;

	VistaWindow* pVistaWindow = NewWindow( pDisplay, pData );
	if( !pVistaWindow )
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge]: - unable to create window" << std::endl;
		delete pData;
		return NULL;
	}

	m_pWindowingToolkit->RegisterWindow( pVistaWindow );

	pVistaWindow->GetProperties()->SetPropertiesByList( oProps );

	// finally create the window
	m_pWindowingToolkit->InitWindow( pVistaWindow );

	// set mouse cursor visibility
	// to default on start.
	m_pWindowingToolkit->SetCursorIsEnabled( pVistaWindow, GetShowCursor() );

	pData->m_ptrWindow = osg::PassiveWindow::create();

	/// @todo ID for passive window? gg508531
	//pData->m_ptrWindow->setId(pData->m_iWindowId);
	pData->m_ptrWindow->init();
	int nWidth, nHeight;
	m_pWindowingToolkit->GetWindowSize( pVistaWindow, nWidth, nHeight );
	pData->m_ptrWindow->setSize( nWidth, nHeight );

	pData->ObserveWindow( pVistaWindow, this );

	return pVistaWindow;
}

bool VistaOpenSGDisplayBridge::DestroyVistaWindow( VistaWindow* pWindow )
{
	if (!pWindow)
		return true;

	// destroy data containers
	WindowData * pData = (WindowData*)pWindow->GetData();

	m_pWindowingToolkit->UnregisterWindow( pWindow );

	delete pData;
	vstr::debugi() << "VistaOpenSGDisplayBridge::DestroyVistaWindow(" << pWindow->GetNameForNameable() << ")" << std::endl;
	delete pWindow;

	return true;
}

bool VistaOpenSGDisplayBridge::SetWindowStereo( bool bStereo, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetUseStereo( pTarget, bStereo );

}

bool VistaOpenSGDisplayBridge::GetWindowAccumBufferEnabled( const VistaWindow * pTarget )
{
	return m_pWindowingToolkit->GetUseAccumBuffer( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowAccumBufferEnabled( bool bUse, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetUseAccumBuffer( pTarget, bUse );
}

bool VistaOpenSGDisplayBridge::GetWindowStencilBufferEnabled( const VistaWindow * pTarget )
{
	return m_pWindowingToolkit->GetUseStencilBuffer( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowStencilBufferEnabled( bool bUse, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetUseStencilBuffer( pTarget, bUse );
}

bool VistaOpenSGDisplayBridge::GetWindowDrawBorder( const VistaWindow* pTarget )
{
	return m_pWindowingToolkit->GetDrawBorder( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowDrawBorder( bool bDrawBorder, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetDrawBorder( pTarget, bDrawBorder );
}

bool VistaOpenSGDisplayBridge::GetWindowStereo( const VistaWindow * pTarget )
{
	return m_pWindowingToolkit->GetUseStereo( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowPosition( int x, int y, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetWindowPosition( pTarget, x, y );
}

bool VistaOpenSGDisplayBridge::GetWindowPosition( int& x, int& y, const VistaWindow* pTarget )
{
	return m_pWindowingToolkit->GetWindowPosition( pTarget, x, y );
}

bool VistaOpenSGDisplayBridge::SetWindowSize( int w, int h, VistaWindow * pTarget )
{
	return m_pWindowingToolkit->SetWindowSize( pTarget, w, h );
}

bool VistaOpenSGDisplayBridge::GetWindowSize( int& w, int& h, const VistaWindow* pTarget )
{
	return m_pWindowingToolkit->GetWindowSize( pTarget, w, h );
}

int VistaOpenSGDisplayBridge::GetWindowId( const VistaWindow* pTarget )
{
	return m_pWindowingToolkit->GetWindowId( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowVSync( bool bEnabled, VistaWindow *pTarget )
{
	return m_pWindowingToolkit->SetVSyncMode( pTarget, bEnabled );	
}
int VistaOpenSGDisplayBridge::GetWindowVSync( const VistaWindow* pTarget )
{
	return m_pWindowingToolkit->GetVSyncMode( pTarget );	
}

bool VistaOpenSGDisplayBridge::SetWindowFullScreen( bool bFullScreen, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetFullscreen( pTarget, bFullScreen );
}

bool VistaOpenSGDisplayBridge::GetWindowFullScreen( const VistaWindow* pTarget )
{
	return m_pWindowingToolkit->GetFullscreen( pTarget );
}

bool VistaOpenSGDisplayBridge::GetWindowIsOffscreenBuffer( const VistaWindow* pWindow ) const
{
	return m_pWindowingToolkit->GetUseOffscreenBuffer( pWindow );
}

bool VistaOpenSGDisplayBridge::SetWindowIsOffscreenBuffer( VistaWindow* pWindow, const bool bSet )
{
	return m_pWindowingToolkit->SetUseOffscreenBuffer( pWindow, bSet );
}

bool VistaOpenSGDisplayBridge::GetWindowRGBImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData )
{
	return m_pWindowingToolkit->GetRGBImage( pWindow, vecData );
}

bool VistaOpenSGDisplayBridge::GetWindowDepthImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData )
{
	return m_pWindowingToolkit->GetDepthImage( pWindow, vecData );
}

bool VistaOpenSGDisplayBridge::GetWindowContextVersion( int& nMajor, int& nMinor, const VistaWindow* pTarget ) const
{
	return m_pWindowingToolkit->GetContextVersion( nMajor, nMinor, pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowContextVersion( int nMajor, int nMinor, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetContextVersion( nMajor, nMinor, pTarget );
}

bool VistaOpenSGDisplayBridge::GetWindowIsDebugContext( const VistaWindow* pTarget ) const
{
	return m_pWindowingToolkit->GetIsDebugContext( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowIsDebugContext( const bool bIsDebug, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetIsDebugContext( bIsDebug, pTarget );
}

bool VistaOpenSGDisplayBridge::GetWindowIsForwardCompatible( const VistaWindow* pTarget ) const
{
	return m_pWindowingToolkit->GetIsForwardCompatible( pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowIsForwardCompatible( const bool bIsForwardCompatible, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetIsForwardCompatible( bIsForwardCompatible, pTarget );
}

bool VistaOpenSGDisplayBridge::SetWindowTitle( const std::string& sTitle, VistaWindow* pTarget )
{
	return m_pWindowingToolkit->SetWindowTitle( pTarget, sTitle );
}

std::string VistaOpenSGDisplayBridge::GetWindowTitle( const VistaWindow * pTarget )
{
	return m_pWindowingToolkit->GetWindowTitle( pTarget );
}

void VistaOpenSGDisplayBridge::DebugVistaWindow( std::ostream& out, const VistaWindow * pTarget )
{
	pTarget->Debug(out);
}

void VistaOpenSGDisplayBridge::OnWindowSizeUpdate( VistaWindow *pTarget )
{
	osg::WindowPtr ptrWindow = dynamic_cast<WindowData *>( pTarget->GetData() )->m_ptrWindow;
	int nWidth, nHeight;
	m_pWindowingToolkit->GetWindowSize( pTarget, nWidth, nHeight );

	beginEditCP( ptrWindow );
	ptrWindow->resize( nWidth, nHeight );
	endEditCP( ptrWindow );

	std::vector<VistaViewport*>& vecViewports = pTarget->GetViewports();
	for( std::vector<VistaViewport*>::iterator itViewport = vecViewports.begin();
			itViewport != vecViewports.end(); ++itViewport )
	{
		(*itViewport)->GetViewportProperties()->Notify( VistaViewport::VistaViewportProperties::MSG_SIZE_CHANGE );
	}
}

/**
* Methods for viewport management.
*/
VistaViewport * VistaOpenSGDisplayBridge::CreateViewport(
											VistaDisplaySystem* pDisplaySystem,
											VistaWindow* pWindow,
											const VistaPropertyList & refProps)
{
	if (!pDisplaySystem)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
				<< "no display system given" << std::endl;
		return NULL;
	}

	if (!pWindow)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
				<< "no window given" << std::endl;
		return NULL;
	}

	// create new data container
	ViewportData * pData = new ViewportData();

	if (!pData)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport -"
					"cannot create toolkit specific data" << std::endl;
		return NULL;
	}

	VistaViewport * pViewport = NewViewport( pDisplaySystem, pWindow, pData );
	if (!pViewport)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport" << std::endl;
		delete pData;
		return NULL;
	}

	// RWTH blue as background color
	pData->m_pSolidBackground = osg::SolidBackground::create();
	beginEditCP(pData->m_pSolidBackground);
	pData->m_pSolidBackground->setColor( osg::Color3f( 0.0f,0.4f,0.8f ) );
	endEditCP(pData->m_pSolidBackground);

	WindowData* pWindowData = static_cast<WindowData*>( pWindow->GetData() );

	pData->m_bStereo = GetWindowStereo( pWindow ); // just in case somone asks :)
	pData->m_bAccumBufferEnabled = GetWindowAccumBufferEnabled( pWindow );
	pData->m_bStencilBufferEnabled = GetWindowStencilBufferEnabled( pWindow );
	if( pData->m_bStereo ) // active STEREO ?
	{
		osg::StereoBufferViewportPtr StereoViewport;
		StereoViewport = osg::StereoBufferViewport::create();
		beginEditCP(StereoViewport);
		StereoViewport->setSize(0,0,1,1);
		StereoViewport->setBackground(pData->m_pSolidBackground);
		StereoViewport->setRoot(m_pRealRoot);
		StereoViewport->setLeftBuffer(true);
		StereoViewport->setRightBuffer(false);
		endEditCP(StereoViewport);
		pData->m_Viewport = StereoViewport;

		StereoViewport = osg::StereoBufferViewport::create();
		beginEditCP(StereoViewport);
		StereoViewport->setSize(0,0,1,1);
		StereoViewport->setBackground(pData->m_pSolidBackground);
		StereoViewport->setRoot(m_pRealRoot);
		StereoViewport->setLeftBuffer(false);
		StereoViewport->setRightBuffer(true);
		endEditCP(StereoViewport);
		pData->m_RightViewport = StereoViewport;

		// insert both viewports into window
		osg::beginEditCP(pWindowData->m_ptrWindow);
		pWindowData->m_ptrWindow->addPort(pData->m_Viewport);
		pWindowData->m_ptrWindow->addPort(pData->m_RightViewport);
		osg::endEditCP(pWindowData->m_ptrWindow);

		addRefCP(pData->m_Viewport);
		addRefCP(pData->m_RightViewport);
	}
	else // MONO
	{
		pData->m_Viewport = osg::Viewport::create();
		beginEditCP(pData->m_Viewport);
		pData->m_Viewport->setSize(0,0,1,1);
		pData->m_Viewport->setBackground(pData->m_pSolidBackground);
		pData->m_Viewport->setRoot(m_pRealRoot);
		endEditCP(pData->m_Viewport);

		// insert viewport into window
		osg::beginEditCP(pWindowData->m_ptrWindow);
		pWindowData->m_ptrWindow->addPort(pData->m_Viewport);
		osg::endEditCP(pWindowData->m_ptrWindow);

		addRefCP(pData->m_Viewport);
	}


	if (!pData->m_Viewport)
	{
		// hrrk! something still went wrong...
		vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
					<< "OpenSG doesn't allow it" << std::endl;
		delete pViewport;
		delete pData;
		return NULL;
	}

	
	VistaPropertyList oCleanedProps = refProps;
	oCleanedProps.RemoveProperty( "PROJECTION" );
	oCleanedProps.RemoveProperty( "WINDOW" );
	pViewport->GetProperties()->SetPropertiesByList( oCleanedProps );

	return pViewport;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyViewport                                             */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyViewport
( VistaViewport * pViewport )
{
	if (!pViewport)
		return true;

	// destroy data containers
	ViewportData * pData = (ViewportData*)pViewport->GetData();

	if(pData->m_Viewport      != osg::NullFC) subRefCP(pData->m_Viewport);
	if(pData->m_RightViewport != osg::NullFC) subRefCP(pData->m_RightViewport);

	delete pData;
	delete pViewport;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetViewportPosition                                         */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewportPosition( int x, int y, VistaViewport * pTarget )
{
	osg::ViewportPtr pViewport = ((ViewportData *)pTarget->GetData())->m_Viewport;

	float fWidth = (float)pViewport->getPixelWidth();
	float fHeight = (float)pViewport->getPixelHeight();

	float fLeft = (float)x;
	float fRight = fLeft+fWidth;
	float fBottom = (float)y;
	float fTop = fBottom+fHeight;

	//int fWindowX = 0;
	//int fWindowY = 0;
	//pTarget->GetWindow()->GetWindowProperties()->GetSize( fWindowX, fWindowY );

	//fLeft /= (float)fWindowX;
	//fRight /= (float)fWindowX;
	//fTop /= (float)fWindowY;
	//fBottom /= (float)fWindowY;

	beginEditCP(pViewport);
	pViewport->setSize( fLeft, fBottom, fRight, fTop );
	endEditCP(pViewport);

	if( ((ViewportData *)pTarget->GetData())->m_bStereo )
	{
		pViewport = ((ViewportData *)pTarget->GetData())->m_RightViewport;
		beginEditCP(pViewport);
		pViewport->setSize( fLeft, fBottom, fRight, fTop );
		endEditCP(pViewport);
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportPosition                                         */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::GetViewportPosition( int& x, int &y, const VistaViewport* pTarget )
{
	osg::ViewportPtr viewport = ((ViewportData *)pTarget->GetData())->m_Viewport;
	x = viewport->getPixelLeft();
	y = ((WindowData *)pTarget->GetWindow()->GetData())
		->m_ptrWindow->getHeight() - viewport->getPixelTop();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetViewportSize                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewportSize( int iWidth, int iHeight, VistaViewport* pTarget )
{
	osg::ViewportPtr pViewport = ((ViewportData *)pTarget->GetData())->m_Viewport;

	// lower-left is (0,0)
	float fLeft = (float) pViewport->getPixelLeft();
	float fBottom = (float) pViewport->getPixelBottom();
	float fWidth = (float) iWidth;
	float fHeight = (float) iHeight;

	float fRight = fLeft + fWidth;
	float fTop = fBottom + fHeight;

	int fWindowX = 0;
	int fWindowY = 0;
	pTarget->GetWindow()->GetWindowProperties()->GetSize( fWindowX, fWindowY );

	//fLeft /= (float)fWindowX;
	//fRight /= (float)fWindowX;
	//fTop /= (float)fWindowY;
	//fBottom /= (float)fWindowY;

	beginEditCP(pViewport);
	pViewport->setSize( fLeft, fBottom, fRight, fTop );
	endEditCP(pViewport);

	if( ( (ViewportData*)pTarget->GetData() )->m_bStereo )
	{
		pViewport = ((ViewportData *)pTarget->GetData())->m_RightViewport;
		beginEditCP(pViewport);
		pViewport->setSize( fLeft, fBottom, fRight, fTop );
		endEditCP(pViewport);
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportSize                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::GetViewportSize( int& w, int& h, const VistaViewport* pTarget )
{
	osg::ViewportPtr viewport = ((ViewportData *)pTarget->GetData())->m_Viewport;
	w = viewport->getPixelWidth();
	h = viewport->getPixelHeight();
}


void VistaOpenSGDisplayBridge::SetViewportHasPassiveBackground( bool bSet, VistaViewport* pTarget )
{
	ViewportData* pData = static_cast<ViewportData*>( pTarget->GetData() );
	if( bSet )
	{
		ViewportData* pData = static_cast<ViewportData*>( pTarget->GetData() );
		if( pData->m_pPassiveBackground == osg::NullFC )
			pData->m_pPassiveBackground = osg::PassiveBackground::create();

		osg::ViewportPtr pViewport = pData->GetOpenSGViewport();
		beginEditCP( pViewport );
		pViewport->setBackground( pData->m_pPassiveBackground );
		endEditCP( pViewport );

		if( pData->GetStereo() )
		{
			pViewport = pData->GetOpenSGRightViewport();
			beginEditCP( pViewport );
			pViewport->setBackground( pData->m_pPassiveBackground );
			endEditCP( pViewport );
		}		
	}
	else
	{
		osg::ViewportPtr pViewport = pData->GetOpenSGViewport();
		beginEditCP( pViewport );
		pViewport->setBackground( pData->m_pSolidBackground );
		endEditCP( pViewport );

		if( pData->GetStereo() )
		{
			pViewport = pData->GetOpenSGRightViewport();
			beginEditCP( pViewport );
			pViewport->setBackground( pData->m_pSolidBackground );
			endEditCP( pViewport );
		}
	}
	pData->m_bHasPassiveBackground = bSet;
}

bool VistaOpenSGDisplayBridge::GetViewportHasPassiveBackground( const VistaViewport* pTarget )
{
	return static_cast<ViewportData*>( pTarget->GetData() )->m_bHasPassiveBackground;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugViewport                                               */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugViewport( std::ostream & out, const VistaViewport * pTarget )
{
	pTarget->Debug(out);
}

/**
* Methods for projection management.
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProjection                                            */
/*                                                                            */
/*============================================================================*/
VistaProjection * VistaOpenSGDisplayBridge::CreateProjection( 
							VistaViewport * pViewport, const VistaPropertyList & refProps )
{
	if (!pViewport)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge] -Unable to create projection - "
				<< "no viewport given..." << std::endl;
		return NULL;
	}

	// create new data container
	ProjectionData * pData = new ProjectionData;
	if (!pData)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge] -Unable to create projection - "
				<< "cannot create toolkit specific data..." << std::endl;
		return NULL;
	}

	DisplaySystemData * pDS = (DisplaySystemData *)pViewport->GetDisplaySystem()->GetData();

	// connect both beacons (display system and head)
	// since the camera decorator won't do it on it's own
	beginEditCP(pDS->m_ptrCameraBeacon);
	pDS->m_ptrCameraBeacon->addChild( pData->m_ptrBeacon );
	endEditCP(pDS->m_ptrCameraBeacon);

	// also append the eye offset beacons
	beginEditCP(pData->m_ptrBeacon);
	pData->m_ptrBeacon->addChild( pData->m_ptrEyeBeacon );
	pData->m_ptrBeacon->addChild( pData->m_ptrRightEyeBeacon );
	endEditCP  (pData->m_ptrBeacon);

	// left camera
	beginEditCP(pData->m_ptrCamDeco);
	pData->m_ptrPerspCam = osg::PerspectiveCamera::create();
	beginEditCP(pData->m_ptrPerspCam);
	pData->m_ptrPerspCam->setBeacon( pDS->m_ptrCameraBeacon );
	pData->m_ptrPerspCam->setNear( 0.01f );
	pData->m_ptrPerspCam->setFar( 65000.0f );
	pData->m_ptrPerspCam->setFov( osg::deg2rad(90) );
	pData->m_ptrPerspCam->setAspect( 1 );
	endEditCP(pData->m_ptrPerspCam);
	pData->m_ptrCamDeco->setDecoratee( pData->m_ptrPerspCam );
	// set the user beacon later on
	// once the projection knows about the eyes
	pData->m_ptrCamDeco->getSurface().clear();
	// push the points defining the projection surface's edges
	// MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
	// default 4:3 aspect ratio
	pData->m_ptrCamDeco->getSurface().push_back(osg::Pnt3f(-1,-0.75f, 0));
	pData->m_ptrCamDeco->getSurface().push_back(osg::Pnt3f( 1,-0.75f, 0));
	pData->m_ptrCamDeco->getSurface().push_back(osg::Pnt3f( 1, 0.75f, 0));
	pData->m_ptrCamDeco->getSurface().push_back(osg::Pnt3f(-1, 0.75f, 0));
	endEditCP(pData->m_ptrCamDeco);

	// set the viewport's cameras
	osg::ViewportPtr osgViewport;
	osgViewport = ((ViewportData *)(pViewport->GetData()))->m_Viewport;
	beginEditCP(osgViewport);
	osgViewport->setCamera(pData->m_ptrCamDeco);
	endEditCP(osgViewport);

	// deactivate the right eye iff stereo is off
	bool bVPStereo = ((ViewportData *)(pViewport->GetData()))->GetStereo();
	pData->m_ptrRightEyeBeacon->setActive(bVPStereo);

	if( bVPStereo ) // STEREO ?
	{
		// right camera
		beginEditCP(pData->m_ptrRightCamDeco);
		pData->m_ptrRightPerspCam = osg::PerspectiveCamera::create();
		beginEditCP(pData->m_ptrRightPerspCam);
		pData->m_ptrRightPerspCam->setBeacon(pDS->m_ptrCameraBeacon); /** @todo camerabeacon */
		pData->m_ptrRightPerspCam->setNear( 0.01f );
		pData->m_ptrRightPerspCam->setFar( 65000.0f );
		pData->m_ptrRightPerspCam->setFov(osg::deg2rad(90));
		pData->m_ptrRightPerspCam->setAspect(1);
		endEditCP(pData->m_ptrRightPerspCam);
		pData->m_ptrRightCamDeco->setDecoratee(pData->m_ptrRightPerspCam);
		// set the user beacon later on
		// once the projection knows about the eyes
		pData->m_ptrRightCamDeco->getSurface().clear();
		// push the points defining the projection surface's edges
		// MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
		// default 4:3 aspect ratio
		pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f(-1,-0.75f, 0));
		pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f( 1,-0.75f, 0));
		pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f( 1, 0.75f, 0));
		pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f(-1, 0.75f, 0));
		endEditCP(pData->m_ptrRightCamDeco);

		osgViewport = ((ViewportData *)(pViewport->GetData()))->m_RightViewport;
		beginEditCP(osgViewport);
		osgViewport->setCamera( pData->m_ptrRightCamDeco );
		endEditCP(osgViewport);
	}

	VistaProjection * pProjection = NewProjection(pViewport, pData);
	if (!pProjection)
	{
		vstr::errp() << "[VistaOpenSGDisplayBridge] - Unable to create projection"
				<< std::endl;
		delete pData;
		return NULL;
	}

	ProjectionData* pProjData = (ProjectionData*)pProjection->GetData();


	// stereo key must be set before setpropertiesbylist, since the set-functors for e.g.
	// clipping range depend on it being already set (in the projection data!)
	// this should be cleaned up on occasion...
	std::string sMode;
	refProps.GetValue( "STEREO_MODE", sMode );
	if( sMode == "FULL_STEREO" )
		pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_FULL_STEREO;
	else if( sMode == "MONO" )
		pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_MONO;
	if( sMode == "LEFT_EYE" )
		pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_LEFT_EYE;
	if( sMode == "RIGHT_EYE" )
		pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_RIGHT_EYE;


	pProjection->GetProperties()->SetPropertiesByList(refProps);

	// get and then set the initial viewer position and orientation for the display system
	pProjData->SetCameraTranslation(pDS->m_v3InitialViewerPosition);
	pProjData->SetCameraOrientation(pDS->m_qInitialViewerOrientation);

	// set the appropriate user beacon for the camera decorator -> Head or Eye?
	pData->SetEyes(pDS->m_v3LeftEyeOffset,pDS->m_v3RightEyeOffset);

	return pProjection;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyProjection                                           */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyProjection
( VistaProjection * pProjection )
{
	if (!pProjection)
		return true;

	ProjectionData * pData = static_cast<ProjectionData*>(pProjection->GetData());

	delete pData;
	delete pProjection;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjectionPlane                                      */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjectionPlane
( const VistaVector3D & v3MidPoint, const VistaVector3D & v3NormalVector,
 const VistaVector3D & v3UpVector, VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();

	pData->m_v3MidPoint     = v3MidPoint;
	pData->m_v3NormalVector = v3NormalVector.GetNormalized();
	pData->m_v3UpVector     = v3UpVector.GetNormalized();

	pData->SetProjectionPlane();
}

void VistaOpenSGDisplayBridge::GetProjectionPlane
( VistaVector3D & v3MidPoint, VistaVector3D & v3NormalVector,
 VistaVector3D & v3UpVector, const VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();

	v3MidPoint     = pData->m_v3MidPoint;
	v3NormalVector = pData->m_v3NormalVector;
	v3UpVector     = pData->m_v3UpVector;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjPlaneExtents                                     */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjPlaneExtents
( double dLeft, double dRight, double dBottom, double dTop, VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();

	pData->m_dLeft   = dLeft;
	pData->m_dRight  = dRight;
	pData->m_dBottom = dBottom;
	pData->m_dTop    = dTop;

	pData->SetProjectionPlane();
}

void VistaOpenSGDisplayBridge::GetProjPlaneExtents
( double & dLeft, double & dRight, double & dBottom, double & dTop, const VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();

	dLeft   = pData->m_dLeft;
	dRight  = pData->m_dRight;
	dBottom = pData->m_dBottom;
	dTop    = pData->m_dTop;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjClippingRange                                    */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjClippingRange
( double dNear, double dFar, VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();

	beginEditCP( pData->m_ptrCamDeco );
	pData->m_ptrCamDeco->setNear( (float)dNear );
	pData->m_ptrCamDeco->setFar( (float)dFar );
	endEditCP( pData->m_ptrCamDeco );

	if( pData->m_iStereoMode == VistaProjection::VistaProjectionProperties::SM_FULL_STEREO )
	{
		beginEditCP( pData->m_ptrRightCamDeco );
		pData->m_ptrRightCamDeco->setNear( (float)dNear );
		pData->m_ptrRightCamDeco->setFar( (float)dFar );
		endEditCP( pData->m_ptrRightCamDeco );
	}
}

void VistaOpenSGDisplayBridge::GetProjClippingRange
( double & dNear, double & dFar, const VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();

	dNear = pData->m_ptrCamDeco->getNear();
	dFar  = pData->m_ptrCamDeco->getFar();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjStereoMode                                       */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjStereoMode
( int iMode, VistaProjection * pTarget )
{
	if (iMode >= VistaProjection::VistaProjectionProperties::SM_MONO
		&& iMode <= VistaProjection::VistaProjectionProperties::SM_FULL_STEREO)
	{
		ProjectionData *pData = (ProjectionData *)pTarget->GetData();

		if (pTarget->GetViewport()->GetWindow()->GetWindowProperties()->GetStereo())
		{
			if (iMode != VistaProjection::VistaProjectionProperties::SM_FULL_STEREO)
			{
				vstr::warnp() << "[VistaOpenSGDisplayBridge] - invalid stereo mode ["
							<< iMode << "] for stereo window..." << std::endl;
			}
		}
		else
		{
			if (iMode == VistaProjection::VistaProjectionProperties::SM_FULL_STEREO)
			{
				vstr::warnp() << "[VistaOpenSGDisplayBridge] - invalid stereo mode "
					<< "[FULL_STEREO] for mono window..." << std::endl;
			}
		}

		pData->m_iStereoMode = iMode;

		// update eyes after mode change
		pData->SetEyes( pData->m_v3LeftEye, pData->m_v3RightEye );
	}
}

int VistaOpenSGDisplayBridge::GetProjStereoMode
( const VistaProjection * pTarget )
{
	ProjectionData * pData = (ProjectionData *)pTarget->GetData();
	return pData->m_iStereoMode;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugProjection                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugProjection
( std::ostream & out, const VistaProjection * pTarget )
{
	pTarget->Debug(out);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   RenderAction							                      */
/*                                                                            */
/*============================================================================*/
osg::RenderAction* VistaOpenSGDisplayBridge::GetRenderAction() const
{
	return m_pRenderAction;
}

bool VistaOpenSGDisplayBridge::MakeScreenshot( const VistaWindow& oWin, const std::string& strFilename,
												const bool bDelayUntilNextRender ) const
{
	// get the OpenSG window data from the specified ViSTA window
	WindowData* pWindow = dynamic_cast<WindowData*>( oWin.GetData() );

	// check if the image already has an extension
	size_t iIndex = strFilename.find_last_of( '.' );
	string sFileName = strFilename;
	if( iIndex == string::npos )
		sFileName += ".jpg";

	osg::FileGrabForegroundPtr ptrGrabber = pWindow->GetFileGrabForeground();

	bool bInitialActiveState = ptrGrabber->getActive();
	std::string sInitialFile = ptrGrabber->getName();
	beginEditCP( ptrGrabber,
				osg::FileGrabForeground::ActiveFieldMask |
				osg::FileGrabForeground::NameFieldMask );
	{
		ptrGrabber->setActive( true );
		ptrGrabber->setName( sFileName.c_str() );
	}
	endEditCP( ptrGrabber,
				osg::FileGrabForeground::ActiveFieldMask |
				osg::FileGrabForeground::NameFieldMask );

	if( bDelayUntilNextRender )
	{
		// list it so that it is deactivated after the next draw
		m_vecFileGrabWindows.push_back( pWindow );
	}
	else
	{
		// render once, and deactivate it afterwards
		pWindow->m_ptrWindow->render(m_pRenderAction);

		beginEditCP( ptrGrabber,
					osg::FileGrabForeground::ActiveFieldMask |
					osg::FileGrabForeground::NameFieldMask );
		{
			ptrGrabber->setActive( bInitialActiveState );
			ptrGrabber->setName( sInitialFile.c_str() );
		}
		endEditCP( ptrGrabber,
					osg::FileGrabForeground::ActiveFieldMask |
					osg::FileGrabForeground::NameFieldMask );
	}

	return true;
}

bool VistaOpenSGDisplayBridge::AddSceneOverlay( IVistaSceneOverlay *pDraw,
												VistaViewport* pViewport )
{
	if( pViewport == NULL )
		return false;

	ViewportData* pViewportData = static_cast<ViewportData*>( pViewport->GetData() );
	osg::VistaOpenSGGLOverlayForegroundPtr pOverlays = pViewportData->GetOverlayForeground();
	if( pOverlays == osg::NullFC )
	{
		pOverlays = osg::VistaOpenSGGLOverlayForeground::create();
		if( pOverlays == osg::NullFC )
			return false;

		pViewportData->m_pOverlays = pOverlays;

		osg::ViewportPtr osgvp = pViewportData->GetOpenSGViewport();

		osgvp->getForegrounds().push_back(pOverlays);

		if(pViewportData->GetOpenSGRightViewport())
		{
			pViewportData->GetOpenSGRightViewport()->getForegrounds().push_back(pOverlays);
		}
	}

	beginEditCP(pOverlays, osg::VistaOpenSGGLOverlayForeground::GLOverlaysFieldMask);
	pOverlays->getMFGLOverlays()->push_back(pDraw);
	endEditCP(pOverlays, osg::VistaOpenSGGLOverlayForeground::GLOverlaysFieldMask);

	return true;
}

bool VistaOpenSGDisplayBridge::RemSceneOverlay( IVistaSceneOverlay *pDraw,
												VistaViewport* pViewport )
{
	if( pViewport == NULL )
		return false;

	ViewportData* vpdata = static_cast<ViewportData*>( pViewport->GetData() );
	osg::ViewportPtr osgvp = vpdata->GetOpenSGViewport();

	osg::MField<void*> &overls = vpdata->GetOverlayForeground()->getGLOverlays();
	osg::MField<void*>::iterator it = overls.find(pDraw);

	if(it != overls.end())
	{
		overls.erase(it);
		return true;
	}

	return false;
}


Vista2DText* VistaOpenSGDisplayBridge::New2DText(const std::string &strViewportName)
{
	Vista2DText * Text2DObjekt = NULL ;

	VistaViewport* viewport = m_pDisplayManager->GetViewportByName( strViewportName ) ;
	if( viewport != NULL )
	{
		ViewportData* vpdata = (ViewportData*)viewport->GetData() ;

		osg::VistaOpenSGTextForegroundPtr textfg = vpdata->GetTextForeground() ;
		if( textfg == osg::NullFC )
		{
			osg::ViewportPtr osgviewport = vpdata->GetOpenSGViewport() ;
			textfg = osg::VistaOpenSGTextForeground::create();

			if( textfg == osg::NullFC )
				return NULL ;

			vpdata->m_TextForeground = textfg;

			// we store a pointer ourselves
			osgviewport->getForegrounds().push_back(textfg);

			if(vpdata->GetOpenSGRightViewport())
			{
				vpdata->GetOpenSGRightViewport()->getForegrounds().push_back(textfg);
			}
		}

		Text2DObjekt = new Vista2DText;
		vpdata->m_liOverlays.push_back(Text2DObjekt);

		beginEditCP(textfg);
		textfg->getMFTexts()->push_back( Text2DObjekt ) ;
		endEditCP(textfg);
	}
	else
	{
		vstr::warnp() << "OpenSGDisplayBridge::Add2DText(): non-existent viewport '"
			<< strViewportName << "' specified!" << std::endl ;
	}

	return Text2DObjekt;
}

Vista2DBitmap*	VistaOpenSGDisplayBridge::New2DBitmap	(const std::string &strViewportName )
{
	Vista2DBitmap *pBm = NULL;
	VistaViewport* viewport = m_pDisplayManager->GetViewportByName( strViewportName ) ;
	if( viewport != NULL )
	{
		ViewportData* vpdata = (ViewportData*)viewport->GetData() ;
		osg::ImageForegroundPtr bitmaps = vpdata->m_oBitmaps;
		if(bitmaps == osg::NullFC)
		{
			// create image forground
			bitmaps = osg::ImageForeground::create();
			osg::ViewportPtr vp = vpdata->GetOpenSGViewport();
			vp->getForegrounds().push_back(bitmaps);

			vp = vpdata->GetOpenSGRightViewport();
			if(vp != osg::NullFC)
				vp->getForegrounds().push_back(bitmaps);

			vpdata->m_oBitmaps = bitmaps;
			addRefCP(vpdata->m_oBitmaps);
		}

		pBm = new VistaOpenSG2DBitmap(this, strViewportName);
		vpdata->m_liOverlays.push_back(pBm);
	}
	return pBm;
}

Vista2DLine*	VistaOpenSGDisplayBridge::New2DLine	(const std::string &strWindowName )
{
	return NULL;
}

Vista2DRectangle*	VistaOpenSGDisplayBridge::New2DRectangle	(const std::string &strWindowName )
{
	return NULL;
}

bool VistaOpenSGDisplayBridge::Get2DOverlay(const std::string &strWindowName, std::list<Vista2DDrawingObject*>&)
{
	return false;
}


bool VistaOpenSGDisplayBridge::DoLoadBitmap(const std::string &strNewFName,
											VistaType::byte** pBitmapData,
											 int& nWidth, int& nHeight, bool& bAlpha)
{
	// this call may return osg::NullFC, which is ok (clear image)
	osg::ImagePtr image = osg::Image::create();


	if(image == osg::NullFC)
		return false;

	addRefCP(image);

	std::string sCwd = VistaFileSystemDirectory::GetCurrentWorkingDirectory();
	if(image->read((sCwd
		+ VistaFileSystemDirectory::GetOSSpecificSeparator()
		+ strNewFName).c_str()) == true)
	{
		beginEditCP(image);

		if(image->getDataType() != osg::Image::OSG_UINT8_IMAGEDATA)
			image->convertDataTypeTo(osg::Image::OSG_UINT8_IMAGEDATA);

		endEditCP(image);

		VistaType::byte* pNewBitmap = image->getData();
		nWidth     = image->getWidth();
		nHeight    = image->getHeight();
		bAlpha     = image->hasAlphaChannel();

		//copy buffer
		int iBuffersize = nWidth * nHeight * (bAlpha ? 4 : 3);

		// alloc memory
		*pBitmapData = new VistaType::byte[iBuffersize];

		// deep copy image data
		memcpy(*pBitmapData, pNewBitmap, iBuffersize);
	}

	//delete the image container and allocated memory
	subRefCP(image);

	return true;
}


bool VistaOpenSGDisplayBridge::Delete2DDrawingObject(Vista2DDrawingObject* p2DObject)
{
	return false;
}

bool VistaOpenSGDisplayBridge::ReplaceForegroundImage(const std::string &sVpName,
													   osg::ImagePtr oOld,
													   osg::ImagePtr oNew,
													   float xPos, float yPos)
{
	VistaViewport* viewport = m_pDisplayManager->GetViewportByName( sVpName ) ;
	if( viewport != NULL )
	{
		ViewportData* vpdata = (ViewportData*)viewport->GetData() ;
		osg::ImageForegroundPtr bitmaps = vpdata->m_oBitmaps;

		if(bitmaps == osg::NullFC)
		{
			// create image forground
			bitmaps = osg::ImageForeground::create();
			osg::ViewportPtr vp = vpdata->GetOpenSGViewport();
			vp->getForegrounds().push_back(bitmaps);

			vp = vpdata->GetOpenSGRightViewport();
			if(vp != osg::NullFC)
				vp->getForegrounds().push_back(bitmaps);

			vpdata->m_oBitmaps = bitmaps;
			addRefCP(vpdata->m_oBitmaps);
		}


		if(oOld == osg::NullFC)
		{
			// simple, just add
			beginEditCP(bitmaps, osg::ImageForeground::ImagesFieldMask);
			bitmaps->addImage(oNew, osg::Pnt2f(xPos, yPos));
			endEditCP(bitmaps, osg::ImageForeground::ImagesFieldMask);

			return true;
		}
		else
		{
			// find and replace old image
			osg::MFImagePtr *images = bitmaps->getMFImages();
			for(osg::UInt32 n=0; n < (*images).size(); ++n)
			{
				if( oOld == (*images)[n] )
				{
					// replace image
					beginEditCP(bitmaps, osg::ImageForeground::ImagesFieldMask
						| osg::ImageForeground::PositionsFieldMask);

					(*images)[n] = oNew;

					// replace positions
					osg::MFPnt2f *positions = bitmaps->getMFPositions();
					(*positions)[n] = osg::Pnt2f(xPos, yPos);

					endEditCP(bitmaps, osg::ImageForeground::ImagesFieldMask
						| osg::ImageForeground::PositionsFieldMask);

					return true;
				}
			}

		}
	}
	return false;
}

void VistaOpenSGDisplayBridge::SetShowCursor(bool bShowCursor)
{
	m_bShowCursor = bShowCursor;	

	// iterate over all glut window contexts from the DM
	map<string, VistaWindow*> mapWindows = GetDisplayManager()->GetWindows();
	map<string, VistaWindow*>::iterator itWindow = mapWindows.begin();
	for( ; itWindow != mapWindows.end(); ++itWindow )
	{
		// toggle cursor
		m_pWindowingToolkit->SetCursorIsEnabled( (*itWindow).second, bShowCursor );		
	}
}

bool VistaOpenSGDisplayBridge::GetShowCursor() const
{
	return m_bShowCursor;
}




