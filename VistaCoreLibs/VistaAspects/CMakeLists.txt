# $Id$

vista_adopt_version( VistaAspects VistaCoreLibs )

if( UNIX AND NOT APPLE )
	set( LIBRARIES rt )
endif( UNIX AND NOT APPLE )

include( "./_SourceFiles.cmake" )

add_library( VistaAspects ${ProjectSources} )
target_link_libraries( VistaAspects
	VistaBase
	${LIBRARIES}
)

vista_configure_lib( VistaAspects )
vista_install( VistaAspects VistaAspects )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaAspects )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaAspects )
