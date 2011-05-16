set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs AsyncIO Concurrency Connections DataLaVista Hardware IPNet SharedMemory )

set( DirFiles
	VistaInterProcCommConfig.h
	VistaInterProcCommOut.cpp
	VistaInterProcCommOut.h
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

