# $Id$

vista_adopt_version( VistaKernel VistaCoreLibs )

set( VISTAKERNEL_INTERNAL_DEPENDENCIES 
			VistaBase
			VistaAspects
			VistaInterProcComm
			VistaMath
			VistaTools
			VistaDeviceDriversBase
			VistaDataFlowNet )
set( VISTAKERNEL_INTERNAL_DEPENDENCIES ${VISTAKERNEL_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

vista_use_package( OpenGL REQUIRED )
vista_use_package( GLUT REQUIRED )
vista_use_package( OpenSG 1.8 REQUIRED )

add_definitions( -DVISTA_SYS_OPENSG -DVISTAKERNEL_USE_PROFILING )

include( "./_SourceFiles.cmake" )

add_library( VistaKernel ${ProjectSources} )
target_link_libraries( VistaKernel
	${VISTAKERNEL_INTERNAL_DEPENDENCIES}
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib( VistaKernel )
vista_install( VistaKernel VistaKernel )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaKernel )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaKernel )
