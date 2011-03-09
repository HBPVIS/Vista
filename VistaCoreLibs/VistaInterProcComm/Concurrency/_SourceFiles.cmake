set( RelativeDir "./Concurrency" )
set( RelativeSourceGroup "Source Files\\Concurrency" )
set( SubDirs Imp )

set( DirFiles
	VistaBarrier.cpp
	VistaBarrier.h
	VistaDefSemaphoreImp.cpp
	VistaDefSemaphoreImp.h
	VistaFork.cpp
	VistaFork.h
	VistaIpcThreadModel.h
	VistaMutex.cpp
	VistaMutex.h
	VistaPriority.cpp
	VistaPriority.h
	VistaProcess.cpp
	VistaProcess.h
	VistaReaderWriterLock.cpp
	VistaReaderWriterLock.h
	VistaSemaphore.cpp
	VistaSemaphore.h
	VistaThread.cpp
	VistaThread.h
	VistaThreadCondition.cpp
	VistaThreadCondition.h
	VistaThreadEvent.cpp
	VistaThreadEvent.h
	VistaThreadFunction.cpp
	VistaThreadFunction.h
	VistaThreadLoop.cpp
	VistaThreadLoop.h
	VistaThreadPool.cpp
	VistaThreadPool.h
	VistaThreadTask.cpp
	VistaThreadTask.h
	VistaTicker.cpp
	VistaTicker.h
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

