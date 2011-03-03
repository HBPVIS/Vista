set( RelativeDir "./Stuff" )
set( LocalSourceGroup "Source Files\\Stuff" )

set( DirFiles
	VistaAC3DLoader.cpp
	VistaAC3DLoader.h
	VistaInteractionHandlerBase.cpp
	VistaInteractionHandlerBase.h
	VistaKernelProfiling.h
	VistaStreamManagerExt.cpp
	VistaStreamManagerExt.h
	VistaVirtualConsole.cpp
	VistaVirtualConsole.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

