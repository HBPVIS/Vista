		 
# Transcoder

set( dirFiles_transcoder
    Vista3DCSpaceNavigatorTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( Vista3DCSpaceNavigatorTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( Vista3DCSpaceNavigatorTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( Vista3DCSpaceNavigatorTranscoder )
vista_install( Vista3DCSpaceNavigatorTranscoder "VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver" "DriverPlugins" )	 
set_target_properties( Vista3DCSpaceNavigatorTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( Vista3DCSpaceNavigatorTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
if( WIN32 )
try_compile( _CAN_BULD_DRIVER "${CMAKE_CURRENT_BINARY_DIR}/CMakeTest" "${CMAKE_CURRENT_SOURCE_DIR}/Vista3DCSpaceNavigatorCMakeTest.cpp" )
else( WIN32 )
	set( _CAN_BULD_DRIVER 1 )
endif( WIN32 )

if( NOT _CAN_BULD_DRIVER )
	message( STATUS "Vista3DCSpaceNavigatorDriver requires running SpaceNavigator driver, which is not available - only transcoder will be built" )
else( NOT _CAN_BULD_DRIVER )
			 
	# Driver Plugin

	set( dirFiles_plugin
		Vista3DCSpaceNavigatorPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( Vista3DCSpaceNavigatorPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( Vista3DCSpaceNavigatorPlugin
		Vista3DCSpaceNavigatorDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( Vista3DCSpaceNavigatorPlugin )
	vista_install( Vista3DCSpaceNavigatorPlugin "VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver" "DriverPlugins" )
	set_target_properties( Vista3DCSpaceNavigatorPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( Vista3DCSpaceNavigatorPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	set( dirFiles_driver
		Vista3DCSpaceNavigatorDriver.h
		Vista3DCSpaceNavigatorDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( Vista3DCSpaceNavigatorDriver
		${dirFiles_driver}
	)

	target_link_libraries( Vista3DCSpaceNavigatorDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( Vista3DCSpaceNavigatorDriver )
	vista_install( Vista3DCSpaceNavigatorDriver "VistaDeviceDrivers/Vista3DCSpaceNavigatorDriver" "DriverPlugins" )
	set_target_properties( Vista3DCSpaceNavigatorDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( Vista3DCSpaceNavigatorDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET Vista3DCSpaceNavigatorDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver

endif( NOT _CAN_BULD_DRIVER )