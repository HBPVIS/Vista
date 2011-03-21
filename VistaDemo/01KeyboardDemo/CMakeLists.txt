cmake_minimum_required( VERSION 2.6 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 01KeyboardDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaLibCommon )
ENDif( NOT VISTADEMO_COMMON_BUILD )

find_package_versioned( VistaCoreLibs "HEAD" REQUIRED )
vista_use_VistaCoreLibs()

if( WIN32 )
	list( APPEND LIBRARIES opengl32 glu32 )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_executable( 01KeyboardDemo ${ProjectSources} )
target_link_libraries( 01KeyboardDemo
	${VISTACORELIBS_LIBRARIES}
	${LIBRARIES}
)

vista_configure_app( 01KeyboardDemo )
vista_set_app_outdir( 01KeyboardDemo ${CMAKE_CURRENT_SOURCE_DIR} )
vista_install( 01KeyboardDemo )

