set(relDir "OpenSG")
set(dirFiles_in
    CMakeLists.txt
    OSGAC3DFileType.cpp
    OSGVistaOpenGLDrawCoreBase.cpp
    OSGVistaOpenGLDrawCore.cpp
    VistaOpenSGDisplayBridge.cpp
    VistaOpenSGGLOverlayForegroundBase.cpp
    VistaOpenSGGLOverlayForeground.cpp
    VistaOpenSGGraphicsBridge.cpp
    VistaOpenSGNodeBridge.cpp
    VistaOpenSGSystemClassFactory.cpp
    VistaOpenSGTextForegroundBase.cpp
    VistaOpenSGTextForeground.cpp
    VistaOpenSGThreadImp.cpp
    OSGAC3DFileType.h
    OSGVistaOpenGLDrawCoreBase.h
    OSGVistaOpenGLDrawCoreFields.h
    OSGVistaOpenGLDrawCore.h
    VistaOpenSGDisplayBridge.h
    VistaOpenSGGLOverlayForegroundBase.h
    VistaOpenSGGLOverlayForegroundFields.h
    VistaOpenSGGLOverlayForeground.h
    VistaOpenSGGraphicsBridge.h
    VistaOpenSGNodeBridge.h
    VistaOpenSGSystemClassFactory.h
    VistaOpenSGTextForegroundBase.h
    VistaOpenSGTextForegroundFields.h
    VistaOpenSGTextForeground.h
    VistaOpenSGThreadImp.h
    OSGVistaOpenGLDrawCore.inl
    OSGVistaOpenGLDrawCoreBase.inl
    VistaOpenSGGLOverlayForeground.inl
    VistaOpenSGGLOverlayForegroundBase.inl
    VistaOpenSGTextForeground.inl
    VistaOpenSGTextForegroundBase.inl
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_OpenSG "${relDir}/${file}")
endforeach(file)

set(dirFiles_OpenSG ${dirFiles_OpenSG} PARENT_SCOPE)
