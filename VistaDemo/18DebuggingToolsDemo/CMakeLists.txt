# $Id$

cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 18DebuggingToolsDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
	
	vista_use_package( VistaCoreLibs HEAD REQUIRED VistaKernel FIND_DEPENDENCIES )
else()
	vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )
endif()


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 18DebuggingToolsDemo ${ProjectSources} )
target_link_libraries( 18DebuggingToolsDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 18DebuggingToolsDemo )
vista_create_default_info_file( 18DebuggingToolsDemo )

