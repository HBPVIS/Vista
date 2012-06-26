# $Id$

set( RelativeDir "./DisplayManager" )
set( RelativeSourceGroup "Source Files\\DisplayManager" )
set( SubDirs )

if( VISTACORELIBS_BUILD_WINDOWIMP_GLUT )
	list( APPEND SubDirs GlutWindowImp )
endif()
if( VISTACORELIBS_BUILD_WINDOWIMP_OSG )
	list( APPEND SubDirs OpenSceneGraphWindowImp )
endif()

set( DirFiles
	Vista2DDrawingObjects.cpp
	Vista2DDrawingObjects.h
	Vista3DTextOverlay.cpp
	Vista3DTextOverlay.h
	VistaDisplay.cpp
	VistaDisplay.h
	VistaDisplayBridge.cpp
	VistaDisplayBridge.h
	VistaDisplayEntity.cpp
	VistaDisplayEntity.h
	VistaDisplayManager.cpp
	VistaDisplayManager.h
	VistaDisplaySystem.cpp
	VistaDisplaySystem.h
	VistaProjection.cpp
	VistaProjection.h
	VistaSceneOverlay.cpp
	VistaSceneOverlay.h
	VistaSimpleTextOverlay.cpp
	VistaSimpleTextOverlay.h
	VistaTextEntity.cpp
	VistaTextEntity.h
	VistaViewport.cpp
	VistaViewport.h
	VistaVirtualPlatform.cpp
	VistaVirtualPlatform.h
	VistaWindow.cpp
	VistaWindow.h
	VistaWindowingToolkit.cpp
	VistaWindowingToolkit.h
	_SourceFiles.cmake
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

