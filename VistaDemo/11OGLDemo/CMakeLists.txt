cmake_minimum_required(VERSION 2.6)
project(VISTADEMO_OGL)

set(EXEC_NAME VistaOGLDemo)

find_package(VISTA REQUIRED)

set(files
    main.cpp
    OglDemoAppl.cpp
    OglDrawObject.cpp
    OglDemoAppl.h
    OglDrawObject.h
)
source_group("Source Files" FILES ${files})

add_executable(${EXEC_NAME}
    ${files}
)

if(WIN32)
    set(EXTRALIBS
        opengl32
    )
endif(WIN32)

target_link_libraries(${EXEC_NAME}
    ${VistaKernel}
	${VistaAspects}
	${VistaMath}
	${EXTRALIBS}
)

if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (${CMAKE_CURRENT_BINARY_DIR}/start.sh "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9")
endif(UNIX)
