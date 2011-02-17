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

#include "VistaKernel/GraphicsManager/VistaLeafNode.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"
#include "VistaKernel/GraphicsManager/VistaTransformNode.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaLeafNode::VistaLeafNode()
{
	m_nType = VISTA_LEAFNODE;
}

VistaLeafNode::VistaLeafNode(	VistaGroupNode*				pParent, 
								IVistaNodeBridge*				pBridge,
								IVistaNodeData*					pData,
								const std::string	&strName) 
: VistaNode(pParent,pBridge,pData,strName)
{
	m_nType = VISTA_LEAFNODE;
}

VistaLeafNode::~VistaLeafNode()
{
	// IAR: is alread done by VistaNode
	//if(m_pParent)
	//    m_pParent->DisconnectChild(this);
}

// ============================================================================
// ============================================================================