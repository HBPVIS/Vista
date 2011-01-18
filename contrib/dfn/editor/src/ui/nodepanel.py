'''
Created on 16.08.2009

@author: ingo
'''
import wx

import parampanel
import model.node, model.parameter
import model.protodb as pdb
import model.observer as observer

class NodePanel(wx.Panel, observer.Observer):
    '''
    classdocs
    '''


    def __init__(self, parent, ID, model=None):
        '''
        Constructor
        '''
        wx.Panel.__init__(self, parent,ID)
        observer.Observer.__init__(self)
        
        self.SetAutoLayout(True)
        self.model = None
        
        self.protodb = pdb.ProtoDB.db
        
        # widgets
        p = wx.Panel(self, -1, style = wx.TAB_TRAVERSAL
                     | wx.CLIP_CHILDREN
                     | wx.FULL_REPAINT_ON_RESIZE
                     | wx.EXPAND
                     )
        
        gbs = self.gbs = wx.GridBagSizer(3,2)
        self.namep = wx.TextCtrl(p, ID, "", size=wx.DefaultSize)
        self.typep = wx.ComboBox(p, -1, choices=sorted(self.protodb.protos.keys()))
        self.params = parampanel.ParamPanel(self)
        self.typectrl = wx.Button(p, -1, "Error" )
        self.typectrl.Show(False)
         
        gbs.Add( wx.StaticText(p, -1, "Name:", style=wx.ALIGN_LEFT | wx.ALIGN_CENTER_VERTICAL ), (0,0))
        gbs.Add( self.namep, (0,1), flag = wx.EXPAND )
        gbs.Add( wx.StaticText(p, -1, "Type:", style=wx.ALIGN_LEFT| wx.ALIGN_CENTER_VERTICAL ), (1,0) )
        gbs.Add( self.typep, (1,1), flag = wx.EXPAND )
        gbs.Add( wx.StaticText( p, -1, "Parameter:", style=wx.ALIGN_LEFT ), (2,0), flag = wx.EXPAND )
        gbs.Add( self.typectrl, (1,2) )
        
        lc = wx.LayoutConstraints()
        lc.top.Below( p )
        lc.left.SameAs( self, wx.Left )
        lc.right.SameAs( self, wx.Right )
        lc.bottom.SameAs( self, wx.Bottom )
        self.params.SetConstraints(lc)
                    
        box = wx.BoxSizer()
        box.Add(gbs, 0, wx.ALL, 10)
        
        p.SetSizerAndFit(box)
        self.SetClientSize(p.GetSize())   
        
        # event stuff
        self.Bind(wx.EVT_TEXT, self.OnTextEnter, self.namep)
        self.GetParent().Bind(wx.EVT_CLOSE, self.OnCloseMe )
        
        
        # setup values when model was given!
        if model <> None:
            self.fromModel(model)
                  
    
    def OnCloseMe(self, evt):
        if self.model <> None:
            self.model.detachObserver(self)
            self.model = None
            self.GetParent().Show(False)
            self.GetParent().Destroy()
        return True
            
    def OnTextEnter(self, evt):
        if self.model <> None:
            self.model.setName(self.namep.GetValue())
        pass
    
    def fromModel(self, model):
        if self.model <> None:
            self.model.detachObserver(self)
            
        self.model = model
        if self.model <> None:
            self.model.attachObserver(self)
            self.namep.SetValue(model.name)
            
            if self.protodb.getPrototypeByName(model.type) == None:
                self.typectrl.SetBackgroundColour(wx.RED)
                self.typectrl.Show(True)
            else:
                self.typectrl.SetBackgroundColour(wx.NullColour)
                self.typep.SetValue(model.type)
                self.typectrl.Show(False)
                                
            self.params.fromParams(self.model.params)
            #print model.GetEdges().__len__()
            #if model.GetEdges().__len__() <> 0:
            #    self.typep.Disable()
            #lse:
            #    self.typep.Enable()
        else:
            self.namep.SetValue("")
            self.typep.SetValue("")
            self.typectrl.SetBackgroundColour(wx.NullColour)
            self.typectrl.Show(False)
            self.params.fromParams(None)
        pass
    
    