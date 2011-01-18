'''
Created on Aug 12, 2009

@author: assenmac
'''

import wx
import wx.lib.ogl as ogl

class EdgeEventHandler(ogl.ShapeEvtHandler):
    '''
    classdocs
    '''

    def __init__(self, parent):
        '''
        Constructor
        '''
        ogl.ShapeEvtHandler.__init__(self)
        self.ID_DEL_EDGE     = wx.NewId()
        self.ID_RECON_FROM = wx.NewId()
        self.ID_RECON_TO   = wx.NewId()
        self.parent = parent       
        
        
    def OnRightClick(self, x, y, keys=0, attachment=0):       
         # make a menu
        menu = wx.Menu()
        # Show how to put an icon in the menu
        menu.AppendItem(wx.MenuItem(menu, self.ID_DEL_EDGE,"Delete Edge"))
        menu.AppendItem(wx.MenuItem(menu, self.ID_RECON_FROM,"Reconnect FROM"))
        menu.AppendItem(wx.MenuItem(menu, self.ID_RECON_TO,"Reconnect TO"))
        
        self.parent.Bind(wx.EVT_MENU, self.OnDelEdge, id=self.ID_DEL_EDGE)
        self.parent.Bind(wx.EVT_MENU, self.OnReconSource, id=self.ID_RECON_FROM)
        self.parent.Bind(wx.EVT_MENU, self.OnReconDest, id=self.ID_RECON_TO)
        
        self.parent.PopupMenu(menu)
        menu.Destroy()
        
    def OnDelEdge(self, evt):
        shape = self.GetShape()
        edge  = shape.model
        graph = edge.graph
        graph.RemEdge(edge)
        self.parent.RemEdgeShape(shape, True)
        

    
    def OnReconSource(self, evt):
        pass
    
    def OnReconDest(self, evt):
        pass
        
         