# $Id$

vista_adopt_version( VistaInterProcComm VistaCoreLibs )

if( WIN32 )
	add_definitions( -D_USE_WIN32_MUTEX )
	set( LIBRARIES winmm ws2_32 )
endif( WIN32 )

set( VISTA_INTERPROCCOMM_USE_EXCEPTIONS ON CACHE BOOL "Specifies wether or nor the IPC throws exceptions when errors occur" )
if( VISTA_INTERPROCCOMM_USE_EXCEPTIONS )
	add_definitions( -DVISTA_IPC_USE_EXCEPTIONS )
endif( VISTA_INTERPROCCOMM_USE_EXCEPTIONS )

include( "./_SourceFiles.cmake" )

add_library( VistaInterProcComm ${ProjectSources} )
target_link_libraries( VistaInterProcComm
    VistaAspects
    VistaTools
	${LIBRARIES}
)

vista_configure_lib( VistaInterProcComm )
vista_install( VistaInterProcComm VistaInterProcComm )
if( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
	vista_create_cmake_configs( VistaInterProcComm )
endif( VISTAS_CREATE_CONFIGS_FOR_EACH_LIBRARY )
vista_create_default_info_file( VistaInterProcComm )
