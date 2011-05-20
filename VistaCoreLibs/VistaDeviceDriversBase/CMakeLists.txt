# $Id$

vista_adopt_version( VistaDeviceDriversBase VistaCoreLibs )

if( WIN32 )
	set( LIBRARIES setupapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

if( VISTA_64BIT )
	set( VISTADEVICEDRIVERS_USE_ATOMICS FALSE CACHE BOOL "Specifies if the Device Driver History uses atomics" )
else( VISTA_64BIT )
	set( VISTADEVICEDRIVERS_USE_ATOMICS TRUE CACHE BOOL "Specifies if the Device Driver History uses atomics" )
endif( VISTA_64BIT )

if( VISTADEVICEDRIVERS_USE_ATOMICS )
	add_definitions( -DVISTA_USE_ATOMICS )
endif( VISTADEVICEDRIVERS_USE_ATOMICS )

add_library( VistaDeviceDriversBase ${ProjectSources} )
target_link_libraries( VistaDeviceDriversBase
    VistaAspects
	VistaMath
    VistaInterProcComm
    VistaTools
	${LIBRARIES}
)

vista_configure_lib( VistaDeviceDriversBase )
vista_install( VistaDeviceDriversBase VistaDeviceDriversBase )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaDeviceDriversBase )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaDeviceDriversBase )
