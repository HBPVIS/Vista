if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTADEVICEDRIVERS_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTADEVICEDRIVERS_STATIC )
	endif( BUILD_SHARED_LIBS )
	set( LIBRARIES setupapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

if( VISTA_64BIT )
	set( VISTA_DRIVERS_USE_ATOMICS FALSE CACHE BOOL "Specifies if the Device Driver History uses atomics" )
else( VISTA_64BIT )
	set( VISTA_DRIVERS_USE_ATOMICS TRUE CACHE BOOL "Specifies if the Device Driver History uses atomics" )
endif( VISTA_64BIT )

if( VISTA_DRIVERS_USE_ATOMICS )
	add_definitions( -DVISTA_USE_ATOMICS )
endif( VISTA_DRIVERS_USE_ATOMICS )

add_library( VistaDeviceDriversBase ${ProjectSources} )
target_link_libraries( VistaDeviceDriversBase
    VistaAspects
    VistaInterProcComm
    VistaTools
    VistaMath
	${LIBRARIES}
)

vista_install( VistaDeviceDriversBase VistaDeviceDriversBase )
vista_install_package_config( VistaDeviceDriversBase )
vista_install_package_version( VistaDeviceDriversBase VistaCoreLibs )
