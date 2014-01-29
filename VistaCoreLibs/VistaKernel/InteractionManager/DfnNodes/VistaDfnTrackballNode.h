/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#ifndef _VISTADFNTRACKBALLNODE_H
#define _VISTADFNTRACKBALLNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class VistaDisplayManager;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VISTAKERNELAPI VistaVdfnTrackball : public IVdfnNode
{
public:
	VistaVdfnTrackball();
	~VistaVdfnTrackball();

	bool PrepareEvaluationRun();
	bool GetIsValid() const;

protected:
	bool DoEvalNode();

private:

	bool HandleRotate();

	TVdfnPort<VistaTransformMatrix> *m_pOut;

	TVdfnPort<VistaVector3D> *m_pCenter,		
	                          *m_pGrab,
		                      *m_pOffset;

	/**
	 * Caches the offset input-port update count to see wether it is dirty, 
	 * and calculate a new transform only in that case.
	 */
	unsigned int m_iUpdateCountOffset;
		
	VistaVector3D   m_v3BegDrag;
	VistaVector3D   m_v3TBCenter;

	VistaTransformMatrix m_mtZoom;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNTRACKBALLNODE_H

