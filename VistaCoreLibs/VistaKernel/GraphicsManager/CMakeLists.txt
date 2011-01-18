set(relDir "GraphicsManager")
set(dirFiles_in
    CMakeLists.txt
    VistaOpenGLDebug.cpp
    VistaOpenGLDraw.cpp
    Vista3DText.cpp
    VistaAxes.cpp
    VistaExtensionNode.cpp
    VistaGeometry.cpp
    VistaGeometryFactory.cpp
    VistaGeomNode.cpp
    VistaGraphicsBridge.cpp
    VistaGraphicsManager.cpp
    VistaGroupNode.cpp
    VistaLeafNode.cpp
    VistaLightNode.cpp
    VistaLODNode.cpp
    VistaNodeBridge.cpp
    VistaNode.cpp
    VistaOpenGLNode.cpp
    VistaOpenGLPolyLine.cpp
    VistaSG.cpp
    VistaSwitchNode.cpp
    VistaTextNode.cpp
    VistaTransformNode.cpp
    VistaOpenGLDebug.h
    VistaOpenGLDraw.h
    Vista3DText.h
    VistaAxes.h
    VistaExtensionNode.h
    VistaGeometryFactory.h
    VistaGeometry.h
    VistaGeomNode.h
    VistaGraphicsBridge.h
    VistaGraphicsManager.h
    VistaGroupNode.h
    VistaLeafNode.h
    VistaLightNode.h
    VistaLODNode.h
    VistaNodeBridge.h
    VistaNode.h
    VistaNodeInterface.h
    VistaOpenGLNode.h
    VistaOpenGLPolyLine.h
    VistaSG.h
    VistaSwitchNode.h
    VistaTextNode.h
    VistaTransformNode.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_GraphicsManager "${relDir}/${file}")
endforeach(file)

set(dirFiles_GraphicsManager ${dirFiles_GraphicsManager} PARENT_SCOPE)
