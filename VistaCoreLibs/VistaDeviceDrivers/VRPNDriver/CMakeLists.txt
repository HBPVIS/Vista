vista_use_package( vrpn REQUIRED )

set(dirFiles_driver
    VistaVRPNDriver.h
    VistaVRPNDriver.cpp
    VistaVRPNDriverConfig.h
)
set(dirFiles_plugin
	VistaVRPNDriverPlugin.cpp
    VistaVRPNDriverPlugin.h
)
source_group("Source Files" FILES ${dirFiles_driver})
source_group("Source Files" FILES ${dirFiles_plugin})

add_library(VistaVRPNDriver
    ${dirFiles_driver}
)
add_library(VistaVRPNDriverPlugin
    ${dirFiles_plugin}
)

target_link_libraries(VistaVRPNDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)
target_link_libraries(VistaVRPNDriverPlugin
	VistaVRPNDriver
	VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib( VistaVRPNDriver )
vista_install( VistaVRPNDriver )
set_target_properties( VistaVRPNDriver PROPERTIES FOLDER "Drivers" )

vista_configure_lib( VistaVRPNDriverPlugin )
vista_install( VistaVRPNDriverPlugin )
set_target_properties( VistaVRPNDriverPlugin PROPERTIES FOLDER "Drivers" )
