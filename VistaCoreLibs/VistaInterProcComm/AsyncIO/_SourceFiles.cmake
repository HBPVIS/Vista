set( RelativeDir "./AsyncIO" )
set( LocalSourceGroup "Source Files\\AsyncIO" )

set( DirFiles
	VistaAsyncEventObserver.cpp
	VistaAsyncEventObserver.h
	VistaIOHandleBasedMultiplexer.cpp
	VistaIOHandleBasedMultiplexer.h
	VistaIOMultiplexer.cpp
	VistaIOMultiplexer.h
	VistaIOMultiplexerIP.cpp
	VistaIOMultiplexerIP.h
	VistaIORequest.cpp
	VistaIORequest.h
	VistaIOScheduler.cpp
	VistaIOScheduler.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

