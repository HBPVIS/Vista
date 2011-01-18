cmake_minimum_required(VERSION 2.6)
# Project name
project(VISTADEMO_ChangeMe)

# Name of your executable
set(EXEC_NAME VistaChangeMeDemo)

# Import the ViSTA package file, which
# imports all necessary ViSTA stuff.
# If the file is not found immediately,
# you can set the variable VISTA_DIR to
# the path with the file VISTAConfig.cmake.
find_package(VISTA REQUIRED)

# Define the files needed in your executable.
# This is done with a listvariable so that we
# can change some properties afterwards.
set(files
    main.cpp
    MyDemoAppl.cpp
    MyDemoAppl.h
)

# Group the source files defined above in
# just one single group called "Source Files".
# Without setting this  parameter, we would
# get one group for .cpp files and one for
# .h files.
source_group("Source Files" FILES ${files})

# Define your executable.
# The first parameter sets the name,
# the remaining parameters define which files
# to include in the build. Here, we use the
# list variable defined before.
# Note that, for IDE projects, we also include
# header files. This is not necessary for
# makefiles.
add_executable(${EXEC_NAME}
    ${files}
)

# Link the executable against external libraries.
# The first parameter is the executable, the others
# define which libraries are needed.
# Note that for ViSTA libraries, we use variables, which
# are defined in the ViSTA package file included above,
# so you don't have to care if you are building a debug
# or a release version.
target_link_libraries(${EXEC_NAME}
    ${VistaAspects}
    ${VistaKernel}
    ${VistaMath}
)

# If running on linux, the following commands will create
# a shell script start.sh, which enables you to start your
# application without having to set environment veriables
# like library search paths.
# The script will be put in your build directory.
# The command CREATE_SHELLSCRIPT needs the following
# two parameters:
# (1) The filename of the script
# (2) The code to be executed. In this case we change
#     to the source directory and then start the application.
if(UNIX)
    include(${VISTA_DIR}/VISTAShellScripts.cmake)
    CREATE_SHELLSCRIPT (
        ${CMAKE_CURRENT_BINARY_DIR}/start.sh
        "cd ${CMAKE_CURRENT_SOURCE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} $1 $2 $3 $4 $5 $6 $7 $8 $9"
    )
endif(UNIX)
