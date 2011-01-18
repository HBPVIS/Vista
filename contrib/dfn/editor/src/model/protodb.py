'''
Created on Aug 18, 2009

@author: assenmac
'''

import nodeprototype as np
import basemodel
import string


class ProtoDB(basemodel.BaseModel):
    '''
    classdocs
    '''

    db = None
    
    def __init__(self):
        '''
        Constructor
        '''
        basemodel.BaseModel.__init__(self)
        self.protos = dict() # map from type-name (factory) to prototype description
     
    def addPrototype(self, nproto):
        if self.protos.has_key(nproto.classtype):
            return False
        self.protos[nproto.classtype] = nproto
        self.notify( dict({'add': nproto}) )
        return True
    
    def getPrototypeByName(self, name):
        if self.protos.has_key(name):
            return self.protos[name] # ok direct name match
        idx = string.find(name, "[")
        if idx > 0:
            prefix = name[:idx] # determine prefix once (stays constant)
            for x in self.protos.keys():
                if idx > len(x):
                    continue
                if x == prefix:
                    # well, yes...
                    # check subtype matching
                    proto = self.protos[prefix] # x == prefix :)
                    if proto <> None and proto.hasSubType(name) == True: # check for subtypes
                        return proto
        return None
    
    
        