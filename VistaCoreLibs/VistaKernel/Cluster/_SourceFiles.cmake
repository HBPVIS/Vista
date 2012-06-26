# $Id$

set( RelativeDir "./Cluster" )
set( RelativeSourceGroup "Source Files\\Cluster" )

set( DirFiles
	VistaClusterMaster.cpp
	VistaClusterMaster.h
	VistaClusterMode.cpp
	VistaClusterMode.h
	VistaClusterSlave.cpp
	VistaClusterSlave.h
	VistaClusterStandalone.cpp
	VistaClusterStandalone.h
	VistaDataTunnel.cpp
	VistaDataTunnel.h
	VistaMasterDataTunnel.cpp
	VistaMasterDataTunnel.h
	#VistaMasterNetworkSync.cpp
	#VistaMasterNetworkSync.h
	VistaSlaveDataTunnel.cpp
	VistaSlaveDataTunnel.h
	#VistaSlaveNetworkSync.cpp
	#VistaSlaveNetworkSync.h
	VistaStandaloneDataTunnel.cpp
	VistaStandaloneDataTunnel.h
	#VistaStandaloneNetworkSync.cpp
	#VistaStandaloneNetworkSync.h
	VistaNewClusterMaster.cpp
	VistaNewClusterMaster.h
	VistaNewClusterSlave.cpp
	VistaNewClusterSlave.h
	VistaClusterMessage.cpp
	VistaClusterMessage.h
	VistaNetSyncedTimerImp.cpp
	VistaNetSyncedTimerImp.h
	VistaGSyncSwapBarrier.cpp
	VistaGSyncSwapBarrier.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )


if( VISTACORELIBS_USE_ZEROMQ )
	include( "${RelativeDir}/ZeroMQExt/_SourceFiles.cmake" )
endif()
