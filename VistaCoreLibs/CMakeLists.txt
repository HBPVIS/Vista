# $Id$

cmake_minimum_required( VERSION 2.8 )
project( VistaCoreLibs )

# For the CoreLibs, we verify that the VistaCMakeCommon really exist
if( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )
	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
elseif( EXISTS "${VistaCoreLibs_SOURCE_DIR}/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${VistaCoreLibs_SOURCE_DIR}/VistaCMakeCommon" )
else()
	find_file( VISTA_CMAKE_COMMON_TEST_FILE "VistaCommon.cmake" 
				PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH}
				[NO_DEFAULT_PATH] )
	set( VISTA_CMAKE_COMMON_TEST_FILE ${VISTA_CMAKE_COMMON_TEST_FILE} CACHE INTERNAL "" FORCE )
	if( NOT VISTA_CMAKE_COMMON_TEST_FILE )
		message( FATAL_ERROR 
			"ERROR: Folder VistaCMakeCommon not found, but is required for successfull build. "
			"Please make the VistaCMakeCommon folder available using one ofthe following methods:\n"
			"\t- define VISTA_CMAKE_COMMON environment var pointing to the folder\n"
			"\t- add folder to environment variables CMAKE_MODULE_PATH and CMAKE_PREFIX_PATH\n"
			"\t- set folder as subdirectory of VistaCoreLibs"
		)
	endif( NOT VISTA_CMAKE_COMMON_TEST_FILE )
endif( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

# Self include
include_directories( ${VistaCoreLibs_SOURCE_DIR} )

# the version type can be either HEAD, RELASE, BRANCH, or TAG
vista_set_version( VistaCoreLibs HEAD HEAD 1 10 1 svn_rev )

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
vista_conditional_add_subdirectory( VISTA_BUILD_KERNEL				VistaKernel				ON            ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTA_BUILD_KERNELOPENSGEXT		VistaKernelOpenSGExt	ON            ADD_TO_LIST VISTACORELIBS_COMPONENTS )

add_subdirectory( VistaDeviceDrivers )

set( VISTACORELIBS_LIBRARY_OUTDIR "${CMAKE_BINARY_DIR}/lib" "${CMAKE_BINARY_DIR}/lib/DriverPlugins" )
vista_create_cmake_configs( VistaCoreLibs 
						"${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-build.cmake_proto"
						"${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-install.cmake_proto" )

