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

#ifndef _VISTADFNREFERENCEFRAMETRANSFORMNODE_H
#define _VISTADFNREFERENCEFRAMETRANSFORMNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaReferenceFrame;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * @todo documentation missing!
 */
class VISTAKERNELAPI VistaDfnReferenceFrameTransformNode : public IVdfnNode
{
public:
	VistaDfnReferenceFrameTransformNode( VistaReferenceFrame* pFrame,
											bool bTransformToFrame,
											int iNrPositionPorts,
											int iNrOrientationPorts,
											int iNrMatrixPorts );
	~VistaDfnReferenceFrameTransformNode();

	virtual bool GetIsValid() const;

	bool PrepareEvaluationRun();
protected:
	virtual bool DoEvalNode();
private:
	VistaReferenceFrame*			m_pFrame;
	bool							m_bTransformFromFrame;

	std::vector<TVdfnPort<VistaVector3D>*>			m_vecInPortsPos;
	std::vector<TVdfnPort<VistaVector3D>*>			m_vecOutPortsPos;
	std::vector<TVdfnPort<VistaQuaternion>*>		m_vecInPortsOri;
	std::vector<TVdfnPort<VistaQuaternion>*>		m_vecOutPortsOri;
	std::vector<TVdfnPort<VistaTransformMatrix>*>	m_vecInPortsMat;
	std::vector<TVdfnPort<VistaTransformMatrix>*>	m_vecOutPortsMat;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNREFERENCEFRAMETRANSFORMNODE_H

