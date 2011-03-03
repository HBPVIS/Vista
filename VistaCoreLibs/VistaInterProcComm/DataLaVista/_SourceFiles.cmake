set( RelativeDir "./DataLaVista" )
set( LocalSourceGroup "Source Files\\DataLaVista" )
set( SubDirs Base Common Construct )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

