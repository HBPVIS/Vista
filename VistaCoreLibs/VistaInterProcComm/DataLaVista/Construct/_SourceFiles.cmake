set( RelativeDir "./DataLaVista/Construct" )
set( LocalSourceGroup "Source Files\\DataLaVista\\Construct" )

set( DirFiles
	VistaCheckError.cpp
	VistaCheckError.h
	VistaNullError.cpp
	VistaNullError.h
	VistaPipeChecker.cpp
	VistaPipeChecker.h
	VistaPipeConstructionKit.cpp
	VistaPipeConstructionKit.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

