set( RelativeDir "." )
set( LocalSourceGroup "Source Files" )
set( SubDirs AsyncIO Concurrency Connections DataLaVista Hardware IPNet SharedMemory )

set( DirFiles
	VistaInterProcCommConfig.h
	VistaInterProcCommOut.cpp
	VistaInterProcCommOut.h
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

