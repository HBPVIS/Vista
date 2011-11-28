# $Id$

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	VistaKernelOpenSGExtConfig.h
	VistaOpenSGGeometryTools.cpp
	VistaOpenSGGeometryTools.h
	VistaOpenSGNormalMapMaterial.cpp
	VistaOpenSGNormalMapMaterial.h
	VistaOpenSGNormalMapMaterialShaders.cpp
	VistaOpenSGParticleManager.cpp
	VistaOpenSGParticleManager.h
	VistaOpenSGParticleManagerParticleChanger.cpp
	VistaOpenSGParticleManagerParticleChanger.h
	VistaOpenSGParticles.cpp
	VistaOpenSGParticles.h
	VistaOpenSGPerMaterialShader.cpp
	VistaOpenSGPerMaterialShader.h
	VistaOpenSGPhongShader.cpp
	VistaOpenSGPhongShader.h
	VistaOpenSGShadow.cpp
	VistaOpenSGShadow.h
	VistaOpenSGSkybox.cpp
	VistaOpenSGSkybox.h
	VistaOpenSGToonShader.cpp
	VistaOpenSGToonShader.h
	VistaOpenSGWebInterface.cpp
	VistaOpenSGWebInterface.h
	VistaOpenSGModelOptimizer.cpp
	VistaOpenSGModelOptimizer.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

