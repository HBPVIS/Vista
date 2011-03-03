set( RelativeDir "./Half" )
set( LocalSourceGroup "Source Files\\Half" )

set( DirFiles
	VistaHalf.cpp
	VistaHalf.h
	VistaHalfFunction.h
	VistaHalfLimits.h
	eLut.h
	toFloat.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

