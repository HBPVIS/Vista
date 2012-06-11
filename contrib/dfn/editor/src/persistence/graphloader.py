'''
Created on Aug 12, 2009

@author: assenmac
'''
import xml.dom.minidom

import model.node, model.graph, model.edge, model.port, model.parameter, model.protodb
import xmlutils


def CreateParameter(xmlNode):
    params  = model.parameter.Parameter()
    
    while xmlNode <> None:
        prName  = xmlNode.getAttribute("name")
        prValue = xmlNode.getAttribute("value")
        
        if len(prValue) == 0 and xmlNode.hasAttribute("value") == False:
            # see whether this node has another param as child node
            prChild = xmlutils.FindChildElement(xmlNode, "param")
            if prChild == None:
                # can be, check if the first child node is a text node
                if xmlNode.firstChild and xmlNode.firstChild.nodeType == xml.dom.minidom.Node.TEXT_NODE:
                    # ok, let's take that as a value
                    params[prName] = (xmlNode.firstChild.data, 1)
            else:
                # ok, found a child node, so go there (recurse)
                params[prName] = CreateParameter(prChild)
        else:
            params[prName] = (prValue,0) # store as string
        xmlNode = xmlutils.FindSiblingElement( xmlNode.nextSibling, "param" )
        
    return params

def CreateNode(xmlNode):
    ndName  = xmlNode.getAttribute("name")
    ndType  = xmlNode.getAttribute("type")
    
    if len(ndName) == 0 or len(ndType) == 0:
        return None
    nd = model.node.Node(ndName)
    nd.type = ndType
    # find type in proto db!
    nd.prototype = model.protodb.ProtoDB.db.getPrototypeByName(ndType)
    if nd.prototype == None:
        nd.valid = False
    else:
        nd.prototype.setupInports(nd)
        nd.prototype.setupOutports(nd)
    
    nd.comment = xmlutils.FindComment(xmlNode.previousSibling)

    # start recursion with first param child
    paramNode = xmlutils.FindChildElement(xmlNode, "param")
    if paramNode <> None:
            nd.params = CreateParameter(paramNode) # store top-level
    else:
        nd.params = None # mark: no params for this node!
         
    return nd
        
    

def CreateEdge(xmlEdge, graph):
    fromNode = xmlEdge.getAttribute("fromnode")
    toNode   = xmlEdge.getAttribute("tonode")
    fromPort = xmlEdge.getAttribute("fromport")
    toPort   = xmlEdge.getAttribute("toport")

    if len(fromNode) == 0 or len(toNode) == 0 or len(fromPort) == 0 or len(toPort) == 0:
        return None
    
    edg = model.edge.Edge()
    
    edg.comment = xmlutils.FindComment(xmlEdge.previousSibling)
    
    fromNd = graph.GetNode(fromNode)
    fromPt = None
    
    toNd   = graph.GetNode(toNode)
    toPt   = None
    
    if fromNd == None:
        fromNd = model.node.Node(fromNode) # construct invalid node!
        fromNd.valid = False
        fromNd.type = "## invalid ##"
        fromNd.status = "from-node with name [" + fromNode + "] not contained in graph"
        fromPt = fromNd.AddOutPort(fromPort)
        fromPt.valid = False
        graph.AddNode(fromNd)
    else:
        fromPt = fromNd.GetOutPort(fromPort)
        if fromPt == None:
            fromPt = fromNd.AddOutPort( fromPort )
            if fromNd.prototype <> None:
                style = fromNd.prototype.portStyle 
                if  style == model.nodeprototype.NodePrototype.STATIC_PORTS:
                    fromPt.valid = model.port.Port.STATE_INVALID
                else:
                    fromPt.valid = model.port.Port.STATE_PROBABLY_OK
            else:
                fromPt.valid = model.port.Port.STATE_INVALID
            
            
            
    # ok, we set fromNd and fromPt and clinch that to edge
    edg.setFrom(fromNd, fromPt)
    
    if toNd == None:
        toNd = model.node.Node(toNode) # construct invalid node!
        toNd.status = "to-node with name [" + toNode + "] not contained in graph"
        toNd.type = "## invalid ##"
        toPt = toNd.AddInPort(toPort)
        toPt.valid = False
        graph.AddNode(toNd)
    else:
        toPt = toNd.GetInPort(toPort)
        if toPt == None:
            toPt = toNd.AddInPort(toPort)
            if toNd.prototype <> None:
                style = toNd.prototype.portStyle
                if style == model.nodeprototype.NodePrototype.STATIC_PORTS:
                    toPt.valid = model.port.Port.STATE_INVALID
                else:
                    toPt.valid = model.port.Port.STATE_PROBABLY_OK
                    if style == model.nodeprototype.NodePrototype.OUTPORTS_SAME_AS_INPORTS_MIXIN:
                        toPt.type = "passed"
                    elif style == model.nodeprototype.NodePrototype.DYNAMIC_INPORTS_ONLY:
                        toPt.type = "passed"
                    elif style == model.nodeprototype.NodePrototype.DYNAMIC_INPORTS_MIXIN:
                        toPt.type = "passed"
                    else:
                        toPt.type = "guessed"
            else:
                toPt.valid = model.port.Port.STATE_INVALID                    
    
    if fromPt.type == None:
        fromPt.type = toPt.type 

    # set to relation
    edg.setTo(toNd, toPt)
    
    return edg
    

def CreateGraph(xmlNodes, xmlEdges):
    graph = model.graph.Graph()
    
    for xnd in xmlNodes[:]:
        nd = CreateNode(xnd)
        if nd <> None:
            graph.AddNode(nd)            
        
    for xed in xmlEdges[:]:
        eg = CreateEdge(xed, graph)
        if eg <> None:
            graph.AddEdge(eg)
        
    return graph

def LoadGraph(fileName):
    xmlDoc = xml.dom.minidom.parse(fileName)
        
    nds = xmlDoc.documentElement.getElementsByTagName("node")
    edg = xmlDoc.documentElement.getElementsByTagName("edge")
    
    graph = CreateGraph(nds, edg)
    graph.origXML = xmlDoc
    return graph
   
    