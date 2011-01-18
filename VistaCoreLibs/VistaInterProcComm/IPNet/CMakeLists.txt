set(relDir "IPNet")
set(dirFiles_in
    CMakeLists.txt
    VistaAcceptor.cpp
    VistaIPAddress.cpp
    VistaIPComm.cpp
    VistaMcastIPAddress.cpp
    VistaMcastSocketAddress.cpp
    VistaMcastSocket.cpp
    VistaNetworkInfo.cpp
    VistaServiceIndicator.cpp
    VistaSocketAddress.cpp
    VistaSocket.cpp
    VistaTCPClientSocket.cpp
    VistaTCPServer.cpp
    VistaTCPServerSocket.cpp
    VistaTCPSocket.cpp
    VistaUDPSocket.cpp
    VistaAcceptor.h
    VistaIPAddress.h
    VistaIPComm.h
    VistaMcastIPAddress.h
    VistaMcastSocketAddress.h
    VistaMcastSocket.h
    VistaNetworkInfo.h
    VistaServiceIndicator.h
    VistaSocketAddress.h
    VistaSocket.h
    VistaTCPClientSocket.h
    VistaTCPServer.h
    VistaTCPServerSocket.h
    VistaTCPSocket.h
    VistaUDPSocket.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_IPNet "${relDir}/${file}")
endforeach(file)

set(dirFiles_IPNet ${dirFiles_IPNet} PARENT_SCOPE)
