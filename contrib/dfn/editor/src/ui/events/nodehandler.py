'''
Created on Aug 12, 2009

@author: assenmac
'''
import wx
import wx.lib.ogl as ogl
import ui.nodeframe as nodeframe


class NodeEventHandler(ogl.ShapeEvtHandler):
    '''
    classdocs-1
    '''


    def __init__(self, canvas, model):
        '''
        Constructor
        '''
        ogl.ShapeEvtHandler.__init__(self)
        self.canvas = canvas
        self.model = model
        self.ID_SHOW_PROPS = wx.NewId()
        self.ID_SHOW_PATH  = wx.NewId()
        self.ID_DELETE_NODE = wx.NewId()
        
        
    def OnRightClick(self, x, y, keys=0, attachment=0):
        menu = wx.Menu()
        # Show how to put an icon in the menu
        menu.AppendItem(wx.MenuItem(menu, self.ID_SHOW_PROPS,"Properties"))
        menu.AppendSeparator()
        menu.AppendItem(wx.MenuItem(menu, self.ID_SHOW_PATH,"Show Nodepath"))
        menu.AppendSeparator()
        menu.AppendItem(wx.MenuItem(menu, self.ID_DELETE_NODE,"Delete Node"))
        
        
        self.canvas.Bind(wx.EVT_MENU, self.OnShowProps, id=self.ID_SHOW_PROPS)
        self.canvas.Bind(wx.EVT_MENU, self.OnShowPath, id=self.ID_SHOW_PATH)
        self.canvas.Bind(wx.EVT_MENU, self.OnDeleteNode, id=self.ID_DELETE_NODE)

        
        self.canvas.PopupMenu(menu)
        menu.Destroy()

    def OnShowProps(self, evt):        
        shape = self.GetShape()
        if shape.GetParent().model <> None:
            nd = nodeframe.NodeFrame(-1, "["+shape.GetParent().model.name+"]", (50,50),
                                    model=shape.GetParent().model)
            nd.Show(True)
            
    def OnShowPath(self, evt):
        pass
    
    def OnDeleteNode(self, evt):
        shape = self.GetShape()
        if shape.GetParent().model <> None:
            self.canvas.RemNodeShape(shape, True)
            shape.GetParent().model.graph.RemNode(shape.GetParent().model)
              
        pass
