'''
Created on Aug 10, 2009

@author: assenmac
'''

import port, parameter, nodeprototype, basemodel

class Node(basemodel.BaseModel):
    '''
    classdocs
    '''
    def __init__(self, name):
        '''
        Constructor
        '''
        basemodel.BaseModel.__init__(self)
        
        self.__inports   = []
        self.__outports  = []
        self.name      = name
        self.type      = None
        self.prototype = None
        self.valid     = True
        self.status    = None
        self.params    = parameter.Parameter()
        self.comment   = None
        self.graph     = None
        
        self._shape    = None
        
    
    def GetInPorts(self):
        #if self.prototype <> None:
        #    prt = self.prototype.getInPorts().values()
        #    prt = prt + self.inports
        #    return prt
        return self.__inports
    
    def GetOutPorts(self):
        #if self.prototype <> None:
        #    prt = self.prototype.getOutPorts().values()
        #    prt = prt + self.outports
        #    return prt
        return self.__outports
    
    def AddInPortAsPort(self, port):
        self.__inports.append(port)
        port.parent = self
        
    def AddOutPortAsPort(self, port):
        self.__outports.append(port)
        port.parent = self
        
        
    def AddInPort(self, name):
        '''
        adds an inport with the name given
        '''
        pt = port.Port(name)
        self.__inports.append(pt)
        pt.parent = self
        pt.direction = port.Port.INPORT
        return pt
        
    def AddOutPort(self, name):
        '''
        adds an outport prototype with the name given
        '''
        pt = port.Port(name)
        self.__outports.append(pt)
        pt.parent = self
        pt.direction = port.Port.OUTPORT
        return pt
        
    def GetOutPort(self, name):
        #if self.prototype <> None:
        #    p = self.prototype.getOutPort(name)
        #    if p == None:
        #        return self.GetPortFrom(name, self.outports)
        #    else:
        #        return p
        #else:
            return self.GetPortFrom(name, self.__outports)
    
    def GetInPort(self, name):
        #if self.prototype <> None:
        #    p = self.prototype.getInPort(name)
        #    if p == None:
        #        return self.GetPortFrom(name, self.inports)
        #    else:
        #        return p
        #else:
            return self.GetPortFrom(name, self.__inports)
    
    def GetPortFrom(self, name, list):
        for x in list[:]:
            if x.name == name:
                return x
        return None
    
    def GetEdges(self):
        return self.graph.GetEdges(self)
    
    def GetHasSpuriousPorts(self):
        for x in self.__inports[:]:
            if x.valid <> port.Port.STATE_VALID:
                return True
        
        for x in self.__outports[:]:
            if x.valid <> port.Port.STATE_VALID:
                return True
            
        return False
    
    def setName(self, name):
        if len(name) > 0:
            self.name = name
            self.notify(dict({'name' : self.name}))