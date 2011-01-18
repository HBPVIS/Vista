'''
Created on Aug 13, 2009

@author: assenmac
'''

import xml.dom.minidom
import model.graph, model.node, model.port, model.edge, model.parameter



def CreateParameterElement(param, xmlNode, doc):
    for key in param.keys():
        parent = doc.createElement("param")
        parent.setAttribute("name", key)
        if isinstance(param[key], model.parameter.Parameter):
            parent.appendChild(CreateParameterElement( param[key], parent, doc))
        else:
            if param[key][1] == 0: # found a normal value attribute
                parent.setAttribute( "value", str(param[key][0]) )
            else: # found as text node
                tel = doc.createTextNode(param[key][0])
                parent.appendChild(tel)
        
        if param.comment <> None:
            xmlNode.appendChild( doc.createComment( str( param.comment ) ) )
        xmlNode.appendChild(parent)
        
    return parent
    
            


def CreateNodeElement(node, xmlNode, doc):
    xmlNode.setAttribute( "name", node.name )
    xmlNode.setAttribute( "type", node.type )
    if node.params <> None and node.params.__len__() > 0:
        CreateParameterElement( node.params, xmlNode, doc )
    return xmlNode
    

def CreateEdgeElement(edge, xmlNode):
    xmlNode.setAttribute( "fromnode", edge.getFrom()[0].name )
    xmlNode.setAttribute( "tonode", edge.getTo()[0].name )
    xmlNode.setAttribute( "fromport", edge.getFrom()[1].name )
    xmlNode.setAttribute( "toport", edge.getTo()[1].name )
    
    return xmlNode


def WriteNodeTree(graph, doc):
    parent = doc.createElement("graph")
    for nd in graph.nodes[:]:
        xmlNode = doc.createElement("node")
        el = CreateNodeElement(nd, xmlNode, doc)
        if el <> None:
            if nd.comment <> None:
                parent.appendChild(doc.createComment(str(nd.comment)))
            parent.appendChild(el)
    return parent

def WriteEdgeTree(graph, doc):
    parent = doc.createElement("edges")
    for eg in graph.edges[:]:
        xmlNode = doc.createElement("edge")
        el = CreateEdgeElement(eg, xmlNode)
        if el <> None:
            if eg.comment <> None:
                parent.appendChild(doc.createComment(str(eg.comment)))
            parent.appendChild(el)
    
    return parent

def WriteGraph(graph, filename):
    fout = open(filename, "w+") # directly try to open the file for writing
    assert(fout <> None) # make sure, this worked

    xmlDom = xml.dom.minidom.getDOMImplementation()
    doc = xmlDom.createDocument(None, "module", None)
    
    # write the graph tree (including parameters)
    grp = WriteNodeTree(graph, doc)
    if grp <> None:
        doc.documentElement.appendChild(grp)
    
    # write the edge tree    
    edges = WriteEdgeTree(graph, doc)
    if edges <> None:
        doc.documentElement.appendChild(edges)
    
    
    # write out of the xml tree, pretty-printed
    doc.writexml(fout, addindent="\t", newl="\n")

    # be nice: close the file
    fout.close()
    