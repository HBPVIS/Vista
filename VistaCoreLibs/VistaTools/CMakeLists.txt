vista_adopt_version( VistaTools VistaCoreLibs )

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTATOOLS_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTATOOLS_STATIC )
	endif( BUILD_SHARED_LIBS )
	set( LIBRARIES PowrProf dbghelp Psapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaTools ${ProjectSources} )
target_link_libraries( VistaTools
	VistaBase
	${LIBRARIES}
)

vista_configure_lib( VistaTools )
vista_install( VistaTools VistaTools )
if( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
	vista_create_cmake_configs( VistaTools )
endif( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
