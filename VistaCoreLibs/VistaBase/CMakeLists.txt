# $Id: CMakeLists.txt 21407 2011-05-20 13:36:52Z dr165799 $

vista_adopt_version( VistaBase VistaCoreLibs )

if( UNIX AND NOT APPLE )
	set( LIBRARIES rt )
endif( UNIX AND NOT APPLE )

include( "./_SourceFiles.cmake" )

if( WIN32 )
	set( 64BIT_ATOMICS_FLAG TRUE )
	if( "${CMAKE_SYSTEM_VERSION}" VERSION_LESS 5.2 )
		# WIN XP and below don't support InterlockedExchange64
		set( 64BIT_ATOMICS_FLAG FALSE )
	endif()
	set( VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS ${64BIT_ATOMICS_FLAG} CACHE BOOL 
		"Toggle between using the Win32-native InterlockedExchange64, or a Semaphore lock when using 64-bit-atomics. Windows XP and below don't support InterlockedExchange64, so when compiling on or for such a machine, disable this flag." )
	if( NOT VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS )
		add_definitions( -DVISTA_INTERLOCKEDEXCHANGE64_NOT_AVAILABLE )
	endif( NOT VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS )
endif()

add_library( VistaBase ${ProjectSources} )
target_link_libraries( VistaBase
	${LIBRARIES}
)

vista_configure_lib( VistaBase )
vista_install( VistaBase VistaBase )
if( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
	vista_create_cmake_configs( VistaBase )
endif( VISTACORELIBS_CREATE_CONFIG_PER_LIBRARY )
vista_create_default_info_file( VistaBase )