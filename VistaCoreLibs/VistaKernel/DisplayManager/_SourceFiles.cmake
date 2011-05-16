set( RelativeDir "./DisplayManager" )
set( RelativeSourceGroup "Source Files\\DisplayManager" )

set( DirFiles
	Vista2DDrawingObjects.cpp
	Vista2DDrawingObjects.h
	VistaDisplay.cpp
	VistaDisplay.h
	VistaDisplayBridge.cpp
	VistaDisplayBridge.h
	VistaDisplayEntity.cpp
	VistaDisplayEntity.h
	VistaDisplayManager.cpp
	VistaDisplayManager.h
	VistaDisplaySystem.cpp
	VistaDisplaySystem.h
	VistaProjection.cpp
	VistaProjection.h
	VistaSceneOverlay.cpp
	VistaSceneOverlay.h
	VistaViewport.cpp
	VistaViewport.h
	VistaVirtualPlatform.cpp
	VistaVirtualPlatform.h
	VistaWindow.cpp
	VistaWindow.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

