set(relDir "EventManager")
set(dirFiles_in
    CMakeLists.txt
    VistaCentralEventHandler.cpp
    VistaCommandEvent.cpp
    VistaDisplayEvent.cpp
    VistaEvent.cpp
    VistaEventHandler.cpp
    VistaEventManager.cpp
    VistaExternalMsgEvent.cpp
    VistaGraphicsEvent.cpp
    VistaPickEvent.cpp
    VistaSystemEvent.cpp
    VistaTickTimer.cpp
    VistaTickTimerEvent.cpp
    VistaTimeoutHandler.cpp
    VistaTimeoutRouter.cpp
    VistaCentralEventHandler.h
    VistaCommandEvent.h
    VistaDisplayEvent.h
    VistaEvent.h
    VistaEventHandler.h
    VistaEventManager.h
    VistaEventObserver.h
    VistaExternalMsgEvent.h
    VistaGraphicsEvent.h
    VistaPickEvent.h
    VistaSystemEvent.h
    VistaTickTimerEvent.h
    VistaTickTimer.h
    VistaTimeoutHandler.h
    VistaTimeoutRouter.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_EventManager "${relDir}/${file}")
endforeach(file)

set(dirFiles_EventManager ${dirFiles_EventManager} PARENT_SCOPE)
