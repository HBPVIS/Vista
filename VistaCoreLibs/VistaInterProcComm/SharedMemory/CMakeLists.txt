set(relDir "SharedMemory")
set(dirFiles_in
    CMakeLists.txt
    VistaSharedMem.cpp
    VistaSharedMem.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_SharedMemory "${relDir}/${file}")
endforeach(file)

set(dirFiles_SharedMemory ${dirFiles_SharedMemory} PARENT_SCOPE)
