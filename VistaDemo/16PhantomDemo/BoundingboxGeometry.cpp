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
/*
 * $Id: code_skel.h,v 1.3 2008/02/27 14:14:39 tb552214 Exp $
 */
#include "BoundingboxGeometry.h"

#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeomNode.h>

BoundingboxGeometry::BoundingboxGeometry( VistaSG * vistaSG, VistaBoundingBox &bb )
{
	float * min = bb.m_min;
	float * max = bb.m_max;

	pNode = vistaSG->NewGroupNode(NULL);

	// creating lines
	pNode->AddChild(createLine(vistaSG, VistaVector3D(min[0],min[1],min[2]), VistaVector3D(min[0],min[1],max[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(min[0],min[1],max[2]), VistaVector3D(max[0],min[1],max[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(max[0],min[1],max[2]), VistaVector3D(max[0],min[1],min[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(max[0],min[1],min[2]), VistaVector3D(min[0],min[1],min[2])));

	pNode->AddChild(createLine(vistaSG, VistaVector3D(min[0],min[1],min[2]), VistaVector3D(min[0],max[1],min[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(min[0],min[1],max[2]), VistaVector3D(min[0],max[1],max[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(max[0],min[1],max[2]), VistaVector3D(max[0],max[1],max[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(max[0],min[1],min[2]), VistaVector3D(max[0],max[1],min[2])));

	pNode->AddChild(createLine(vistaSG, VistaVector3D(max[0],max[1],min[2]), VistaVector3D(min[0],max[1],min[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(min[0],max[1],min[2]), VistaVector3D(min[0],max[1],max[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(min[0],max[1],max[2]), VistaVector3D(max[0],max[1],max[2])));
	pNode->AddChild(createLine(vistaSG, VistaVector3D(max[0],max[1],max[2]), VistaVector3D(max[0],max[1],min[2])));

	// balls...
	// i := min
	// x := max
	VistaGeometryFactory gf(vistaSG);
	VistaGeometry *ball = gf.CreateSphere(0.02f);

	VistaTransformNode *iii = vistaSG->NewTransformNode(pNode);
	iii->SetTranslation(min);
	vistaSG->NewGeomNode(iii,ball);

	VistaTransformNode *iix = vistaSG->NewTransformNode(pNode);
	iix->SetTranslation(min[0],min[1],max[2]);
	vistaSG->NewGeomNode(iix,ball);

	VistaTransformNode *xix = vistaSG->NewTransformNode(pNode);
	xix->SetTranslation(max[0],min[1],max[2]);
	vistaSG->NewGeomNode(xix,ball);

	VistaTransformNode *xii = vistaSG->NewTransformNode(pNode);
	xii->SetTranslation(max[0],min[1],max[2]);
	vistaSG->NewGeomNode(xii,ball);

	VistaTransformNode *ixi = vistaSG->NewTransformNode(pNode);
	ixi->SetTranslation(min[0],max[1],min[2]);
	vistaSG->NewGeomNode(ixi,ball);

	VistaTransformNode *ixx = vistaSG->NewTransformNode(pNode);
	ixx->SetTranslation(min[0],max[1],max[2]);
	vistaSG->NewGeomNode(ixx,ball);

	VistaTransformNode *xxx = vistaSG->NewTransformNode(pNode);
	xxx->SetTranslation(max);
	vistaSG->NewGeomNode(xxx,ball);

	VistaTransformNode *xxi = vistaSG->NewTransformNode(pNode);
	xxi->SetTranslation(max[0],max[1],min[2]);
	vistaSG->NewGeomNode(xxi,ball);

}

BoundingboxGeometry::~BoundingboxGeometry()
{
}

VistaGroupNode* BoundingboxGeometry::getVistaNode()
{
	return pNode;
}

/**
 * Little helper to connect two points with an cone
 */
VistaTransformNode* BoundingboxGeometry::createLine( VistaSG * vistaSG, const VistaVector3D &a, const VistaVector3D &b )
{

	VistaTransformNode *trans = vistaSG->NewTransformNode(NULL);
	VistaVector3D direction = b - a;
	float length = direction.GetLength();
	VistaQuaternion q(VistaVector3D(0,1,0),direction);
	VistaGeometryFactory gf(vistaSG);
	VistaGeometry *line = gf.CreateCone(0.01f,0.01f,length);
	vistaSG->NewGeomNode(trans,line);

	// move up to avoid rotate around the center
	trans->Translate( VistaVector3D(0,length/2.0f,0) );
	// rotate to new direction
	trans->Rotate(q);
	// move to position
	trans->Translate(a);

	return trans;
}
