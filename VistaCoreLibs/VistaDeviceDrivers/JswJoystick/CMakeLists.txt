vista_use_package( jsw REQUIRED )

set(dirFiles_driver
    VistaJswJoystickDriver.h
    VistaJswJoystickDriver.cpp
    VistaJswJoystickDriverConfig.h
)
set(dirFiles_plugin
	VistaJswJoystickDriverPlugin.cpp
    VistaJswJoystickDriverPlugin.h
)
source_group("Source Files" FILES ${dirFiles_driver})
source_group("Source Files" FILES ${dirFiles_plugin})

add_library(VistaJswJoystickDriver
    ${dirFiles_driver}
)
add_library(VistaJswJoystickDriverPlugin
    ${dirFiles_plugin}
)

target_link_libraries(VistaJswJoystickDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)
target_link_libraries(VistaJswJoystickDriverPlugin
	VistaJswJoystickDriver
	VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib( VistaJswJoystickDriver )
vista_install( VistaJswJoystickDriver )
set_target_properties( VistaJswJoystickDriver PROPERTIES FOLDER "Drivers" )

vista_configure_lib( VistaJswJoystickDriverPlugin )
vista_install( VistaJswJoystickDriverPlugin )
set_target_properties( VistaJswJoystickDriverPlugin PROPERTIES FOLDER "Drivers" )
