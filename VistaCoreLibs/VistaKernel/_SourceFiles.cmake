set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs DisplayManager EventManager GraphicsManager InteractionManager OpenSG PickManager Stuff WindowingToolkit )

set( DirFiles
	VistaClientDataTunnel.cpp
	VistaClientDataTunnel.h
	VistaClusterAux.cpp
	VistaClusterAux.h
	VistaClusterClient.cpp
	VistaClusterClient.h
	VistaClusterMaster.cpp
	VistaClusterMaster.h
	VistaClusterServer.cpp
	VistaClusterServer.h
	VistaClusterSlave.cpp
	VistaClusterSlave.h
	VistaDataTunnel.cpp
	VistaDataTunnel.h
	VistaDataTunnelFactory.cpp
	VistaDataTunnelFactory.h
	VistaDriverPropertyConfigurator.cpp
	VistaDriverPropertyConfigurator.h
	VistaErrorMsgHandler.h
	VistaKernelConfig.h
	VistaKernelMsgPort.cpp
	VistaKernelMsgPort.h
	VistaKernelMsgTab.h
	VistaKernelOut.cpp
	VistaKernelOut.h
	VistaMasterDataTunnel.cpp
	VistaMasterDataTunnel.h
	VistaStandaloneDataTunnel.cpp
	VistaStandaloneDataTunnel.h
	VistaSystem.cpp
	VistaSystem.h
	VistaSystemClassFactory.h
	VistaSystemCommands.cpp
	VistaSystemCommands.h
	VistaSystemConfigurators.cpp
	VistaSystemConfigurators.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

