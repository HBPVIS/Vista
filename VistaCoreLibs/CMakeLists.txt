cmake_minimum_required( VERSION 2.6 )
project( VistaCoreLibs )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaLibCommon )

# Self include
include_directories( ${VistaCoreLibs_SOURCE_DIR} )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaLibCommon )

find_Package( VGLUT )
find_Package( VOpenSG )



# Let the user decide which libraries to build
# Default is to build all libraries
vista_conditional_add_subdirectory( VISTA_BUILD_BASE 				VistaBase				ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_BASE 				VistaAspects			ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_MATH 				VistaMath				ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_INTERPROCCOMM		VistaInterProcComm		ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_TOOLS				VistaTools				ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_DEVICEDRIVERSBASE	VistaDeviceDriversBase	ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_DATAFLOWNET			VistaDataFlowNet		ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_KERNEL				VistaKernel				ON ADVANCED )
vista_conditional_add_subdirectory( VISTA_BUILD_KERNELOPENSGEXT		VistaKernelOpenSGExt	ON ADVANCED )

add_subdirectory( VistaDeviceDrivers )

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
