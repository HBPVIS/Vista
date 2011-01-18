'''
Created on Aug 20, 2009

@author: assenmac
'''

class Observer():
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
    
    def updateFromSubject(self, subject, tag):
        print "update with "+ str(self) + " on subject [" + str(subject) + "]" 
        dir(tag)
        
    def onDeleteSubject(self, subject):
        pass
        