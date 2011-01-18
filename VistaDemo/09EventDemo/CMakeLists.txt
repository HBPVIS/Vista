cmake_minimum_required(VERSION 2.6)
project(VISTADEMO_Event)

set(EXEC_NAME VistaEventDemo)

find_package(VISTA REQUIRED)

set(files
    DemoEvent.cpp
    DemoHandler.cpp
    EventDemoAppl.cpp
    GreedyObserver.cpp
    main.cpp
    ObserverRegisterCallback.cpp
    ProcessCallback.cpp
    TimeObserver.cpp
    DemoEvent.h
    DemoHandler
    EventDemoAppl.h
    GreedyObserver.h
    ObserverRegisterCallback.h
    ProcessCallback.h
    TimeObserver.h
)
source_group("Source Files" FILES ${files})

add_executable(${EXEC_NAME}
    ${files}
)

target_link_libraries(${EXEC_NAME}
    ${VistaKernel}
	${VistaAspects}
)

if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (${CMAKE_CURRENT_BINARY_DIR}/start.sh "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9")
endif(UNIX)
