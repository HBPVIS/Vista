'''
Created on Aug 18, 2009

@author: assenmac
'''

import model.protodb, model.nodeprototype, model.port
import xmlutils
import graphloader
import os, fnmatch, string
import xml.dom.minidom
import string

def CreatePort(xmlNode, instanceTags = None):

    assert(xmlNode.hasAttribute("name") and xmlNode.hasAttribute("type"))

    xmlName = xmlNode.getAttribute("name")
    xmlType = xmlNode.getAttribute("type")
    
    port = model.port.Port(xmlName)
        
    if instanceTags <> None:
        if xmlType[:1] == "@":
            # referencing sub-type here
            sub = xmlType[1:] # index is given after @
            if len(sub): # ok, is there an index?
                idx = min(int(sub),len(instanceTags)-1) # this is a bit tricky:
                                                        # it allows to shorten a type given
                                                        # for example a bi-template float,float can be abbreviated with just float
                                                        # the trick is to assign the last value as the resolving one
                try:
                    port.type = instanceTags[idx] # see if we got an index (should)
                except IndexError:
                    print "index error?"
                    port.valid = model.port.Port.STATE_INVALID # no, simply mark invalid
                    
            else: # syntax error
                port.valid = model.port.Port.STATE_INVALID
                port.status = "type could not be deduced during load"
    else:
        port.type  = xmlType # no instance tags? normal type then!
        port.valid = model.port.Port.STATE_VALID
            
    return port

def CreatePorts( xmlNode, prototype, instanceTags ):
    # create inports first
    xmlInports = xmlutils.FindChildElement(xmlNode, "inports")
    xmlOutports = xmlutils.FindChildElement(xmlNode, "outports")
    
    ip = xmlInports.getElementsByTagName("port")
    op = xmlOutports.getElementsByTagName("port")
    
    for x in ip[:]:
        p = CreatePort( x, instanceTags )
        if p <> None:
            prototype.inports[p.name] = p
            p.direction = model.port.Port.INPORT

    for x in op[:]:
        p = CreatePort(x, instanceTags )
        if p <> None:
            prototype.outports[p.name] = p
            p.direction = model.port.Port.OUTPORT
    pass

def CreatePrototype( xmlNode ):
    prototype = model.nodeprototype.NodePrototype()
    
    xmlName = xmlutils.FindChildElement(xmlNode, "name")
    prototype.type = xmlName.getAttribute("value")
    
    instanceTags = None
    
    prototype.template = False
    xmlType = xmlutils.FindChildElement(xmlNode, "type")
    if xmlType <> None:
        prototype.classtype = xmlType.getAttribute("value")
    else:
        prototype.template = True
        # type set from above -> template type
        prototype.classtype = prototype.type # + "[" + type + "]"
        
        #prototype.subtypes  = type # mark variant type here
        #instanceTags = string.split( type, "," )
        
    
    xmlOrigin = xmlutils.FindChildElement(xmlNode, "origin")
    prototype.origin = xmlOrigin.getAttribute("value")
    
    xmlPortStyle = xmlutils.FindChildElement(xmlNode, "portstyle")
    if xmlPortStyle <> None:
        portStyle = xmlPortStyle.getAttribute("value")
        prototype.portStyle = prototype.getPortStyleFromString(portStyle)
        if prototype.portStyle == model.nodeprototype.NodePrototype.NOT_DEFINED:
            prototype.valid = False
        else:
            prototype.valid = True
            
                
    xmlParams = xmlutils.FindChildElement(xmlNode, "params")
    xmlFirstParam = xmlutils.FindChildElement(xmlParams, "param") 
    prototype.params = graphloader.CreateParameter( xmlFirstParam )
    
    if prototype.portStyle == model.nodeprototype.NodePrototype.STATIC_PORTS or prototype.portStyle == model.nodeprototype.NodePrototype.OUTPORTS_DEPEND_ON_INPORT_VALUE or prototype.portStyle == model.nodeprototype.NodePrototype.OUTPORTS_SAME_AS_INPORTS_MIXIN:
        xmlPorts = xmlutils.FindChildElement(xmlNode, "ports")
        if xmlPorts <> None:
            CreatePorts(xmlPorts, prototype, instanceTags)
        
    return prototype
    
    

def CreatePrototypes( xmlNode, protodb ):
    # tag lists are defined by the instances tag
    instances = xmlutils.FindChildElement(xmlNode, "instances")
    if instances <> None and len(instances.childNodes) > 0: # instanced node
        tps = xmlutils.FindFirstChildByType(instances, xml.dom.minidom.Node.TEXT_NODE)
        if tps == None:
            return None
        
        ltps = string.split(tps.data, ":") # split along colons, strip whitespace
        subtypes = []
        for word in ltps[:]:
            subtypes.append(string.strip(word))
            
        # create one prototype for all subtypes
        p = CreatePrototype(xmlNode)
        p.setSubypes(subtypes) # link list of subtypes
        
        assert(protodb.addPrototype(p)) # add to protodb
    else: # non-instanced node
        p = CreatePrototype(xmlNode)
        if p <> None:
            assert(protodb.addPrototype(p))
    

def LoadPrototype(fileName, protodb):
    xmlDoc = xml.dom.minidom.parse(fileName)
    CreatePrototypes( xmlDoc.documentElement, protodb )
            
    return protodb
    
def LoadPrototypeDirectory(dirName, protodb = None):
    if protodb == None:
        protodb = model.protodb.ProtoDB()
    try:
        entries = os.listdir(dirName)
        
        for x in entries[:]:
            if fnmatch.fnmatch(x,"*.proto"):
                LoadPrototype(dirName+"/"+x, protodb)
          
    except OSError:
        print "directory [" + dirName + "] does not exist. wd = [" + os.getcwd() + "]"

    return protodb 
    
