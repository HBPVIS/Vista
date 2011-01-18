'''
Created on Aug 10, 2009

@author: assenmac
'''

from types import *

import wx
import wx.lib.ogl as ogl
import wx.lib.evtmgr as em
import wx.lib.dragscroller
import os
import sys
import tempfile
import string
import pydot


import nodeshape, edgeshape
import model


class GraphCanvas(ogl.ShapeCanvas):
    '''
    classdocs
    '''


    def __init__(self, parent):
        '''
        Constructor
        '''
        ogl.ShapeCanvas.__init__(self, parent)
            
        self.SetBackgroundColour(wx.LIGHT_GREY)
        self.diagram = ogl.Diagram()
        self.SetDiagram(self.diagram)
        self.diagram.SetCanvas(self)
        self.shapes   = dict()
        self.edges    = []
        self.save_gdi = []
        self.graph    = None
        self.middle   = None
        self.showPortHull = None
        self.newedgeshape = None
        # look for the graphviz executables
        # (we cache that per graphviz window... could
        # probably be done somewhere else...)
        self.dot = pydot.find_graphviz()

     
    def ResizePane(self, width, height):
        self.maxWidth  = width
        self.maxHeight = height
        self.SetScrollbars(1, 1, self.maxWidth, self.maxHeight)

    def ResetState(self):
        self.diagram.RemoveAllShapes()
        self.shapes = dict() # kill old shapes
        self.edges  = [] # kill old edges
        self.showPortHull = None
        self.newedgeshape = None
        self.graph = None

     
        
    def OnMouseEvent(self, evt):
        
        if evt.LeftDClick():
            self.OnLeftDoubleClick(evt)
            
        if self.middle <> None:
            vx,vy = self.GetViewStart()
            x,y = evt.GetPosition()
            self.Scroll( (self.middle[0]-x)+vx, (self.middle[1]-y)+vy )
            self.middle = (x,y)
        
        if evt.MiddleDown():
            if self.middle == None:
                self.CaptureMouse()
                x,y = evt.GetPosition()
                self.middle = (x,y)
                self.SetCursor(wx.StockCursor(wx.CURSOR_SIZING))            
        elif evt.MiddleUp():
            self.ReleaseMouse()
            self.middle = None
            self.SetCursor(wx.STANDARD_CURSOR)
        else:
            # unhandled, pass to super-class
            ogl.ShapeCanvas.OnMouseEvent(self, evt)     

          
    def OnLeftDoubleClick(self, evt):
        pass
    
    
    def GetGraph(self):
        return self.graph
    
    def SetGraph(self, graph):
        self.ResetState()
        
        self.graph = graph
                       
        for gnd in graph.nodes[:]:
            shp = nodeshape.NodeShape(self, gnd)
            bx = self.AddNodeShape( shp, 0, 0, gnd.name )            
                                                        
        for ged in graph.edges[:]:
            shp = edgeshape.EdgeShape(self, ged)
            self.AddEdgeShape(shp)
                          
        self.doLayout()        
        self.Refresh()
          
    
    def doLayout(self):
        dc = wx.ClientDC(self)
        self.PrepareDC(dc)

        # intermediate fix: we need the new pydot version for this to
        # work, but this will not work with python 2.5.x, so 
        # we keep the old version (including the need for pygraphviz)
        # side-by-side to the new version. Note that we do only check
        # for the python version, and not for the platform or else
        # as python 2.5.x on a windows platform will most likely fail
        # to do the layout in any case
        if sys.version_info[0] >= 2 and sys.version_info[1] >= 6:
            # create a graph on the level of pydot
            G = pydot.Graph("G", graph_type='digraph', strict=False)
            for nd in self.shapes.keys(): #iterate over all shapes
                shp = self.shapes[nd]
                # dot does not accept all names, so we escape the
                # name using pydots routines for that
                nd = pydot.quote_if_necessary(nd) 
                
                # add a node, specify outer rectangle as bounding box to respect
                # during the layout, use fixedsize to state this cleanly 
                # we convert from pixels to inches, as dotty seems to do
                # all layouts in inches, for whatever reason
                node = pydot.Node(str(nd), shape='box', fixedsize='true', width=str(shp.GetWidth()/72), height=str(shp.GetHeight()/72))
                
                # add this node to the set of nodes on the pydot graph
                G.add_node(node)
    
            # now create some edges in this graph
            for ed in self.graph.edges[:]:
                # note that we changed the name of the nodes during the
                # upper loop, and this is what we have to respect here:
                # convert/escape any name correctly
                # we try to recover this node from the pydot graph using its
                # escaped name
                nd_from = G.get_node( pydot.quote_if_necessary(str(ed.getFrom()[0].name)) )
                nd_to   = G.get_node( pydot.quote_if_necessary(str(ed.getTo()[0].name)) )
                
                # can be a list-type in case we have more than one node
                # having the same name (how can that happen?
                # we ignore this case, as here this is probably an error
                # so just progress with the node when we have a non
                # list type for either the source or the sink of this edge
                if type(nd_from) is not ListType and type(nd_to) is not ListType:
                    Aedge = pydot.Edge(nd_from, nd_to)
                    G.add_edge(Aedge)
                    
            # ok, done, do the layout now
            try:
                # create a temp file, but do not delete it (we need
                # its file name), sidenode: the delete=False option is new
                # to python 2.6, it will not work with python 2.5.x
                fd = tempfile.NamedTemporaryFile(delete=False)
                fd.write(G.to_string())
                fd.close() # does not delete it, so the file name is still ok
                fd_name = fd.name
                # same here for the output file
                fd_out = tempfile.NamedTemporaryFile(delete=False)
                fd_out.close()
                fd_out_name = fd_out.name                   
                    
                # get the dot executable from the cached set of
                # dotty executables
                dot_exe = self.dot["dot"]
                
                if dot_exe <> None:
                    # construct command line, using dot as output format
                    # again (we need the position and not much more)
                    dot_exe = dot_exe + " -Tdot -o "+fd_out_name+" "+fd_name
                    # execute
                    print(dot_exe)
                    os.system(dot_exe)
                    # re-parse the file again, overwrite old graph
                    G = pydot.graph_from_dot_file(fd_out_name)                
    
                # cleanup
                os.unlink(fd_name)
                # cleanup
                os.unlink(fd_out_name)
            except IOError:
                print "I/O error on layout. Aborting."
                return
    
            # if the above failed, we might not be here (have to test that)
            # if it did not fail, we have a bounding box
            # G.get_bb() will deliver "x,y,w,h" as a string, including the
            # quotes, that is why we strip them first (tail and head)
            bb = string.split(string.strip(G.get_bb(),'\"'), ',', 4)
            
            # w,h are in bb[2] and bb[3] respectively
            self.ResizePane(int(bb[2]), int(bb[3]))
            
            # now set the positions for all the nodes we have
            for nd in G.get_node_list()[:]:
                # get pos attrib
                # for some reason, we can get a Node object as nd, which
                # is not a regular node of our graph. Is this internal to
                # pydot or a parsing error? However, we just check whether
                # we got a "pos" attrib as a result of the dot exec
                pos = nd.get('pos')
                if pos <> None:
                    # yes, this is the string "x,y" (including quotes)
                    a = string.split( string.strip(pos, '\"'), ',', 2)
                    # move from (0,0) to their layout position
                    # note the shift in the y-component related to bb[3] (ymax)
                    # move seems to be an absolute API, so not translate
                    try:
                        # we now need the name in the graph. here, we
                        # rely on the quote escaping done by pydot...
                        # maybe there is a method to "unescape" a name again?
                        # could be the source of an error
                        nd_name = string.strip(nd.get_name(), '\"')
                        
                        # can fail when we did not hit the right name
                        # note that we have to "flip" the y coordinate,
                        # as dotty doe not only calc in inch, but also from 
                        # bottom to top
                        self.shapes[nd_name].Move(dc, int(a[0]), int(bb[3])-int(a[1]), display=False)
                    except KeyError:
                            print "key error: node ["+str(nd)+"] not found in shapes?"     
        else: # python 2.5.x
            import pygraphviz as pgv        
            # create an empty graph first
            G = pgv.AGraph(directed=True,strict=True)
            for nd in self.shapes.keys():
                shp = self.shapes[nd]
                G.add_node(str(nd))
                ANd = G.get_node(str(nd))
                ANd.attr['shape'] = 'box'
                ANd.attr['fixedsize'] = 'true'
                ANd.attr['width']  = str(shp.GetWidth()/72)
                ANd.attr['height'] = str(shp.GetHeight()/72)
                        
            for ed in self.graph.edges[:]:
                Aedge = G.add_edge( str(ed.getFrom()[0].name), str(ed.getTo()[0].name) )
                 
            try:
                G.layout(prog='dot',args='-q1')
            except IOError:
                print "I/O error on layout. Aborting."
                return
    
                
            
            # dotty seems to layout as American, as can get:
            # y points up, node sizes are in inches and so on
            # assumed is a 72dot per inch scaling
            # so what we need it to 
            # - 'flip' the nodes (we layout from top to bottom)
            # - recenter the graph (we need the bb for that)
            # unfortunately, pygraphviz does not seem to offer the
            # bb property of the graph (it IS written to the dot!)
            # so we iterate over all nodes and use the max of the
            # x and y vars (related to border, not to center) as
            # an approximate of the bounding box we will have for
            # the graph
            xmax, ymax = 0,0
            for nd in G.nodes():
                pos = nd.attr['pos']
                w   = (float(nd.attr['width'])  * 72) / 2 # in inch, calc 72dots per inch, calculated from center
                h   = (float(nd.attr['height']) * 72) / 2 # in inch, calc 72dots per inch, calculated from center
                
                a = string.split(pos,',',2)
                 
                xmax = max(int(a[0])+w, xmax)
                ymax = max(int(a[1])+h, ymax)    
                
            # resize canvas
            self.ResizePane(xmax, ymax)
    
            for nd in G.nodes():
                pos = nd.attr['pos']
                a = string.split(pos,',',2)
                # move from (0,0) to their layout position
                # note the shift in the y-component related to ymax
                # move seems to be an absolute API, so not translate
                try:
                    self.shapes[nd].Move(dc, int(a[0]), ymax-int(a[1]))
                except KeyError:
                    print "key error: node ["+str(nd)+"] not found in shapes?"
        pass
    
    def AddEdgeShape(self, edgeshp):
        edge = edgeshp.model
        if self.shapes.has_key(edge.getFrom()[0].name):
            frPortShp = self.shapes[edge.getFromNode().name].getPortShape(edge.getFromPort().name, False)
        if self.shapes.has_key(edge.getTo()[0].name):
            toPortShp = self.shapes[edge.getToNode().name].getPortShape(edge.getToPort().name, True)
            
        # color edge according to its validity state
        if frPortShp <> None and toPortShp <> None:
            # default: BLACK
            color = wx.BLACK_PEN
            # possible type error
            if edge.getFrom()[1].valid == model.port.Port.STATE_INVALID or edge.getTo()[1].valid == model.port.Port.STATE_INVALID:
                # one of the ports is INVALID
                color = wx.RED_PEN
            elif edge.getFrom()[1].valid == model.port.Port.STATE_PROBABLY_OK or edge.getTo()[1].valid == model.port.Port.STATE_PROBABLY_OK:
                # none is invalid, but not valid as well, this is a warning
                color = wx.Pen(wx.Color(0,0,255))
            elif edge.getFrom()[1].type <> edge.getTo()[1].type:
                # last resort: does at least the type match?
                color = wx.RED_PEN

        self.diagram.AddShape(edgeshp) # keep a copy in the diagram
        edgeshp.SetPen(color)
        frPortShp.AddLine(edgeshp,toPortShp) # create a line reference
        edgeshp.Show(True)
        self.edges.append(edgeshp) # for fast lookup: keep a copy as edge, 
                                   # and not only as shape 
        return edgeshp
        

    def AddNodeShape(self, shape, x, y, name):
        # Composites have to be moved for all children to get in place
        if isinstance(shape, ogl.CompositeShape):
            dc = wx.ClientDC(self)
            self.PrepareDC(dc)
            shape.Move(dc, x, y,display=False)
        else:
            shape.SetDraggable(True, True)
            
        shape.SetCanvas(self)
        shape.SetX(x)
        shape.SetY(y)

        self.diagram.AddShape(shape)
        shape.Show(True)
        
        self.shapes[name] = shape
        return shape
    
    def RemEdgeShape(self, edgeshape, refresh=False):
        # remove shape
        ged = edgeshape.model # we search the edge by the model
        frPortShp = self.shapes[ged.getFromNode().name].getPortShape(ged.getFromPort().name, False)
                     # find the port-shape in the lookup on shapes (only rects in there)
                     
        self.diagram.RemoveShape(edgeshape) # remove it from the diagram
        frPortShp.RemoveLine(edgeshape) # remove the line associated from the port shape
        self.edges.remove(edgeshape) # remove from the fast edge lookup 
        
        if refresh == True:
            self.Refresh()
    
    
    def DoRemove(self, shape):
        if len(shape._children) > 0:
            for child in shape._children[:]:
                self.DoRemove(child) # get rid of the kids
                                
            self.RemoveShape(shape) # now remove me :)

        else:
            self.RemoveShape(shape) # plain kid, just remove me
        pass
    
    
    
    def RemNodeShape(self, nodeshape, refresh=False):
        '''
            nodeshape is of type Rectangle, as this is the one we clicked
            on, its parent contains all elements to be drawn as a flat layout
            so we start removing on the parent 
        '''            
        # remove shape (recursively)        
        self.DoRemove(nodeshape.GetParent())

        # get rid of all edges pointing to this node
        if nodeshape.GetParent().model <> None:
            edges = self.graph.GetEdges(nodeshape.GetParent().model)           
            for e in edges[:]:
                edShp =  self.GetEdgeForElement(e)
                self.RemEdgeShape( edShp )
            
        if refresh == True:
            self.Refresh()
                
    def ShowPath(self, path):
        self.diagram.ShowAll(False) # turn them all off
        for x in path[:]:
            shp = None
            if x._shape <> None:
                shp = x._shape
            else:
                shp = self.GetShapeForElement(x)
                
            if shp <> None:
                shp.Show(True) # includes edges
                
        self.Refresh()
        pass

    def ShowsHull(self, port=None):
        if port == None:
            if self.showPortHull <> None:
                return True
            else:
                return False
        else:
            if port == self.showPortHull:
                return True
            else:
                return False

    
    def ShowTransitiveHull(self, port):
        if port == None:
            self.ShowAll()
            return
        
        if self.showPortHull == port:
            # reset state
            self.showPortHull = None
            self.ShowAll()
        else:
            # show transitive hull for new port
            if port.parent <> None:
                dir = None
                if port.direction == model.port.Port.INPORT:
                    dir = model.graph.Graph.BACKWARD
                elif port.direction == None:
                    print "none?"
                else:
                    dir = model.graph.Graph.FORWARD
                     
                l = self.graph.GetTransitiveHull( port.parent, dir, port )
                if len(l) > 0:
                    self.ShowPath(l)
                    self.showPortHull = port
                else:
                    self.ShowAll()
                    self.showPortHull = None

            pass
        
    def OnLeftClick(self, x=0, y=0, keys=0):
        if self.showPortHull <> None:
            self.showPortHull = None
            self.ShowAll()
        
    def OnRightClick(self, x=0, y=0, keys=0):
        pass
    
        
    def GetEdgeForElement(self, model):
        for x in self.edges[:]:
            if x.model == model:
                return x
        return None
    
    def GetShapeForElement(self, model):
        l = self.shapes.values()
        for x in l[:]:
            if x.model == model:
                return x
        return None
        
    
    def ShowAll(self):
        self.diagram.ShowAll(True)
        self.Refresh()

        
    def BeginNewEdge(self, x, y):
        # create edge if not existing
        if self.newedgeshape is None:
            self.newedgeshape = edgeshape.EdgeShape(self, model.edge.Edge())
            self.diagram.AddShape(self.newedgeshape)
            self.newedgeshape.SetPen(wx.GREY_PEN)
        # manually set start and endpoints and make visible
        self.newedgeshape.SetEnds(x, y, x, y)
        self.newedgeshape.Show(True)

    def DragNewEdge(self, x, y):
        x0, y0, x1, y1 = self.newedgeshape.GetEnds()
        self.newedgeshape.SetEnds(x0, y0, x, y)
        #self.Refresh(False, wx.Rect(x0, y0, max(x1,x), max(y1,y)))
        self.Refresh()

    def EndNewEdge(self, x, y):
        # check wether we start on an out- and end on an inport.
        x0, y0, x1, y1 = self.newedgeshape.GetEnds()
        
        # get the source and dest shapes
        l = self.shapes.values()
        fromshp = None
        toshp = None
        for shp in l[:]:
            # check outport shapes for source coordinate hit
            for portshape in shp.outp[0]:
                if portshape.HitTest(x0,y0):
                    fromshp = portshape
                   
            # check inport shapes for dest coordinate hit
            for portshape in shp.inp[0]:
                if portshape.HitTest(x,y):
                    toshp = portshape

        fromnode = None
        fromport = None
        tonode = None
        toport = None
        # if we found a source and dest shape
        if fromshp <> None and toshp <> None:
            # get models corresponding to the shapes
            for node in self.graph.nodes[:]:
                for port in node.GetOutPorts():
                    if port._shape == fromshp:
                        fromport = port
                        fromnode = node
                        break
                for port in node.GetInPorts():
                    if port._shape == toshp:
                        toport = port
                        tonode = node
                        break

        # if we found src and dest port
        if fromport <> None and toport <> None:
            # construct edge model
            self.newedgeshape.model.setFrom(fromnode, fromport)
            self.newedgeshape.model.setTo(tonode, toport)

            # check if an edge with same toport doesn't yet exist.
            createedge = True
            for edge in self.graph.edges[:]:
                if edge.getTo() == self.newedgeshape.model.getTo():
                    print "warning: creating multiple connections to the same inport"
                    createedge = False
                    break

            # check if fromnode is in the tonodes transitive hull
            hull = self.graph.GetTransitiveHull(tonode, self.graph.FORWARD)
            for node in hull:
                if node == fromnode:
                    print "warning: attempt to create a cycle"
                    createedge = False
                    break

            if createedge:
                # we add the edge model to the graph
                # and create a corresponding shape.
                # todo: move this to the notification callback
                # for graph.AddEdge for true model/view separation!
                self.graph.AddEdge(self.newedgeshape.model)
                self.AddEdgeShape(self.newedgeshape)
                self.newedgeshape = None
            else:
                # there is a reason not to create the edge, so we do nothing
                # but disabling the new edge shape again.
                self.newedgeshape.Show(False)

        else:
            self.newedgeshape.Show(False)

        self.Refresh()
