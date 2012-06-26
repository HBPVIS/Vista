# $Id$

# configure_vista_test( Name sourcefile ... )
macro( configure_vista_test TEST_NAME )
	add_executable( ${TEST_NAME} ${ARGN} )
	vista_set_outdir( ${TEST_NAME} ${CMAKE_CURRENT_BINARY_DIR} )
	vista_configure_app( ${TEST_NAME} DONT_COPY_EXECUTABLE )
	vista_create_default_info_file( ${TEST_NAME} )
	set_target_properties( ${TEST_NAME} PROPERTIES FOLDER "Tests" )
	
	add_test( NAME ${TEST_NAME}Execute 
		COMMAND ${TEST_NAME} 
		"--gtest_output=xml:${PROJECT_BINARY_DIR}/TestResults/${TEST_NAME}_results.xml" )
	
	# configure add_test with sufficient dll path envvar
	set( _DYNAMIC_LIB_DIRS ${VISTA_TARGET_LINK_DIRS} ${VISTA_ADDITIONAL_PATHENTRIES} )
	if( _DYNAMIC_LIB_DIRS )
		if( WIN32 )
			set( _ENVIRONMENT "PATH=${_DYNAMIC_LIB_DIRS};%PATH%" )
		else()
			set( _ENVIRONMENT "LD_LIBRARY_PATH=" )
			foreach (_ENTRY ${_DYNAMIC_LIB_DIRS} )
				set( _ENTRIES "${_ENTRIES}:${_ENTRY}" )		
			endforeach (_ENTRY)
			set( _ENVIRONMENT "${_ENVIRONMENT}$LD_LIBRARY_PATH${_ENTRIES}\n" )
		endif()
	endif()
	set_tests_properties( ${TEST_NAME}Execute PROPERTIES ENVIRONMENT "${_ENVIRONMENT}" )
	
	# put all source files into the same folder
	source_group( "Source Files" FILES ${ARGN} )
endmacro()


vista_use_package( GTest REQUIRED )

vista_add_pathscript_dynamic_lib_path( "${CMAKE_BINARY_DIR}/lib" )
vista_add_pathscript_dynamic_lib_path( "${CMAKE_BINARY_DIR}/lib/DriverPlugins" )



if( VISTACORELIBS_BUILD_ASPECTS )

	configure_vista_test( VistaReflectionableTest VistaReflectionableTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaReflectionableTest
		VistaBase
		VistaAspects
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	
	configure_vista_test( VistaConversionTest VistaConversionTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaConversionTest
		VistaBase
		VistaAspects
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
endif()	

if( VISTACORELIBS_BUILD_KERNEL )
	configure_vista_test( VistaSerializablesTest VistaSerializablesTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaSerializablesTest
		VistaBase
		VistaAspects
		VistaInterProcComm
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	
endif()
