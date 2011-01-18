'''
Created on Aug 13, 2009

@author: assenmac
'''

class Parameter(dict):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        dict.__init__(self)
        self.comment = None
        
    def Print(self, ident=""):
        for key in self.keys():
            el = self[key]
            if isinstance( el, Parameter ):
                el.Print(ident + "\t")
            else:
                print "[" + key + "] = [" + str(self[key]) + "]"
        