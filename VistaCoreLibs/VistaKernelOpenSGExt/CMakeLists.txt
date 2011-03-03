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
	add_definitions( -DVISTAKERNELOPENSGEXT_EXPORTS )
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

