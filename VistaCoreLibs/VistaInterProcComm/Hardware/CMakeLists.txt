set(relDir "Hardware")
set(dirFiles_in
    CMakeLists.txt
    VistaSerialPort.cpp
    VistaSerialPort.h
    VistaSerialPortStateDesc.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Hardware "${relDir}/${file}")
endforeach(file)

add_subdirectory(Imp)
set(dirFiles_Hardware     ${dirFiles_Hardware}     PARENT_SCOPE)
set(dirFiles_Hardware_Imp ${dirFiles_Hardware_Imp} PARENT_SCOPE)
