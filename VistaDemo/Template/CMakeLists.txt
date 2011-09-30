# $Id$

cmake_minimum_required( VERSION 2.8 )
project( Template )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaLibCommon )

find_package_versioned( VistaCoreLibs "HEAD" REQUIRED )
vista_use_VistaCoreLibs()

if( WIN32 )
	list( APPEND LIBRARIES opengl32 glu32 )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_executable( Template ${ProjectSources} )
target_link_libraries( Template
	${VISTACORELIBS_LIBRARIES}
	${LIBRARIES}
)

vista_configure_app( Template )
vista_set_app_outdir( Template ${CMAKE_CURRENT_SOURCE_DIR} )
vista_install( Template )

