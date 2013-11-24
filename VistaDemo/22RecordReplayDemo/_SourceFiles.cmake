# $Id: _SourceFiles.cmake 39462 2013-11-24 20:10:20Z dr165799 $

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	main.cpp
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

