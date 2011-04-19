cmake_minimum_required( VERSION 2.8 )
project( VistaCoreLibs )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaCommon )

# Self include
include_directories( ${VistaCoreLibs_SOURCE_DIR} )

# the version type can be either HEAD, RELASE, BRANCH, or TAG
vista_set_version( VistaCoreLibs RELEASE HEAD 1 10 1 svn_rev )

set( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY OFF CACHE BOOL "If set to true, a <Package>Config.cmake file will be created and installed for each library, not just one for the CoreLibs" )
mark_as_advanced( VISTA_CREATE_CMAKE_CONFIGS_FOR_LIBRARY )

# Let the user decide which libraries to build
# Default is to build all libraries
vista_conditional_add_subdirectory( VISTA_BUILD_BASE 				VistaBase				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_ASPECTS				VistaAspects			ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_MATH 				VistaMath				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_INTERPROCCOMM		VistaInterProcComm		ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_TOOLS				VistaTools				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_DEVICEDRIVERSBASE	VistaDeviceDriversBase	ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_DATAFLOWNET			VistaDataFlowNet		ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_KERNEL				VistaKernel				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_KERNELOPENSGEXT		VistaKernelOpenSGExt	ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )

add_subdirectory( VistaDeviceDrivers )

# Step 2: Configure the Package Config.cmake files
set( VISTACORELIBS_LIBRARY_OUTDIR "${CMAKE_BINARY_DIR}/lib" )
vista_create_cmake_configs( VistaCoreLibs VistaBuild/CMake/VistaCoreLibsConfig.cmake_proto )

