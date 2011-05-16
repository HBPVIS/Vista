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

#include "VistaDfnClusterNodeInfoNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/VistaClusterAux.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaPropertyAwareable.h>

#include <VistaDataFlowNet/VdfnUtil.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnClusterNodeInfoNode::VdfnClusterNodeInfoNode(VistaClusterAux *pClAux )
: IVdfnNode()
, m_pAux(pClAux)
, m_pClusterNodeName( new TVdfnPort<std::string> )
, m_pClusterMode( new TVdfnPort<int> )
, m_pClusterClock( new TVdfnPort<double> )
, m_pAvgUpdate( new TVdfnPort<microtime> )
, m_pAvgSwap( new TVdfnPort<microtime> )
{
	RegisterOutPort( "node_name", m_pClusterNodeName );
	RegisterOutPort( "node_mode", m_pClusterMode );
	RegisterOutPort( "node_clock", m_pClusterClock );
	RegisterOutPort( "node_updtime", m_pAvgUpdate );
	RegisterOutPort( "node_swaptime", m_pAvgSwap );

	// we mark this mode as unconditional eval for the frameclock
	SetEvaluationFlag(true);
}

VdfnClusterNodeInfoNode::~VdfnClusterNodeInfoNode()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VdfnClusterNodeInfoNode::DoEvalNode()
{
	if( m_pClusterNodeName->GetValueConstRef().empty() )
		m_pClusterNodeName->SetValue( m_pAux->GetNodeName(), GetUpdateTimeStamp() );

	int nMode = 0;
	if( m_pAux->GetIsMaster() )
		nMode = 0x01;
	else if( m_pAux->GetIsSlave() )
		nMode = 0x02;
	else if( m_pAux->GetIsStandalone() )
		nMode = 0x04;
	else if( m_pAux->GetIsServer() )
		nMode = 0x08;
	else if( m_pAux->GetIsClient() )
		nMode = 0x10;

	if( m_pClusterMode->GetValue() != nMode )
		m_pClusterMode->SetValue( nMode, GetUpdateTimeStamp() );

	m_pClusterClock->SetValue( m_pAux->GetFrameClock(), GetUpdateTimeStamp() );

	m_pAvgUpdate->SetValue( m_pAux->GetAvgUpdateTime(), GetUpdateTimeStamp() );

	m_pAvgSwap->SetValue( m_pAux->GetAvgSwapTime(), GetUpdateTimeStamp() );

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


