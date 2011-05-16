
set(dirFiles_driver
    Vista3DCSpaceNavigatorDriver.h
    Vista3DCSpaceNavigatorDriver.cpp
    Vista3DCSpaceNavigatorConfig.h
)
source_group("Source Files" FILES ${dirFiles_driver})
add_library(Vista3DCSpaceNavigatorDriver
    ${dirFiles_driver}
)
target_link_libraries(Vista3DCSpaceNavigatorDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( Vista3DCSpaceNavigatorDriver )
vista_install( Vista3DCSpaceNavigatorDriver )
set_target_properties( Vista3DCSpaceNavigatorDriver PROPERTIES FOLDER "Drivers" )

set(dirFiles_plugin
    Vista3DCSpaceNavigatorPlugin.cpp
    Vista3DCSpaceNavigatorPlugin.h
)
source_group("Source Files" FILES ${dirFiles_plugin})
add_library(Vista3DCSpaceNavigatorDriverPlugin
    ${dirFiles_plugin}
) 
target_link_libraries(Vista3DCSpaceNavigatorDriverPlugin
	Vista3DCSpaceNavigatorDriver
	VistaDeviceDriversBase
)
vista_configure_lib( Vista3DCSpaceNavigatorDriverPlugin )
vista_install( Vista3DCSpaceNavigatorDriverPlugin )
set_target_properties( Vista3DCSpaceNavigatorDriverPlugin PROPERTIES FOLDER "Drivers" )
