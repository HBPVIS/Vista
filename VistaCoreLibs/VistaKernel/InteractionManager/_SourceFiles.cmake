set( RelativeDir "./InteractionManager" )
set( LocalSourceGroup "Source Files\\InteractionManager" )
set( SubDirs DfnNodes )

set( DirFiles
	VistaDriverWindowAspect.cpp
	VistaDriverWindowAspect.h
	VistaIntentionSelect.cpp
	VistaIntentionSelect.h
	VistaInteractionContext.cpp
	VistaInteractionContext.h
	VistaInteractionEvent.cpp
	VistaInteractionEvent.h
	VistaInteractionManager.cpp
	VistaInteractionManager.h
	VistaKeyboardSystemControl.cpp
	VistaKeyboardSystemControl.h
	VistaObjectSetMove.cpp
	VistaObjectSetMove.h
	VistaUserPlatform.cpp
	VistaUserPlatform.h
	VistaVirtualPlatformAdapter.cpp
	VistaVirtualPlatformAdapter.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

