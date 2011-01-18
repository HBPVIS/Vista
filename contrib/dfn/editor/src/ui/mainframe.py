'''
Created on Aug 10, 2009

@author: assenmac
'''

import wx
import os
import graphcanvas

import persistence.graphloader as graphloader
import persistence.graphwriter as graphwriter

import model.graph

import ConfigParser
import protobrowser


class MainFrame(wx.Frame):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        self.currentGraph = None
        
        wx.Frame.__init__(self, None, -1, "DFN XML Edit",
                             wx.DefaultPosition, wx.Size(640, 480))
        self.CreateStatusBar()
        
        self.protobrowser = None
        self.currentGraphPath = None
    
        ID_EXIT     = wx.NewId()
        ID_DFN_LOAD = wx.NewId()
        ID_DFN_RELOAD = wx.NewId()
        
        ID_DFN_SAVE = wx.NewId()
        ID_DFN_NEW  = wx.NewId()
        
        ID_VIEW_LAYOUT = wx.NewId()
        ID_VIEW_ALL    = wx.NewId()
        
        ID_VIEW_NEWWINDOW = wx.NewId()
        ID_VIEW_PROTOBROWSER = wx.NewId()

        menuBar = wx.MenuBar()       
        
        menu = wx.Menu()
        menu.Append(ID_DFN_NEW, "&New DFN graph", "Clear old graph")
        menu.AppendSeparator()
        
        menu.Append(ID_DFN_LOAD, "&Load DFN xml", "Load DFN XML File")
        menu.Append(ID_DFN_RELOAD, "&Reload DFN xml", "Reload DFN XML File")
        menu.Append(ID_DFN_SAVE, "&Save DFN xml", "Save DFN XML File")
        menu.AppendSeparator()
        
        menu.Append(ID_EXIT, "&Close Window", "Closes this window")                
        self.filehistory = wx.FileHistory()
        self.filehistory.UseMenu(menu)
        self.Bind(
            wx.EVT_MENU_RANGE, self.OnFileHistory, id=wx.ID_FILE1, id2=wx.ID_FILE9
            )



        menuBar.Append(menu, "&File");
        
        menu = wx.Menu()
        menu.Append(ID_VIEW_LAYOUT, "La&yout", "Re-layout the graph")
        menu.Append(ID_VIEW_ALL, "View all", "Show all shapes")
        menu.AppendSeparator()
        menu.Append(ID_VIEW_NEWWINDOW, "New Window", "Open a new graph window")
        menu.Append(ID_VIEW_PROTOBROWSER, "Prototype browser", "Open the protobrowser")
        
        menuBar.Append(menu, "&View")
        
    
        self.SetMenuBar(menuBar)
        wx.EVT_MENU(self, ID_EXIT,  self.quit)
        wx.EVT_MENU(self, ID_DFN_LOAD, self.LoadGraph)
        wx.EVT_MENU(self, ID_DFN_RELOAD, self.ReloadGraph)
        wx.EVT_MENU(self, ID_DFN_SAVE, self.SaveGraph)
        wx.EVT_MENU(self, ID_DFN_NEW, self.NewGraph)
        wx.EVT_MENU(self, ID_VIEW_LAYOUT, self.LayoutGraph)
        wx.EVT_MENU(self, ID_VIEW_ALL, self.ShowAllShapes)
        wx.EVT_MENU(self, ID_VIEW_NEWWINDOW, self.OpenNewView)
        wx.EVT_MENU(self, ID_VIEW_PROTOBROWSER, self.OpenProtoBrowser)


         
        
        # the graph canvas to show mainly
        
        self.gcv = graphcanvas.GraphCanvas(self)       
        
        self.cfg = ConfigParser.RawConfigParser()
        self.cwd = os.getcwd()
        
        self.cfg.read("gfxedt.ini")
        
        if self.cfg.has_section("history") == False:
            self.cfg.add_section("history")

        if self.cfg.has_option("history", "num") == False:
            self.cfg.set("history", "num", '0')
            
        num = self.cfg.getint("history", "num")
        for i in range(1,num+1):
            try:
                entry = self.cfg.get("history", "entry"+str(i))
                if entry <> None:
                    self.filehistory.AddFileToHistory(entry)
            except ConfigParser.NoOptionError:
                pass # slightly ignore this
        
     
    def OpenProtoBrowser(self, evt):
        p = protobrowser.ProtoFrame()
        p.Show()
        pass
     
    def OpenNewView(self, evt):
        frame = MainFrame()
        frame.Show(True)
    
                   
    def AddToHistory(self, path):
        self.filehistory.AddFileToHistory(path)
        self.cfg.set("history", "entry"+str(self.filehistory.GetCount()), path)
        num = self.cfg.getint("history", "num")
        num = num + 1
        self.cfg.set("history", "num", str(num) )
        
        fp = open(self.cwd + "/" + "gfxedt.ini", mode="w+")
        self.cfg.write(fp)
        fp.close()
        
           
    def OnFileHistory(self, evt):
        if self.CheckGraph() == False:
            return None
        
        fileNum = evt.GetId() - wx.ID_FILE1
        path = self.filehistory.GetHistoryFile(fileNum)
        # add it back to the history so it will be moved up the list

        if os.path.exists(path):
            if self.doLoadGraph(path) <> None:
                self.filehistory.AddFileToHistory(path)
        
         
    def doSaveGraph(self):
        dlg = wx.FileDialog(
            self, message="Choose DFN file for saving",
            defaultDir=os.getcwd(), 
            defaultFile="",
            wildcard="*.xml",
            style=wx.OPEN | wx.CHANGE_DIR | wx.SAVE
            )
        
        # Show the dialog and retrieve the user response. If it is the OK response, 
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            path = dlg.GetPath()
            graphwriter.WriteGraph(self.currentGraph, path)
        
        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

        
    def quit(self, event):        
        if self.currentGraph <> None:
            dlg = wx.MessageDialog(self, 'A graph in progress.\nSave before exit?',
                   'Save Empty DFN?',
                   wx.YES_NO | wx.ICON_WARNING
                   #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION
                   )
            if dlg.ShowModal() == wx.ID_YES:
                self.doSaveGraph()
            dlg.Destroy()

        self.Close(True) 
        
        
    def LayoutGraph(self, event):
        self.gcv.doLayout()
        self.gcv.Refresh()

    def ShowAllShapes(self, evt):
        self.gcv.ShowAll()
        

    def SaveGraph(self, event):
        cont = True
        if self.currentGraph == None:
            dlg = wx.MessageDialog(self, 'No graph yet. Save empty file?!',
                               'Save Empty DFN?',
                               wx.YES_NO | wx.ICON_WARNING
                               #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION
                               )
            if dlg.ShowModal() == wx.ID_NO:
                cont = False
            dlg.Destroy()
        
        
        if cont == True:
            self.doSaveGraph()
        

    def CheckGraph(self):
        ret = False
        if self.currentGraph <> None:
            dlg = wx.MessageDialog(self, 'A graph is currently active.\nProgress and clean?!',
                               'New DFN Graph',
                               wx.YES_NO | wx.ICON_WARNING
                               #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION
                               )
            if dlg.ShowModal() == wx.ID_YES:
                ret = True
            dlg.Destroy()
        else:
            ret = True # empty graph is ok
        
        return ret
        
    
    def NewGraph(self, event):
        if self.CheckGraph() == True:
            self.currentGraph = model.graph.Graph()
            self.gcv.SetGraph(self.currentGraph)
        
    
    def ReloadGraph(self, event):
        if self.currentGraphPath == None:
            return
        self.doLoadGraph(self.currentGraphPath)        
        pass
    
    def LoadGraph(self, event):
        # dialog is set up to change the current working directory to the path chosen.
        if self.CheckGraph() == False:
            return None
        
        dlg = wx.FileDialog(
            self, message="Choose DFN file",
            defaultDir=os.getcwd(), 
            defaultFile="",
            wildcard="*.xml",
            style=wx.OPEN | wx.CHANGE_DIR
            )

        # Show the dialog and retrieve the user response. If it is the OK response, 
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            path = dlg.GetPath()
            if self.doLoadGraph(path) <> None:
                self.AddToHistory(path)
                

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()
        
    def doLoadGraph(self, path):
        graph = graphloader.LoadGraph(path)
        if graph <> None:
            self.currentGraph = graph
            self.gcv.SetGraph(self.currentGraph)
            self.currentGraphPath = path
            self.SetTitle("DFN XML Edit " + " [" + os.path.basename(path) + "]")
        else:
            self.currentGraphPath = None
            self.currentGraph = None
            
            
        return graph
    
    
