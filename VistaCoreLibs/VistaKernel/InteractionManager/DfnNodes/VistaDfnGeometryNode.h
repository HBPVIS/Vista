/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#ifndef _VISTADFNGEOMETRYNODE_H
#define _VISTADFNGEOMETRYNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <vector>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class VistaGeometry;
class IVistaNode;
class VistaTransformNode;
class VistaSceneGraph;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VISTAKERNELAPI VistaDfnGeometryNode : public IVdfnNode
{
public:
	VistaDfnGeometryNode( VistaGeometry* pGeometry, VistaSceneGraph* pSceneGraph );
	VistaDfnGeometryNode( IVistaNode* pSubtreeRoot, VistaSceneGraph* pSceneGraph,
							bool bDeleteSubtree = true );
	VistaDfnGeometryNode( const VistaPropertyList& oGeometryConfig,
							VistaSceneGraph* pSceneGraph );
	~VistaDfnGeometryNode();

	bool PrepareEvaluationRun();
	bool GetIsValid() const;

	VistaTransformNode* GetOffsetNode() const;

protected:
	bool DoEvalNode();

private:
	unsigned int m_nPositionPortUpdate;
	unsigned int m_nOrientationPortUpdate;
	unsigned int m_nScalePortUpdate;
	unsigned int m_nUniformScalePortUpdate;
	unsigned int m_nTransformUpdate;
	unsigned int m_nEnableUpdate;
	TVdfnPort<VistaVector3D>* m_pPositionPort;
	TVdfnPort<VistaVector3D>* m_pScalePort;
	TVdfnPort<float>* m_pUniformScalePort;
	TVdfnPort<VistaQuaternion>* m_pOrientationPort;
	TVdfnPort<VistaTransformMatrix>* m_pTransformPort;
	TVdfnPort<bool>* m_pEnablePort;

	VistaTransformNode* m_pTransformNode;
	VistaTransformNode* m_pOffsetNode;
	VistaGeometry* m_pGeometry;
	bool m_bDeleteSubtree;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNGEOMETRYNODE_H

