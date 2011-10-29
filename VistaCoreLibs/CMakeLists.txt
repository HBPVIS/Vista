# $Id$

cmake_minimum_required( VERSION 2.8 )
project( VistaCoreLibs )

# For the CoreLibs, we verify that the VistaCMakeCommon really exist
if( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )
	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
elseif( EXISTS "${VistaCoreLibs_SOURCE_DIR}/VistaCMakeCommon" )
	list( APPEND CMAKE_MODULE_PATH "${VistaCoreLibs_SOURCE_DIR}/VistaCMakeCommon" )
else()
	find_file( VISTACORELIBS_CMAKE_COMMON_TEST_FILE "VistaCommon.cmake" 
				PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH}
				[NO_DEFAULT_PATH] )
	set( VISTACORELIBS_CMAKE_COMMON_TEST_FILE ${VISTACORELIBS_CMAKE_COMMON_TEST_FILE} CACHE INTERNAL "" FORCE )
	if( NOT VISTACORELIBS_CMAKE_COMMON_TEST_FILE )
		message( FATAL_ERROR 
			"ERROR: Folder VistaCMakeCommon not found, but is required for successfull build. "
			"Please make the VistaCMakeCommon folder available using one ofthe following methods:\n"
			"\t- define VISTA_CMAKE_COMMON environment var pointing to the folder\n"
			"\t- add folder to environment variables CMAKE_MODULE_PATH and CMAKE_PREFIX_PATH\n"
			"\t- set folder as subdirectory of VistaCoreLibs"
		)
	endif( NOT VISTACORELIBS_CMAKE_COMMON_TEST_FILE )
endif( EXISTS "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )
vista_enable_most_compiler_warnings()

vista_find_package( OpenSG REQUIRED )

# Self include
include_directories( ${VistaCoreLibs_SOURCE_DIR} )

# the version type can be either HEAD, RELASE, BRANCH, or TAG
vista_set_version( VistaCoreLibs BRANCH CLUSTERMODE_TEST svn_rev )

vista_find_package( OpenGL QUIET )
vista_find_package( GLUT QUIET )
vista_find_package( OpenSG 1.8 QUIET )

set( BUILD_KERNEL_DEFAULT OFF )

if( FIRST_CONFIGURE_RUN )
	if( NOT OPENGL_FOUND )
		message( "OpenGL not found - VistaKernel will be disabled by default" )
	elseif( NOT GLUT_FOUND )
		message( "Glut not found - VistaKernel will be disabled by default" )
	elseif( NOT OPENSG_FOUND )
		message( "OpenSG not found - VistaKernel will be disabled by default" )
	else()
		set( BUILD_KERNEL_DEFAULT ON )
	endif( NOT OPENGL_FOUND )
endif( FIRST_CONFIGURE_RUN )

set( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY OFF CACHE BOOL "If set to true, a <Package>Config.cmake file will be created and installed for each library, not just one for the CoreLibs" )

# Let the user decide which libraries to build
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_BASE 				VistaBase				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_ASPECTS				VistaAspects			ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_MATH 				VistaMath				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_INTERPROCCOMM		VistaInterProcComm		ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_TOOLS				VistaTools				ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_DEVICEDRIVERSBASE	VistaDeviceDriversBase	ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_DATAFLOWNET			VistaDataFlowNet		ON ADVANCED   ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_KERNEL				VistaKernel				${BUILD_KERNEL_DEFAULT}  ADD_TO_LIST VISTACORELIBS_COMPONENTS )
vista_conditional_add_subdirectory( VISTACORELIBS_BUILD_KERNELOPENSGEXT		VistaKernelOpenSGExt	${BUILD_KERNEL_DEFAULT}  ADD_TO_LIST VISTACORELIBS_COMPONENTS )

add_subdirectory( VistaDeviceDrivers )

set( VISTACORELIBS_BUILD_TESTS OFF CACHE BOOL "If set to true, tests to verify the correctness of some of the CoreLib's functions will be performed" )
if( VISTACORELIBS_BUILD_TESTS )
	enable_testing()
	add_subdirectory( Tests )
endif()


set( VISTACORELIBS_LIBRARY_OUTDIR "${CMAKE_BINARY_DIR}/lib" "${CMAKE_BINARY_DIR}/lib/DriverPlugins" )
set( VISTACORELIBS_LIBRARY_INSTALLDIR "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/lib/DriverPlugins" )
set( VISTACORELIBS_CONFIG_DEFINITIONS "${VISTA_PLATFORM_DEFINE}" )

vista_create_cmake_configs( VistaCoreLibs 
						"${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-build.cmake_proto"
						"${CMAKE_CURRENT_SOURCE_DIR}/VistaBuild/CMake/VistaCoreLibsConfig-install.cmake_proto" )
						
vista_create_doxygen_target( "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/Doxyfile" )
vista_create_uninstall_target()

# install license files
install( FILES "${CMAKE_CURRENT_SOURCE_DIR}/VistaBase/LICENSE" "${CMAKE_CURRENT_SOURCE_DIR}/VistaBase/LICENSE.LESSER" DESTINATION "share/VistaCoreLibs" )

