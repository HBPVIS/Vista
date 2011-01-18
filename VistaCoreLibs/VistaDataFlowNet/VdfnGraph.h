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

#ifndef _VDFNGRAPH_H
#define _VDFNGRAPH_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"
#include <VistaAspects/VistaSerializable.h>


#include <map>
#include <list>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVdfnNode;
class IVdfnReEvalNode;
class IVdfnPort;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The graph class. Contains nodes and edges.
 * It is really not meant to traverse this graph's structure by hand as a programmer.
 * There is code to do that. But you can do so, if you wish.
 * Consequently, the use of typedefs like Nodes, Edges, ConInfo and Connect
 * are used for expert access only.
 */
class VISTADFNAPI VdfnGraph : public IVistaSerializable
{
public:
	class ExportData
	{
	public:

		enum eDir
		{
			UNDEFINED=-1,
			INPORT=0,
			OUTPORT
		};

		ExportData()
		: m_nDirection(UNDEFINED),
		  m_pTargetNode(NULL)
		{

		}

		ExportData( const std::string &strNodeName,
				 const std::string &strPortName,
				 const std::string &strMapName,
				 eDir direction,
				 IVdfnNode *pTargetNode = NULL)
		: m_strNodeName(strNodeName),
		  m_strPortName(strPortName),
		  m_strMapName(strMapName),
		  m_nDirection(direction),
		  m_pTargetNode(pTargetNode)
		{

		}

		std::string m_strNodeName;
		std::string m_strPortName;
		std::string m_strMapName;

		eDir m_nDirection;
		IVdfnNode *m_pTargetNode;
	};

	/**
	 * typdef a list of nodes
	 */
	typedef std::list<IVdfnNode*>             Nodes;

	/**
	 * is meant to reflect a target node and a corresponding port
	 * where the edge in coming in.
	 */
	typedef std::list<std::pair<IVdfnNode*, IVdfnPort*> > ConInfo;

	/**
	 * maps all the nodes that are connected by Connect.first to
	 * the list of Connect.second.
	 */
	typedef std::pair<IVdfnPort*, ConInfo > Connect;

	/**
	 * Edges define a mapping from a node which is connected over
	 * a number of connects, which in term map from a port to all
	 * the nodes which are reached by that port. The ConInfo
	 * provides a link between the target node of the Connect.Port
	 * and the TargetNode.Port
	 */
	typedef std::map <IVdfnNode*, std::list<Connect> >      Edges;

	/**
	 * A list of exported ports.
	 */
	typedef std::list<ExportData>               ExportList;

	VdfnGraph();
	virtual ~VdfnGraph();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CALCULATION
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * @param nTimeStamp provide a timestamp that is passed as update time
	                     stamp to all nodes that are evaluated.
						 Pass 0 here for the initial build which will build
						 up the edges and will call the PrepareEvaluationRun()
						 method on all nodes which are valid.
	 * @return true off no error was encountered during setup
	 */
	bool EvaluateGraph(double nTimeStamp);

	/**
	 * activates this graph. if the dTimeStamp is 0, the activation
	 * routine on all nodes will be run, even if the current state of
	 * this graph is active. If the dTimeStamp is not 0, and an already
	 * active graph is to be activated, nothing will happen.
	 * If a graph is activated, it runs over <b>all</b> nodes in the graph
	 * and calls IVdfnNode::OnActivate() on them, passing dTimeStamp.
	 * If a graph is deactivated, it runs over <b>all</b> nodes in the graph
	 * and calls IVdfnNode::OnDeactivate() on them, passing dTimeStamp.
	 * @param bIsActive true if this graph is marked active, false else
	 * @param dTimeStamp a timestamp, 0 for "force-state-propagation-to-nodes"
	 */
	void SetIsActive( bool bIsActive, double dTimeStamp );

	/**
	 * Returns whether this graph is active or nor. By default, a graph is
	 * active.
	 * @see SetIsActive()
	 * @return true if this graph is active, false else.
	 */
	bool GetIsActive() const;

	/**
	 * The graph will iterate over all nodes and check whether they are
	 * dirty or not. For that, the UnconditionalEvaluation() flag will be
	 * @return true when this graph is dirty and needs to be evaluated.
	 */
	bool NeedsEvaluation() const;

	/**
	 * Reinitializes the Graph, thereby fetching the action objects again,
	 * and checking for proper edge/port setup
	 */
	bool ReloadActionObjects();


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// NODE MANAGEMENT
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 *  Adds a node to this graph. This has no direct conseqeuence.
	 *  It is not checked, whether the node is already part of this graph.
	 *  The graph is markes dirty when a node is added. On the next request
	 *  to evaluate the graph, the internal traversal stuctures will be rebuild.
	 *  @return always true
	 *  @param pNode the node to be added for evaluation
	 */
	bool AddNode( IVdfnNode *pNode );

	/**
	 * removes a node (does not claim the memory for it) and mark this graph dirty
	 */
	bool RemNode( IVdfnNode * );

	/**
	 * @return true when the node given is part of this graph
	 */
	bool GetIsNode( IVdfnNode *) const;

	/**
	 * Searches a node in this graph by name. Note that is is assumes that any
	 * node in this graph has a unique name.
	 * The node is searched by a case-sensitive comparison to the node name.
	 * @return NULL iff a node with nam strName was not found in this graph.
	 * @param strName the case sensitive name to be searched for.
	 */
	IVdfnNode *GetNodeByName(const std::string &strName) const;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// EDGE MANAGEMENT
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * @return true if there is a direct connection from n1 to n2, false else
	 * @param n1 non NULL pointer to the first (parent) node
	 * @param n2 non NULL pointer to the second (child) node
	 */
	bool GetIsConnectedFromTo( IVdfnNode *n1, IVdfnNode *n2) const;

	/**
	 * Query the node that has pPort as its OUT port.
	 * @param pPort the port that is part of the return node as an out port
	 * @return the node that owns / contains the outport pPort or NULL
	 */
	IVdfnNode *GetNodeForPort( IVdfnPort *pPort ) const;


	/**
	 * @return a const reference to the nodes stored in the graph
	 */
	const Nodes &GetNodes() const;

	/**
	 * @return all nodes that have a non-empty tag field
	 */
	Nodes GetNodesWithTag() const;

	/**
	 * @return all nodes that have the tag strGroupTag set as their tag
	 */
	Nodes GetNodesByGroupTag( const std::string &strGroupTag ) const;

	/**
	 * @return a const reference on all edges of this graph
	 */
	const Edges &GetEdges() const;

	/**
	 * @return get all nodes that have 0 inports, thus can not depend on any other
	           nodes in this graph (and serve as 'sources')
	 */
	Nodes GetSourceNodes() const;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// COMPOSITE/EXPORTS MANAGEMENT
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * Graphs can contain mappings for nodes and ports that can be used
	 * for exporting nodes. While this is not a true feature of a "graph",
	 * it is embodied here as API for storage. Maybe some day, the information
	 * can be used for dependency checking between graphs. Inports may not be
	 * set twice, while outports, as usual, can be used more than one time
	 * @param liExports container for the list of exports defined for this graph
	 */
	void GetExports( ExportList &liExports ) const;

	/**
	 * will only allow to set exports for existing nodes, checking of port availability
	 * will result in a warning only (as ports may be dynamically created after a call
	 * to SetExports().
	 * @todo implement consistency checking
	 * @param liExports the list of ports which are to be exported to the outside, for
	          inports serve as input to this graph
	 */
	void SetExports( const ExportList &liExports );

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ViSTA CLUSTER API
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 *  returns the number of nodes that are / have to be evaluated
	 *  on the "master-simulation-node" in a cluster environment.
	 *  0 if there are only nodes that can compute locally.
	 */
	unsigned int GetMasterSimCount() const;

	/**
	 * control API, it tells the serializer of this graph to respect the
	 * IVdfnNode::GetIsMasterSim() flag or not. If this method returns
	 * fallse, the master sim flag can be ignored during serialization, which
	 * will then dump <b>all</b> nodes of this graph,
	 * not only the ones sitting on the master.
	 * @see SetCheckMasterSim()
	 */
	bool GetCheckMasterSim() const;

	/**
	 * determines whether this graph will only serialize those nodes that
	 * are tagged as being simulated / evaluated only on the master machine.
	 */
	void SetCheckMasterSim( bool bCheck );

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SERIALIZABLE
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual int    Serialize(IVistaSerializer &) const;
	virtual int    DeSerialize(IVistaDeSerializer &);
	virtual std::string GetSignature() const;

private:
	typedef std::map <IVdfnPort*, IVdfnNode*> PortLookup;
	typedef std::vector<IVdfnNode*>           NodeVec;

protected:
	bool EvaluateSubGraph( const NodeVec& vecSubGraph, const double nTimeStamp );

	bool UpdateTraversalVector();
	bool UpdatePortLookupMap();
	bool UpdateEdgeMap();
	bool UpdatePorts();

private:
	Connect &GetAdjacencyList( IVdfnNode *, IVdfnPort * );

	Nodes      m_liNodes;
	NodeVec    m_vecTraversal;
	Edges      m_mpEdges;
	PortLookup m_mpPortLookup;
	std::map<IVdfnReEvalNode*, NodeVec> m_mpReEvalSubgraphs;

	ExportList m_liExports;

	bool m_bUpToDate,
		 m_bIsActive,
	     m_bCheckMasterSim;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNGRAPH_H
