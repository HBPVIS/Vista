set(relDir "DataLaVista/Common")
set(dirFiles_in
    CMakeLists.txt
    VistaByteBufferPacket.cpp
    VistaFileRegistration.cpp
    VistaFileWriter.cpp
    VistaHRTimerRTC.cpp
    VistaNetDataCollector.cpp
    VistaNetDataDispatcher.cpp
    VistaPacketPump.cpp
    VistaPentiumBasedRTC.cpp
    VistaProfilingFilter.cpp
    VistaRamQueuePipe.cpp
    VistaSerialTeeFilter.cpp
    VistaSlotPipe.cpp
    VistaByteBufferPacket.h
    VistaDummyRTC.h
    VistaFileRegistration.h
    VistaFileWriter.h
    VistaHRTimerRTC.h
    VistaNetDataCollector.h
    VistaNetDataDispatcher.h
    VistaPacketPump.h
    VistaPentiumBasedRTC.h
    VistaProfilingFilter.h
    VistaRamQueuePipe.h
    VistaSerialTeeFilter.h
    VistaSlotPipe.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_DataLaVista_Common "${relDir}/${file}")
endforeach(file)

set(dirFiles_DataLaVista_Common ${dirFiles_DataLaVista_Common} PARENT_SCOPE)
