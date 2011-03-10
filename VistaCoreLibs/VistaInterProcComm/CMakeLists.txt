if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAINTERPROCCOMM_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAINTERPROCCOMM_STATIC )
	endif( BUILD_SHARED_LIBS )
	add_definitions( -D_USE_WIN32_MUTEX )
	set( LIBRARIES winmm ws2_32 )
endif( WIN32 )

set( VISTA_INTERPROCCOMM_USE_EXCEPTIONS ON CACHE BOOL "Specifies wether or nor the IPC throws exceptions when errors occur" )
mark_as_advanced( VISTA_INTERPROCCOMM_USE_EXCEPTIONS )
if( VISTA_INTERPROCCOMM_USE_EXCEPTIONS )
	add_definitions( -DUSE_IPC_EXCEPTIONS )
endif( VISTA_INTERPROCCOMM_USE_EXCEPTIONS )

include( "./_SourceFiles.cmake" )

add_library( VistaInterProcComm ${ProjectSources} )
target_link_libraries( VistaInterProcComm
    VistaAspects
    VistaInterProcComm
    VistaTools
    VistaMath
	${LIBRARIES}
)

vista_install( VistaInterProcComm VistaInterProcComm )

