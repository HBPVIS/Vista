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
 * $Id$
 */

#include "GeometryDemoAppl.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
GeometryDemoAppl::GeometryDemoAppl()
: m_pVistaSystem( NULL )
{
}

GeometryDemoAppl::~GeometryDemoAppl()
{
	delete m_pVistaSystem;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool GeometryDemoAppl::Init( int argc, char  *argv[] )
{
	m_pVistaSystem = new VistaSystem;

	std::list<std::string> liSearchPaths;

	// here is a shared configfile used for the demos
	liSearchPaths.push_back( "../configfiles/" );
	m_pVistaSystem->SetIniSearchPaths( liSearchPaths );

	// ViSTA's standard intro message
	m_pVistaSystem->IntroMsg();

	if( !m_pVistaSystem->Init( argc, argv ) )
		return false;

	if( m_pVistaSystem->GetDisplayManager()->GetDisplaySystem()==0 )
		VISTA_THROW( "No DisplaySystem found", 1 );

	// Get the scenegraph, which manages the drawable objects 
	VistaSceneGraph *pSceneGraph = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph();

	// First, we need some geometry to draw. For simple shapes, we can
	// use the VistaGeometryFactory to create them.
	// Since Geometries can be reused multiple times, they are reference-counted internally
	// for each GeomNode that uses them, and are automatically deleted once the last
	// GeomNode referencing them is deleted. Take care, however, that this means
	// that a Geometry you created will become invalid once all referencing
	// nodes are deleted, so you cannot reuse it after cleaning the whole scenegraph.
	VistaGeometryFactory oGeometryFactory( pSceneGraph );
	// Create the box
	m_pBox = oGeometryFactory.CreateBox ();
	// Create a sphere
	m_pSphere = oGeometryFactory.CreateSphere( 1, 300, VistaColor::WHITE );
	// some nice texture
	m_pSphere->SetTexture( "../data/bus.jpg");

	// We now have created the geometries, but still need to tell the scenegraph to render them.
	// Therefore, we first will use to types of nodes here: TransformNodes and GeomNodes
	// GeomNodes tell the scenegraph to actually draw the Geometry linked to them
	// TransformNodes build a hierarchy that determines the position of the objects in GeomNodes
	// Note that if the nodes are added to the SceneGraph's tree (i.e. they can be reached
	// from the SceneGraphs root node), they will automatically be deleted,
	// so we don't have to care about that
	
	// we will first create two TransformNodes: One that moves the whole scenery, and
	// one that moves the Sphere relative to the box
	VistaTransformNode* pSceneryTrans = pSceneGraph->NewTransformNode( pSceneGraph->GetRoot() );
	pSceneryTrans->Translate( 0, 0, -2.5 );
	// notice that we now add the SpherreTrans as childnode of the SceneryTrans. Thus, in the end,
	// the sphere will be affected by both transformnodes, and is thus moved by (1,1,-3)
	VistaTransformNode* pSphereTrans = pSceneGraph->NewTransformNode( pSceneryTrans );
	pSphereTrans->Translate( 1, 1, -1 );

	// We now add the two GeomNodes holding our box and our sphere
	pSceneGraph->NewGeomNode( pSceneryTrans, m_pBox );
	pSceneGraph->NewGeomNode( pSphereTrans, m_pSphere );

	m_pVistaSystem->GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle( argv[0] );

	return true;
}

void GeometryDemoAppl::Run ()
{
	// Start Universe
	m_pVistaSystem->Run();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
