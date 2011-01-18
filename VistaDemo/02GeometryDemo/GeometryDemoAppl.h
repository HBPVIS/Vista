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

/*============================================================================*/
/*                                                                            */
/* This Demo will create a box and a sphere (textured), both located in       */
/* another sphere.                                                            */
/* Two ways of creating a sphere are demonstrated:                            */
/* oGeometryFactory.CreateSphere() and oGeometryFactory.CreateFromPropertyList*/
/*                                                                            */
/*============================================================================*/

#ifndef _GEOMETRYDEMOAPPL_H
#define _GEOMETRYDEMOAPPL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class GeometryDemoAppl
{
public:
	GeometryDemoAppl( int argc, char* argv[] );
	virtual ~GeometryDemoAppl();

	void Run();

private:
	VistaSystem* m_pVistaSystem;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _GEOMETRYDEMOAPPL_H

