vista_adopt_version( VistaKernelOpenSGExt VistaCoreLibs )

vista_use_package( OpenGL )
vista_use_package( GLUT )
vista_use_package( OpenSG )

include( "./_SourceFiles.cmake" )

add_library( VistaKernelOpenSGExt ${ProjectSources} )
target_link_libraries( VistaKernelOpenSGExt
	VistaBase
	VistaAspects
	VistaTools
	VistaInterProcComm
	VistaDeviceDriversBase
    VistaDataFlowNet
	VistaKernel
	${GLUT_LIBRARIES}
	${OPENSG_LIBRARIES}
	${LIBRARIES}
)

vista_configure_lib( VistaKernelOpenSGExt )
vista_install( VistaKernelOpenSGExt VistaKernelOpenSGExt )
if( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
	vista_create_cmake_configs( VistaKernelOpenSGExt )
endif( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )

