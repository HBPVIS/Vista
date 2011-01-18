'''
Created on Aug 10, 2009

@author: assenmac
'''

import wx
import wx.lib.ogl as ogl

import model
import events.nodehandler, events.porthandler

class NodeShape(ogl.CompositeShape, model.observer.Observer):
    '''
    classdocs
    '''
    def __init__(self, canvas, nodemodel):
        '''
        Constructor
        '''
        ogl.CompositeShape.__init__(self)
        model.observer.Observer.__init__(self)
        
        self.SetCanvas(canvas)
        
        self.inp_dict = dict()
        self.oup_dict = dict()        
        self.model = nodemodel
        
        self.SetSensitivityFilter(0)
        self.SetDraggable(True)
        
        self.fromNodeModel()
                        

    def Delete(self):
        ogl.CompositeShape.Delete(self)
        if self.model <> None:
            self.model.detachObserver(self)
    
    def updateFromSubject(self, subject, tag):
        if tag.has_key("name"):
            region = self.constr_shape._regions[0]
            text = region.GetFormattedText()
            if text[0]._line <> subject.name:
                text[0].SetText(subject.name)
                self.constr_shape.Recentre(wx.ClientDC(self.GetCanvas()))
                self.UpdateLayout()
                self.GetCanvas().Refresh()
        
        if tag.has_key("delete"):
            self.model.detachObserver(self)               
        
                    
    def doPortUI(self, ptlist, brush, ui_container, shp_dict):
        out_list = []
        X   = 0 # count the total width (approximately), temporary variable
        m_H = 0 # count the height (approximately), temporary variable
        
        dc = wx.ClientDC(self.GetCanvas()) # needed for font sizing below
        
        for port in ptlist[:]:
            pt_name, pt_type = port.name, "["+str(port.type)+"]"
            width, height = max(dc.GetTextExtent(pt_name), dc.GetTextExtent(str(pt_type)))
            pt_width, pt_height = 1.25*width + 4, height*2.5     # note width and height of this box
            shp = ogl.RectangleShape( pt_width, pt_height ) # set
            
            port._shape = shp # backlink
            
            shp.SetDraggable(False)
            shp.SetCanvas(self.GetCanvas())
            if port.valid == model.port.Port.STATE_VALID:            
                # properties
                shp.SetBrush(brush)
            elif port.valid == model.port.Port.STATE_PROBABLY_OK:
                shp.SetBrush(wx.Brush("YELLOW"))
            else:
                shp.SetBrush(wx.Brush( wx.Color(100,100,255)))

            shp.AddText(pt_name)
            shp.AddText(pt_type)
                
            
            # add to inport list (temporarily)
            ui_container.AddChild(shp)
            
            # 
            X = X + pt_width
            pt_height = max(pt_height, m_H)
            
            # add to list of inports
            out_list.append(shp)
            shp_dict[port.name] = shp
            
            # event handling on ports
            evh = events.porthandler.PortEventHandler(port, self.GetCanvas())
            evh.SetPreviousHandler(shp.GetEventHandler())
            evh.SetShape(shp)
            shp.SetEventHandler(evh)
            
            
        return out_list, X, m_H
            
            
    def getPortShape(self, name, isInport):
        if isInport:
            if self.inp_dict.has_key(name):
                return self.inp_dict[name]
            else:
                return None
        else:
            if self.oup_dict.has_key(name):
                return self.oup_dict[name]
            else:
                return None
            

        
    def fromNodeModel(self):
        ''' 
        assign properties from the nodemodel provided
        '''
        self.model.attachObserver(self)
               
        self.model._shape = self # backlink for faster search  
             
                     
        constr_shape = self.constr_shape = ogl.RectangleShape(0,0)
        if self.model.valid == True:
            if self.model.GetHasSpuriousPorts() == True:
                constr_shape.SetBrush(wx.Brush("#ffe65a"))
            else:
                constr_shape.SetBrush(wx.Brush("#FFE4B5"))
        else:
            constr_shape.SetBrush(wx.RED_BRUSH)
            
        if self.model.name:
            for line in self.model.name.split('\n'):
                self.constr_shape.AddText(line)
        if self.model.type:
            for line in self.model.type.split('\n'):
                self.constr_shape.AddText(line)            
            
        constr_shape.SetDraggable(False)
        constr_shape.SetCanvas(self.GetCanvas())
        constr_shape.SetPen(wx.BLACK_PEN)
        constr_shape.SetShadowMode(ogl.SHADOW_RIGHT)
        
        evh = events.nodehandler.NodeEventHandler(self.GetCanvas(), self.model)
        evh.SetPreviousHandler(constr_shape.GetEventHandler())
        constr_shape.SetEventHandler(evh)
        evh.SetShape(constr_shape)        
        
        constr_shape.SetX(0)
        constr_shape.SetY(0)
        self.AddChild(constr_shape)    

        
        # top_shape holds inport rows
        self.top_shape = top_shape = ogl.CompositeShape()
        top_shape.SetDraggable(False) # attached to inner box
        top_shape.SetCanvas(self.GetCanvas())
        self.AddChild(top_shape) 

        # bottom shape for outports
        self.bt_shape = bt_shape = ogl.CompositeShape()
        bt_shape.SetDraggable(False)
        bt_shape.SetCanvas(self.GetCanvas())
        self.AddChild(bt_shape)

#        inp, m_Xt, m_Ht  = self.doPortUI(self.model.GetInPorts(),  wx.GREEN_BRUSH, top_shape, self.inp_dict)
#        outp, m_Xb, m_Hb = self.doPortUI(self.model.GetOutPorts(), wx.GREEN_BRUSH, bt_shape,   self.oup_dict)

        # tuple: ports, m_Xb, m_Hb
        self.inp = (self.doPortUI(self.model.GetInPorts(),  wx.GREEN_BRUSH, top_shape, self.inp_dict))
        self.outp= (self.doPortUI(self.model.GetOutPorts(), wx.GREEN_BRUSH, bt_shape,  self.oup_dict))
        
        self.UpdateLayout()
        
        
    def UpdateLayout(self):
        # adjust width and height, according to largest box (top or bottom)
        dc = wx.ClientDC(self.GetCanvas()) # needed for font sizing below
        width,height    = dc.GetTextExtent(self.model.name)
        twidth, theight = dc.GetTextExtent(self.model.type)
        width = max(width, twidth)
        height= height + theight
                        
        self.constr_shape.SetWidth( max(self.inp[1], self.outp[1], width)+10)
        self.constr_shape.SetHeight(max(self.inp[2], self.outp[2], height)*2.5)
        self.SetWidth(self.constr_shape.GetWidth()) # assign w/h from child (or clipping will occur?)
        self.SetHeight(self.constr_shape.GetHeight())
        
               
        if len(self.GetConstraints()) == 0:
            # center inports along top shape
            if len(self.inp[0]) > 0:
                self.AddConstraint( ogl.Constraint( ogl.CONSTRAINT_CENTRED_HORIZONTALLY, self.top_shape, self.inp[0] ) )
                # arrange top shape centered on top of the inner box
                self.AddConstraint( ogl.Constraint( ogl.CONSTRAINT_MIDALIGNED_TOP,    self.constr_shape, [ self.top_shape ] ) )                
            
            # center outports along bottom shape
            if len(self.outp[0]) > 0:        
                self.AddConstraint( ogl.Constraint( ogl.CONSTRAINT_CENTRED_HORIZONTALLY, self.bt_shape, self.outp[0] ) )
                # arrange bottom shape centered on bottom to the inner box
                self.AddConstraint( ogl.Constraint( ogl.CONSTRAINT_MIDALIGNED_BOTTOM, self.constr_shape, [ self.bt_shape ] ) )
            
            # center the inner box in the mid of top/bottom 
            #self.AddConstraint( ogl.Constraint( ogl.CONSTRAINT_CENTRED_VERTICALLY, self.constr_shape, [ self ] ) )  

        # update layout
        if self.Recompute() == False:
            print "layout bad.."        
        
        
                
