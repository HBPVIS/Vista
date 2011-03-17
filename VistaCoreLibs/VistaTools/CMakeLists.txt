if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTATOOLS_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTATOOLS_STATIC )
	endif( BUILD_SHARED_LIBS )
	set( LIBRARIES PowrProf dbghelp Psapi )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaTools ${ProjectSources} )
target_link_libraries( VistaTools
	VistaBase
	${LIBRARIES}
)

vista_install( VistaTools VistaTools )
vista_install_package_config( VistaTools )
vista_install_package_version( VistaTools VistaCoreLibs )
