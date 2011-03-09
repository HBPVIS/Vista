set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	Vdfn3DNormalizeNode.cpp
	Vdfn3DNormalizeNode.h
	VdfnActionNode.cpp
	VdfnActionNode.h
	VdfnActionObject.cpp
	VdfnActionObject.h
	VdfnAggregateNode.cpp
	VdfnAggregateNode.h
	VdfnApplyTransformNode.cpp
	VdfnApplyTransformNode.h
	VdfnAxisRotateNode.cpp
	VdfnAxisRotateNode.h
	VdfnBinaryOpNode.cpp
	VdfnBinaryOpNode.h
	VdfnChangeDetectNode.cpp
	VdfnChangeDetectNode.h
	VdfnCompose3DVectorNode.cpp
	VdfnCompose3DVectorNode.h
	VdfnCompositeNode.cpp
	VdfnCompositeNode.h
	VdfnConditionalRouteNode.cpp
	VdfnConditionalRouteNode.h
	VdfnConfig.h
	VdfnConstantValueNode.cpp
	VdfnConstantValueNode.h
	VdfnCounterNode.cpp
	VdfnCounterNode.h
	VdfnDecompose3DVectorNode.cpp
	VdfnDecompose3DVectorNode.h
	VdfnDelayNode.cpp
	VdfnDelayNode.h
	VdfnDriverSensorNode.cpp
	VdfnDriverSensorNode.h
	VdfnDumpHistoryNode.cpp
	VdfnDumpHistoryNode.h
	VdfnForceFeedbackNode.cpp
	VdfnForceFeedbackNode.h
	VdfnGetTransformNode.cpp
	VdfnGetTransformNode.h
	VdfnGraph.cpp
	VdfnGraph.h
	VdfnHistoryPort.cpp
	VdfnHistoryPort.h
	VdfnHistoryProjectNode.cpp
	VdfnHistoryProjectNode.h
	VdfnInvertNode.cpp
	VdfnInvertNode.h
	VdfnLatestUpdateNode.cpp
	VdfnLatestUpdateNode.h
	VdfnLoggerNode.cpp
	VdfnLoggerNode.h
	VdfnMatrixComposeNode.cpp
	VdfnMatrixComposeNode.h
	VdfnModuloCounterNode.cpp
	VdfnModuloCounterNode.h
	VdfnNode.cpp
	VdfnNode.h
	VdfnNodeCreators.cpp
	VdfnNodeCreators.h
	VdfnNodeFactory.cpp
	VdfnNodeFactory.h
	VdfnObjectRegistry.cpp
	VdfnObjectRegistry.h
	VdfnOut.cpp
	VdfnOut.h
	VdfnPersistence.cpp
	VdfnPersistence.h
	VdfnPort.cpp
	VdfnPort.h
	VdfnPortFactory.cpp
	VdfnPortFactory.h
	VdfnProjectVectorNode.cpp
	VdfnProjectVectorNode.h
	VdfnReEvalNode.cpp
	VdfnReEvalNode.h
	VdfnReadWorkspaceNode.cpp
	VdfnReadWorkspaceNode.h
	VdfnSamplerNode.cpp
	VdfnSamplerNode.h
	VdfnSerializer.cpp
	VdfnSerializer.h
	VdfnSetTransformNode.cpp
	VdfnSetTransformNode.h
	VdfnShallowNode.cpp
	VdfnShallowNode.h
	VdfnThresholdNode.cpp
	VdfnThresholdNode.h
	VdfnTimerNode.cpp
	VdfnTimerNode.h
	VdfnTypeConvertNode.cpp
	VdfnTypeConvertNode.h
	VdfnUpdateThresholdNode.cpp
	VdfnUpdateThresholdNode.h
	VdfnUtil.cpp
	VdfnUtil.h
	VdfnValueToTriggerNode.cpp
	VdfnValueToTriggerNode.h
	VdfnVectorDecomposeNode.cpp
	VdfnVectorDecomposeNode.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

