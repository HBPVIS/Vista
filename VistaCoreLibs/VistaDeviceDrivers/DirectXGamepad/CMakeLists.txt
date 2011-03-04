vista_use_OpenSG()

set(DIRECTX_ROOT $ENV{DIRECTX_ROOT} CACHE PATH "Base directory of the DirectX SDK")

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

if(CMAKE_CL_64)
	link_directories( "${DIRECTX_ROOT}/Lib/x64" )
else()
	link_directories( "${DIRECTX_ROOT}/Lib/x86" )
endif()

link_directories( "${OPENSG_ROOT}/lib" )
include_directories( "${DIRECTX_ROOT}/Include" )

add_library(VistaDirectXGamepad
    ${dirFiles_driver}
)
add_library(VistaDirectXGamepadPlugin
    ${dirFiles_plugin}
) 

target_link_libraries(VistaDirectXGamepad
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
	dxguid
	dxerr
	dinput8
)
target_link_libraries(VistaDirectXGamepadPlugin
	VistaDirectXGamepad
	VistaDeviceDriversBase
	VistaKernel
)

if(WIN32)
	set_target_properties( VistaDirectXGamepad PROPERTIES COMPILE_DEFINITIONS "VISTADIRECTXGAMEPAD_EXPORTS" )
	set_target_properties( VistaDirectXGamepadPlugin PROPERTIES COMPILE_DEFINITIONS "VISTADIRECTXGAMEPADPLUGIN_EXPORTS" )
endif(WIN32)

install(TARGETS VistaDirectXGamepad
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
install(TARGETS VistaDirectXGamepadPlugin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
		 
		 
		 
install(DIRECTORY .
         DESTINATION include/VistaDeviceDrivers/VistaDirectXGamepad
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
