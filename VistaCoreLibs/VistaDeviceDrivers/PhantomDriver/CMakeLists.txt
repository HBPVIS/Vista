vista_use_package( OpenHaptics REQUIRED )

set(dirFiles_driver
    VistaPhantomDriver.h
    VistaPhantomDriver.cpp
    VistaPhantomDriverConfig.h
)
set(dirFiles_plugin
	VistaPhantomDriverPlugin.cpp
    VistaPhantomDriverPlugin.h
)
source_group("Source Files" FILES ${dirFiles_driver})
source_group("Source Files" FILES ${dirFiles_plugin})

add_library(VistaPhantomDriver
    ${dirFiles_driver}
)
add_library(VistaPhantomDriverPlugin
    ${dirFiles_plugin}
)

target_link_libraries(VistaPhantomDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)
target_link_libraries(VistaPhantomDriverPlugin
	VistaPhantomDriver
	VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib( VistaPhantomDriver )
vista_install( VistaPhantomDriver )
set_target_properties( VistaPhantomDriver PROPERTIES FOLDER "Drivers" )

vista_configure_lib( VistaPhantomDriverPlugin )
vista_install( VistaPhantomDriverPlugin )
set_target_properties( VistaPhantomDriverPlugin PROPERTIES FOLDER "Drivers" )
