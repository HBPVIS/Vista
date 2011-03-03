set( RelativeDir "." )
set( LocalSourceGroup "Source Files" )
set( SubDirs Drivers )

set( DirFiles
	VistaConnectionUpdater.cpp
	VistaConnectionUpdater.h
	VistaDeviceDriver.cpp
	VistaDeviceDriver.h
	VistaDeviceDriverAspectRegistry.cpp
	VistaDeviceDriverAspectRegistry.h
	VistaDeviceDriversConfig.h
	VistaDeviceDriversOut.cpp
	VistaDeviceDriversOut.h
	VistaDeviceIdentificationAspect.cpp
	VistaDeviceIdentificationAspect.h
	VistaDeviceSensor.cpp
	VistaDeviceSensor.h
	VistaDriverAbstractWindowAspect.cpp
	VistaDriverAbstractWindowAspect.h
	VistaDriverConnectionAspect.cpp
	VistaDriverConnectionAspect.h
	VistaDriverForceFeedbackAspect.cpp
	VistaDriverForceFeedbackAspect.h
	VistaDriverGenericParameterAspect.cpp
	VistaDriverGenericParameterAspect.h
	VistaDriverInfoAspect.cpp
	VistaDriverInfoAspect.h
	VistaDriverLoggingAspect.cpp
	VistaDriverLoggingAspect.h
	VistaDriverManager.h
	VistaDriverMap.cpp
	VistaDriverMap.h
	VistaDriverMeasureHistoryAspect.cpp
	VistaDriverMeasureHistoryAspect.h
	VistaDriverProtocolAspect.cpp
	VistaDriverProtocolAspect.h
	VistaDriverReferenceFrameAspect.cpp
	VistaDriverReferenceFrameAspect.h
	VistaDriverSensorMappingAspect.cpp
	VistaDriverSensorMappingAspect.h
	VistaDriverThreadAspect.cpp
	VistaDriverThreadAspect.h
	VistaDriverUtils.cpp
	VistaDriverUtils.h
	VistaDriverWorkspaceAspect.cpp
	VistaDriverWorkspaceAspect.h
	VistaKeyboardDriver.cpp
	VistaKeyboardDriver.h
	VistaMouseDriver.cpp
	VistaMouseDriver.h
	VistaShallowDriver.cpp
	VistaShallowDriver.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

