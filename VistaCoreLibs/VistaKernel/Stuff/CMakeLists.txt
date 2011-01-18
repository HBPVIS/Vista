set(relDir "Stuff")
set(dirFiles_in
    CMakeLists.txt
    VistaAC3DLoader.cpp
    VistaInteractionHandlerBase.cpp
    VistaStreamManagerExt.cpp
    VistaVirtualConsole.cpp
    VistaAC3DLoader.h
    VistaInteractionHandlerBase.h
    VistaStreamManagerExt.h
    VistaVirtualConsole.h
	VistaKernelProfiling.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Stuff "${relDir}/${file}")
endforeach(file)

set(dirFiles_Stuff ${dirFiles_Stuff} PARENT_SCOPE)
