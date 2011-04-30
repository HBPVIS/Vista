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

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(disable: 4231)
#pragma warning(disable: 4244)
#endif

#include "VistaOpenSGDisplayBridge.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaTimerImp.h>

#include <VistaBase/VistaVectorMath.h>
#include <VistaTools/VistaProfiler.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/VistaClusterClient.h>
#include <VistaKernel/VistaClusterSlave.h>
#include <VistaKernel/VistaKernelOut.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaDisplay.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGTextForeground.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaDisplayEvent.h>

#include <VistaKernel/WindowingToolkit/VistaWindowingToolkit.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSolidBackground.h>
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

	return VistaVector3D(trans[0], trans[1], trans[2], trans[3]);
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

	return VistaVector3D(trans[0], trans[1], trans[2], trans[3]);
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

void VistaOpenSGDisplayBridge::DisplaySystemData::SetHMDModeActive( const bool &bSet )
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


VistaOpenSGDisplayBridge::WindowData::WindowData()
: m_iWindowId(-1),
m_strTitle(""),
m_bStereo(false),
m_bDrawBorder(true),
m_bFullScreen(false),
m_ptrWindow(osg::NullFC),
m_pOldWindowSize(new int[2])
{
}

VistaOpenSGDisplayBridge::WindowData::~WindowData()
{
	//if(m_Window != osg::NullFC)
	//	subRefCP(m_Window);
}

int VistaOpenSGDisplayBridge::WindowData::GetWindowId()                const
{
	return m_iWindowId;
}

std::string VistaOpenSGDisplayBridge::WindowData::GetTitle()               const
{
	return m_strTitle;
}


bool VistaOpenSGDisplayBridge::WindowData::GetDrawBorder()                 const
{
	return m_bDrawBorder;
}

bool VistaOpenSGDisplayBridge::WindowData::GetFullScreen()                 const
{
	return m_bFullScreen;
}

bool VistaOpenSGDisplayBridge::WindowData::GetStereo()                     const
{
	return m_bStereo;
}

osg::WindowPtr VistaOpenSGDisplayBridge::WindowData::GetOpenSGWindow() const
{
	return m_ptrWindow;
}

// ##########################
// CViewportData
// ##########################

VistaOpenSGDisplayBridge::ViewportData::ViewportData()
: IVistaDisplayEntityData(),
m_Viewport(osg::NullFC),
m_RightViewport(osg::NullFC),
m_TextForeground(osg::NullFC),
m_overlays(osg::NullFC),
m_oBitmaps(osg::NullFC)
{
}

VistaOpenSGDisplayBridge::ViewportData::~ViewportData()
{
	if(m_Viewport      != osg::NullFC)
		subRefCP(m_Viewport);
	if(m_RightViewport != osg::NullFC)
		subRefCP(m_RightViewport);

	if(m_overlays != osg::NullFC)
		subRefCP(m_overlays);

	if(m_TextForeground != osg::NullFC)
		subRefCP(m_TextForeground);

	if(m_oBitmaps != osg::NullFC)
		subRefCP(m_oBitmaps);
}

bool VistaOpenSGDisplayBridge::ViewportData::GetStereo() const
{
	return m_bStereo;
}

osg::ViewportPtr VistaOpenSGDisplayBridge::
ViewportData::
GetOpenSGViewport()      const
{
	return m_Viewport;
}

osg::ViewportPtr VistaOpenSGDisplayBridge::
ViewportData::
GetOpenSGRightViewport() const
{
	return m_RightViewport;
}

osg::VistaOpenSGTextForegroundPtr VistaOpenSGDisplayBridge::
ViewportData::
GetTextForeground() const
{
	return m_TextForeground ;
}

osg::VistaOpenSGGLOverlayForegroundPtr VistaOpenSGDisplayBridge::
ViewportData::
GetOverlayForeground() const
{
	return m_overlays;
}


void VistaOpenSGDisplayBridge::ViewportData::ReplaceViewport
(osg::ViewportPtr pNewPort, bool bCopyData,	 bool bRight)
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


VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetLeftEye()      const
{
	return m_v3LeftEye;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetRightEye()     const
{
	return m_v3RightEye;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetMidPoint()     const
{
	return m_v3MidPoint;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetNormalVector() const
{
	return m_v3NormalVector;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetUpVector()     const
{
	return m_v3UpVector;
}

double         VistaOpenSGDisplayBridge::ProjectionData::GetLeft()         const
{
	return m_dLeft;
}

double         VistaOpenSGDisplayBridge::ProjectionData::GetRight()        const
{
	return m_dRight;
}

double         VistaOpenSGDisplayBridge::ProjectionData::GetBottom()       const
{
	return m_dBottom;
}

double         VistaOpenSGDisplayBridge::ProjectionData::GetTop()          const
{
	return m_dTop;
}

int            VistaOpenSGDisplayBridge::ProjectionData::GetStereoMode()   const
{
	return m_iStereoMode;
}
osg::ProjectionCameraDecoratorPtr VistaOpenSGDisplayBridge::ProjectionData::GetCamera () const
{
	return m_ptrCamDeco;
}


void VistaOpenSGDisplayBridge::ProjectionData::SetCameraTransformation
( const VistaVector3D & v3CamPos, const VistaQuaternion & qCamOri )
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

void VistaOpenSGDisplayBridge::ProjectionData::GetCameraTransformation
( VistaVector3D & v3CamPos, VistaQuaternion & qCamOri ) const
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

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraTranslation
( const VistaVector3D & v3CamPos )
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

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraOrientation
( const VistaQuaternion & qCamOri )
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
			vkernerr << "Warning: Unknown stereo mode -> defaulting to mono mode" << endl;
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


// we need the static member here, as glut uses static callbacks without
// context information
// sad but true.
VistaOpenSGDisplayBridge *VistaOpenSGDisplayBridge::g_DispBridge = NULL;


VistaOpenSGDisplayBridge::VistaOpenSGDisplayBridge(	osg::RenderAction *pRenderAction,
														osg::NodePtr pRealRoot)
: m_bShowCursor(true)
{
	m_pDMgr = NULL;
	g_DispBridge = this;


	m_pRealRoot = pRealRoot;
	if(m_pRealRoot != osg::NullFC)
		addRefCP(m_pRealRoot);


	m_pRenderAction = pRenderAction;
	m_pActionFunction = NULL;

	// append the OpenSGL text node to the real root
}

VistaOpenSGDisplayBridge::~VistaOpenSGDisplayBridge()
{
	g_DispBridge = NULL;
	subRefCP(m_pRealRoot);
}


void VistaOpenSGDisplayBridge::SetDisplayManager(VistaDisplayManager *pDMgr)
{
	m_pDMgr = pDMgr;
}

VistaDisplayManager *VistaOpenSGDisplayBridge::GetDisplayManager() const
{
	return m_pDMgr;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaOpenSGDisplayBridge::SetActionFunction(void (*pF)())
{
	if(m_pActionFunction)
		return false;
	m_pActionFunction = pF;
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Draw                                                        */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::Draw()
{
	VistaSystem *vistaSystem = GetVistaSystem();
	IVistaWindowingToolkit *wta = vistaSystem->GetWindowingToolkit();
	// render to all windows in the display manager
	WindowData * pWindow;
	const std::map<std::string, VistaWindow *> &mapWindows =
		VistaOpenSGDisplayBridge::g_DispBridge->GetDisplayManager()->GetWindowsConstRef();
	std::map<std::string, VistaWindow *>::const_iterator winIter = mapWindows.begin();
	for( ; winIter != mapWindows.end(); ++winIter )
	{
		pWindow = (WindowData *)winIter->second->GetData();

		int id = pWindow->GetWindowId();
		wta->SetWindow(id);

		pWindow->m_ptrWindow->render(m_pRenderAction);

		if(vistaSystem->IsClient())
		{
		}
		else if(vistaSystem->GetIsSlave())
		{
			vistaSystem->GetVistaClusterSlave()->SwapSync();
		}

		wta->SwapBuffers();

	}
	wta->Redisplay();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UpdateDisplaySystems                                        */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::UpdateDisplaySystems
( VistaDisplayManager * pDisplayManager )
{
	return true;//the OpenSG implementation doesn't need any DisplaySystem updates
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UpdateDisplaySystem                                         */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::UpdateDisplaySystem
( VistaDisplaySystem * pDisplaySystem )
{
	//the OpenSG implementation doesn't need any DisplaySystem updates
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserverUpdate                                              */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::ObserverUpdate
( IVistaObserveable *pObserveable, int msg, int ticket, VistaDisplaySystem *pTarget)
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

VistaDisplaySystem * VistaOpenSGDisplayBridge::CreateDisplaySystem
( VistaVirtualPlatform * pReferenceFrame, VistaDisplayManager * pDisplayManager,
 const VistaPropertyList & refProps)
{
	// create new data container
	DisplaySystemData * pData = new DisplaySystemData;

	if (!pData)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create display system..." << endl;
		vkernout << "                                      cannot create toolkit specific data..." << endl;
		return NULL;
	}

	// get the real OpenSG root node to attach display system beacons to it
	// this means that display systems and cameras will be hidden for pure ViSTA applications

	beginEditCP(m_pRealRoot);
	m_pRealRoot->addChild(pData->m_ptrPlatformBeacon); // append beacon to root node
	endEditCP(m_pRealRoot);

	beginEditCP(pData->m_ptrPlatformBeacon);
	pData->m_ptrPlatformBeacon->addChild(pData->m_ptrCameraBeacon);
	endEditCP(pData->m_ptrPlatformBeacon);

	VistaDisplaySystem * pDisplaySystem = NewDisplaySystem(pReferenceFrame, pData, pDisplayManager);

	if (!pDisplaySystem)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create display system..." << endl;
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
bool VistaOpenSGDisplayBridge::DestroyDisplaySystem
( VistaDisplaySystem * pDisplaySystem )
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
void VistaOpenSGDisplayBridge::DebugDisplaySystem
( std::ostream & out, const VistaDisplaySystem * pTarget)
{
	pTarget->Debug(out);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetViewerPosition                                       */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewerPosition
( const VistaVector3D & v3Pos, VistaDisplaySystem * pTarget)
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

void VistaOpenSGDisplayBridge::GetViewerPosition
( VistaVector3D & v3Pos, const VistaDisplaySystem * pTarget)
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
void VistaOpenSGDisplayBridge::SetViewerOrientation
(const VistaQuaternion & qOrient, VistaDisplaySystem * pTarget)
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

void VistaOpenSGDisplayBridge::GetViewerOrientation
(VistaQuaternion & qOrient, const VistaDisplaySystem * pTarget)
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
void VistaOpenSGDisplayBridge::SetEyeOffsets
( const VistaVector3D & v3LeftOffset, const VistaVector3D & v3RightOffset,
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
		vkernout << " [VistaOpenSGDisplayBridge - ERROR - unable to create display..." << endl;
		vkernout << "                                     no display string given..." << endl;
		return NULL;
	}

	string strName = refProps.GetStringValue("DISPLAY_STRING");

	// create new data container
	DisplayData *pData = new DisplayData;
	if (!pData)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create display..." << endl;
		vkernout << "                                      cannot create toolkit specific data..." << endl;
		return NULL;
	}

	pData->m_sDisplayName = strName;

	VistaDisplay *pDisplay = NewDisplay(pData, pDisplayManager);
	if (!pDisplay)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create display..." << endl;
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

void VistaOpenSGDisplayBridge::DebugDisplay
( std::ostream & out, const VistaDisplay * pTarget)
{
	pTarget->Debug(out);
}

/**
* Methods for window management.
*/
VistaWindow * VistaOpenSGDisplayBridge::CreateVistaWindow
( VistaDisplay * pDisplay, const VistaPropertyList & refProps )
{
	// create new data container
	WindowData * pData = new WindowData;
	if (!pData)
	{
		vkernerr << " [VistaOpenSGDisplayBridge] - ERROR - unable to create window..." << endl;
		vkernerr << "                                      cannot create toolkit specific data..." << endl;
		return NULL;
	}

	VistaWindow * pVistaWindow = NewWindow(pDisplay, pData);
	if (!pVistaWindow)
	{
		vkernerr << " [VistaOpenSGDisplayBridge] - ERROR - unable to create window..." << endl;
		delete pData;
		return NULL;
	}

	/// @todo think about display modes
	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	unsigned int displayMode = wta->RGB | wta->DEPTH | wta->DOUBLE;
	if (refProps.HasProperty("STEREO") && refProps.GetBoolValue("STEREO"))
	{
		pData->m_bStereo = true;
		displayMode |= wta->STEREO;
	}
	else
		pData->m_bStereo = false;

	// finally create the window
	wta->SetDisplayMode(displayMode);
	pData->m_iWindowId = wta->CreateOpenGLContext("ViSTA");

	// set this title as a new title in the internal structure of ViSTA
	pVistaWindow->GetWindowProperties()->SetTitle("ViSTA");

#ifdef _DEBUG
	vkernout << "[VistaOpenSGDisplayBridge] register GLUT functions..." << std::endl;
#endif

	wta->DisplayFunctor(m_pActionFunction);
	wta->IdleFunctor(m_pActionFunction);
	wta->ReshapeFunctor(ReshapeFunction);

	// set mouse cursor visibility
	// to default on start.
	SetShowCursor(GetShowCursor());

#ifdef _DEBUG
	vkernout << "[VistaOpenSGDisplayBridge] creating passive window..." << std::endl;
#endif

	pData->m_ptrWindow = osg::PassiveWindow::create();
	pData->m_ptrWindow->init();

	/// @todo ID for passive window? gg508531
	//pData->m_ptrWindow->setId(pData->m_iWindowId);
	pData->m_ptrWindow->init();

	//pVistaWindow->SetPropertiesByList(refProps);
	pVistaWindow->GetProperties()->SetPropertiesByList(refProps);

	return pVistaWindow;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyVistaWindow                                          */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyVistaWindow
( VistaWindow * pWindow )
{
	if (!pWindow)
		return true;

	// destroy data containers
	WindowData * pData = (WindowData*)pWindow->GetData();

	GetVistaSystem()->GetWindowingToolkit()->DestroyWindow(pData->GetWindowId());

	delete pData;
#ifdef _DEBUG
	vkernout << "VistaOpenSGDisplayBridge::DestroyVistaWindow(" << pWindow << ")\n";
#endif
	delete pWindow;

	return true;
}

void VistaOpenSGDisplayBridge::SetWindowStereo
( bool bStereo, VistaWindow * pTarget )
{
	if(bStereo)
		vkernout << " Warning: VistaOpenSGDisplayBridge::SetWindowStereo -> this setting can't be changed after window creation... \n";

}

bool VistaOpenSGDisplayBridge::GetWindowStereo
( const VistaWindow * pTarget )
{
	return ((WindowData *)pTarget->GetData())->m_bStereo;
}

void VistaOpenSGDisplayBridge::SetWindowPosition
( int x, int y, VistaWindow * pTarget )
{
	// can not ask glut for beeing fullscreen
	if(GetFullScreen(pTarget))
		return;

	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	wta->SetWindowPosition(x, y);
	wta->SetWindow(win);
}

void VistaOpenSGDisplayBridge::GetWindowPosition
( int & x, int & y, const VistaWindow * pTarget )
{
	int *pos = new int[2];
	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	pos = wta->GetWindowPosition();
	x = pos[0];
	y = pos[1];
	wta->SetWindow(win);
}

void VistaOpenSGDisplayBridge::SetWindowSize
( int w, int h, VistaWindow * pTarget )
{
	if(GetFullScreen(pTarget))
		return;

	if( w < 1 || h < 1 )
	{
		vkernout << " VistaOpenSGDisplayBridge::SetWindowSize(w,h) Warning :: invalid parameters " << endl;
		return;
	}

	osg::WindowPtr ptrWindow = ((WindowData *)pTarget->GetData())->m_ptrWindow;

	// maybe OpenSG needs this to stay informed of the window size ... who knows?
	beginEditCP(ptrWindow);
	ptrWindow->resize(w,h);
	endEditCP(ptrWindow);

	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	wta->SetWindowSize(w, h);
	wta->SetWindow(win);
}

void VistaOpenSGDisplayBridge::GetWindowSize
( int & w, int & h, const VistaWindow * pTarget )
{
	// cannot ask window for its size
	osg::WindowPtr window = ((WindowData *)pTarget->GetData())->m_ptrWindow;
	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	int *size = new int[2];
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	size = wta->GetWindowSize();
	wta->SetWindow(win);
	w = size[0];
	h = size[1];
}

bool VistaOpenSGDisplayBridge::SetWindowVSync( bool bEnabled, VistaWindow *pTarget )
{
	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	bool bReturn = wta->SetVSyncEnabled( bEnabled );
	wta->SetWindow(win);

	return bReturn;
}
int VistaOpenSGDisplayBridge::GetWindowVSync( const VistaWindow *pTarget )
{
	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	int nReturn = wta->GetVSyncMode();
	wta->SetWindow(win);

	return nReturn;
}

void VistaOpenSGDisplayBridge::SetFullScreen
( bool bFullScreen, VistaWindow * pTarget )
{

	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	if( bFullScreen )
	{
		// save window size of switching to fullscreen
		static_cast<WindowData*>(pTarget->GetData())->m_pOldWindowSize = wta->GetWindowSize();
		wta->ActivateFullscreen();
	}
	else // windowed
	{
		int w = static_cast<WindowData*>(pTarget->GetData())->m_pOldWindowSize[0];
		int h = static_cast<WindowData*>(pTarget->GetData())->m_pOldWindowSize[1];

		// it can happen that we try to resize a 0 window, so we will
		// make it one pixel (to avoid a warning)
		w = w < 1 ? 1:w;
		h = h < 1 ? 1:h;

		wta->SetWindowSize(w,h);
	}
	wta->SetWindow(win);

	((WindowData *)pTarget->GetData())->m_bFullScreen = bFullScreen;
}

bool VistaOpenSGDisplayBridge::GetFullScreen
( const VistaWindow * pTarget )
{
	return ((WindowData *)pTarget->GetData())->m_bFullScreen;
}

void VistaOpenSGDisplayBridge::SetWindowTitle
( const std::string & strTitle, VistaWindow * pTarget )
{
	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(((WindowData *)pTarget->GetData())->m_iWindowId);
	wta->SetTitle(strTitle);
	wta->SetWindow(win);
	((WindowData *)pTarget->GetData())->m_strTitle = strTitle;
}

std::string VistaOpenSGDisplayBridge::GetWindowTitle
( const VistaWindow * pTarget )
{
	return ((WindowData *)pTarget->GetData())->m_strTitle;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowId                                                 */
/*                                                                            */
/*============================================================================*/
void * VistaOpenSGDisplayBridge::GetWindowId
( const VistaWindow * pTarget )
{
	return (void *)((WindowData *)pTarget->GetData())->m_iWindowId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugVistaWindow                                            */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugVistaWindow
( std::ostream & out, const VistaWindow * pTarget )
{
	pTarget->Debug(out);
}

/**
* Methods for viewport management.
*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateViewport                                              */
/*                                                                            */
/*============================================================================*/
VistaViewport * VistaOpenSGDisplayBridge::CreateViewport
( VistaDisplaySystem * pDisplaySystem, VistaWindow * pWindow,
 const VistaPropertyList & refProps)
{
	if (!pDisplaySystem)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create viewport..." << endl;
		vkernout << "                                      no display system given..." << endl;
		return NULL;
	}

	if (!pWindow)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create viewport..." << endl;
		vkernout << "                                      no window given..." << endl;
		return NULL;
	}

	// create new data container
	ViewportData * pData = new ViewportData();

	if (!pData)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create viewport..." << endl;
		vkernout << "                                      cannot create toolkit specific data..." << endl;
		return NULL;
	}

	VistaViewport * pViewport = NewViewport(pDisplaySystem, pWindow, pData);
	if (!pViewport)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create viewport..." << endl;
		delete pData;
		return NULL;
	}

	WindowData * pWindowData = (WindowData *)pWindow->GetData();

	// RWTH blue as background color
	osg::SolidBackgroundPtr bg = osg::SolidBackground::create();
	beginEditCP(bg);
	bg->setColor( osg::Color3f( 0.0f,0.4f,0.8f ) );
	endEditCP(bg);

	// get the real OpenSG root node
	//osg::NodePtr pOSGRootNode = ((VistaOpenSGNodeData*)GetVistaSystem()
	// ->GetGraphicsManager()->GetSceneGraph()->GetRealRoot()->GetData())->GetBaseNode();

	pData->m_bStereo = pWindowData->GetStereo(); // just in case somone asks :)
	if( pWindowData->GetStereo() ) // active STEREO ?
	{
		osg::StereoBufferViewportPtr StereoViewport;
		StereoViewport = osg::StereoBufferViewport::create();
		beginEditCP(StereoViewport);
		StereoViewport->setSize(0,0,1,1);
		StereoViewport->setBackground(bg);
		StereoViewport->setRoot(m_pRealRoot);
		StereoViewport->setLeftBuffer(true);
		StereoViewport->setRightBuffer(false);
		endEditCP(StereoViewport);
		pData->m_Viewport = StereoViewport;

		StereoViewport = osg::StereoBufferViewport::create();
		beginEditCP(StereoViewport);
		StereoViewport->setSize(0,0,1,1);
		StereoViewport->setBackground(bg);
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
		pData->m_Viewport->setBackground(bg);
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
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create viewport..." << endl;
		vkernout << "                                      OpenSG doesn't allow it..." << endl;
		delete pViewport;
		delete pData;
		return NULL;
	}

	//pViewport->SetPropertiesByList(refProps);
	pViewport->GetProperties()->SetPropertiesByList(refProps);

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

	VistaDisplaySystem * pDisplaySystem = pViewport->GetDisplaySystem();

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
void VistaOpenSGDisplayBridge::SetViewportPosition
( int x, int y, VistaViewport * pTarget )
{
	osg::ViewportPtr pViewport = ((ViewportData *)pTarget->GetData())->m_Viewport;
	IVistaWindowingToolkit *pWindowingToolkit = GetVistaSystem()->GetWindowingToolkit();
	int iWindowID = pWindowingToolkit->GetWindow();
	pWindowingToolkit->SetWindow(static_cast<int>(reinterpret_cast<long long>(pTarget->GetWindow()->GetWindowId())));

	float width = (float)pViewport->getPixelWidth();
	float height = (float)pViewport->getPixelHeight();

	float left = (float)x;
	float right = left+width;
	float bottom = (float)y;
	float top = bottom+height;


	beginEditCP(pViewport);
	pViewport->setSize( left, bottom, right, top );
	endEditCP(pViewport);

	if( ((ViewportData *)pTarget->GetData())->m_bStereo )
	{
		pViewport = ((ViewportData *)pTarget->GetData())->m_RightViewport;
		beginEditCP(pViewport);
		pViewport->setSize( left, bottom, right, top );
		endEditCP(pViewport);
	}
	pWindowingToolkit->SetWindow(iWindowID);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportPosition                                         */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::GetViewportPosition
( int & x, int & y, const VistaViewport * pTarget )
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
void VistaOpenSGDisplayBridge::SetViewportSize
( int w, int h, VistaViewport * pTarget )
{
	osg::ViewportPtr viewport = ((ViewportData *)pTarget->GetData())->m_Viewport;

	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();
	int win = wta->GetWindow();
	wta->SetWindow(static_cast<int>(reinterpret_cast<long long>(pTarget->GetWindow()->GetWindowId())));
	// ask the window for its size and not the scenegraph!
	int *winSize = wta->GetWindowSize();

	// lower-left is (0,0)
	float left  = static_cast<float>( viewport->getPixelLeft() );
	float bottom  = static_cast<float>( viewport->getPixelBottom() );
	float width  = static_cast<float>(w);
	float height  = static_cast<float>(h);

	float right = left + width;
	float top = bottom + height;

	beginEditCP(viewport);
	viewport->setSize( left, bottom, right, top );
	endEditCP(viewport);

	if( ((ViewportData *)pTarget->GetData())->m_bStereo )
	{
		viewport = ((ViewportData *)pTarget->GetData())->m_RightViewport;
		beginEditCP(viewport);
		viewport->setSize( left, bottom, right, top );
		endEditCP(viewport);
	}

	wta->SetWindow(win);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportSize                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::GetViewportSize
( int & w, int & h, const VistaViewport * pTarget )
{
	osg::ViewportPtr viewport = ((ViewportData *)pTarget->GetData())->m_Viewport;
	w = viewport->getPixelWidth();
	h = viewport->getPixelHeight();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugViewport                                               */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugViewport
( std::ostream & out, const VistaViewport * pTarget )
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
VistaProjection * VistaOpenSGDisplayBridge::CreateProjection
( VistaViewport * pViewport, const VistaPropertyList & refProps )
{
	if (!pViewport)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create projection..." << endl;
		vkernout << "                                      no viewport given..." << endl;
		return NULL;
	}

	// create new data container
	ProjectionData * pData = new ProjectionData;
	if (!pData)
	{
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create projection..." << endl;
		vkernout << "                                      cannot create toolkit specific data..." << endl;
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
		vkernout << " [VistaOpenSGDisplayBridge] - ERROR - unable to create projection..." << endl;
		delete pData;
		return NULL;
	}

	ProjectionData* pProjData = (ProjectionData*)pProjection->GetData();


	// stereo key must be set before setpropertiesbylist, since the set-functors for e.g.
	// clipping range depend on it being already set (in the projection data!)
	// this should be cleaned up on occasion...
	std::string sMode = refProps.GetStringValue( "STEREO_MODE" );
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
				vkernout << " [VistaOpenSGDisplayBridge] - WARNING - invalid stereo mode "
					<< iMode << " for stereo window..." << endl;
			}
		}
		else
		{
			if (iMode == VistaProjection::VistaProjectionProperties::SM_FULL_STEREO)
			{
				vkernout << " [VistaOpenSGDisplayBridge] - WARNING - invalid stereo mode "
					<< "FULL_STEREO for mono window..." << endl;
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
osg::RenderAction * VistaOpenSGDisplayBridge::GetRenderAction() const
{
	return m_pRenderAction;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   ReshapeFunction							                      */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::ReshapeFunction(int width, int height)
{
	WindowData * pWindow;
	std::map<std::string, VistaWindow *> mapWindows;

	// this is a static callback hook function for glut,
	// somehow, we have to get the context we are working on
	// we use the static member in this class scope to get
	// a hand on the display manager
	// this is not much better than retrieving the global
	// VistaSystem, but a bit better, as we do not need to know
	// the complete system at this point
	if(VistaOpenSGDisplayBridge::g_DispBridge
		&& VistaOpenSGDisplayBridge::g_DispBridge->GetDisplayManager())
		mapWindows = VistaOpenSGDisplayBridge::g_DispBridge->GetDisplayManager()->GetWindows();
	else
		return;


	std::map<std::string, VistaWindow *>::const_iterator winIter = mapWindows.begin();
	for( ; winIter != mapWindows.end(); ++winIter )
	{
		pWindow = (WindowData *)winIter->second->GetData();
		// adapt the window content size for the window that is beeing reshaped only
		if( pWindow->m_iWindowId == GetVistaSystem()->GetWindowingToolkit()->GetWindow() )
		{
			osg::beginEditCP(pWindow->m_ptrWindow);
			pWindow->m_ptrWindow->resize(width,height);
			osg::endEditCP(pWindow->m_ptrWindow);

			/// @todo one event for each viewport
			VistaDisplayEvent oEvent;
			oEvent.SetViewport((*winIter).second->GetViewport(0));
			oEvent.SetId(VistaDisplayEvent::VDE_RATIOCHANGE);
			GetVistaSystem()->GetEventManager()->ProcessEvent(&oEvent);
		}
	}
	GetVistaSystem()->GetWindowingToolkit()->Redisplay();
}

bool VistaOpenSGDisplayBridge::MakeScreenshot
			(const VistaWindow & oWin, const std::string & strFilenamePrefix,
			const bool &bNoScreenshotOnClients ) const
{
	// only works in standalone and servers
	if( bNoScreenshotOnClients && GetVistaSystem()->IsClient() && GetVistaSystem()->GetIsSlave() )
		return false;

	// get the OpenSG window data from the specified ViSTA window
	WindowData * window = (WindowData *)oWin.GetData();

	size_t iIndex = strFilenamePrefix.find_last_of( '.' );
	string sFileName;
	if( iIndex == string::npos )
	{
		sFileName = strFilenamePrefix + ".jpg";
	}
	else
	{
		string sExt = strFilenamePrefix.substr( iIndex );
		if( sExt == ".jpg" || sExt == ".png" || sExt == ".bmp"
			|| sExt == ".gif" || sExt == ".tif" )
		{
			sFileName = strFilenamePrefix;
		}
		else
		{
			sFileName = strFilenamePrefix + ".jpg";
		}
	}

	// create a foreground with screenshot functionality
	osg::FileGrabForegroundRefPtr fileGrab(osg::FileGrabForeground::create());

	// the FileGrabForeground has a bug, leaving the internal Image alive on deletion...
	// thus we handle the iamge buffer ourselfes
	osg::ImageRefPtr ptrImg(osg::Image::create());
	ptrImg->set(osg::Image::OSG_RGB_PF, 1);
	beginEditCP(fileGrab,
		osg::FileGrabForeground::ActiveFieldMask |
		osg::FileGrabForeground::NameFieldMask |
		osg::FileGrabForeground::ImageFieldMask);
	{
		fileGrab->setActive(true);
		// numbered screenshot series
		//fileGrab->setName((strFilename + "_%04d.jpg").c_str());
		//fileGrab->setName((strFilenamePrefix + ".gif").c_str());
		//fileGrab->setName((strFilenamePrefix + ".tif").c_str());
		//fileGrab->setName((strFilenamePrefix + ".jpg").c_str());
		fileGrab->setName(strFilenamePrefix.c_str());

		fileGrab->setImage(ptrImg);
	}
	endEditCP(fileGrab,
		osg::FileGrabForeground::ActiveFieldMask |
		osg::FileGrabForeground::NameFieldMask |
		osg::FileGrabForeground::ImageFieldMask);


	// add the grabber to the next best viewport
	window->m_ptrWindow->getPort(0)->getMFForegrounds()->push_back(fileGrab);

	// render once
	window->m_ptrWindow->render(m_pRenderAction);

	// deactivate grabber
	beginEditCP(fileGrab, osg::FileGrabForeground::ActiveFieldMask);
	fileGrab->setActive(false);
	endEditCP(fileGrab, osg::FileGrabForeground::ActiveFieldMask);

	// remove the grabber from the next best viewport
	window->m_ptrWindow->getPort(0)->getMFForegrounds()
		->erase(window->m_ptrWindow->getPort(0)->getMFForegrounds()->end() - 1);

	return true;
}

bool VistaOpenSGDisplayBridge::AddSceneOverlay(IVistaSceneOverlay *pDraw,
												const std::string &strViewportName)
{
	VistaViewport* viewport = m_pDMgr->GetViewportByName( strViewportName ) ;
	if( viewport != NULL )
	{
		ViewportData* vpdata = (ViewportData*)viewport->GetData() ;
		osg::VistaOpenSGGLOverlayForegroundPtr overlays = vpdata->GetOverlayForeground();
		if(overlays == osg::NullFC)
		{
			overlays = osg::VistaOpenSGGLOverlayForeground::create();
			if(overlays == osg::NullFC)
				return false;

			vpdata->m_overlays = overlays;

			osg::ViewportPtr osgvp = vpdata->GetOpenSGViewport();

			osgvp->getForegrounds().push_back(overlays);

			if(vpdata->GetOpenSGRightViewport())
			{
				vpdata->GetOpenSGRightViewport()->getForegrounds().push_back(overlays);
			}
		}

		beginEditCP(overlays, osg::VistaOpenSGGLOverlayForeground::GLOverlaysFieldMask);
		overlays->getMFGLOverlays()->push_back(pDraw);
		endEditCP(overlays, osg::VistaOpenSGGLOverlayForeground::GLOverlaysFieldMask);

		return true;
	}
	return false;
}

bool VistaOpenSGDisplayBridge::RemSceneOverlay(IVistaSceneOverlay *pDraw,
												const std::string &strVpName)
{
	VistaViewport* viewport = m_pDMgr->GetViewportByName( strVpName ) ;
	if( viewport != NULL )
	{
		ViewportData* vpdata = (ViewportData*)viewport->GetData() ;
		osg::ViewportPtr osgvp = vpdata->GetOpenSGViewport();

		osg::MField<void*> &overls = vpdata->GetOverlayForeground()->getGLOverlays();
		osg::MField<void*>::iterator it = overls.find(pDraw);

		if(it != overls.end())
		{
			overls.erase(it);
			return true;
		}
	}

	return false;
}


Vista2DText* VistaOpenSGDisplayBridge::Add2DText(const std::string &strViewportName)
{
	Vista2DText * Text2DObjekt = NULL ;

	VistaViewport* viewport = m_pDMgr->GetViewportByName( strViewportName ) ;
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
		vkernerr << "OpenSGDisplayBridge::Add2DText(): WARNING: non-existent viewport '"
			<< strViewportName << "' specified!" << std::endl ;
	}

	return Text2DObjekt;
}

Vista2DBitmap*	VistaOpenSGDisplayBridge::Add2DBitmap	(const std::string &strViewportName )
{
	Vista2DBitmap *pBm = NULL;
	VistaViewport* viewport = m_pDMgr->GetViewportByName( strViewportName ) ;
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

Vista2DLine*	VistaOpenSGDisplayBridge::Add2DLine	(const std::string &strWindowName )
{
	return NULL;
}

Vista2DRectangle*	VistaOpenSGDisplayBridge::Add2DRectangle	(const std::string &strWindowName )
{
	return NULL;
}

bool VistaOpenSGDisplayBridge::Get2DOverlay(const std::string &strWindowName, std::list<Vista2DDrawingObject*>&)
{
	return false;
}


bool VistaOpenSGDisplayBridge::DoLoadBitmap(const std::string &strNewFName,
											 unsigned char** pBitmapData,
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

		unsigned char* pNewBitmap = image->getData();
		nWidth     = image->getWidth();
		nHeight    = image->getHeight();
		bAlpha     = image->hasAlphaChannel();

		//copy buffer
		int iBuffersize = nWidth * nHeight * (bAlpha ? 4 : 3);

		// alloc memory
		*pBitmapData = new unsigned char[iBuffersize];

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
	VistaViewport* viewport = m_pDMgr->GetViewportByName( sVpName ) ;
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

bool VistaOpenSGDisplayBridge::CreateDisplaysFromIniFile(const std::string &strDisplayIniFile,
														  const std::string &strDisplaySection)
{
	return true;
}

void VistaOpenSGDisplayBridge::SetShowCursor(bool bShowCursor)
{
	m_bShowCursor = bShowCursor;

	IVistaWindowingToolkit *wta = GetVistaSystem()->GetWindowingToolkit();

	// store current window
	int window = wta->GetWindow();

	// iterate over all glut window contexts from the DM
	VistaOpenSGDisplayBridge::WindowData * pWindow;
	map<string, VistaWindow *> mapWindows = GetDisplayManager()->GetWindows();
	map<string, VistaWindow *>::iterator winIter = mapWindows.begin();
	for( ; winIter != mapWindows.end(); ++winIter )
	{
		pWindow = (VistaOpenSGDisplayBridge::WindowData *)winIter->second->GetData();
		// set glut window context to the current window
		wta->SetWindow(pWindow->GetWindowId());
		// toggle cursor
		bShowCursor ? wta->SetCursor(wta->CURSOR_NORMAL) : wta->SetCursor(wta->CURSOR_NONE);
	}

	// restore previous glut window context
	wta->SetWindow(window);
}
