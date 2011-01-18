set(relDir "DataLaVista/Base")
set(dirFiles_in
    CMakeLists.txt
    VistaActiveComponent.cpp
    VistaActiveDataConsumer.cpp
    VistaActiveDataProducer.cpp
    VistaActiveFilter.cpp
    VistaDataConsumer.cpp
    VistaDataPacket.cpp
    VistaDataProducer.cpp
    VistaDataSink.cpp
    VistaDataSource.cpp
    VistaDemux.cpp
    VistaFilter.cpp
    VistaInPlaceFilter.cpp
    VistaPacketQueue.cpp
    VistaPipeComponent.cpp
    VistaPipe.cpp
    VistaRegistration.cpp
    VistaRoutingPipe.cpp
    VistaRTC.cpp
    VistaTeeFilter.cpp
    VistaTimeStamp.cpp
    VistaTransformFilter.cpp
    VistaActiveComponent.h
    VistaActiveDataConsumer.h
    VistaActiveDataProducer.h
    VistaActiveFilter.h
    VistaDataConsumer.h
    VistaDataPacket.h
    VistaDataProducer.h
    VistaDataSink.h
    VistaDataSource.h
    VistaDemux.h
    VistaDLVTypes.h
    VistaFilter.h
    VistaInPlaceFilter.h
    VistaPacketQueue.h
    VistaPipeComponent.h
    VistaPipe.h
    VistaRegistration.h
    VistaRoutingPipe.h
    VistaRTC.h
    VistaTeeFilter.h
    VistaTimeStamp.h
    VistaTransformFilter.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_DataLaVista_Base "${relDir}/${file}")
endforeach(file)

set(dirFiles_DataLaVista_Base ${dirFiles_DataLaVista_Base} PARENT_SCOPE)
