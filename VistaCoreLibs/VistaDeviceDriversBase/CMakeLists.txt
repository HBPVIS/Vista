if(WIN32)
    add_definitions(-DVISTADEVICEDRIVERS_EXPORTS)
    set(LIBRARIES setupapi)
elseif(UNIX)
endif(WIN32)

set(dirFiles
    VistaConnectionUpdater.cpp
    VistaDeviceDriverAspectRegistry.cpp
    VistaDeviceDriver.cpp
    VistaDeviceDriversOut.cpp
    VistaDeviceIdentificationAspect.cpp
    VistaDeviceSensor.cpp
    VistaDriverAbstractWindowAspect.cpp
    VistaDriverConnectionAspect.cpp
    VistaDriverForceFeedbackAspect.cpp
    VistaDriverGenericParameterAspect.cpp
    VistaDriverInfoAspect.cpp
    VistaDriverLoggingAspect.cpp
    VistaDriverMap.cpp
    VistaDriverMeasureHistoryAspect.cpp
    VistaDriverProtocolAspect.cpp
    VistaDriverReferenceFrameAspect.cpp
    VistaDriverSensorMappingAspect.cpp
    VistaDriverThreadAspect.cpp
    VistaDriverUtils.cpp
    VistaDriverWorkspaceAspect.cpp
    VistaKeyboardDriver.cpp
    VistaMouseDriver.cpp
    VistaShallowDriver.cpp
    VistaConnectionUpdater.h
    VistaDeviceDriverAspectRegistry.h
    VistaDeviceDriver.h
    VistaDeviceDriversConfig.h
    VistaDeviceDriversOut.h
    VistaDeviceIdentificationAspect.h
    VistaDeviceSensor.h
    VistaDriverAbstractWindowAspect.h
    VistaDriverConnectionAspect.h
    VistaDriverForceFeedbackAspect.h
    VistaDriverGenericParameterAspect.h
    VistaDriverInfoAspect.h
    VistaDriverLoggingAspect.h
    VistaDriverManager.h
    VistaDriverMap.h
    VistaDriverMeasureHistoryAspect.h
    VistaDriverProtocolAspect.h
    VistaDriverReferenceFrameAspect.h
    VistaDriverSensorMappingAspect.h
    VistaDriverThreadAspect.h
    VistaDriverUtils.h
    VistaDriverWorkspaceAspect.h
    VistaKeyboardDriver.h
    VistaMouseDriver.h
    VistaShallowDriver.h
)
source_group("Source Files" FILES ${dirFiles})

add_subdirectory(Drivers)
source_group("Source Files\\Drivers" FILES ${dirFiles_Drivers})

add_library(VistaDeviceDriversBase
    ${dirFiles}
    ${dirFiles_Drivers}
) 

target_link_libraries(VistaDeviceDriversBase
    VistaAspects
    VistaInterProcComm
    VistaTools
    VistaMath
    ${LIBRARIES}
)

install(TARGETS VistaDeviceDriversBase
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
install(DIRECTORY .
         DESTINATION include/VistaDeviceDriversBase
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
