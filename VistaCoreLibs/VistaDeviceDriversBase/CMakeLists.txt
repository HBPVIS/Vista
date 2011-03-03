if( WIN32 )
	add_definitions( -DVISTADEVICEDRIVERS_EXPORTS )
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

