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
// $Id: VistaOpenGLNode.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include "VistaOpenGLNode.h"
#include "VistaNodeBridge.h"
#include "VistaOpenGLDraw.h"

#include <VistaMath/VistaBoundingBox.h>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaOpenGLNode::VistaOpenGLNode( VistaGroupNode*				pParent, 
						IVistaOpenGLDraw*					pDI, 
						IVistaNodeBridge*				pBridge,
						IVistaNodeData*					pData,
						const std::string &strName)
: VistaExtensionNode(pParent, pDI, pBridge, pData, strName)
{
	m_nType = VISTA_OPENGLNODE;
}

VistaOpenGLNode::~VistaOpenGLNode()
{
}

// ============================================================================
// ============================================================================

bool VistaOpenGLNode::Init()
{
	//init only once 
	if(!m_bValid)
		m_bValid = m_pBridge->InitOpenGLNode(m_pData, this);

	return m_bValid;
}

bool VistaOpenGLNode::GetBoundingBox(VistaVector3D &pMin, VistaVector3D &pMax) const
{
	if(dynamic_cast<IVistaOpenGLDraw*>(m_pExtension))
	{
		VistaBoundingBox bb;
		bool bRet = static_cast<IVistaOpenGLDraw*>(m_pExtension)->GetBoundingBox(bb);
		bb.GetBounds(&pMin[0], &pMax[0]);		
		return true;
	}
	else
	{
		return false;
	}
}
