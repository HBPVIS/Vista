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

#ifndef _VISTAXES_H
#define _VISTAXES_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaGeometry.h"
#include <vector>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSG;
class IVistaNode;
class VistaGroupNode;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Creates coordinate axes under the root node
 */
class VISTAKERNELAPI VistaAxes
{
public:

	VistaAxes(VistaSG * pVistaSG,
			  float sizeX=1.0f, float sizeY=1.0f, float sizeZ=1.0f);
	virtual ~VistaAxes();

	IVistaNode * GetVistaNode();

private:

	std::vector<VistaGeometry*>	m_axes;
	std::vector<VistaGeometry*>	m_tips;
	VistaGeometry*				m_center;
	VistaGroupNode*			m_group;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAXES_H


