if(WIN32)
    add_definitions(-DVISTADFN_EXPORTS)
endif(WIN32)

set(dirFiles
    Vdfn3DNormalizeNode.cpp
    VdfnActionNode.cpp
    VdfnActionObject.cpp
    VdfnAggregateNode.cpp
    VdfnApplyTransformNode.cpp
    VdfnAxisRotateNode.cpp
    VdfnBinaryOpNode.cpp
    VdfnChangeDetectNode.cpp
    VdfnCompose3DVectorNode.cpp
    VdfnMatrixComposeNode.cpp
    VdfnCompositeNode.cpp
    VdfnConditionalRouteNode.cpp
    VdfnConstantValueNode.cpp
    VdfnCounterNode.cpp
    VdfnDecompose3DVectorNode.cpp
    VdfnDelayNode.cpp
    VdfnDriverSensorNode.cpp
    VdfnDumpHistoryNode.cpp
    VdfnForceFeedbackNode.cpp
    VdfnGetTransformNode.cpp
    VdfnGraph.cpp
    VdfnHistoryPort.cpp
    VdfnHistoryProjectNode.cpp
    VdfnInvertNode.cpp
    VdfnLatestUpdateNode.cpp
    VdfnLoggerNode.cpp
    VdfnModuloCounterNode.cpp
    VdfnNode.cpp
    VdfnNodeCreators.cpp
    VdfnNodeFactory.cpp
    VdfnObjectRegistry.cpp
    VdfnOut.cpp
    VdfnPersistence.cpp
    VdfnPort.cpp
    VdfnPortFactory.cpp
    VdfnProjectVectorNode.cpp
    VdfnReadWorkspaceNode.cpp
    VdfnReEvalNode.cpp
    VdfnSamplerNode.cpp
    VdfnSerializer.cpp
    VdfnSetTransformNode.cpp
    VdfnShallowNode.cpp
    VdfnThresholdNode.cpp
    VdfnTimerNode.cpp
    VdfnTypeConvertNode.cpp
    VdfnUpdateThresholdNode.cpp
    VdfnUtil.cpp
    VdfnValueToTriggerNode.cpp
    VdfnVectorDecomposeNode.cpp
    Vdfn3DNormalizeNode.h
    VdfnActionNode.h
    VdfnActionObject.h
    VdfnAggregateNode.h
    VdfnApplyTransformNode.h
    VdfnAxisRotateNode.h
    VdfnBinaryOpNode.h
    VdfnChangeDetectNode.h
    VdfnCompose3DVectorNode.h
    VdfnMatrixComposeNode.h
    VdfnCompositeNode.h
    VdfnConditionalRouteNode.h
    VdfnConfig.h
    VdfnConstantValueNode.h
    VdfnCounterNode.h
    VdfnDecompose3DVectorNode.h
    VdfnDelayNode.h
    VdfnDriverSensorNode.h
    VdfnDumpHistoryNode.h
    VdfnForceFeedbackNode.h
    VdfnGetTransformNode.h
    VdfnGraph.h
    VdfnHistoryPort.h
    VdfnHistoryProjectNode.h
    VdfnInvertNode.h
    VdfnLatestUpdateNode.h
    VdfnLoggerNode.h
    VdfnModuloCounterNode.h
    VdfnNodeCreators.h
    VdfnNodeFactory.h
    VdfnNode.h
    VdfnObjectRegistry.h
    VdfnOut.h
    VdfnPersistence.h
    VdfnPortFactory.h
    VdfnPort.h
    VdfnProjectVectorNode.h
    VdfnReadWorkspaceNode.h
    VdfnReEvalNode.h
    VdfnSamplerNode.h
    VdfnSerializer.h
    VdfnSetTransformNode.h
    VdfnShallowNode.h
    VdfnThresholdNode.h
    VdfnTimerNode.h
    VdfnTypeConvertNode.h
    VdfnUpdateThresholdNode.h
    VdfnUtil.h
    VdfnValueToTriggerNode.h
    VdfnVectorDecomposeNode.h
)
source_group("Source Files" FILES ${dirFiles})

add_library(VistaDataFlowNet
    ${dirFiles}
) 

target_link_libraries(VistaDataFlowNet
	VistaBase
	VistaAspects
	VistaTools
	VistaMath
	VistaInterProcComm
    VistaDeviceDriversBase    
)

install(TARGETS VistaDataFlowNet
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaDataFlowNet
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
