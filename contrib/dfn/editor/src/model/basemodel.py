'''
Created on Aug 20, 2009

@author: assenmac
'''

class BaseModel():
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        self.__observer = []
        self.__ondelete = False
        self.__onunregister = []
        
    def attachObserver(self, observer):
        self.__observer.append(observer)
        
    def detachObserver(self, observer):
        if self.__ondelete == False:
            self.__observer.remove(observer)
        else:
            self.__onunregister.append(observer)
        
    def notify(self, tag):
        for x in self.__observer[:]:
            x.updateFromSubject(self, tag)
            
    def onDeleteSubject(self):
        l = []
        l[:0] = self.observer # copy entries
        self.__ondelete = True
        for x in l:
            x.onDeleteSubject(self)
            
        self.__ondelete = False
        for x in self.__onunregister[:]:
            self.__observer.remove(x)
        self.__onunregister = [] # clear intermediate list
        
        
    
        