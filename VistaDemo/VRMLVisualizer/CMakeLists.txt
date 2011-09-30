# $Id$

cmake_minimum_required( VERSION 2.8 )
project( VRMLVisualizer )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

vista_use_package( VistaCoreLibs "HEAD" REQUIRED FIND_DEPENDENCIES )


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( VRMLVisualizer ${ProjectSources} )
target_link_libraries( VRMLVisualizer
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( VRMLVisualizer )
vista_create_default_info_file( VRMLVisualizer )

