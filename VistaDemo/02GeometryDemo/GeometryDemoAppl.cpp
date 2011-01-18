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
#include <VistaKernel/GraphicsManager/VistaSG.h>
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
GeometryDemoAppl::GeometryDemoAppl( int argc, char  *argv[] )
: m_pVistaSystem( new VistaSystem )
{
	std::list<std::string> liSearchPaths;

	// here is a shared configfile used
	// for your application use:
	liSearchPaths.push_back( "../configfiles/" );
	m_pVistaSystem->SetIniSearchPaths( liSearchPaths );

	// ViSTA's standard intro message
	m_pVistaSystem->IntroMsg();

	if( !m_pVistaSystem->Init( argc, argv ) )
		return;

	if( m_pVistaSystem->GetDisplayManager()->GetDisplaySystem()==0 )
		VISTA_THROW( "No DisplaySystem found", 1 );

	// Get the scenegraph
	VistaSG *pSceneGraph = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph();
	// Create the Geometryfactory
	VistaGeometryFactory oGeometryFactory( pSceneGraph );
	// Create the box
	VistaGeometry *pBox = oGeometryFactory.CreateBox ();
	// Create a sphere
	VistaGeometry *pSphere = oGeometryFactory.CreateSphere( 1, 300, VistaColorRGB::WHITE );
	// some nice texture
	pSphere->SetTexture( "../data/bus.jpg");
	// Add the box to the scene
	pSceneGraph->NewGeomNode( pSceneGraph->GetRoot(), pBox );
	// move the sphere around
	VistaTransformNode *pSphereTrans = pSceneGraph->NewTransformNode( pSceneGraph->GetRoot() );
	pSphereTrans->Translate(1,1,1);
	// Add the sphere
	pSceneGraph->NewGeomNode( pSphereTrans, pSphere);

	// create a PropertyList to use it in the factory
	VistaPropertyList oProps;
	oProps.SetStringValue( "TYPE", "sphere" );
	oProps.SetDoubleValue( "RADIUS", 2 );
	oProps.SetIntValue( "COLOR", VistaColorRGB::RED );
	VistaGeometry *pGeometry = oGeometryFactory.CreateFromPropertyList( oProps );

	pSceneGraph->NewGeomNode( pSceneGraph->GetRoot(), pGeometry );

	m_pVistaSystem->GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle( argv[0] );
}

GeometryDemoAppl::~GeometryDemoAppl()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

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
