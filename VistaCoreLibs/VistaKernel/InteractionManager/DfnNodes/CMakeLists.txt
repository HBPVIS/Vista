set(relDir "InteractionManager/DfnNodes")
set(dirFiles_in
	CMakeLists.txt
	VistaDfn3DMouseTransformNode.cpp
	VistaDfnClusterNodeInfoNode.cpp
    VistaDfnDeviceDebugNode.cpp
    VistaDfnDumpHistoryNodeClusterCreate.cpp
    VistaDfnEventSourceNode.cpp
	VistaDfnMouseWheelChangeDetectNode.cpp
    VistaDfnNavigationNode.cpp
	VistaDfnPortChangeHandlerBase.cpp
    VistaDfnProjectionSourceNode.cpp
	VistaDfnReferenceFrameTransformNode.cpp
    VistaDfnSensorFrameNode.cpp
    VistaDfnSystemTriggerControlNode.cpp
    VistaDfnTextOverlayNode.cpp
    VistaDfnTrackballNode.cpp
    VistaDfnViewerSinkNode.cpp
    VistaDfnViewerSourceNode.cpp
	VistaDfnViewportSourceNode.cpp
    VistaDfnWindowSourceNode.cpp
	VistaKernelDfnNodeCreators.cpp
	VistaDfn3DMouseTransformNode.h
	VistaDfnClusterNodeInfoNode.h
    VistaDfnDeviceDebugNode.h
    VistaDfnDumpHistoryNodeClusterCreate.h
    VistaDfnEventSourceNode.h
	VistaDfnMouseWheelChangeDetectNode.h
    VistaDfnNavigationNode.h
	VistaDfnPortChangeHandlerBase.h
    VistaDfnProjectionSourceNode.h
	VistaDfnReferenceFrameTransformNode.h
    VistaDfnSensorFrameNode.h
    VistaDfnSystemTriggerControlNode.h
    VistaDfnTextOverlayNode.h
    VistaDfnTrackballNode.h
    VistaDfnViewerSinkNode.h
    VistaDfnViewerSourceNode.h
	VistaDfnViewportSourceNode.h
    VistaDfnWindowSourceNode.h
	VistaKernelDfnNodeCreators.h
	
   
    
   
   
)


foreach(file ${dirFiles_in})
    list(APPEND dirFiles_InteractionManager_DfnNodes "${relDir}/${file}")
endforeach(file)

set(dirFiles_InteractionManager_DfnNodes ${dirFiles_InteractionManager_DfnNodes} PARENT_SCOPE)
