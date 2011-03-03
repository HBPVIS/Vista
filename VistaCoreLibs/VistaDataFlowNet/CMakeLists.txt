if( WIN32 )
	add_definitions( -DVISTADFN_EXPORTS )
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

