# $Id$

vista_use_package( wiiuse REQUIRED )

set(dirFiles_driver
    VistaWiimoteDriver.h
    VistaWiimoteDriver.cpp
    VistaWiimoteDriverConfig.h
)
set(dirFiles_plugin
	VistaWiimoteDriverPlugin.cpp
    VistaWiimoteDriverPlugin.h
)
source_group("Source Files" FILES ${dirFiles_driver})
source_group("Source Files" FILES ${dirFiles_plugin})

add_library(VistaWiimoteDriver
    ${dirFiles_driver}
)
add_library(VistaWiimoteDriverPlugin
    ${dirFiles_plugin}
)

target_link_libraries(VistaWiimoteDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)
target_link_libraries(VistaWiimoteDriverPlugin
	VistaWiimoteDriver
	VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib( VistaWiimoteDriver )
vista_install( VistaWiimoteDriver )
set_target_properties( VistaWiimoteDriver PROPERTIES FOLDER "Drivers" )
vista_create_default_info_file( VistaWiimoteDriver )

vista_configure_lib( VistaWiimoteDriverPlugin )
vista_install( VistaWiimoteDriverPlugin )
set_target_properties( VistaWiimoteDriverPlugin PROPERTIES FOLDER "Drivers" )
vista_create_default_info_file( VistaWiimoteDriverPlugin )
