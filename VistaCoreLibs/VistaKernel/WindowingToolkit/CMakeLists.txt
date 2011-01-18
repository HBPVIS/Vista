set(relDir "WindowingToolkit")
set(dirFiles_in
    CMakeLists.txt
    Vista3DTextOverlay.cpp
    VistaSimpleTextOverlay.cpp
    VistaTextEntity.cpp
    VistaWindowingToolkit.cpp
    Vista3DTextOverlay.h
    VistaSimpleTextOverlay.h
    VistaTextEntity.h
    VistaWindowingToolkit.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_WindowingToolkit "${relDir}/${file}")
endforeach(file)

add_subdirectory(glut)
set(dirFiles_WindowingToolkit      ${dirFiles_WindowingToolkit}      PARENT_SCOPE)
set(dirFiles_WindowingToolkit_glut ${dirFiles_WindowingToolkit_glut} PARENT_SCOPE)
