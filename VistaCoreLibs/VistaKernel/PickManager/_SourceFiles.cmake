set( RelativeDir "./PickManager" )
set( RelativeSourceGroup "Source Files\\PickManager" )

set( DirFiles
	VistaBeam.cpp
	VistaBeam.h
	VistaPickManager.cpp
	VistaPickManager.h
	VistaPickable.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

