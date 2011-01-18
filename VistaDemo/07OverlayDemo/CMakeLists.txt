cmake_minimum_required(VERSION 2.6)
project(VISTADEMO_Overlay)

find_package(VISTA REQUIRED)

set(EXEC_NAME VistaOverlayDemo)

if(WIN32)
    set(LIBRARIES
        ${VistaAspects}
        ${VistaMath}
        opengl32
    )
endif(WIN32)

set(files
    main.cpp
)
source_group("Source Files" FILES ${files})

add_executable(${EXEC_NAME}
    ${files}
)

target_link_libraries(${EXEC_NAME}
    ${VistaKernel}
    ${LIBRARIES}
)

if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (${CMAKE_CURRENT_BINARY_DIR}/start.sh "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9")
endif(UNIX)
