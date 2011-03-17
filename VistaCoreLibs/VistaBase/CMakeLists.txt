if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTABASE_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTABASE_STATIC )
	endif( BUILD_SHARED_LIBS )
else( WIN32 )
	set( LIBRARIES rt )
endif( WIN32 )


include( "./_SourceFiles.cmake" )

add_library( VistaBase ${ProjectSources} )
target_link_libraries( VistaBase
	${LIBRARIES}
)

vista_install( VistaBase VistaBase )
vista_install_package_config( VistaBase )
vista_install_package_version( VistaBase VistaCoreLibs )
