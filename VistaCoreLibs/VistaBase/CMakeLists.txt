# $Id$

vista_adopt_version( VistaBase VistaCoreLibs )

if( UNIX )
	set( LIBRARIES rt )
endif( UNIX )

include( "./_SourceFiles.cmake" )

add_library( VistaBase ${ProjectSources} )
target_link_libraries( VistaBase
	${LIBRARIES}
)

vista_configure_lib( VistaBase )
vista_install( VistaBase VistaBase )
if( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
	vista_create_cmake_configs( VistaBase )
endif( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
