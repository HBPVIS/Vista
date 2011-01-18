'''
Created on Aug 18, 2009

@author: assenmac
'''
import xml.dom.minidom

def FindComment(node):
    # the comment is the set of all comment nodes before the 
    # last sibling element of the same type!
    cm = None
    while node <> None:
        if node.nodeType == xml.dom.minidom.Node.COMMENT_NODE:
            if cm == None:
                cm = str(node.data)
            else:
                cm = cm + "\n" + str(node.data)
        elif node.nodeType == xml.dom.minidom.Node.ELEMENT_NODE: 
                # ok, we are done
                break
        # no, maybe a text node (end-line?)
        node = node.previousSibling
        
    return cm       


def FindChildElement(parentNode, tag):
    for child in parentNode.childNodes[:]:
        if child.nodeType == xml.dom.minidom.Node.ELEMENT_NODE:
            if child.tagName == tag:
                return child
    return None

def FindFirstChildByType(parentNode, type):
    for child in parentNode.childNodes[:]:
        if child.nodeType == type:
                return child
    return None

def FindFirstSiblingByType(parentNode, type):
    while parentNode <> None:
        if parentNode.nodeType == type:
            return parentNode
        parentNode = parentNode.nextSibling
    
    return None

def FindSiblingElement( element, tag ):
    while element <> None:
        if element.nodeType == xml.dom.minidom.Element.ELEMENT_NODE:
            if element.tagName == tag:
                return element
        element = element.nextSibling        
    return None

