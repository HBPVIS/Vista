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

#include "MyDemoAppl.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
CMyDemoAppl::CMyDemoAppl( int argc, char  *argv[] )
	:	mVistaSystem( VistaSystem() )
{

	std::list<std::string> liSearchPaths;
	liSearchPaths.push_back("../configfiles/");
	mVistaSystem.SetIniSearchPaths(liSearchPaths);

	// ViSTA's standard intro message
	mVistaSystem.IntroMsg ();

	// ADD YOUR CODE HERE...
	
	mVistaSystem.Init (argc, argv);
}

CMyDemoAppl::~CMyDemoAppl()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void CMyDemoAppl::Run ()
{
	// Start Universe
	mVistaSystem.Run();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
