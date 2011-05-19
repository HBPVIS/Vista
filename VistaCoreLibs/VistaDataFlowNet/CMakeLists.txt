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
if( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
	vista_create_cmake_configs( VistaDataFlowNet )
endif( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
vista_create_default_info_file( VistaDataFlowNet )
