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
// $Id$

#ifndef _VISTADFNCLUSTERNODEINFONODE_H
#define _VISTADFNCLUSTERNODEINFONODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeCreators.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnHistoryPort.h>


#include <VistaBase/VistaBaseTypes.h>

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaClusterAux;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This node take the information given in VistaClusterAux and passes it to
 * the network. Can be useful for per-client logfiles in cluster mode or more
 * debugging.
 * The node is always evaluated, but tries to be calm on resources, basically
 * only the frame clock is updated after the first call.
 * @outport{node_name,string,the name property of the cluster node where this node is running on}
 * @outport{node_mode,int,a hint on the cluster mode\, see code\,plz}
 * @outport{node_clock,double,the frame clock value for the current iteration}
 */
class VISTAKERNELAPI VdfnClusterNodeInfoNode : public IVdfnNode
{
public:
	VdfnClusterNodeInfoNode( VistaClusterAux *pClAux );
	virtual ~VdfnClusterNodeInfoNode();

protected:
	bool DoEvalNode();
private:
	VistaClusterAux *m_pAux;
	TVdfnPort<std::string> *m_pClusterNodeName;
	TVdfnPort<int>         *m_pClusterMode;
	TVdfnPort<double>      *m_pClusterClock;
	TVdfnPort<microtime>   *m_pAvgUpdate, *m_pAvgSwap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNDEVICEDEBUGNODE_H

