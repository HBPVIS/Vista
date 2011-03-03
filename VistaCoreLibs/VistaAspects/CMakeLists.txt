if( WIN32 )
	add_definitions( -DVISTAASPECTS_EXPORTS )
else( WIN32 )
	set( LIBRARIES rt )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaAspects ${ProjectSources} )
target_link_libraries( VistaAspects
	VistaBase
	${LIBRARIES}
)

vista_install( VistaAspects VistaAspects )

