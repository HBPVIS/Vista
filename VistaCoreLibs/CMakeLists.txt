cmake_minimum_required(VERSION 2.6)
project(VISTA)

# Search for precompiled OpenSG path
set(OPENSG_ROOT $ENV{OPENSG_ROOT} CACHE PATH "Base directory for OpenSG")
message(STATUS "Found OpenSG in ${OPENSG_ROOT}")

# Search for GLUT
find_package(GLUT REQUIRED)

# Self include
include_directories(${VISTA_SOURCE_DIR})

# Definitions needed for OpenSG
add_definitions(-DOSG_WITH_GLUT)
add_definitions(-DVISTA_SYS_OPENSG)
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" )




##############
## Start to commons
#############

# Use a postfix for debug libraries
set(CMAKE_DEBUG_POSTFIX "D")

# Platform dependent definitions
if( UNIX )
    add_definitions(-DLINUX)
elseif( WIN32 )
    add_definitions(-DWIN32)
    if(MSVC)
		# msvc disable some warnings
        add_definitions(/D_CRT_SECURE_NO_WARNINGS /wd4251 /wd4275 /wd4503)
		#Enable string pooling
		add_definitions(-GF)
		# Parallel build for Visual Studio?
		set(VISTA_USE_PARALLEL_BUILD ON CACHE BOOL "Add /MP flag for parallel build on Visual Studio")
		if(VISTA_USE_PARALLEL_BUILD)
            add_definitions(/MP)
        else()
            remove_definitions(/MP)
        endif(VISTA_USE_PARALLEL_BUILD)
		# Check for sse optimization
		set(VISTA_USE_SSE_OPTIMIZATION ON CACHE BOOL "Use automatic SSE2 optimizations")
		if(VISTA_USE_SSE_OPTIMIZATION)
            add_definitions(/arch:SSE2)
        else()
            remove_definitions(/arch:SSE2)
        endif(VISTA_USE_SSE_OPTIMIZATION)
    endif(MSVC)
endif( UNIX )

# Should we use rpath?
# This enables us to use OpenSG etc. within the Vista* libraries without having
# to set a LIBRARY_PATH while linking against these libraries
set(VISTA_USE_RPATH ON CACHE BOOL "Use rpath")
mark_as_advanced(VISTA_USE_RPATH)

##############
## End to commons
#############



# Let the user decide whether to build static or shared libs
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries if ON, static libraries if OFF")

# Let the user decide which libraries to build
# Default is to build all libraries
set(VISTA_BUILD_BASE              ON CACHE BOOL "Build the base library")
set(VISTA_BUILD_MATH              ON CACHE BOOL "Build the math library")
set(VISTA_BUILD_ASPECTS           ON CACHE BOOL "Build the aspects library")
set(VISTA_BUILD_INTERPROCCOMM     ON CACHE BOOL "Build the interprocess communication library")
set(VISTA_BUILD_TOOLS             ON CACHE BOOL "Build the tools library")
set(VISTA_BUILD_DEVICEDRIVERSBASE ON CACHE BOOL "Build the device drivers library")
set(VISTA_BUILD_DATAFLOWNET       ON CACHE BOOL "Build the data flow net library")
set(VISTA_BUILD_KERNEL            ON CACHE BOOL "Build the kernel")
set(VISTA_BUILD_KERNELOPENSGEXT   ON CACHE BOOL "Build the OpenSG extension for the kernel")

# Make the settings advanced
mark_as_advanced(VISTA_BUILD_BASE)
mark_as_advanced(VISTA_BUILD_MATH)
mark_as_advanced(VISTA_BUILD_ASPECTS)
mark_as_advanced(VISTA_BUILD_INTERPROCCOMM)
mark_as_advanced(VISTA_BUILD_TOOLS)
mark_as_advanced(VISTA_BUILD_DEVICEDRIVERSBASE)
mark_as_advanced(VISTA_BUILD_DATAFLOWNET)
mark_as_advanced(VISTA_BUILD_KERNEL)
mark_as_advanced(VISTA_BUILD_KERNELOPENSGEXT)

# Let the user decide which extra modules to install
set(VISTA_BUILD_3DCSPACENAVIGATORDRIVER OFF CACHE BOOL "Build the 3dc spacenavigator driver")
set(VISTA_BUILD_PHANTOMDRIVER OFF CACHE BOOL "Build the Phantom driver")
#WIN32-only drivers
if( WIN32 )
	set(VISTA_BUILD_DIRECTXGAMEPADDRIVER OFF CACHE BOOL "Build the DirectX Gamepad driver")
endif( WIN32 )
#LINUX-only driver
if( UNIX )
	set(VISTA_BUILD_JSWJOYSTICKDRIVER OFF CACHE BOOL "Build the JSW Joystick driver")
endif( UNIX )



# Build libraries
if(VISTA_BUILD_BASE)
    add_subdirectory(VistaBase)
endif(VISTA_BUILD_BASE)
if(VISTA_BUILD_MATH)
    add_subdirectory(VistaMath)
endif(VISTA_BUILD_MATH)
if(VISTA_BUILD_ASPECTS)
    add_subdirectory(VistaAspects)
endif(VISTA_BUILD_ASPECTS)
if(VISTA_BUILD_INTERPROCCOMM)
    add_subdirectory(VistaInterProcComm)
endif(VISTA_BUILD_INTERPROCCOMM)
if(VISTA_BUILD_TOOLS)
    add_subdirectory(VistaTools)
endif(VISTA_BUILD_TOOLS)
if(VISTA_BUILD_DEVICEDRIVERSBASE)
    add_subdirectory(VistaDeviceDriversBase)
endif(VISTA_BUILD_DEVICEDRIVERSBASE)
if(VISTA_BUILD_DATAFLOWNET)
    add_subdirectory(VistaDataFlowNet)
endif(VISTA_BUILD_DATAFLOWNET)
if(VISTA_BUILD_KERNEL)
    add_subdirectory(VistaKernel)
endif(VISTA_BUILD_KERNEL)
if(VISTA_BUILD_KERNELOPENSGEXT)
    add_subdirectory(VistaKernelOpenSGExt)
endif(VISTA_BUILD_KERNELOPENSGEXT)

# Build extra driver libraries
if(VISTA_BUILD_3DCSPACENAVIGATORDRIVER)
    add_subdirectory(VistaDeviceDrivers/3DCSpaceNavigator)
endif(VISTA_BUILD_3DCSPACENAVIGATORDRIVER)
if(VISTA_BUILD_PHANTOMDRIVER)
    add_subdirectory(VistaDeviceDrivers/PhantomDriver)
endif(VISTA_BUILD_PHANTOMDRIVER)
if(VISTA_BUILD_DIRECTXGAMEPADDRIVER)
    add_subdirectory(VistaDeviceDrivers/DirectXGamepad)
endif(VISTA_BUILD_DIRECTXGAMEPADDRIVER)
if(VISTA_BUILD_JSWJOYSTICKDRIVER)
    add_subdirectory(VistaDeviceDrivers/JswJoystick)
endif(VISTA_BUILD_JSWJOYSTICKDRIVER)


# Create and install the config file for this package
# Step 1: Set necessary variables
set(OPENSG_INC_DIR "${OPENSG_ROOT}/include")
if(UNIX)
    set(OPENSG_LIB_DIR "${OPENSG_ROOT}/lib/opt")
elseif(WIN32)
    set(OPENSG_LIB_DIR "${OPENSG_ROOT}/lib")
endif(UNIX)
# Step 2: Configure the file. Words written in @@ are replaced by corresponding
#         variables. I.e. @VISTA_INSTALL_INC_DIR@ is replaced by the content
#         of the variable VISTA_INSTALL_INC_DIR
configure_file(
    VistaBuild/CMake/VISTAConfig.cmake_proto
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/VISTAConfig.cmake
    @ONLY
)
# Eventually install the file
if(UNIX)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/cmake/VISTAConfig.cmake
		DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/VISTA"
	)
elseif(WIN32)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/VISTAConfig.cmake
        DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
    )
endif(UNIX)

# Experimental uninstall tag. Use with care!
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
