set(relDir "AsyncIO")
set(dirFiles_in
    CMakeLists.txt
    VistaAsyncEventObserver.cpp
    VistaIOHandleBasedMultiplexer.cpp
    VistaIOMultiplexer.cpp
    VistaIOMultiplexerIP.cpp
    VistaIORequest.cpp
    VistaIOScheduler.cpp
    VistaAsyncEventObserver.h
    VistaIOHandleBasedMultiplexer.h
    VistaIOMultiplexer.h
    VistaIOMultiplexerIP.h
    VistaIORequest.h
    VistaIOScheduler.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_AsyncIO "${relDir}/${file}")
endforeach(file)

set(dirFiles_AsyncIO ${dirFiles_AsyncIO} PARENT_SCOPE)
