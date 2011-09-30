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
// $Id: VistaOpenSGGeometryTools.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#if defined(WIN32)
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif


#include "VistaOpenSGGeometryTools.h"

#include <VistaBase/VistaVersion.h>

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGGeoFunctions.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
COpenSGGeometryGrabber::COpenSGGeometryGrabber()
{
}

COpenSGGeometryGrabber::~COpenSGGeometryGrabber()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void COpenSGGeometryGrabber::Traverse(OSG::NodePtr node)
{
	m_lstAllGeoNodes.clear();
	m_setAllGeos.clear();

	OSG::traverse(node,
		OSG::osgTypedMethodFunctor1ObjPtrCPtrRef<
			OSG::Action::ResultE,
			COpenSGGeometryGrabber,
			OSG::NodePtr>
			(this, &COpenSGGeometryGrabber::Enter),
		OSG::osgTypedMethodFunctor2ObjPtrCPtrRef<
			OSG::Action::ResultE,
			COpenSGGeometryGrabber,
			OSG::NodePtr,
			OSG::Action::ResultE>
			(this, &COpenSGGeometryGrabber::Leave));
}


const std::list<OSG::NodePtr>* COpenSGGeometryGrabber::GetNodes() const
{
	return &m_lstAllGeoNodes;
}

const std::set<OSG::GeometryPtr>* COpenSGGeometryGrabber::GetGeometries() const
{
	return &m_setAllGeos;
}

OSG::Action::ResultE COpenSGGeometryGrabber::Enter(OSG::NodePtr& node)
{
	if(node->getCore()->getType().isDerivedFrom(OSG::Geometry::getClassType()))
	{
		m_lstAllGeoNodes.push_back(node);
		OSG::GeometryPtr geo = OSG::GeometryPtr::dcast(node->getCore());
		m_setAllGeos.insert(geo);
	}
	return OSG::Action::Continue;
}

OSG::Action::ResultE COpenSGGeometryGrabber::Leave(OSG::NodePtr& node, OSG::Action::ResultE res)
{
	return res;
}

static OSG::NodePtr GetOpenSGNode(IVistaNode *pNode)
{
	VistaNode *pVistaNode = dynamic_cast<VistaNode*>(pNode);
	if(!pVistaNode) return OSG::NullFC;

	VistaOpenSGNodeData *pData
		= dynamic_cast<VistaOpenSGNodeData*>(pVistaNode->GetData());

	if(!pData) return OSG::NullFC;

	return pData->GetNode();
}

bool CalcVertexNormalsOnSubtree(IVistaNode *pNode, const float &fCreaseAngle)
{
	OSG::NodePtr node = GetOpenSGNode(pNode);
	if(!node)
		return false;

	COpenSGGeometryGrabber grabber;
	grabber.Traverse(node);

	const COpenSGGeometryGrabber::GeoSet* geos = grabber.GetGeometries();
	COpenSGGeometryGrabber::GeoSet::const_iterator it;
	for(it = geos->begin(); it != geos->end(); ++it)
	{
		OSG::calcVertexNormals(*it, fCreaseAngle);
	}

	return true;
}

bool CalcFaceNormalsOnSubtree  (IVistaNode *pNode)
{
	OSG::NodePtr node = GetOpenSGNode(pNode);
	if(!node)
		return false;

	COpenSGGeometryGrabber grabber;
	grabber.Traverse(node);

	const COpenSGGeometryGrabber::GeoSet* geos = grabber.GetGeometries();
	COpenSGGeometryGrabber::GeoSet::const_iterator it;
	for(it = geos->begin(); it != geos->end(); ++it)
	{
		OSG::calcFaceNormals(*it);
	}

	return true;
}

bool CalcVertexNormals(VistaGeometry *pGeo, const float &fCreaseAngle)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	OSG::calcVertexNormals(pData->GetGeometry(), fCreaseAngle);

	return true;
}


bool CalcFaceNormals(VistaGeometry *pGeo)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	OSG::calcFaceNormals(pData->GetGeometry());

	return true;
}

/**
 *	@todo Return ViSTA geometry. 
 */

bool CalcVertexNormalsGeo(VistaGeometry *pGeo, const float &fCreaseAngle, bool pCalc)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	if(pCalc)
		OSG::calcVertexNormals(pData->GetGeometry());
	OSG::NodePtr p = OSG::calcVertexNormalsGeo(pData->GetGeometry(),0.1);
	osg::NodePtr parent = pData->GetGeometry()->getParents()[0];
	beginEditCP(parent);
		parent->addChild(p);
	endEditCP(parent);
	
	return true;
}

/**
 *	@todo Return ViSTA geometry. 
 */

bool CalcFaceNormalsGeo(VistaGeometry *pGeo, bool pCalc)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	if(pCalc)
		OSG::calcFaceNormals(pData->GetGeometry());
	OSG::NodePtr p = OSG::calcFaceNormalsGeo(pData->GetGeometry(),1);
	osg::NodePtr parent = pData->GetGeometry()->getParents()[0];
	beginEditCP(parent);
		parent->addChild(p);
	endEditCP(parent);
	
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


