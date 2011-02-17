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

#include "VistaKernel/GraphicsManager/VistaGeomNode.h"
#include "VistaKernel/GraphicsManager/VistaGeometry.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"

#include <cassert>
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaGeomNode::VistaGeomNode(	VistaGroupNode*			pParent, 
								VistaGeometry*				pGeom, 
								IVistaNodeBridge*			pBridge,
								IVistaNodeData*				pData,
								std::string strName)
				:VistaLeafNode(pParent,pBridge,pData,strName)
{
	m_nType = VISTA_GEOMNODE;
	m_pGeometry = pGeom;
	if(m_pGeometry)
		m_pBridge->RegisterGeometry(m_pGeometry);
}


VistaGeomNode::VistaGeomNode(const VistaGeomNode &)
{
	assert(false);
}

VistaGeomNode& VistaGeomNode::operator =(const VistaGeomNode &)
{
	assert(false);
	return *this;
}

VistaGeomNode::~VistaGeomNode()
{
	// it should be ok to give NULL here
	if(m_pGeometry)
	{
		if(m_pBridge->UnregisterGeometry(m_pGeometry))
		{
			//clear toolkit data
			m_pBridge->ClearGeomNodeData(this);
		}
	}
}

// ============================================================================
// ============================================================================
VistaGeometry* VistaGeomNode::GetGeometry() const
{
	return m_pGeometry;
}
// ============================================================================
// ============================================================================
bool VistaGeomNode::SetGeometry(VistaGeometry* pGeom)
{
	if(m_pGeometry)
	{
		// unregister geometry
		bool deleted = m_pBridge->UnregisterGeometry(m_pGeometry);
		if(deleted)
		{
			m_pGeometry = NULL;

			// clean toolkit node
			m_pBridge->ClearGeomNodeData(this);
		}
	}

	
	if(pGeom)
	{
		if(m_pBridge->SetGeometry(pGeom->m_pData, m_pData))
		{
			m_pGeometry = pGeom;
			m_pBridge->RegisterGeometry(m_pGeometry);
			return true;
		}
	}
	else
	{
		m_pGeometry = NULL;
		return true;
	}
	return false;
}


bool VistaGeomNode::ScaleGeometry(const float fX, const float fY, const float fZ)
{
	if(m_pGeometry)
	{
		return m_pGeometry->ScaleGeometry(fX, fY, fZ);
	}
	return false;
}

// ============================================================================
// ============================================================================
bool VistaGeomNode::CanHaveChildren() const
{
	return false;
}
// ============================================================================
// ============================================================================
