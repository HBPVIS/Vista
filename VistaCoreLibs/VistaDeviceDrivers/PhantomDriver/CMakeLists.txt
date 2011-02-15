if(WIN32)
    add_definitions(-DVISTAPHANTOMDRIVER_EXPORTS)
    add_definitions(-DVISTAPHANTOMDRIVERPLUGIN_EXPORTS)
endif(WIN32)

set(OPENHAPTICS_ROOT $ENV{OPENHAPTICS_ROOT} CACHE PATH "Base directory of the DirectX SDK")

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

if(CMAKE_CL_64)
	link_directories( "${OPENHAPTICS_ROOT}/lib/x64" )
	link_directories( "${OPENHAPTICS_ROOT}/utilities/lib/x64/$(OutDir)" )
else()
	link_directories( "${OPENHAPTICS_ROOT}/lib/win32" )
	link_directories( "${OPENHAPTICS_ROOT}/utilities/lib/win32/$(OutDir)" )
endif()

include_directories( "${OPENHAPTICS_ROOT}/include" )
include_directories( "${OPENHAPTICS_ROOT}/utilities/include" )

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
	hd
	hdu
)
target_link_libraries(VistaPhantomDriverPlugin
	VistaPhantomDriver
)

install(TARGETS VistaPhantomDriver
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
install(TARGETS VistaPhantomDriverPlugin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
		 
		 
		 
install(DIRECTORY .
         DESTINATION include/VistaDeviceDrivers/VistaPhantomDriver
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
