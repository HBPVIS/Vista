cmake_minimum_required(VERSION 2.6)
project(VISTADEMO_Text)

set(EXEC_NAME VistaTextDemo)

find_package(VISTA REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_glut_LIBRARY})

set(files
    main.cpp
    TextDemoAppl.cpp
    TextDemoAppl.h
)
source_group("Source Files" FILES ${files})

add_executable(${EXEC_NAME}
    ${files}
)

target_link_libraries(${EXEC_NAME}
    ${VistaAspects}
    ${VistaKernel}
    ${VistaMath}
)

if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (${CMAKE_CURRENT_BINARY_DIR}/start.sh "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9")
endif(UNIX)
