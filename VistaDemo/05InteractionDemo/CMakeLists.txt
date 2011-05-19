# $Id: $

cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 05InteractionDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif( NOT VISTADEMO_COMMON_BUILD )

#include the sener sub-app into the build
add_subdirectory( Sender )

vista_use_package( VistaCoreLibs "HEAD" REQUIRED VistaKernel FIND_DEPENDENCIES )


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 05InteractionDemo ${ProjectSources} )
target_link_libraries( 05InteractionDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 05InteractionDemo )
vista_create_default_info_file( 05InteractionDemo )

