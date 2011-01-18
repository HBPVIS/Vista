set(relDir "Connections")
set(dirFiles_in
    CMakeLists.txt
    VistaByteBufferDeSerializer.cpp
    VistaByteBufferSerializer.cpp
    VistaConnection.cpp
    VistaConnectionFile.cpp
    VistaConnectionFileTimed.cpp
    VistaConnectionIP.cpp
    VistaConnectionPipe.cpp
    VistaConnectionSerial.cpp
    VistaConnectionUSB.cpp
    VistaCSVDeSerializer.cpp
    VistaCSVSerializer.cpp
    VistaMasterNetworkSync.cpp
    VistaMsgChannel.cpp
    VistaMsg.cpp
    VistaNetworkSync.cpp
    VistaProgressMessage.cpp
    VistaSlaveNetworkSync.cpp
    VistaByteBufferDeSerializer.h
    VistaByteBufferSerializer.h
    VistaConnectionFile.h
    VistaConnectionFileTimed.h
    VistaConnection.h
    VistaConnectionIP.h
    VistaConnectionPipe.h
    VistaConnectionSerial.h
    VistaConnectionUSB.h
    VistaCSVDeSerializer.h
    VistaCSVSerializer.h
    VistaMasterNetworkSync.h
    VistaMsgChannel.h
    VistaMsg.h
    VistaNetworkSync.h
    VistaProgressMessage.h
    VistaSlaveNetworkSync.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Connections "${relDir}/${file}")
endforeach(file)

set(dirFiles_Connections ${dirFiles_Connections} PARENT_SCOPE)
