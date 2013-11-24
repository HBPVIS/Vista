# $Id: CMakeLists.txt 39462 2013-11-24 20:10:20Z dr165799 $

cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 22RecordReplayDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
	
	vista_use_package( VistaCoreLibs HEAD REQUIRED VistaKernel FIND_DEPENDENCIES )
else()
	vista_use_package( VistaCoreLibs REQUIRED VistaKernel FIND_DEPENDENCIES )
endif()


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 22RecordReplayDemo ${ProjectSources} )
target_link_libraries( 22RecordReplayDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 22RecordReplayDemo )
vista_create_default_info_file( 22RecordReplayDemo )

