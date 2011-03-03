set( RelativeDir "./GraphicsManager" )
set( LocalSourceGroup "Source Files\\GraphicsManager" )

set( DirFiles
	Vista3DText.cpp
	Vista3DText.h
	VistaAxes.cpp
	VistaAxes.h
	VistaExtensionNode.cpp
	VistaExtensionNode.h
	VistaGeomNode.cpp
	VistaGeomNode.h
	VistaGeometry.cpp
	VistaGeometry.h
	VistaGeometryFactory.cpp
	VistaGeometryFactory.h
	VistaGraphicsBridge.cpp
	VistaGraphicsBridge.h
	VistaGraphicsManager.cpp
	VistaGraphicsManager.h
	VistaGroupNode.cpp
	VistaGroupNode.h
	VistaLODNode.cpp
	VistaLODNode.h
	VistaLeafNode.cpp
	VistaLeafNode.h
	VistaLightNode.cpp
	VistaLightNode.h
	VistaNode.cpp
	VistaNode.h
	VistaNodeBridge.cpp
	VistaNodeBridge.h
	VistaNodeInterface.h
	VistaOpenGLDebug.cpp
	VistaOpenGLDebug.h
	VistaOpenGLDraw.cpp
	VistaOpenGLDraw.h
	VistaOpenGLNode.cpp
	VistaOpenGLNode.h
	VistaOpenGLPolyLine.cpp
	VistaOpenGLPolyLine.h
	VistaSG.cpp
	VistaSG.h
	VistaSwitchNode.cpp
	VistaSwitchNode.h
	VistaTextNode.cpp
	VistaTextNode.h
	VistaTransformNode.cpp
	VistaTransformNode.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

