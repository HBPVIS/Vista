set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs tinyXML )

set( DirFiles
	CommTimer.cpp
	CommTimer.h
	EncodeBinary.h
	VistaBasicProfiler.cpp
	VistaBasicProfiler.h
	VistaCPUInfo.cpp
	VistaCPUInfo.h
	VistaCRC32.cpp
	VistaCRC32.h
	VistaColoredConsoleMessage.cpp
	VistaColoredConsoleMessage.h
	VistaDLL.cpp
	VistaDLL.h
	VistaEnvironment.cpp
	VistaEnvironment.h
	VistaFastMeshAdaptor.cpp
	VistaFastMeshAdaptor.h
	VistaFileDataSet.cpp
	VistaFileDataSet.h
	VistaFileSystemDirectory.cpp
	VistaFileSystemDirectory.h
	VistaFileSystemFile.cpp
	VistaFileSystemFile.h
	VistaFileSystemNode.cpp
	VistaFileSystemNode.h
	VistaHalfedgeAdaptor.cpp
	VistaHalfedgeAdaptor.h
	VistaMemoryInfo.cpp
	VistaMemoryInfo.h
	VistaOctree.cpp
	VistaOctree.h
	VistaProfiler.cpp
	VistaProfiler.h
	VistaProgressBar.cpp
	VistaProgressBar.h
	VistaProtocol.cpp
	VistaProtocol.h
	VistaRandomNumberGenerator.cpp
	VistaRandomNumberGenerator.h
	VistaRingBuffer.h
	VistaStackWalker.cpp
	VistaStackWalker.h
	VistaTimer.cpp
	VistaTimer.h
	VistaToolsConfig.h
	VistaToolsOut.cpp
	VistaToolsOut.h
	VistaToolsStd.h
	VistaTopologyGraph.h
	VistaVoxelOctree.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

