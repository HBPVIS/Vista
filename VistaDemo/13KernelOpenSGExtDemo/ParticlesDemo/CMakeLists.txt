cmake_minimum_required(VERSION 2.6)
project(VISTADEMO_Particles)

set(EXEC_NAME VistaParticlesDemo)

find_package(VISTA REQUIRED)

set(files
    VistaParticlesDemo.cpp
    RollingBalls.h
    Smoke.h
)
source_group("Source Files" FILES ${files})

add_executable(${EXEC_NAME}
    ${files}
)

target_link_libraries(${EXEC_NAME}
    ${VistaAspects}
    ${VistaKernel}
    ${VistaBase}
    ${VistaMath}
    ${VistaKernelOpenSGExt}
)

if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (${CMAKE_CURRENT_BINARY_DIR}/start.sh "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9")
endif(UNIX)
