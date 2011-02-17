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
/* This Demo will create several Hello World's displayed on the screen.       */
/* Some of these are fixed on the screen, some are positioned and one is 3D.  */
/* IVistaTextEntity is used to store additional information for the text.     */
/*                                                                            */
/*============================================================================*/

#ifndef _TEXTDEMOAPPL_H
#define _TEXTDEMOAPPL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaSystem.h>

class IVistaSimpleTextOverlay;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class TextDemoAppl
{
public:
	TextDemoAppl(int argc, char  *argv[]);
	virtual ~TextDemoAppl();

	void   Run  ();

private:
	VistaSystem*				m_pVistaSystem;
	IVistaSimpleTextOverlay		*m_pSimpleTextOverlay;
	IVistaTextEntity 			*m_pTextEntity1;
	IVistaTextEntity			*m_pTextEntity2;
	IVistaTextEntity			*m_pTextEntity3;
	IVista3DTextOverlay			*m_pOverlay3D;
};


#endif // _TEXTDEMOAPPL_H