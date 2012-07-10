# $Id$

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
	VdfnBinaryOps.h
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
	VdfnGetElementNode.cpp
	VdfnGetElementNode.h
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
	VdfnPersistence.cpp
	VdfnPersistence.h
	VdfnPort.cpp
	VdfnPort.h
	VdfnPortFactory.cpp
	VdfnPortFactory.h
	VdfnProjectVectorNode.cpp
	VdfnProjectVectorNode.h
	VdfnQuaternionSlerpNode.cpp
	VdfnQuaternionSlerpNode.h
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
	VdfnToggleNode.cpp
	VdfnToggleNode.h
	VdfnUpdateThresholdNode.cpp
	VdfnUpdateThresholdNode.h
	VdfnUtil.cpp
	VdfnUtil.h
	VdfnValueToTriggerNode.cpp
	VdfnValueToTriggerNode.h
	VdfnVectorDecomposeNode.cpp
	VdfnVectorDecomposeNode.h
	VdfnNegateNode.cpp
	VdfnNegateNode.h
	VdfnRangeCheckNode.cpp
	VdfnRangeCheckNode.h
	VdfnMultiplexNode.cpp
	VdfnMultiplexNode.h
	VdfnDemultiplexNode.cpp
	VdfnDemultiplexNode.h
	VdfnVariableNode.cpp
	VdfnVariableNode.h
	
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

