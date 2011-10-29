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
// $Id: VistaOpenGLNode.h 20730 2011-03-30 15:56:24Z dr165799 $

#ifndef _VISTAOPENGLNODE_H
#define _VISTAOPENGLNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaExtensionNode.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNodeBridge;
class IVistaNodeData;
class IVistaOpenGLDraw;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenGLNode : public VistaExtensionNode
{
	friend class IVistaNodeBridge;
	friend class VistaSceneGraph;
public:
	virtual ~VistaOpenGLNode();

	bool Init();
	virtual bool GetBoundingBox(VistaVector3D &pMin, VistaVector3D &pMax) const;
protected:
	VistaOpenGLNode( VistaGroupNode*				pParent,
						IVistaOpenGLDraw*					pDI,
						IVistaNodeBridge*				pBridge,
						IVistaNodeData*					pData,
						const std::string &strName="");
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENGLNODE_H
