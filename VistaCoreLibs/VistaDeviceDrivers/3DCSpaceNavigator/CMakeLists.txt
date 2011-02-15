
set(dirFiles_driver
    Vista3DCSpaceNavigatorDriver.h
    Vista3DCSpaceNavigatorDriver.cpp
    Vista3DCSpaceNavigatorConfig.h
)
source_group("Source Files" FILES ${dirFiles_driver})
add_library(Vista3DCSpaceNavigator
    ${dirFiles_driver}
)
target_link_libraries(Vista3DCSpaceNavigator
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
install(TARGETS Vista3DCSpaceNavigator
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)

set(dirFiles_plugin
    Vista3DCSpaceNavigatorPlugin.cpp
    Vista3DCSpaceNavigatorPlugin.h
)
source_group("Source Files" FILES ${dirFiles_plugin})
add_library(Vista3DCSpaceNavigatorPlugin
    ${dirFiles_plugin}
) 
target_link_libraries(Vista3DCSpaceNavigatorPlugin
	Vista3DCSpaceNavigator
	VistaDeviceDriversBase
)

if(WIN32)
	add_definitions(/MP1)
	set_target_properties( Vista3DCSpaceNavigator PROPERTIES COMPILE_DEFINITIONS "VISTA3DCSPACENAVIGATOR_EXPORTS" )
	set_target_properties( Vista3DCSpaceNavigatorPlugin PROPERTIES COMPILE_DEFINITIONS "VISTA3DCSPACENAVIGATORPLUGIN_EXPORTS" )
endif(WIN32)



install(TARGETS Vista3DCSpaceNavigatorPlugin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)		 
		 
install(DIRECTORY .
         DESTINATION include/VistaDeviceDrivers/3DCSpaceNavigator
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
