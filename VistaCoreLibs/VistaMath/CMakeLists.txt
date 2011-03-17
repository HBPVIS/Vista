if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAMATH_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAMATH_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaMath ${ProjectSources} )
target_link_libraries( VistaMath
	VistaBase
)

vista_install( VistaMath VistaMath )
vista_install_package_config( VistaMath )
vista_install_package_version( VistaMath VistaCoreLibs )
