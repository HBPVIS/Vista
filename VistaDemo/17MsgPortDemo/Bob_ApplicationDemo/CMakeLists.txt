cmake_minimum_required( VERSION 2.6 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 17MsgPortBob )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaLibCommon )
ENDif( NOT VISTADEMO_COMMON_BUILD )

find_package_versioned( VistaCoreLibs "HEAD" REQUIRED )
vista_use_VistaCoreLibs()

if( WIN32 )
	list( APPEND LIBRARIES opengl32 glu32 )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_executable( 17MsgPortBob ${ProjectSources} )
target_link_libraries( 17MsgPortBob
	${VISTACORELIBS_LIBRARIES}
	${LIBRARIES}
)

vista_configure_app( 17MsgPortBob )
vista_set_app_outdir( 17MsgPortBob ${CMAKE_CURRENT_SOURCE_DIR} )
vista_install( 17MsgPortBob )

