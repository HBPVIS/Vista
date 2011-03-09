set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	VistaBoundingBox.cpp
	VistaBoundingBox.h
	VistaBoundingSphere.cpp
	VistaBoundingSphere.h
	VistaFilter.h
	VistaGeometries.cpp
	VistaGeometries.h
	VistaIndirectXform.cpp
	VistaIndirectXform.h
	VistaLeastSquaresPlane.cpp
	VistaLeastSquaresPlane.h
	VistaMathConfig.h
	VistaMathOut.cpp
	VistaMathOut.h
	VistaMathTools.h
	VistaMatrix.h
	VistaMatrixDeComposer.cpp
	VistaMatrixDeComposer.h
	VistaPolynomial.cpp
	VistaPolynomial.h
	VistaSquareMatrix.h
	VistaVector.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )
