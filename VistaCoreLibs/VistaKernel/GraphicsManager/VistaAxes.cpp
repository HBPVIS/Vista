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

#include "VistaAxes.h"

#include <iostream>

#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include "../VistaKernelOut.h"


#include <cstdio>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR
VistaAxes::VistaAxes(	VistaSG * pVistaSG,
						float sizeX, float sizeY, float sizeZ )
{
	if( sizeX < 0.01f ||
		sizeY < 0.01f ||
		sizeZ < 0.01f )
	{
		vkernerr << "[VistaAxes] Size to small!" << std::endl;
		return;
	}

	VistaGroupNode * me = pVistaSG->NewGroupNode( NULL );
	VistaGeometryFactory geomFactory(pVistaSG);

	// create axes
	m_axes.push_back(geomFactory.CreateCone(0.05f,0.05f,sizeX-0.1f,32,32,32,VistaColorRGB::RED,true,true,true));
	m_axes.push_back(geomFactory.CreateCone(0.05f,0.05f,sizeY-0.1f,32,32,32,VistaColorRGB::GREEN,true,true,true));
	m_axes.push_back(geomFactory.CreateCone(0.05f,0.05f,sizeZ-0.1f,32,32,32,VistaColorRGB::BLUE,true,true,true));

	// create tips
	m_tips.push_back(geomFactory.CreateCone(0.07f,0,0.1f,32,32,32,VistaColorRGB::GRAY,true,true,true));
	m_tips.push_back(geomFactory.CreateCone(0.07f,0,0.1f,32,32,32,VistaColorRGB::GRAY,true,true,true));
	m_tips.push_back(geomFactory.CreateCone(0.07f,0,0.1f,32,32,32,VistaColorRGB::GRAY,true,true,true));

	// center sphere
	m_center = geomFactory.CreateSphere(0.1f,21,VistaColorRGB::GRAY);

	// put all together
	m_group = pVistaSG->NewGroupNode(NULL);

	// X axis
	VistaTransformNode *transX = pVistaSG->NewTransformNode(m_group);
	transX->SetRotation( VistaQuaternion( VistaAxisAndAngle( VistaVector3D(0,0,1), Vista::DegToRad(90) ) ) );
	transX->SetTranslation( VistaVector3D( (sizeX-0.1f) * 0.5f, 0, 0 ) );
	pVistaSG->NewGeomNode(transX,m_axes[0]);
	// tip
	VistaTransformNode *transXtip = pVistaSG->NewTransformNode(transX);
	transXtip->SetRotation( VistaQuaternion( VistaAxisAndAngle( VistaVector3D(0,0,1), Vista::DegToRad(180) ) ) );
	transXtip->SetTranslation( VistaVector3D( 0, -sizeX*0.5f, 0 ) );
	pVistaSG->NewGeomNode(transXtip,m_tips[0]);

	// Y axis
	VistaTransformNode *transY = pVistaSG->NewTransformNode(m_group);
	transY->SetTranslation( VistaVector3D( 0, (sizeY-0.1f) * 0.5f, 0 ) );
	pVistaSG->NewGeomNode(transY,m_axes[1]);
	// tip
	VistaTransformNode *transYtip = pVistaSG->NewTransformNode(transY);
	transYtip->SetTranslation( VistaVector3D( 0, sizeY*0.5f, 0 ) );
	pVistaSG->NewGeomNode(transYtip,m_tips[1]);

	// Z axis
	VistaTransformNode *transZ = pVistaSG->NewTransformNode(m_group);
	transZ->SetRotation( VistaQuaternion( VistaAxisAndAngle( VistaVector3D(1,0,0), Vista::DegToRad(90) ) ) );
	transZ->SetTranslation( VistaVector3D( 0, 0, (sizeZ-0.1f) * 0.5f ) );
	pVistaSG->NewGeomNode(transZ,m_axes[2]);
	// tip
	VistaTransformNode *transZtip = pVistaSG->NewTransformNode(transZ);
	//transZtip->SetRotation( VistaQuaternion( VistaAxisAndAngle( VistaVector3D(0,0,1), Vista::DegToRad(180) ) ) );
	transZtip->SetTranslation( VistaVector3D( 0, sizeZ*0.5f, 0 ) );
	pVistaSG->NewGeomNode(transZtip,m_tips[2]);

	// center
	pVistaSG->NewGeomNode(m_group,m_center);

	m_group->SetName("VistaAxes");
	//Scale(width, height, length);
}

// DESTRUCTOR
VistaAxes::~VistaAxes()
{    
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
IVistaNode *VistaAxes::GetVistaNode()
{
	return m_group;
}

#if 0
bool VistaAxes::Init()
{
	
	m_pGroup = m_pVistaSG->NewGroupNode(m_pVistaNode);
	m_pGroup->SetName("cs_Group");

						// Coordinate System (unit size)
	m_pOrigin = new VistaSphere(m_pVistaSG, m_pGroup, 0.1f, 10, 10, false);
	m_pOrigin->SetName("cs_Origin");
	//m_pOrigin->ShowBoundingBox(true);

						// X Axis
	VistaVector3D axis[3];
	axis[0][2] = 1;
	axis[1][1] = 0;
	axis[2][0] = 1;
	float rotate[3];
	rotate[0] = -90;
	rotate[1] =   0;
	rotate[2] =  90;
	VistaColorRGB colors[3];
	colors[0] =  VistaColorRGB(0.8f,0.2f,0.2f);
	colors[1] =  VistaColorRGB(0.2f,0.8f,0.2f);
	colors[2] =  VistaColorRGB(0.2f,0.2f,0.8f);

	for(int i=0; i < 3; ++i)
	{
		static char buffer[32];
		sprintf(buffer, "(%3.1f, %3.1f, %3.1f)-axis", axis[i][0], axis[i][1], axis[i][2]);

		m_pAxis[i] = new VistaCylinder(m_pVistaSG, m_pGroup, 0.81f, 0.05f, 10, false, false);
		m_pAxis[i]->SetColor(colors[i]);
		m_pAxis[i]->SetName(buffer);
		m_pAxis[i]->SetTranslation(VistaVector3D(0.0f,0.49f,0.0f));
		if(rotate[i])
		m_pAxis[i]->Rotate( VistaQuaternion( VistaAxisAndAngle( axis[i], Vista::DegToRad(rotate[i]) )));

		m_pAxisHat[i] = new VistaCone(m_pVistaSG, m_pGroup, 0.2f, 0.1f, 10, true, true);
		m_pAxisHat[i]->SetColor(colors[i]);
		sprintf(buffer, "(%3.1f, %3.1f, %3.1f)-arrow", axis[i][0], axis[i][1], axis[i][2]);

		m_pAxisHat[i]->SetName(buffer);
		m_pAxisHat[i]->SetTranslation(VistaVector3D(0.0f,0.9f,0.0f));
			if(rotate[i] != 0.0f)
			m_pAxisHat[i]->Rotate( VistaQuaternion( VistaAxisAndAngle( axis[i], Vista::DegToRad(rotate[i]) )));
	 }
	return true;
 }
#endif