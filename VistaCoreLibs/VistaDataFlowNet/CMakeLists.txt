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
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaDataFlowNet )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaDataFlowNet )
