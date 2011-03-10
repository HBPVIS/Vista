if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAASPECTS_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAASPECTS_STATIC )
	endif( BUILD_SHARED_LIBS )
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

