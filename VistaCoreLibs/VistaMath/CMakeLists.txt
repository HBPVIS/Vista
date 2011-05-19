# $Id$

vista_adopt_version( VistaMath VistaCoreLibs )

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAMATH_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAMATH_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaMath ${ProjectSources} )
target_link_libraries( VistaMath
	VistaBase
)

vista_configure_lib( VistaMath )
vista_install( VistaMath VistaMath )
if( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
	vista_create_cmake_configs( VistaMath )
endif( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
vista_create_default_info_file( VistaMath )