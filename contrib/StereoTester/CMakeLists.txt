# $Id:$

cmake_minimum_required( VERSION 2.8 )
project( StereoTester )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

vista_use_package( VistaCoreLibs "HEAD" REQUIRED FIND_DEPENDENCIES )

add_executable( StereoTester main.cpp )
target_link_libraries( StereoTester
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( StereoTester )
vista_create_default_info_file( StereoTester )

