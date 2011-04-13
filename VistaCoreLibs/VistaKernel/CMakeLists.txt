vista_adopt_version( VistaKernel VistaCoreLibs )

vista_use_package( GLUT )
vista_use_package( OpenSG )

add_definitions( -DVISTA_SYS_OPENSG -DVISTAKERNEL_USE_PROFILING )

include( "./_SourceFiles.cmake" )

add_library( VistaKernel ${ProjectSources} )
target_link_libraries( VistaKernel
	VistaBase
	VistaAspects
	VistaTools
	VistaInterProcComm
	VistaDeviceDriversBase
    VistaDataFlowNet
	${GLUT_LIBRARIES}
	${OPENSG_LIBRARIES}
)

vista_configure_lib( VistaKernel )
vista_install( VistaKernel VistaKernel )
if( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
	vista_create_cmake_configs( VistaKernel )
endif( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
