# $Id$

require_vistacommon_version( 35191 )
include( VistaTestingUtils )

vista_use_package( GTest REQUIRED )

vista_add_pathscript_dynamic_lib_path( "${CMAKE_BINARY_DIR}/lib" )
vista_add_pathscript_dynamic_lib_path( "${CMAKE_BINARY_DIR}/lib/DriverPlugins" )


if( VISTACORELIBS_BUILD_BASE )

	vista_configure_test( VistaVectorMathTest VistaVectorMathTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaVectorMathTest
		VistaBase
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	add_dependencies( VistaVectorMathTest VistaBase )
endif()

if( VISTACORELIBS_BUILD_ASPECTS )

	vista_configure_test( VistaReflectionableTest VistaReflectionableTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaReflectionableTest
		VistaBase
		VistaAspects
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	add_dependencies( VistaReflectionableTest VistaAspects )
	
	vista_configure_test( VistaConversionTest VistaConversionTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaConversionTest
		VistaBase
		VistaAspects
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	add_dependencies( VistaReflectionableTest VistaAspects )
endif()	

if( VISTACORELIBS_BUILD_KERNEL )
	vista_configure_test( VistaSerializablesTest VistaSerializablesTest.cpp VistaTestingUtils.h )
	target_link_libraries( VistaSerializablesTest
		VistaBase
		VistaAspects
		VistaInterProcComm
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	add_dependencies( VistaReflectionableTest VistaInterProcComm )
	
endif()


