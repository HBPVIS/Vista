/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#ifndef _VISTADFNPROJECTIONSOURCENODE_H
#define _VISTADFNPROJECTIONSOURCENODE_H

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

class VistaProjection;
class VistaDisplayManager;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDfnProjectionSourceNode : public IVdfnNode
{
public:
	VistaDfnProjectionSourceNode(VistaProjection *pWindow);
	~VistaDfnProjectionSourceNode();

	virtual bool GetIsValid() const;

protected:
	virtual bool   DoEvalNode();

	virtual unsigned int    CalcUpdateNeededScore() const;

private:
	TVdfnPort<VistaVector3D> *m_pMidpoint,
							  *m_pNormal,
							  *m_pUp;
	TVdfnPort<double> *m_pLeft,
				   *m_pRight,
				   *m_pTop,
				   *m_pBottom,
				   *m_pNear,
				   *m_pFar;

	class ProjectionObserver;
	VistaProjection *m_pProjection;
	ProjectionObserver *m_pObs;

	struct _sUpdate
	{
		_sUpdate()
			: m_nUpdateCount(0),
			  m_bNeedsUpdate(true)
		{}

		unsigned int m_nUpdateCount;
		bool m_bNeedsUpdate;
	} *m_pUpd;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNPROJECTIONSOURCENODE_H

