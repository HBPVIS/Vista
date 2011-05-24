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

#include "TextDemoAppl.h"

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaTextNode.h>

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/WindowingToolkit/VistaWindowingToolkit.h>
#include <VistaKernel/WindowingToolkit/VistaSimpleTextOverlay.h>
#include <VistaKernel/WindowingToolkit/Vista3DTextOverlay.h>
#include <VistaKernel/WindowingToolkit/VistaTextEntity.h>

#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
TextDemoAppl::TextDemoAppl( int argc, char  *argv[] )
: m_pVistaSystem( new VistaSystem )
{
	// define searchpathes for ini files
	std::list<std::string> liSearchPaths;
	liSearchPaths.push_back("../configfiles/");
	m_pVistaSystem->SetIniSearchPaths(liSearchPaths);

	// ViSTA's standard intro message
	m_pVistaSystem->IntroMsg ();
	
	// Init the System
	if(!m_pVistaSystem->Init(argc, argv))
		VISTA_THROW("ViSTA System init failed.",1);
	
	VistaSG* pSceneGraph = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph();
	// Add node to scenegraph
	VistaTransformNode* pTransform = pSceneGraph->NewTransformNode( pSceneGraph->GetRoot() );
	pTransform->SetTranslation( -1, 0, -2 );
	VistaTextNode *pTextNode3D = pSceneGraph->NewTextNode( pTransform, "SANS" );

	// Fill with text
	if ( pTextNode3D )
	{
		std::cout << "Setting 3D text." << std::endl;
		pTextNode3D->SetText( "Hello world! (3D)" );
	}
	else
		std::cerr << "3D Text cannot be initialized" << std::endl;

	IVistaWindowingToolkit* pWindowingToolkit = m_pVistaSystem->GetWindowingToolkit();

	//you can create the IVistaSimpleTextOverlay as a local or global Variable!
	//DO NOT forget to set the Position.
	IVistaSimpleTextOverlay* pSimpleTOverlay = pWindowingToolkit->CreateSimpleTextOverlay();
	m_pTextEntity1 = pWindowingToolkit->CreateTextEntity();

	m_pTextEntity1->SetText( "Hello World 1!" );
	m_pTextEntity1->SetXPos( 1.5f );
	m_pTextEntity1->SetYPos( 1.5f );
	m_pTextEntity1->SetZPos( 1.5f );
	m_pTextEntity1->SetColor( VistaColorRGB::CYAN );

	pSimpleTOverlay->AddText( m_pTextEntity1 );

	// All methods which start with "Create" will return a pointer
	// note the the method's caller is responsible for this pointer
	// and have to clean up those by themselves!
	m_pSimpleTextOverlay = pWindowingToolkit->CreateSimpleTextOverlay();

	m_pTextEntity2 = pWindowingToolkit->CreateTextEntity();
	m_pTextEntity2->SetText( "Hello World 2!" );
	m_pTextEntity2->SetXPos( 3.0f );
	m_pTextEntity2->SetYPos( 3.0f );
	m_pTextEntity2->SetZPos( 3.0f );

	m_pSimpleTextOverlay->AddText( m_pTextEntity2 );


	//"Hello World 3!" is there, too! Can you find it?
	m_pTextEntity3 = pWindowingToolkit->CreateTextEntity();

	m_pTextEntity3->SetText( "Hello World 3!" );
	m_pTextEntity3->SetFont( "SERIF", 30 );
	m_pTextEntity3->SetColor( VistaColorRGB::GREEN );
	m_pTextEntity3->SetXPos( 0.0f );
	m_pTextEntity3->SetYPos( 1.0f );
	m_pTextEntity3->SetZPos( 0.0f );

	m_pOverlay3D = pWindowingToolkit->Create3DTextOverlay();
	m_pOverlay3D->AddText( m_pTextEntity3 );

	if (!m_pVistaSystem->GetDisplayManager()->GetDisplaySystem())
		VISTA_THROW("No DisplaySystem found", 1);

	m_pVistaSystem->GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle( argv[0] );
}

TextDemoAppl::~TextDemoAppl()
{
	// clean up all memory
	if( m_pTextEntity1 )
		delete m_pTextEntity1;

	if( m_pTextEntity2 )
		delete m_pTextEntity2;

	if( m_pTextEntity3 )
		delete m_pTextEntity3;

	if( m_pSimpleTextOverlay )
		delete m_pSimpleTextOverlay;

	if( m_pOverlay3D )
		delete m_pOverlay3D;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void TextDemoAppl::Run ()
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
