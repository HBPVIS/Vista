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
if( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
	vista_create_cmake_configs( VistaBase )
endif( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
vista_create_default_info_file( VistaBase )