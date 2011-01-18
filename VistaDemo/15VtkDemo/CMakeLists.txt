cmake_minimum_required(VERSION 2.6)
project(VISTADEMO_VTK)

set(EXEC_NAME VistaVtkDemo)

find_package(VISTA REQUIRED)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(files
    main.cpp
    VtkDemoAppl.cpp
    VtkDrawObject.cpp
    VtkDemoAppl.h
    VtkDrawObject.h
)
source_group("Source Files" FILES ${files})

add_executable(${EXEC_NAME}
    ${files}
)

target_link_libraries(${EXEC_NAME}
    ${VistaAspects}
    ${VistaKernel}
    vtkCommon
    vtkRendering
    vtkGraphics
    vtkFiltering
    vtksys
)

if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (${CMAKE_CURRENT_BINARY_DIR}/start.sh "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9")
endif(UNIX)
