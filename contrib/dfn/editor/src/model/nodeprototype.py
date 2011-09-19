'''
Created on Aug 18, 2009

@author: assenmac
'''

import basemodel
import string
import model.port



class NodePrototype(basemodel.BaseModel):
    '''
    classdocs
    '''
    NOT_DEFINED                     = -1
    STATIC_PORTS                    =  0 # all ports are compile-time bound
    OUTPORTS_DEPEND_ON_INPORT_VALUE =  1 # inport provides a value that determines outports / inports are static
    ALLPORTS_DEPEND_ON_STATE        =  2 # ports are determined by state (difference to 3?)
    PORTS_DEPEND_ON_PARAMETER       =  3 # ports are determined by a specific parameter
    OUTPORTS_SAME_AS_INPORTS        =  4 # node does not provide ports, except the ones routed in
    OUTPORTS_SAME_AS_INPORTS_MIXIN  =  5 # node accepts all ports, but provides its own, too
    OUTPORTS_DEPEND_ON_PARAMETER    =  6 # node has static inports, but configurable outports
    INPORTS_DEPEND_ON_PARAMETER        =  7 # node has static outports, but configurable inports
    
         
    def __init__(self):
        '''
        Constructor
        '''
        basemodel.BaseModel.__init__(self)
        
        self.type      = None  # a string giving the type (nodefactory) name
        self.classtype = None  # a string giving the type of the C++ class
        self.subtypes  = None  # for template classes: list of strings containing the template args
        self.params    = None  # a collection of parameters during creation
        self.portStyle = self.NOT_DEFINED # the port-style flags (see above)
        self.valid     = False # is this prototype valid?
        self.origin    = None  # a string giving the so / source module of this prototype
        self.template  = False # the C++ class is a template (informative)
        self.inports   = dict()
        self.outports  = dict()
        self.sublookup = dict()
        
        self.portStyleString = dict(
                                    { "STATIC_PORTS": NodePrototype.STATIC_PORTS,
                                    "OUTPORTS_DEPEND_ON_INPORT_VALUE" : NodePrototype.OUTPORTS_DEPEND_ON_INPORT_VALUE,
                                    "ALLPORTS_DEPEND_ON_STATE" : NodePrototype.ALLPORTS_DEPEND_ON_STATE,
                                    "PORTS_DEPEND_ON_PARAMETER" : NodePrototype.PORTS_DEPEND_ON_PARAMETER,
                                    "OUTPORTS_SAME_AS_INPORTS" : NodePrototype.OUTPORTS_SAME_AS_INPORTS,
                                    "OUTPORTS_SAME_AS_INPORTS_MIXIN" : NodePrototype.OUTPORTS_SAME_AS_INPORTS_MIXIN,
                                    "NOT_DEFINED" : NodePrototype.NOT_DEFINED,
                                    "OUTPORTS_DEPEND_ON_PARAMETER" : NodePrototype.OUTPORTS_DEPEND_ON_PARAMETER,
                                    "INPORTS_DEPEND_ON_PARAMETER" : NodePrototype.INPORTS_DEPEND_ON_PARAMETER }
                                    )
        self.portStyleId = dict()
        for x in self.portStyleString.keys():
            self.portStyleId[ self.portStyleString[x] ] = x
            
    
    
    def getPortStyleFromString(self, portStyle):
        try:
            return self.portStyleString[portStyle]
        except KeyError:
            return NodePrototype.NOT_DEFINED
    
    def getPortStyleAsString(self, portStyle):
        try:
            return self.portStyleId[portStyle]
        except KeyError:
            return "NOT_DEFINED"
        
        
    def getInPort(self, name):
        if self.inports.has_key(name):
            return self.inports[name]
        return None
    
    
    def getOutPort(self, name):
        if self.outports.has_key(name):
            return self.outports[name]
        return None
        
    def getOutPorts(self):
        return self.outports
    
    def getInPorts(self):
        return self.inports
    
        
    def addSubType(self, st):
        if self.subtypes == None:
            self.subtypes = []
        self.subtypes.append(st)
    
    def getSubTypes(self):
        return self.subtypes
    
    def setSubypes(self, subs):
        self.subtypes = subs
        # expand fast lookup
        self.sublookup = dict() # erase old one
        for x in self.subtypes[:]:
            self.sublookup[self.classtype + "[" + str(x) + "]"] = True
    
    def hasSubType(self, subtype):
        try:
            return self.sublookup[subtype]
        except KeyError:
            return False # not found means: no... not a subtype...
        
    def setupInports(self, node):
        self.setupPort(node, self.inports, node.AddInPortAsPort)
        pass
    
    def setupOutports(self, node):
        self.setupPort(node, self.outports, node.AddOutPortAsPort)
        pass
        
    def setupPort(self, node, portlist, func):
        for x in portlist.values()[:]:
            p = x.Clone() # clone port ;)
            if self.template == True:
                # still have to setup the correct type
                left  = string.find(node.type, "[")
                right = string.rfind(node.type, "]")
                if left > 0 and right > 0:
                    subtype = node.type[left+1:right]
                    ltps = string.split(subtype, ",")
                    
                    if x.type[:1] == "@":
                        # referencing sub-type here
                        sub = x.type[1:] # index is given after @
                        if len(sub): # ok, is there an index?
                            idx = min(int(sub),len(ltps)-1) # this is a bit tricky:
                                                                    # it allows to shorten a type given
                                                                    # for example a bi-template float,float can be abbreviated with just float
                                                                    # the trick is to assign the last value as the resolving one
                            try:
                                p.type = ltps[idx] # see if we got an index (should)
                                
                            except IndexError:
                                print "index error?"
                                p.valid = model.port.Port.STATE_INVALID # no, simply mark invalid
                        else: # syntax error
                            p.valid = model.port.Port.STATE_INVALID
                            p.status = "type could not be deduced during setupInports()"
                else:
                    p.type  = x.type # no instance tags? normal type then!
                    p.valid = model.port.Port.STATE_VALID                                                

            func(p)
            #node.AddInPortAsPort(p)
         
        
