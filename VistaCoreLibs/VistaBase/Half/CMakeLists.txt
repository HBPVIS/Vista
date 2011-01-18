set(relDir "Half")
set(dirFiles_in
    CMakeLists.txt
    VistaHalf.cpp
    eLut.h
    toFloat.h
    VistaHalfFunction.h
    VistaHalf.h
    VistaHalfLimits.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Half "${relDir}/${file}")
endforeach(file)

set(dirFiles_Half ${dirFiles_Half} PARENT_SCOPE)
