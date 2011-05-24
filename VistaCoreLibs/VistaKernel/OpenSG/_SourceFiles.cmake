# $Id$

set( RelativeDir "./OpenSG" )
set( RelativeSourceGroup "Source Files\\OpenSG" )

set( DirFiles
	OSGAC3DFileType.cpp
	OSGAC3DFileType.h
	OSGVistaOpenGLDrawCore.cpp
	OSGVistaOpenGLDrawCore.h
	OSGVistaOpenGLDrawCoreBase.cpp
	OSGVistaOpenGLDrawCoreBase.h
	OSGVistaOpenGLDrawCoreFields.h
	VistaOpenSGDisplayBridge.cpp
	VistaOpenSGDisplayBridge.h
	VistaOpenSGGLOverlayForeground.cpp
	VistaOpenSGGLOverlayForeground.h
	VistaOpenSGGLOverlayForegroundBase.cpp
	VistaOpenSGGLOverlayForegroundBase.h
	VistaOpenSGGLOverlayForegroundFields.h
	VistaOpenSGGraphicsBridge.cpp
	VistaOpenSGGraphicsBridge.h
	VistaOpenSGNodeBridge.cpp
	VistaOpenSGNodeBridge.h
	VistaOpenSGSystemClassFactory.cpp
	VistaOpenSGSystemClassFactory.h
	VistaOpenSGTextForeground.cpp
	VistaOpenSGTextForeground.h
	VistaOpenSGTextForegroundBase.cpp
	VistaOpenSGTextForegroundBase.h
	VistaOpenSGTextForegroundFields.h
	VistaOpenSGThreadImp.cpp
	VistaOpenSGThreadImp.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

