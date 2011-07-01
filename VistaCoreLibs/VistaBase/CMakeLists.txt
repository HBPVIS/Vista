# $Id: CMakeLists.txt 21407 2011-05-20 13:36:52Z dr165799 $

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
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaBase )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaBase )