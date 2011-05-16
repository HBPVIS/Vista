vista_use_package( GLUT REQUIRED )
vista_use_package( OpenSG REQUIRED )
vista_use_package( DirectX REQUIRED )

set(dirFiles_driver
    VistaDirectXGamepad.h
    VistaDirectXGamepad.cpp
    VistaDirectXGamepadConfig.h
)
set(dirFiles_plugin
	VistaDirectXGamepadPlugin.cpp
    VistaDirectXGamepadPlugin.h
)
source_group("Source Files" FILES ${dirFiles_driver})
source_group("Source Files" FILES ${dirFiles_plugin})

add_library(VistaDirectXGamepadDriver
    ${dirFiles_driver}
)
add_library(VistaDirectXGamepadDriverPlugin
    ${dirFiles_plugin}
)

target_link_libraries(VistaDirectXGamepadDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	${VISTA_USE_PACKAGE_LIBRARIES}
)
target_link_libraries(VistaDirectXGamepadDriverPlugin
	VistaDirectXGamepadDriver
	VistaDeviceDriversBase
	VistaKernel
	${VISTA_USE_PACKAGE_LIBRARIES}
)

vista_configure_lib( VistaDirectXGamepadDriver )
vista_install( VistaDirectXGamepadDriver )
set_target_properties( VistaDirectXGamepadDriver PROPERTIES FOLDER "Drivers" )

vista_configure_lib( VistaDirectXGamepadDriverPlugin )
vista_install( VistaDirectXGamepadDriverPlugin )
set_target_properties( VistaDirectXGamepadDriverPlugin PROPERTIES FOLDER "Drivers" )

