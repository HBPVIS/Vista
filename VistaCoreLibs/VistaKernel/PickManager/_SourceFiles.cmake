set( RelativeDir "./PickManager" )
set( LocalSourceGroup "Source Files\\PickManager" )

set( DirFiles
	VistaBeam.cpp
	VistaBeam.h
	VistaPickManager.cpp
	VistaPickManager.h
	VistaPickable.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

