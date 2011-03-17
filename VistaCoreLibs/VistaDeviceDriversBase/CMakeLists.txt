if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTADEVICEDRIVERS_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTADEVICEDRIVERS_STATIC )
	endif( BUILD_SHARED_LIBS )
	set( LIBRARIES setupapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

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
