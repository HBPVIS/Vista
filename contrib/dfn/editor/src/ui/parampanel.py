'''
Created on 16.08.2009

@author: ingo
'''

import wx
import model.parameter

class ParamPanel(wx.TreeCtrl):
    '''
    classdocs
    '''


    def __init__(self, parent, ID=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.TR_LINES_AT_ROOT|wx.TR_SINGLE|wx.TR_DEFAULT_STYLE| wx.TR_HIDE_ROOT,
                 params = None):
        '''
        Constructor
        '''
        wx.TreeCtrl.__init__(self, parent, ID, pos, size, style )
        self.params = params        
        self.fromParams(self.params)
    
    def fromParams(self, params, showValues = True):
        self.DeleteAllItems()        
        if params <> None:
            self.params = params
            self.CreateParams(None, self.params, showValues)

    
    def CreateParams(self, root, params, showValues):
        for key in params.keys():
            p = params[key]
            if root == None:
                root = self.AddRoot("Parameter")
                
            nd = self.AppendItem(root, str(key))
            if isinstance(p, model.parameter.Parameter):
                # create new root
                self.CreateParams(nd, p, showValues)
            else:
                if showValues == True:
                    val = self.AppendItem(nd, str(params[key][0]))                 
        pass

         
        