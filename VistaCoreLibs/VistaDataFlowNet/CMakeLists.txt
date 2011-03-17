if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTADFN_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTADFN_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaDataFlowNet ${ProjectSources} )
target_link_libraries( VistaDataFlowNet
	VistaBase
	VistaAspects
	VistaTools
	VistaMath
	VistaInterProcComm
    VistaDeviceDriversBase
)

vista_install( VistaDataFlowNet VistaDataFlowNet )
vista_install_package_config( VistaDataFlowNet )
vista_install_package_version( VistaDataFlowNet VistaCoreLibs )

