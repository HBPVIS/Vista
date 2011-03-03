if( WIN32 )
	add_definitions( -DVISTAMATH_EXPORTS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaMath ${ProjectSources} )
target_link_libraries( VistaMath
	VistaBase
)

vista_install( VistaMath VistaMath )

