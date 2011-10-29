SET (DTRACK_ROOT $ENV{DTRACK_ROOT} CACHE PATH "Base directory of the DTRACK Files")

INCLUDE_DIRECTORIES( "${DTRACK_ROOT}/" )
IF (WIN32)
    LINK_DIRECTORIES("${OPENSG_ROOT}/lib")
	INCLUDE_DIRECTORIES( "${OPENSG_ROOT}/Include" )
ENDIF(WIN32)


#Actual Driver Module
SET (dirFiles_driver
    VistaDTrackSDKDriver.h
    VistaDTrackSDKDriver.cpp
    VistaDTrackSDKDriverConfig.h
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_driver})
ADD_LIBRARY (VistaDTrackSDKDriver
    ${dirFiles_driver}
)

TARGET_LINK_LIBRARIES (VistaDTrackSDKDriver
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)

INSTALL (TARGETS VistaDTrackSDKDriver
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})

		 
#Transcoder
SET (dirFiles_transcoder
    VistaDTrackSDKDriverTranscoder.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_transcoder})
ADD_LIBRARY (VistaDTrackSDKTranscoder
    ${dirFiles_transcoder}
)
TARGET_LINK_LIBRARIES (VistaDTrackSDKTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
INSTALL (TARGETS VistaDTrackSDKTranscoder
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})

SET (dirFiles_plugin
    VistaDTrackSDKDriverPlugin.cpp
)
SOURCE_GROUP ("Source Files" FILES ${dirFiles_plugin})
ADD_LIBRARY (VistaDTrackSDKPlugin
    ${dirFiles_plugin}
) 
TARGET_LINK_LIBRARIES (VistaDTrackSDKPlugin
	VistaDTrackSDKDriver
	VistaDeviceDriversBase
	VistaKernel
	${LIBRARIES}
)

INSTALL (TARGETS VistaDTrackSDKPlugin
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})	

IF (WIN32)
	SET_TARGET_PROPERTIES( VistaDTrackSDKDriver PROPERTIES COMPILE_DEFINITIONS "VISTADTRACKSDK_EXPORTS" )
	SET_TARGET_PROPERTIES( VistaDTrackSDKPlugin PROPERTIES COMPILE_DEFINITIONS "VISTADTRACKSDKPLUGIN_EXPORTS" )
	SET_TARGET_PROPERTIES( VistaDTrackSDKTranscoder PROPERTIES COMPILE_DEFINITIONS "VISTADTRACKSDKTRANSCODER_EXPORTS" )
ENDIF(WIN32)

SET_TARGET_PROPERTIES( VistaDTrackSDKDriver PROPERTIES FOLDER "Drivers" )
SET_TARGET_PROPERTIES( VistaDTrackSDKPlugin PROPERTIES FOLDER "Drivers" )
SET_TARGET_PROPERTIES( VistaDTrackSDKTranscoder PROPERTIES FOLDER "Drivers" )
	 
		 
INSTALL (DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaDeviceDrivers/DTrackSDKDriver
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
