'''
Created on Aug 12, 2009

@author: assenmac
'''
import wx
import wx.lib.ogl as ogl
import events.edgehandler

class EdgeShape(ogl.LineShape):
    '''
    classdocs
    '''

    def __init__(self, canvas, model):
        '''
        Constructor
        '''
        ogl.LineShape.__init__(self)
        self.SetCanvas(canvas)
        self.SetPen(wx.BLACK_PEN)
        self.SetBrush(wx.BLACK_BRUSH)
        self.AddArrow(ogl.ARROW_ARROW)
        self.MakeLineControlPoints(2)
        self.model = model
        self.model._shape = self
        
        # event handling stuff
        evh = events.edgehandler.EdgeEventHandler(canvas)
        evh.SetPreviousHandler(self.GetEventHandler())
        self.SetEventHandler(evh)
        evh.SetShape(self)