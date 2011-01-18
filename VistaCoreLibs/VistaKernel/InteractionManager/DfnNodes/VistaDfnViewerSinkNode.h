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

#ifndef _VISTADFNVIEWERSINKNODE_H
#define _VISTADFNVIEWERSINKNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnSerializer.h>
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
class VistaDisplaySystem;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDfnViewerSinkNode : public IVdfnNode
{
public:
	VistaDfnViewerSinkNode(VistaDisplaySystem *pSys);
	~VistaDfnViewerSinkNode();

	virtual bool GetIsValid() const;
	virtual bool PrepareEvaluationRun();

	VistaDisplaySystem *GetDisplaySystem() const;
	void SetDisplaySystem(VistaDisplaySystem *);
protected:
	virtual bool   DoEvalNode();

private:
	TVdfnPort<VistaVector3D> *m_pPosition,
				   *m_pLeftEye,
				   *m_pRightEye;
	TVdfnPort<VistaQuaternion> *m_pOrientation;

	VistaDisplaySystem *m_pSystem;

	class DispObserver;
	DispObserver *m_pDispObs;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNWINDOWNODE_H

