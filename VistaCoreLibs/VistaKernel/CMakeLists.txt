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
vista_use_package( OpenSG 1.8 REQUIRED )

if( VISTACORELIBS_BUILD_WINDOWIMP_GLUT )
	vista_use_package( Glut REQUIRED )
	add_definitions( -DVISTA_WITH_GLUT )	
endif()
if( VISTACORELIBS_BUILD_WINDOWIMP_OSG )
	vista_use_package( OpenSceneGraph 3+ REQUIRED osgViewer osgGA )
	add_definitions( -DVISTA_WITH_OSG )
endif()

set( VISTACORELIBS_USE_ZEROMQ FALSE CACHE BOOL "if activated, zeromq-specific stuff will be built" )
if( VISTACORELIBS_USE_ZEROMQ )
	vista_use_package( ZMQ REQUIRED )
	add_definitions( -DVISTA_WITH_ZEROMQ )
endif()

if( NOT VISTACORELIBS_BUILD_WINDOWIMP_GLUT AND NOT VISTACORELIBS_BUILD_WINDOWIMP_OSG )
	message( WARNING "No Windowing Toolkit will be built!" )
endif()

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
set_property( TARGET VistaKernel PROPERTY LINK_INTERFACE_LIBRARIES "" )