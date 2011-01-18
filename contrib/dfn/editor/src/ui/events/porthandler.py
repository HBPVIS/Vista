'''
Created on Aug 12, 2009

@author: assenmac
'''
import wx
import wx.lib.ogl as ogl

import model.graph, model.port

class PortEventHandler(ogl.ShapeEvtHandler):
    '''
    classdocs
    '''


    def __init__(self, port, canvas):
        '''
        Constructor
        '''
        ogl.ShapeEvtHandler.__init__(self)
        self.canvas = canvas
        self.port   = port
        
        self.ID_HULL = wx.NewId()
        self.ID_SHOW_PROPERTIES = wx.NewId()
        
    def OnRightClick(self, x, y,  keys=0, attachment=0):
        menu = wx.Menu()
        # Show how to put an icon in the menu
        
        menu.AppendItem(wx.MenuItem(menu, self.ID_SHOW_PROPERTIES, "Properties"))
        menu.AppendSeparator()
                        
        if self.canvas.ShowsHull(self.port) == False:
            menu.AppendItem(wx.MenuItem(menu, self.ID_HULL,"Show transitive hull"))
        else:
            menu.AppendItem(wx.MenuItem(menu, self.ID_HULL,"Show full graph"))
        
            
        self.canvas.Bind(wx.EVT_MENU, self.OnTransitiveHull, id=self.ID_HULL)
        self.canvas.Bind(wx.EVT_MENU, self.OnShowProperties, id=self.ID_SHOW_PROPERTIES)
        
        self.canvas.PopupMenu(menu)
        menu.Destroy()
        

    def	OnBeginDragLeft(self, x, y, keys, attachment):
        self.canvas.BeginNewEdge(x, y)

    def OnDragLeft(self, draw, x, y, keys, attachment):
        self.canvas.DragNewEdge(x, y)

    def	OnEndDragLeft(self, x, y, keys, attachment):
        self.canvas.EndNewEdge(x, y)

    def OnTransitiveHull(self, evt):
        self.canvas.ShowTransitiveHull(self.port)
                
    
    def OnShowProperties(self, evt):
        pass
        
        
