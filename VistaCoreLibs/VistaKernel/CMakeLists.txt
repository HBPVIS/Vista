vista_use_GLUT()
vista_use_OpenSG()

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAKERNEL_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAKERNEL_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

add_definitions( -DVISTA_SYS_OPENSG -DVISTAKERNEL_USE_PROFILING )

include( "./_SourceFiles.cmake" )

add_library( VistaKernel ${ProjectSources} )
target_link_libraries( VistaKernel
	VistaBase
	VistaAspects
	VistaTools
    VistaMath
	VistaInterProcComm
	VistaDeviceDriversBase
    VistaDataFlowNet
	${GLUT_LIBRARIES}
	${OPENSG_LIBRARIES}
)

vista_install( VistaKernel VistaKernel )
vista_install_package_config( VistaKernel )
vista_install_package_version( VistaKernel VistaCoreLibs )
