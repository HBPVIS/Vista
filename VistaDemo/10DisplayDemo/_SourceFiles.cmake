# $Id: $

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	DisplayDemoAppl.cpp
	DisplayDemoAppl.h
	main.cpp
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

