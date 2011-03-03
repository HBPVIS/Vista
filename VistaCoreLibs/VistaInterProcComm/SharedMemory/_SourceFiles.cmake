set( RelativeDir "./SharedMemory" )
set( LocalSourceGroup "Source Files\\SharedMemory" )

set( DirFiles
	VistaSharedMem.cpp
	VistaSharedMem.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

