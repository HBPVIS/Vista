cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 16PhantomDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif( NOT VISTADEMO_COMMON_BUILD )

vista_use_package( VistaCoreLibs "HEAD" REQUIRED FIND_DEPENDENCIES )


# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 16PhantomDemo ${ProjectSources} )
target_link_libraries( 16PhantomDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( 16PhantomDemo )

