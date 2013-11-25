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
 * $Id: main.cpp$
 */

/**
 * Description:
 * This is the RecordReplayDemo. The code is very simple - just loading a
 * geometric model. The main juice of this demo can be found in the startscripts
 */

#include <VistaBase/VistaExceptionBase.h>

#include "VistaKernel/VistaSystem.h"
#include "VistaKernel/GraphicsManager/VistaSceneGraph.h"
#include "VistaKernel/GraphicsManager/VistaTransformNode.h"

#include <iostream>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int	main( int argc, char *argv[] )
{
	try
	{
		VistaSystem* pSystem = new VistaSystem();

		std::list<std::string> liSearchPath;
		liSearchPath.push_back( "../configfiles/" );
		liSearchPath.push_back( "configfiles/" );
		pSystem->SetIniSearchPaths( liSearchPath );

		pSystem->Init( argc, argv );
		
		// laod something to fill the scenery
		VistaSceneGraph* pSG = pSystem->GetGraphicsManager()->GetSceneGraph();
		IVistaNode* pCow = pSG->LoadNode( "../data/cow.obj" );
		VistaTransformNode* pCowTrans = pSG->NewTransformNode( pSG->GetRoot() );
		pCowTrans->Translate( 0, 0, -5 );
		pCowTrans->AddChild( pCow );

		pSystem->Run();

		delete pSystem;
	}
	catch( VistaExceptionBase &e )
	{
		e.PrintException();
	}
	catch( std::exception &e )
	{
		std::cerr << "Exception:" << e.what() << std::endl;
	}

	return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
