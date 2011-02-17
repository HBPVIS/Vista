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

#ifndef _VISTADFNVIEWPORTSOURCENODE_H
#define _VISTADFNVIEWPORTSOURCENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaViewport;
class VistaDisplayManager;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDfnViewportSourceNode : public IVdfnNode
{
public:
	VistaDfnViewportSourceNode(VistaViewport *pWindow);

	~VistaDfnViewportSourceNode();

	virtual bool GetIsValid() const;

protected:
	virtual bool   DoEvalNode();

	virtual unsigned int    CalcUpdateNeededScore() const;

private:
	TVdfnPort<int> *m_pW,
				   *m_pH;


	class ViewportObserver;
	VistaViewport *m_pViewport;
	ViewportObserver *m_pObs;


	mutable unsigned int m_nUpdateCount;
	mutable bool m_bNeedsUpdate;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNVIEWPORTSOURCENODE_H
