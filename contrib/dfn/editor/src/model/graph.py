'''
Created on Aug 12, 2009

@author: assenmac
'''

import node, port, edge


class Graph():
    '''
    classdocs
    '''

    FORWARD  = 0 # transitive hull search direction FORWARD
    BACKWARD = 1 # transitive hull search direction BACKWARD
    
    def __init__(self):
        '''
        Constructor
        '''
        self.edges = []
        self.nodes = []
        self.origXml = None
        
    def AddNode(self, node):
        self.nodes.append(node)
        node.graph = self
        
    def RemNode(self, node):
        # find all adges referring to this node first
        edgelist = self.GetEdges(node)
        for x in edgelist[:]:
            self.RemEdge(x)
                
        # remove all edges first
        # now remove node
        self.nodes.remove(node)
        pass
    
    def AddEdge(self, edge):
        self.edges.append(edge)
        edge.graph = self
        
    def RemEdge(self, edge):
        self.edges.remove(edge)
        
        
    def GetEdges(self, node):
        ed = []
        for x in self.edges[:]:
            if x.getToNode() == node or x.getFromNode() == node:
                ed.append(x)
        return ed
    
    def GetInputEdges(self, node):
        ed = []
        for x in self.edges[:]:
            if x.getToNode() == node:
                ed.append(x)
        return ed
    
    def GetOutputEdges(self, node):
        ed = []
        for x in self.edges[:]:
            if x.getFromNode() == node:
                ed.append(x)
        return ed
        
    def GetNode(self, name):
        for x in self.nodes[:]:
            if x.name == name:
                return x
        print "node [" + name + "] not found"
        return None
    
    def GetTransitiveHull(self, node, dir=FORWARD, port=None):
        l = [node] # the node is in the hull as well
        if dir == self.FORWARD:
            for x in self.GetOutputEdges(node):
                if port == None:  
                    # x is the list of edges from this node
                    l = l + self.GetTransitiveHull( x.getToNode(), dir ) # recursive call (no cycles!)
                    l.append(x)
                elif port == x.getFromPort():
                    l = l + self.GetTransitiveHull( x.getToNode(), dir ) # recursive call (no cycles!)
                    l.append(x)
                            
        else:
            for x in self.GetInputEdges(node):
                if port == None:
                    # x is the list of edges from this node
                    l = l + self.GetTransitiveHull( x.getFromNode(), dir ) # recursive call (no cycles!)
                    l.append(x)
                elif port == x.getToPort():
                    l = l + self.GetTransitiveHull( x.getFromNode(), dir ) # recursive call (no cycles!)
                    l.append(x)
        
        return l
        
        
    