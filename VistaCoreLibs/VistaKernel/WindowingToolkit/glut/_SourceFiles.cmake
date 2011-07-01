# $Id$

set( RelativeDir "./WindowingToolkit/glut" )
set( RelativeSourceGroup "Source Files\\WindowingToolkit\\glut" )

set( DirFiles
	VistaGlut3DTextOverlay.cpp
	VistaGlut3DTextOverlay.h
	VistaGlutSimpleTextOverlay.cpp
	VistaGlutSimpleTextOverlay.h
	VistaGlutTextEntity.cpp
	VistaGlutTextEntity.h
	VistaGlutWindowingToolkit.cpp
	VistaGlutWindowingToolkit.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

