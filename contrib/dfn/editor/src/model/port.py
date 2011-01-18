'''
Created on Aug 10, 2009

@author: assenmac
'''
import basemodel

class Port(basemodel.BaseModel):
    '''
    classdocs
    '''

    STATE_VALID       = 0
    STATE_INVALID     = 1
    STATE_PROBABLY_OK = 2
    
    INPORT  = 0
    OUTPORT = 1
    
    def __init__(self, name):
        '''
        Constructor
        '''
        basemodel.BaseModel.__init__(self)
        self.name   = name
        self.status = None
        self.valid  = self.STATE_VALID
        self.type   = None
        self.parent = None
        self.direction = None
        self._shape  = None
        
    def Clone(self):
        p = Port(self.name)
        p.status = self.status
        p.valid  = self.valid
        p.type   = self.type
        p.parent = self.parent
        p.direction = self.direction
        return p