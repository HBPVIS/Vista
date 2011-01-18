set(relDir "Concurrency")
set(dirFiles_in
    CMakeLists.txt
    VistaBarrier.cpp
    VistaDefSemaphoreImp.cpp
    VistaFork.cpp
    VistaMutex.cpp
    VistaPriority.cpp
    VistaProcess.cpp
    VistaReaderWriterLock.cpp
    VistaSemaphore.cpp
    VistaThreadCondition.cpp
    VistaThread.cpp
    VistaThreadEvent.cpp
    VistaThreadFunction.cpp
    VistaThreadLoop.cpp
    VistaThreadPool.cpp
    VistaThreadTask.cpp
    VistaTicker.cpp
    VistaBarrier.h
    VistaDefSemaphoreImp.h
    VistaFork.h
    VistaIpcThreadModel.h
    VistaMutex.h
    VistaPriority.h
    VistaProcess.h
    VistaReaderWriterLock.h
    VistaSemaphore.h
    VistaThreadCondition.h
    VistaThreadEvent.h
    VistaThreadFunction.h
    VistaThread.h
    VistaThreadLoop.h
    VistaThreadPool.h
    VistaThreadTask.h
    VistaTicker.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Concurrency "${relDir}/${file}")
endforeach(file)

add_subdirectory(Imp)
set(dirFiles_Concurrency     ${dirFiles_Concurrency}     PARENT_SCOPE)
set(dirFiles_Concurrency_Imp ${dirFiles_Concurrency_Imp} PARENT_SCOPE)
