set( RelativeDir "./DataLaVista/Common" )
set( LocalSourceGroup "Source Files\\DataLaVista\\Common" )

set( DirFiles
	VistaByteBufferPacket.cpp
	VistaByteBufferPacket.h
	VistaDummyRTC.h
	VistaFileRegistration.cpp
	VistaFileRegistration.h
	VistaFileWriter.cpp
	VistaFileWriter.h
	VistaHRTimerRTC.cpp
	VistaHRTimerRTC.h
	VistaNetDataCollector.cpp
	VistaNetDataCollector.h
	VistaNetDataDispatcher.cpp
	VistaNetDataDispatcher.h
	VistaPacketPump.cpp
	VistaPacketPump.h
	VistaPentiumBasedRTC.cpp
	VistaPentiumBasedRTC.h
	VistaProfilingFilter.cpp
	VistaProfilingFilter.h
	VistaRamQueuePipe.cpp
	VistaRamQueuePipe.h
	VistaSerialTeeFilter.cpp
	VistaSerialTeeFilter.h
	VistaSlotPipe.cpp
	VistaSlotPipe.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

