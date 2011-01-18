set(relDir "Concurrency/Imp")
set(dirFiles_in
    CMakeLists.txt
    VistaMutexImp.cpp
    VistaPosixProcessImp.cpp
    VistaPosixSemaphoreImp.cpp
    VistaPosixThreadEventImp.cpp
    VistaPriorityImp.cpp
    VistaProcessImp.cpp
    VistaPthreadReaderWriterLockImp.cpp
    VistaPthreadsMutexImp.cpp
    VistaPthreadsPriorityImp.cpp
    VistaPthreadsThreadConditionImp.cpp
    VistaPthreadsThreadImp.cpp
    VistaPthreadThreadEventImp.cpp
    VistaReaderWriterLockImp.cpp
    VistaSemaphoreImp.cpp
    VistaSPROCMutexImp.cpp
    VistaSPROCPriorityImp.cpp
    VistaSPROCThreadImp.cpp
    VistaThreadConditionImp.cpp
    VistaThreadEventImp.cpp
    VistaThreadImp.cpp
    VistaWin32MutexImp.cpp
    VistaWin32PriorityImp.cpp
    VistaWin32ProcessImp.cpp
    VistaWin32SemaphoreImp.cpp
    VistaWin32ThreadConditionImp.cpp
    VistaWin32ThreadEventImp.cpp
    VistaWin32ThreadImp.cpp
    VistaMutexImp.h
    VistaPosixProcessImp.h
    VistaPosixSemaphoreImp.h
    VistaPosixThreadEventImp.h
    VistaPriorityImp.h
    VistaProcessImp.h
    VistaPthreadReaderWriterLockImp.h
    VistaPthreadsMutexImp.h
    VistaPthreadsPriorityImp.h
    VistaPthreadsThreadConditionImp.h
    VistaPthreadsThreadImp.h
    VistaPthreadThreadEventImp.h
    VistaReaderWriterLockImp.h
    VistaSemaphoreImp.h
    VistaSPROCMutexImp.h
    VistaSPROCPriorityImp.h
    VistaSPROCThreadImp.h
    VistaThreadConditionImp.h
    VistaThreadEventImp.h
    VistaThreadImp.h
    VistaWin32MutexImp.h
    VistaWin32PriorityImp.h
    VistaWin32ProcessImp.h
    VistaWin32SemaphoreImp.h
    VistaWin32ThreadConditionImp.h
    VistaWin32ThreadEventImp.h
    VistaWin32ThreadImp.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Concurrency_Imp "${relDir}/${file}")
endforeach(file)

set(dirFiles_Concurrency_Imp ${dirFiles_Concurrency_Imp} PARENT_SCOPE)
