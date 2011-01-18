set(relDir "DisplayManager")
set(dirFiles_in
    CMakeLists.txt
    Vista2DDrawingObjects.cpp
    VistaDisplayBridge.cpp
    VistaDisplay.cpp
    VistaDisplayEntity.cpp
    VistaDisplayManager.cpp
    VistaDisplaySystem.cpp
    VistaProjection.cpp
    VistaSceneOverlay.cpp
    VistaViewport.cpp
    VistaVirtualPlatform.cpp
    VistaWindow.cpp
    Vista2DDrawingObjects.h
    VistaDisplayBridge.h
    VistaDisplayEntity.h
    VistaDisplay.h
    VistaDisplayManager.h
    VistaDisplaySystem.h
    VistaProjection.h
    VistaSceneOverlay.h
    VistaViewport.h
    VistaVirtualPlatform.h
    VistaWindow.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_DisplayManager "${relDir}/${file}")
endforeach(file)

set(dirFiles_DisplayManager ${dirFiles_DisplayManager} PARENT_SCOPE)
