# $Id$

vista_adopt_version( VistaBase VistaCoreLibs )

# set our dependencies to other corelibs
set( VISTABASE_INTERNAL_DEPENDENCIES )
set( VISTABASE_INTERNAL_DEPENDENCIES ${VISTABASE_INTERNAL_DEPENDENCIES} PARENT_SCOPE )

if( UNIX AND NOT APPLE )
	set( LIBRARIES rt )
endif( UNIX AND NOT APPLE )


include( "./_SourceFiles.cmake" )

set( 64BIT_ATOMICS_FLAG TRUE )
if( WIN32 )
	if( "${CMAKE_SYSTEM_VERSION}" VERSION_LESS 5.2 )
		# WIN XP and below don't support InterlockedExchange64
		set( 64BIT_ATOMICS_FLAG FALSE )
	endif()
	set( _ATOMCS64_DOC_TEXT "Toggle between using the Win32-native InterlockedExchange64, or a Semaphore lock when using 64-bit-atomics. Windows XP and below don't support InterlockedExchange64, so when compiling on or for such a machine, disable this flag." )
elseif( UNIX )	
	if( NOT VISTA_64BIT )
		set( 64BIT_ATOMICS_FLAG FALSE )		
	endif()
	set( _ATOMCS64_DOC_TEXT "Toggle between using native support or a Semaphore lock when using 64-bit-atomics. Gcc under 32bit linux, as well as 64bit-linux while compiling 32bit-binaries (with -m32) don't support native 64bit atomics." )
endif()

set( VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS ${64BIT_ATOMICS_FLAG} CACHE BOOL "${_ATOMCS64_DOC_TEXT}" )
if( NOT VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS )
	add_definitions( -DVISTA_NATIVE64BITATOMICS_NOT_AVAILABLE )
endif( NOT VISTACORELIBS_SUPPORT_NATIVE_64BIT_ATOMICS )

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