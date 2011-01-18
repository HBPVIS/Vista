'''
Created on Aug 18, 2009

@author: assenmac
'''
import wx
import nodepanel

class NodeFrame(wx.Frame):
    '''
    classdocs
    '''


    def __init__(self, ID, title, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE,
                 model=None):
        '''
        Constructor
        '''
        wx.Frame.__init__(self, None, ID, title, pos, size, style)
        self.nodepanel = nodepanel.NodePanel(self, ID, model)
        
        