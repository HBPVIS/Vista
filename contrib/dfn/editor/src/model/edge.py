'''
Created on Aug 12, 2009

@author: assenmac
'''

import basemodel

class Edge(basemodel.BaseModel):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        basemodel.BaseModel.__init__(self)
        
        self.nd_from = ()
        self.nd_to   = ()
        self.comment = None
        self.graph   = None
        self._shape  = None
        
    def setFrom(self, nd, pt):
        self.nd_from = nd,pt
    
    def setTo(self, nd, pt):
        self.nd_to = nd,pt
    
    def getFrom(self):
        return self.nd_from
    
    def getTo(self):
        return self.nd_to
    
    def getToNode(self):
        return self.nd_to[0]
    
    def getFromNode(self):
        return self.nd_from[0]
        
    def getFromPort(self):
        return self.nd_from[1]
        
    def getToPort(self):
        return self.nd_to[1]
        
    
    