set( RelativeDir "./EventManager" )
set( RelativeSourceGroup "Source Files\\EventManager" )

set( DirFiles
	VistaCentralEventHandler.cpp
	VistaCentralEventHandler.h
	VistaCommandEvent.cpp
	VistaCommandEvent.h
	VistaDisplayEvent.cpp
	VistaDisplayEvent.h
	VistaEvent.cpp
	VistaEvent.h
	VistaEventHandler.cpp
	VistaEventHandler.h
	VistaEventManager.cpp
	VistaEventManager.h
	VistaEventObserver.h
	VistaExternalMsgEvent.cpp
	VistaExternalMsgEvent.h
	VistaGraphicsEvent.cpp
	VistaGraphicsEvent.h
	VistaPickEvent.cpp
	VistaPickEvent.h
	VistaSystemEvent.cpp
	VistaSystemEvent.h
	VistaTickTimer.cpp
	VistaTickTimer.h
	VistaTickTimerEvent.cpp
	VistaTickTimerEvent.h
	VistaTimeoutHandler.cpp
	VistaTimeoutHandler.h
	VistaTimeoutRouter.cpp
	VistaTimeoutRouter.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

