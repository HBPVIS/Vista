'''
Created on 21.08.2009

@author: ingo
'''
import wx
import wx.lib.customtreectrl as CT
import model.observer as observer
import model.basemodel
import model.protodb
import parampanel
import sys

class ProtoFrame(wx.Frame):
    '''
    classdocs
    '''
    def __init__(self, parent=None, showTypeSelect=False):
        wx.Frame.__init__(self, parent, wx.NewId(), "Protos")
        
        self.panel = Protobrowser( self, model.protodb.ProtoDB.db, showTypeSelect )
        self.Centre()
        pass
    
class Protoviewer(wx.Panel, observer.Observer):
    ''' 
    classdocs
    '''
    
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, wx.NewId())
    
        rows = wx.BoxSizer(wx.VERTICAL)
        
        grid1 = wx.GridSizer(5, 2)
        grid1.Add(wx.StaticText(self, -1, 'Type: ', (5, 5)), 0,  wx.ALIGN_CENTER_VERTICAL)
        self.typelb = wx.StaticText(self, -1, "<none>")
        grid1.Add(self.typelb)
        grid1.Add(wx.StaticText(self, -1, 'Is template?', (5, 5)), 0, wx.ALIGN_CENTER_VERTICAL)
        self.template = wx.CheckBox(self, -1 )
        grid1.Add(self.template)

        grid1.Add(wx.StaticText(self, -1, 'Is valid?', (5, 5)), 0, wx.ALIGN_CENTER_VERTICAL)
        self.validity = wx.CheckBox(self, -1 )
        grid1.Add(self.validity)

        grid1.Add(wx.StaticText(self, -1, 'Origin', (5, 5)), 0, wx.ALIGN_CENTER_VERTICAL)
        self.origin = wx.StaticText(self, -1, "<none>" )
        grid1.Add(self.origin)


        grid1.Add(wx.StaticText(self, -1, 'Port-style', (5, 5)), 0, wx.ALIGN_CENTER_VERTICAL)
        self.portstyle = wx.StaticText(self, -1, "<none>" )
        grid1.Add(self.portstyle)

        rows.Add(grid1, 0, wx.EXPAND | wx.LEFT | wx.RIGHT | wx.TOP, 10)

        
        grid_ports = wx.GridSizer(3, 2)
        self.inports = wx.ListBox(self,-1)
        #self.inports.InsertColumn("name")
        #self.inports.InsertColumn("type")
        grid_ports.Add(wx.StaticText(self, -1, "Inports"))
        
        grid_ports.Add(self.inports, border=5)
        self.outports = wx.ListBox(self,-1)
        #self.outports.InsertColumn("name")
        #self.outports.InsertColumn("type")
        grid_ports.Add(wx.StaticText(self, -1, "Outports"))
        grid_ports.Add(self.outports, border=5)
        
        self.params = parampanel.ParamPanel(self)
        grid_ports.Add(wx.StaticText(self,-1, "Parameters" ))
        grid_ports.Add(self.params, border = 5)
        
        rows.Add(grid_ports, 0, wx.EXPAND | wx.LEFT | wx.RIGHT | wx.BOTTOM )        
        
    
        
        self.SetSizer(rows)
        self.SetAutoLayout(True)
        
        self.model = None
        
        pass

    def fromModel(self, model):
        if self.model == model:
            return
        
        if self.model <> None:
            self.model.detachObserver(self)
            
        self.model = model
        
        if self.model <> None:
            self.model.attachObserver(self)
            self.typelb.SetLabel( self.model.type )
            self.validity.SetValue(self.model.valid)
            self.template.SetValue(self.model.template)
            
            self.portstyle.SetLabel( self.model.getPortStyleAsString(self.model.portStyle ) )
            
            self.inports.Clear()
            self.outports.Clear()
            
            for x in self.model.getInPorts():
                self.inports.Insert(str(x), 0)
            
            for x in self.model.getOutPorts():
                self.outports.Insert(str(x), 0)
                           
                
            self.params.fromParams(self.model.params, False)
                
            
        


class Protobrowser(wx.Panel, observer.Observer):
    '''
    classdocs
    '''
    
    def __init__(self, parent, protodb, showTypeSelect = False ):
        '''
        Constructor
        '''
        wx.Panel.__init__(self, parent, -1)
        observer.Observer.__init__(self)
        self.model = None
        self.showTypeSelect = showTypeSelect
         
        
        self.ctree = CT.CustomTreeCtrl(self, wx.NewId(), wx.DefaultPosition, wx.DefaultSize, 
                                         wx.TR_DEFAULT_STYLE
                                         | wx.TR_FULL_ROW_HIGHLIGHT)
        self.root = self.ctree.AddRoot("Prototypes")
        
        self.Bind(CT.EVT_TREE_SEL_CHANGED, self.OnItemSelected, self.ctree)
        
        self.fromModel(protodb)
        
        all = wx.BoxSizer(wx.HORIZONTAL)
        all.Add(self.ctree, 0, wx.EXPAND | wx.BOTTOM | wx.TOP )
        
        
        right = wx.BoxSizer(wx.VERTICAL)
        all.Add(right, 0, wx.EXPAND | wx.ALL )
                
        self.viewer = Protoviewer(self)
        right.Add(self.viewer, 0, wx.EXPAND | wx.ALL)
        
        self.SetSizer(all)
        self.SetAutoLayout(True)        
        
    
    def OnItemSelected(self, event):
        self.item = event.GetItem()
        if self.item:
            print("OnSelChanged: %s" % self.ctree.GetItemText(self.item))
            self.viewer.fromModel(self.ctree.GetItemPyData(self.item))      
        pass
    
    def fromModel(self, model):
        # we assume a protodb model
        if self.model <> None:
            self.model.detachObserver(self)
        
        self.model = model
        
        for x in self.model.protos.values():
            # add type to tree control
            leaf = self.ctree.AppendItem(self.root, x.type, data=x)
            if self.showTypeSelect == True:
                ct_sub = 2
            else:
                ct_sub = 0
                 
            if x.subtypes <> None and len(x.subtypes):
                for sub in x.subtypes[:]:
                    self.ctree.AppendItem(leaf, sub, ct_type=ct_sub, data=x) 
        
        self.ctree.ExpandAll()
        self.Refresh()
        pass
        
        