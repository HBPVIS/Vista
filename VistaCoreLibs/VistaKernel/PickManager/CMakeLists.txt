set(relDir "PickManager")
set(dirFiles_in
    CMakeLists.txt
    VistaBeam.cpp
    VistaPickManager.cpp
    VistaBeam.h
    VistaPickable.h
    VistaPickManager.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_PickManager "${relDir}/${file}")
endforeach(file)

set(dirFiles_PickManager ${dirFiles_PickManager} PARENT_SCOPE)
