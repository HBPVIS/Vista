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

#ifndef _VISTADFNVIEWERSOURCENODE_H
#define _VISTADFNVIEWERSOURCENODE_H

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
class VISTAKERNELAPI VistaDfnViewerSourceNode : public IVdfnNode
{
public:
	VistaDfnViewerSourceNode(VistaDisplaySystem *pSys);
	~VistaDfnViewerSourceNode();

	virtual bool GetIsValid() const;

	VistaDisplaySystem *GetDisplaySystem() const;
	void SetDisplaySystem(VistaDisplaySystem *);
protected:
	virtual bool   DoEvalNode();
	unsigned int   CalcUpdateNeededScore() const;

private:
	TVdfnPort<VistaVector3D> *m_pPosition,
		*m_pLeftEye,
		*m_pRightEye,
		*m_pTranslation;
	TVdfnPort<VistaQuaternion> *m_pOrientation,
	                            *m_pRotation;
	TVdfnPort<VistaVector3D> *m_pPlatformPosition;
	TVdfnPort<VistaQuaternion> *m_pPlatformOrientation;

	VistaDisplaySystem *m_pSystem;

	class DispObserver;
	friend class DispObserver;

	DispObserver *m_pObs;

	mutable unsigned int m_nUpdateCount;
};


class VISTAKERNELAPI VistaDfnViewerSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnViewerSourceNodeCreate( VistaDisplayManager *pMgr );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

private:
	VistaDisplayManager *m_pMgr;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNVIEWERSOURCENODE_H

