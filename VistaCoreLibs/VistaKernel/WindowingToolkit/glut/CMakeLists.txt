set(relDir "WindowingToolkit/glut")
set(dirFiles_in
    VistaGlut3DTextOverlay.cpp
    VistaGlutSimpleTextOverlay.cpp
    VistaGlutTextEntity.cpp
    VistaGlutWindowingToolkit.cpp
    VistaGlut3DTextOverlay.h
    VistaGlutSimpleTextOverlay.h
    VistaGlutTextEntity.h
    VistaGlutWindowingToolkit.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_WindowingToolkit_glut "${relDir}/${file}")
endforeach(file)

set(dirFiles_WindowingToolkit_glut ${dirFiles_WindowingToolkit_glut} PARENT_SCOPE)
