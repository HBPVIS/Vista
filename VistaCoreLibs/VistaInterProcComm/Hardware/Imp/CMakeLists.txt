set(relDir "Hardware/Imp")
set(dirFiles_in
    CMakeLists.txt
    VistaPosixSerialPortImp.cpp
    VistaSerialPortImp.cpp
    VistaWin32SerialPortImp.cpp
    VistaPosixSerialPortImp.h
    VistaSerialPortImp.h
    VistaWin32SerialPortImp.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Hardware_Imp "${relDir}/${file}")
endforeach(file)

set(dirFiles_Hardware_Imp ${dirFiles_Hardware_Imp} PARENT_SCOPE)
