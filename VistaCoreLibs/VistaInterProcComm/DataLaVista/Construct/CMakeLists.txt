set(relDir "DataLaVista/Construct")
set(dirFiles_in
    CMakeLists.txt
    VistaCheckError.cpp
    VistaNullError.cpp
    VistaPipeChecker.cpp
    VistaPipeConstructionKit.cpp
    VistaCheckError.h
    VistaNullError.h
    VistaPipeChecker.h
    VistaPipeConstructionKit.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_DataLaVista_Construct "${relDir}/${file}")
endforeach(file)

set(dirFiles_DataLaVista_Construct ${dirFiles_DataLaVista_Construct} PARENT_SCOPE)
