set(JSW_ROOT $ENV{JSW_ROOT} CACHE PATH "Base directory of the Jsw toolkit")

set(dirFiles_driver
    VistaJswJoystickDriver.h
    VistaJswJoystickDriver.cpp
)
set(dirFiles_plugin
	VistaJswJoystickDriverPlugin.cpp
    VistaJswJoystickDriverPlugin.h
)
source_group("Source Files" FILES ${dirFiles_driver})
source_group("Source Files" FILES ${dirFiles_plugin})

link_directories( "${JSW_ROOT}/lib" )
include_directories( "${JSW_ROOT}/include" )

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
	jsw
)
target_link_libraries(VistaJswJoystickDriverPlugin
	VistaJswJoystickDriver
)

install(TARGETS VistaJswJoystickDriver
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
install(TARGETS VistaJswJoystickDriverPlugin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
		 
		 
		 
install(DIRECTORY .
         DESTINATION include/VistaDeviceDrivers/VistaJswJoystickDriver
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
