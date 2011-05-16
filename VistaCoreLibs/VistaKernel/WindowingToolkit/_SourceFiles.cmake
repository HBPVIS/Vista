set( RelativeDir "./WindowingToolkit" )
set( RelativeSourceGroup "Source Files\\WindowingToolkit" )
set( SubDirs glut )

set( DirFiles
	Vista3DTextOverlay.cpp
	Vista3DTextOverlay.h
	VistaSimpleTextOverlay.cpp
	VistaSimpleTextOverlay.h
	VistaTextEntity.cpp
	VistaTextEntity.h
	VistaWindowingToolkit.cpp
	VistaWindowingToolkit.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

