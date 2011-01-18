#!/usr/bin/python
# -*- coding: utf-8 -*-

import wx
import wx.lib.ogl as ogl

import ui.mainframe as ui

import model.protodb
import persistence.protoloader


class GraphCanvas(ogl.ShapeCanvas):
    def __init__(self, parent):
        ogl.ShapeCanvas.__init__(self, parent)
        
        self.SetBackgroundColour(wx.LIGHT_GREY)
        self.diagram = ogl.Diagram()
        self.SetDiagram(self.diagram)
        self.diagram.SetCanvas(self)
        self.shapes   = []
        self.save_gdi = []


class MyApp(wx.App):
    def OnInit(self):
        ogl.OGLInitialize()
        model.protodb.ProtoDB.db = persistence.protoloader.LoadPrototypeDirectory("../protos")
        
        frame = ui.MainFrame()
        frame.Show(True)
        self.SetTopWindow(frame)
        
        return True

app = MyApp(0)
app.MainLoop()
