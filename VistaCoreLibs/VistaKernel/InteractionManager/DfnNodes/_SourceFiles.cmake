set( RelativeDir "./InteractionManager/DfnNodes" )
set( LocalSourceGroup "Source Files\\InteractionManager\\DfnNodes" )

set( DirFiles
	VistaDfn3DMouseTransformNode.cpp
	VistaDfn3DMouseTransformNode.h
	VistaDfnClusterNodeInfoNode.cpp
	VistaDfnClusterNodeInfoNode.h
	VistaDfnDeviceDebugNode.cpp
	VistaDfnDeviceDebugNode.h
	VistaDfnDumpHistoryNodeClusterCreate.cpp
	VistaDfnDumpHistoryNodeClusterCreate.h
	VistaDfnEventSourceNode.cpp
	VistaDfnEventSourceNode.h
	VistaDfnMouseWheelChangeDetectNode.cpp
	VistaDfnMouseWheelChangeDetectNode.h
	VistaDfnNavigationNode.cpp
	VistaDfnNavigationNode.h
	VistaDfnPortChangeHandlerBase.cpp
	VistaDfnPortChangeHandlerBase.h
	VistaDfnProjectionSourceNode.cpp
	VistaDfnProjectionSourceNode.h
	VistaDfnReferenceFrameTransformNode.cpp
	VistaDfnReferenceFrameTransformNode.h
	VistaDfnSensorFrameNode.cpp
	VistaDfnSensorFrameNode.h
	VistaDfnSystemTriggerControlNode.cpp
	VistaDfnSystemTriggerControlNode.h
	VistaDfnTextOverlayNode.cpp
	VistaDfnTextOverlayNode.h
	VistaDfnTrackballNode.cpp
	VistaDfnTrackballNode.h
	VistaDfnViewerSinkNode.cpp
	VistaDfnViewerSinkNode.h
	VistaDfnViewerSourceNode.cpp
	VistaDfnViewerSourceNode.h
	VistaDfnViewportSourceNode.cpp
	VistaDfnViewportSourceNode.h
	VistaDfnWindowSourceNode.cpp
	VistaDfnWindowSourceNode.h
	VistaKernelDfnNodeCreators.cpp
	VistaKernelDfnNodeCreators.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

