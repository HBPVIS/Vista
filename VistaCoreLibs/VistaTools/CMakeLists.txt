if( WIN32 )
	add_definitions( -DVISTATOOLS_EXPORTS )
	set( LIBRARIES PowrProf dbghelp Psapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaTools ${ProjectSources} )
target_link_libraries( VistaTools
	VistaBase
	${LIBRARIES}
)

vista_install( VistaTools VistaTools )

