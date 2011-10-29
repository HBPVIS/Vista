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

#include "VistaInteractionContext.h"

#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnPersistence.h>


#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaInteractionContext::SerializeContext( IVistaSerializer& oSerializer,
											   const VistaInteractionContext& oContext )
{
    oSerializer.WriteInt32( oContext.m_nUpdateIndex );

	VdfnGraph *pGraph = oContext.GetTransformGraph();
	if(pGraph != NULL)
	{
		int nRet = 0;
		unsigned int nCnt = (*pGraph).GetMasterSimCount();
		oSerializer.WriteInt32( (VistaType::sint32)nCnt );
		if(nCnt > 0)
		{
			const VdfnGraph::Nodes &nodes = (*pGraph).GetNodes();
			for(VdfnGraph::Nodes::const_iterator cit = nodes.begin();
				cit != nodes.end(); ++cit )
			{
				std::string sNodeName;
				if( (*cit)->GetIsMasterSim() )
				{
					// we found a node that is master-sim... serialize this
					sNodeName = (*cit)->GetNameForNameable();
					if( sNodeName.empty() )
					{
						vstr::warnp() 
									<< "[VistaInteractionContext::SerializeContext]: "
									<< "found node without name - skipping" << std::endl;
						continue;
					}

					nRet += oSerializer.WriteInt32( (VistaType::sint32)sNodeName.size() );
					nRet += oSerializer.WriteString( sNodeName );

					 std::list<std::string> liOutPorts = (*cit)->GetOutPortNames();

					 for(std::list<std::string>::const_iterator lit = liOutPorts.begin();
						 lit != liOutPorts.end(); ++lit )
					 {
						 IVdfnPort *pPort = (*cit)->GetOutPort( *lit );
						 VdfnPortSerializeAdapter *pAdp = pPort->GetSerializeAdapter();
						 oSerializer.WriteSerializable( *pAdp );
					 }
				}
			}
		}
		return nRet;
	}
    return 0;
}

int VistaInteractionContext::DeSerializeContext(IVistaDeSerializer& oDeSerializer,
												 VistaInteractionContext& oContext )
{
    oDeSerializer.ReadInt32( oContext.m_nUpdateIndex );

	VdfnGraph *pGraph = oContext.GetTransformGraph();
	if(pGraph != NULL)
	{
		int nRet = 0;
		unsigned int nCnt = 0;
		oDeSerializer.ReadInt32(nCnt);
		if(nCnt > 0)
		{
			std::string strNodeName;
			for(unsigned int n=0; n < nCnt; ++n)
			{
				// read name
				VistaType::sint32 nLength = 0;
				oDeSerializer.ReadInt32( nLength );
				oDeSerializer.ReadString( strNodeName, nLength );

				IVdfnNode *pNode = pGraph->GetNodeByName( strNodeName );
				if(pNode)
				{
					std::list<std::string> liPortNames = pNode->GetOutPortNames();
					for(std::list<std::string>::const_iterator cit = liPortNames.begin();
						cit != liPortNames.end(); ++cit )
					{
						 IVdfnPort *pPort = pNode->GetOutPort( *cit );
						 VdfnPortSerializeAdapter *pAdp = pPort->GetSerializeAdapter();
						 oDeSerializer.ReadSerializable( *pAdp );
					}
				}
				else
				{
					vstr::errp() << "[VistaInteractionContext::DeSerializeContext]: "
							<< "node [" << strNodeName << "]not available\n";
					vstr::IndentObject oIndent;
					vstr::erri() << "Available nodes are:\n";
					const VdfnGraph::Nodes &nd = pGraph->GetNodes();
					for( VdfnGraph::Nodes::const_iterator cit = nd.begin(); cit != nd.end(); ++cit)
					{
						vstr::erri() << "[" << (*cit)->GetNameForNameable() << "]\n";
					}
					vstr::err().flush();
					VISTA_THROW( "serialization error for dfn graph", 0x00000000 );
				}
			}
		}

		return nRet;
	}
    return 0;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaInteractionContext::VistaInteractionContext(VistaInteractionManager *pMgr,
												   VistaEventManager *pEvMgr)
: IVistaObserveable(),
  m_bEnabled(true),
  m_bEventSource(true),
  m_bRegistered(false),
  m_pTransformGraph(NULL),
  m_pEvent(new VistaInteractionEvent(pMgr)),
  m_pEventManager(pEvMgr),
  m_nUpdateIndex(0),
  m_nRoleId(~0)
{
	m_pEvent->SetInteractionContext(this);
	m_pEvent->SetId( VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE );
}

VistaInteractionContext::VistaInteractionContext( const VistaInteractionContext & )
{

}

VistaInteractionContext::~VistaInteractionContext()
{
	delete m_pEvent;
    delete m_pTransformGraph;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaInteractionContext& VistaInteractionContext::operator=(const VistaInteractionContext &)
{
	return *this;
}

bool VistaInteractionContext::GetIsEnabled() const
{
	return m_bEnabled;
}

void VistaInteractionContext::SetIsEnabled(bool bEnabled)
{
	if(compAndAssignFunc<bool>(bEnabled, m_bEnabled))
	{
		Notify(MSG_ENABLESTATE_CHANGE);
		if(m_pTransformGraph)
		{
			m_pTransformGraph->SetIsActive(bEnabled, 0);
		}
	}
}


VdfnGraph *VistaInteractionContext::GetTransformGraph() const
{
	return m_pTransformGraph;
}

void VistaInteractionContext::SetTransformGraph(VdfnGraph *pGraph)
{
	if(compAndAssignFunc<VdfnGraph*>(pGraph,m_pTransformGraph))
	{
		Notify(MSG_CONTEXT_GRAPH_CHANGE);
	}
}

bool VistaInteractionContext::Evaluate(double nTs)
{
	if(!m_pTransformGraph)
		return false;

	if( (*m_pTransformGraph).EvaluateGraph( nTs ) )
	{
		// the call to set active is *after* EvaluateGraph() on purpose:
		// when nTs is 0, then no "real" evaluation was performed, but
		// the graph init instead. After init -> roll out activation
		// on the graph
	    if(nTs == 0)
	        (*m_pTransformGraph).SetIsActive(true, 0); // system broadcast of 0
	                                                   // call activate on the graph
        ++m_nUpdateIndex;

		if(m_bEventSource)
		{
			m_pEvent->SetId(VistaInteractionEvent::VEID_CONTEXT_CHANGE);
			m_pEvent->SetTime(nTs);
			m_pEventManager->ProcessEvent(m_pEvent);
		}
		return true;
	}
	return false;
}

unsigned int VistaInteractionContext::GetUpdateIndex() const
{
    return m_nUpdateIndex;
}

std::string VistaInteractionContext::GetGraphSource() const
{
	return m_sGraphFile;
}

void VistaInteractionContext::SetGraphSource( const std::string &strGraphFile )
{
    if( compAndAssignFunc<std::string>(strGraphFile, m_sGraphFile) )
        Notify(MSG_GRAPHSOURCE_CHANGE);
}

bool VistaInteractionContext::GetIsEventSource() const
{
    return m_bEventSource;
}

void VistaInteractionContext::SetIsEventSource(bool bSource)
{
    m_bEventSource = bSource;
}


bool VistaInteractionContext::Update(double dTs)
{
	if(!GetIsEnabled())
		return false;

	if(m_pTransformGraph)
	{
		// special case: force update on 0 (initial update)
		// OR every time the graph is considered dirty
		if( (dTs == 0) || (*m_pTransformGraph).NeedsEvaluation() )
		{
				return Evaluate( dTs );
		}
	}
	return false;
}

unsigned int VistaInteractionContext::GetRoleId() const
{
    return m_nRoleId;
}

void         VistaInteractionContext::SetRoleId( unsigned int nRoleId )
{
    if(compAndAssignFunc<unsigned int>(nRoleId, m_nRoleId))
        Notify(MSG_ROLEID_CHANGE);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


