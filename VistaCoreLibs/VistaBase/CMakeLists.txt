if( WIN32 )
	add_definitions( -DVISTABASE_EXPORTS )
else( WIN32 )
	set( LIBRARIES rt )
endif( WIN32 )


include( "./_SourceFiles.cmake" )

add_library( VistaBase ${ProjectSources} )
target_link_libraries( VistaBase
	${LIBRARIES}
)

vista_install( VistaBase VistaBase )

