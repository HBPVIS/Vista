# $Id$

vista_adopt_version( VistaAspects VistaCoreLibs )

if( UNIX )
	set( LIBRARIES rt )
endif( UNIX )

include( "./_SourceFiles.cmake" )

add_library( VistaAspects ${ProjectSources} )
target_link_libraries( VistaAspects
	VistaBase
	${LIBRARIES}
)

vista_configure_lib( VistaAspects )
vista_install( VistaAspects VistaAspects )
if( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
	vista_create_cmake_configs( VistaAspects )
endif( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )
