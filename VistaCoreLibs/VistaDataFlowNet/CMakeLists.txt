# $Id$

vista_adopt_version( VistaDataFlowNet VistaCoreLibs )

include( "./_SourceFiles.cmake" )

add_library( VistaDataFlowNet ${ProjectSources} )
target_link_libraries( VistaDataFlowNet
	VistaBase
	VistaAspects
	VistaTools
	VistaInterProcComm
    VistaDeviceDriversBase
)

vista_configure_lib( VistaDataFlowNet )
vista_install( VistaDataFlowNet VistaDataFlowNet )
if( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
	vista_create_cmake_configs( VistaDataFlowNet )
endif( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )

