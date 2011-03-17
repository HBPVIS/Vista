find_Package( VGLUT REQUIRED )
find_Package( VOpenSG REQUIRED )

vista_use_GLUT()
vista_use_OpenSG()

if( WIN32 )
    set( LIBRARIES
		glu32
		opengl32
    )
endif( WIN32 )

if( WIN32 )
	if( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAKERNELOPENSGEXT_EXPORTS )
	else( BUILD_SHARED_LIBS )
		add_definitions( -DVISTAKERNELOPENSGEXT_STATIC )
	endif( BUILD_SHARED_LIBS )
endif( WIN32 )

include( "./_SourceFiles.cmake" )

add_library( VistaKernelOpenSGExt ${ProjectSources} )
target_link_libraries( VistaKernelOpenSGExt
	VistaBase
	VistaAspects
	VistaTools
    VistaMath
	VistaInterProcComm
	VistaDeviceDriversBase
    VistaDataFlowNet
	VistaKernel
	${GLUT_LIBRARIES}
	${OPENSG_LIBRARIES}
	${LIBRARIES}
)

vista_install( VistaKernelOpenSGExt VistaKernelOpenSGExt )
vista_install_package_config( VistaKernelOpenSGExt )
vista_install_package_version( VistaKernelOpenSGExt VistaCoreLibs )
