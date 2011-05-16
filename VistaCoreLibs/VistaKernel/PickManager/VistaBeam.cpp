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

#include "VistaBeam.h"

#include <VistaKernel/VistaKernelOut.h>

#include "../GraphicsManager/VistaSG.h"
#include "../GraphicsManager/VistaGeometry.h"
#include "../GraphicsManager/VistaGeomNode.h"
#include "../GraphicsManager/VistaGroupNode.h"
#include "../GraphicsManager/VistaTransformNode.h"
#include "../GraphicsManager/VistaGroupNode.h"

#include "../EventManager/VistaEvent.h"
#include "../EventManager/VistaEventManager.h"
#include "../EventManager/VistaSystemEvent.h"
//#include "../EventManager/VistaInputEvent.h"

//#include "../InteractionManager/VistaInputDevice.h"

#include <cassert>


/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaBeam::VistaBeam(VistaEventManager *pEvMgr,
					   VistaGraphicsManager *pGrMgr,
					   IVistaTransformable *pTransform)
					   : m_pEvMgr(pEvMgr)
					   , m_pGrMgr(pGrMgr)
					   , m_pTransform(pTransform)
					   , m_pGeomNode(NULL)
					   , m_pGeomTransform(NULL)
					   , m_fLength(VISTA_BEAM_DEFAULT_LENGTH)
					   , m_fWidth(VISTA_BEAM_DEFAULT_WIDTH)
					   , m_bEnabled(true)
{
#ifdef DEBUG
	vkernout << " [VistaBeam] >> CONSTRUCTOR <<" << endl;
#endif

	for (int i = 0; i < 3; i++)
		m_p3DOrigin[i] = 0.0;	

	for (int i = 0; i < 4; i++)
		m_qOrientation[i] = 0.0;

	m_qOrientation[3] = 1.0;
	

	// construct beam geomtry and add new geom node to scene graph
	bool bOK = BuildBeam();
	assert(bOK);

	// register observer in EventManager for system events
	m_pEvMgr->RegisterObserver(this, VistaSystemEvent::GetTypeId());

	// update origin and orientation with input device data
    float aM[16];
    m_pTransform->GetWorldTransform(aM);

    VistaTransformMatrix m(aM);

    m.GetTranslation(m_p3DOrigin);
    m_qOrientation = VistaQuaternion(m);

	PositionNode();
}

VistaBeam::~VistaBeam() 
{
	/** @todo does no proper cleanup!! Something is leaking here!!!*/

	#ifdef DEBUG
		vkernout << " [VistaBeam] >> DESTRUCTOR <<" << endl;
	#endif

	// important! don't forget to unregister the observer in event manager!
	m_pEvMgr->UnregisterObserver(this, VistaSystemEvent::GetTypeId());


	VistaSG *pSG;
	if (m_pGrMgr && ((pSG = m_pGrMgr->GetSceneGraph())))
	{
		pSG->DeleteSubTree(m_pGeomTransform);
	}

	m_pGeomNode		= NULL;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   BuildBeam                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaBeam::BuildBeam(void)
{
	VistaSG *pSceneGraph = m_pGrMgr->GetSceneGraph();
	VistaGroupNode *pRoot = pSceneGraph->GetRoot();
	VistaColorRGB col;
	
	if(m_pGeomNode)
	{
		// save color for later re-apply
		col = this->GetBeamColor();
		// clear old reference
		// note: this can kill the old geometry
		// (iff unused)
		m_pGeomNode->SetGeometry(NULL); 
//		m_pGeometry = NULL;
	}

	// create vertices and push into list
	vector<VistaIndexedVertex>     listVertices;

	vector<VistaVector3D>   listCoords;
	vector<VistaVector3D>   listTextureCoords2D;
	vector<VistaVector3D>   listNormals;
	vector<VistaColorRGB>   listColors;

	listCoords.reserve(8);
	listVertices.reserve(24);

	// default vertex format
	// (COORDINATE, COLOR_NONE, NORMAL_AUTO, TEXTURE_COORD_NONE)
	VistaVertexFormat  vFormat;

	float fVertexOffset = m_fWidth / 2.0f;

	// create vertices
	// front face
	listCoords.push_back(VistaVector3D(-fVertexOffset, -fVertexOffset, 0 )); 
	listCoords.push_back(VistaVector3D(fVertexOffset, -fVertexOffset, 0 ));
	listCoords.push_back(VistaVector3D(fVertexOffset, fVertexOffset, 0 ));
	listCoords.push_back(VistaVector3D(-fVertexOffset, fVertexOffset, 0 ));

	// back face
	listCoords.push_back(VistaVector3D(-fVertexOffset, -fVertexOffset, m_fLength));
	listCoords.push_back(VistaVector3D(fVertexOffset, -fVertexOffset, m_fLength ));
	listCoords.push_back(VistaVector3D(fVertexOffset, fVertexOffset, m_fLength ));
	listCoords.push_back(VistaVector3D(-fVertexOffset, fVertexOffset, m_fLength ));


	int i;
	for (i=0; i<8; ++i)
	{
		listVertices.push_back(VistaIndexedVertex(i));
	}

	for (i=0; i<4; ++i)
	{
		listVertices.push_back(VistaIndexedVertex(i));
		listVertices.push_back(VistaIndexedVertex((i+1)%4));
		listVertices.push_back(VistaIndexedVertex(((i+1)%4)+4));
		listVertices.push_back(VistaIndexedVertex(i+4));
	}

	// we do not use vertex colors but materials (-> lighting)
	//listColors.clear();

	// create new geometry
	VistaGeometry *pGeo = pSceneGraph->NewIndexedGeometry(listVertices, listCoords,
											   listTextureCoords2D, listNormals,
											   listColors, vFormat, 
											   VistaGeometry::VISTA_FACE_TYPE_QUADS);

	if (!pGeo)
	{
		vkernout << " [VistaBeam]	BuildBeam not successful! ." << endl;
		return false;
	}

	// if geometry successfully created => add node to scene graph
	if (m_pGeomNode)
	{
		m_pGeomNode->SetGeometry(pGeo);
		SetIsEnabled(true);
		vkernout << " [VistaBeam]	Beam rebuilt."<< endl;
	}
	else
	{
		m_pGeomTransform = pSceneGraph->NewTransformNode(pRoot);
		m_pGeomNode = pSceneGraph->NewGeomNode(m_pGeomTransform, pGeo);
		m_pGeomNode->SetName("ViSTA Beam");
		SetIsEnabled(true);
		vkernout << " [VistaBeam]	Beam enabled" << endl;
	}

	// (re-)set the beam color
	SetBeamColor(col);
	return true;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaBeam::Init(void) 
{	
	#ifdef DEBUG
		vkernout << " [VistaBeam] >> Init is deprecated. Do not call it! <<" << endl;
	#endif

	//if (!m_pEvMgr || !m_pGrMgr || !m_pInputDevice)
	//	return false;

	//int i;
	//for (i=0; i<3; i++)
	//	m_p3DOrigin[i] = 0.0;	
	//for (i=0; i<4; i++)
	//	m_qOrientation[i] = 0.0;
	//m_qOrientation[3] = 1.0;
	//
	//m_fLength	=	VISTA_BEAM_DEFAULT_LENGTH;
	//m_fWidth	=	VISTA_BEAM_DEFAULT_WIDTH;
	//m_bEnabled	=	true;

	//// construct beam geomtry and add new geom node to scene graph
	//if (!BuildBeam())
	//	return false;

	//// register observer in EventManager for system events
	//m_pEvMgr->RegisterObserver(this, VistaSystemEvent::GetTypeId());

	//// update origin and orientation with input device data
	//m_pInputDevice->GetPosition(m_p3DOrigin);
	//m_pInputDevice->GetOrientation(m_qOrientation);
	//PositionNode();

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetIsEnabled                                    */
/*                                                                            */
/*============================================================================*/
void VistaBeam::SetIsEnabled(bool v) 
{
	if (m_pGeomNode)
		m_pGeomNode->SetIsEnabled(v);
}

bool VistaBeam::GetIsEnabled(void) const
{
	if (m_pGeomNode)
		return 	m_pGeomNode->GetIsEnabled();
	else 
		return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetOrigin/SetOrigin                                         */
/*                                                                            */
/*============================================================================*/
void VistaBeam::GetOrigin(VistaVector3D &p3DOrigin) const
{	
	p3DOrigin = m_p3DOrigin;
}

void VistaBeam::SetOrigin(const VistaVector3D &p3DOrigin) 
{
	m_p3DOrigin = p3DOrigin;
	PositionNode();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDirection/SetDirection                                   */
/*                                                                            */
/*============================================================================*/
void VistaBeam::GetDirection(VistaVector3D &v3Direction) const
{	
	v3Direction = m_qOrientation.Rotate( VistaVector3D( 0, 0, -1 ) );
}

void VistaBeam::SetDirection(const VistaVector3D &v3Direction) 
{
	VistaVector3D vDir(v3Direction);
	vDir.Normalize();

	m_qOrientation = VistaQuaternion( VistaVector3D( 0, 0, -1 ), v3Direction );

	PositionNode();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetOrientation/SetOrientation                               */
/*                                                                            */
/*============================================================================*/
void VistaBeam::GetOrientation(VistaQuaternion &qOrientation) const
{
//	VistaQuaternion qTemp;
//	m_pGeomNode->GetRotation(&qTemp);
	qOrientation = m_qOrientation;
}

void VistaBeam::SetOrientation(const VistaQuaternion &qOrientation) 
{
	m_qOrientation = qOrientation;
	PositionNode();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetLength/SetLength                                         */
/*                                                                            */
/*============================================================================*/
float VistaBeam::GetLength(void) const
{
	return m_fLength;
}

void VistaBeam::SetLength(const float &fLength) 
{
	// set new length
	m_fLength = fLength;

	// and rebuild beam
	BuildBeam();

	// necessary to update position (origin) and orientation values
//	Update();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWidth/SetWidth                                          */
/*                                                                            */
/*============================================================================*/
float VistaBeam::GetWidth(void) const
{
	return m_fWidth;
}

void VistaBeam::SetWidth(const float &fWidth) 
{
	// set new width
	m_fWidth = fWidth;

	// and rebuild beam
	BuildBeam();

	// necessary to update position (origin) and orientation values
//	Update();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   PositionNode                                                */
/*                                                                            */
/*============================================================================*/
void VistaBeam::PositionNode() 
{	
	//if geom node existent update position (origin) and oriantation data
	if (m_pGeomNode)
	{
		VistaSG *pSceneGraph = m_pGrMgr->GetSceneGraph();
		VistaGroupNode *pRoot = pSceneGraph->GetRoot();

		VistaTransformMatrix m4Xform;
		m4Xform.SetToIdentity();
		//pRoot->GetTransform(m4Xform);

		VistaTransformMatrix m4XformInv = m4Xform.GetInverted();
		m4Xform = VistaTransformMatrix(m_qOrientation);
		m4Xform.SetColumn(3, m_p3DOrigin);

		m4Xform = m4XformInv * m4Xform;

		// apply translation and rotation to geometry node
		m_pGeomTransform->SetTransform(m4Xform);
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Notify                                                      */
/*                                                                            */
/*============================================================================*/
void VistaBeam::Notify(const VistaEvent *pEvent)
{
	// check whether event is a system event
	if (pEvent->GetType() == VistaSystemEvent::GetTypeId())
	{
		// check whether its the pre-draw graphics event
		VistaSystemEvent *pSystemEvent = (VistaSystemEvent *) pEvent;
		if (pSystemEvent->GetId() == VistaSystemEvent::VSE_PREGRAPHICS)
		{
            // hmm... effectively, this is a polling approach... do not like
            // it, but then... I never liked the BEAM.
            // maybe there is a more efficient way to do this?
            float aM[16];
            m_pTransform->GetWorldTransform(aM);

            VistaTransformMatrix m(aM);

            m.GetTranslation(m_p3DOrigin);
            m_qOrientation = VistaQuaternion(m);

			PositionNode();
		}
	}
}

void VistaBeam::SetBeamColor(const VistaColorRGB &col)
{
	assert(m_pGeomNode);
	VistaGeometry *pGeo = m_pGeomNode->GetGeometry();
	if(pGeo)
	{
		pGeo->SetColor(col);
	}
}

VistaColorRGB VistaBeam::GetBeamColor() const
{
	assert(m_pGeomNode);
	VistaGeometry *pGeo = m_pGeomNode->GetGeometry();
	if(pGeo)
		return pGeo->GetColor(0);
	
	return VistaColorRGB();
}


