set( RelativeDir "./Stuff" )
set( RelativeSourceGroup "Source Files\\Stuff" )

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
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

