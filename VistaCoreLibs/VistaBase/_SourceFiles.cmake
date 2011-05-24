# $Id$

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs Half )

set( DirFiles
	VistaBaseConfig.h
	VistaBaseMain.cpp
	VistaBaseOut.cpp
	VistaBaseOut.h
	VistaBaseTypes.h
	VistaDefaultTimerImp.cpp
	VistaDefaultTimerImp.h
	VistaExceptionBase.cpp
	VistaExceptionBase.h
	VistaMathBasics.h
	VistaQuaternion.cpp
	VistaQuaternion.h
	VistaReferenceFrame.cpp
	VistaReferenceFrame.h
	VistaStreamManager.cpp
	VistaStreamManager.h
	VistaStreamUtils.cpp
	VistaStreamUtils.h
	VistaStreams.cpp
	VistaStreams.h
	VistaTimeUtils.cpp
	VistaTimeUtils.h
	VistaTimer.cpp
	VistaTimer.h
	VistaTimerImp.cpp
	VistaTimerImp.h
	VistaTransformMatrix.cpp
	VistaTransformMatrix.h
	VistaVector3D.h
	VistaVectorMath.h
	VistaVersion.cpp
	VistaVersion.h
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

