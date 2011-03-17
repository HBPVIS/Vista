cmake_minimum_required( VERSION 2.6 )
project( VistaCoreLibs )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaLibCommon )

# Self include
include_directories( ${VistaCoreLibs_SOURCE_DIR} )

find_Package( VGLUT )
find_Package( VOpenSG )

# the version type can be either HEAD, RELASE, BRANCH, or TAG
set( VISTACORELIBS_VERSION_TYPE	RELEASE )
# the version head is a string representation of the CoreLibs version, and should be 
# either HEAD, the name of the RELASE, or the BRANCH/TAG name
# The version
set( VISTACORELIBS_VERSION_NAME	HEAD )	
# the version numbers should always represent the last RELASE 
# For the HEAD, these should be the same as for the last release
# For BRANCHes and TAGs, it is not necessary to specify them at all, but they can still be used
set( VISTACORELIBS_VERSION_MAJOR 	1 )
set( VISTACORELIBS_VERSION_MINOR 	10 )
set( VISTACORELIBS_VERSION_REVISION	1 )


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
vista_install_package_config( VistaCoreLibs VistaBuild/CMake/VistaCoreLibsConfig.cmake_proto )

# use a generic version file configure/install macro
vista_install_package_version( VistaCoreLibs )

# Experimental uninstall tag. Use with care!
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
