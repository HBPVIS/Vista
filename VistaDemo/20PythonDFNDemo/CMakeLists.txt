# $Id: $

cmake_minimum_required( VERSION 2.8 )
if( NOT VISTADEMO_COMMON_BUILD )
	project( 20PythonDFNDemo )

	list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
	include( VistaCommon )
endif( NOT VISTADEMO_COMMON_BUILD )

# get boost
#set(Boost_USE_STATIC_LIBS   ON)
#set(Boost_USE_MULTITHREADED ON)
#SET(BOOST_ROOT "$ENV{HOME}/usr")

find_package(Boost COMPONENTS
                system
                thread
                python
             REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# get python
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARIES})

vista_use_package( VistaCoreLibs 1.11 REQUIRED VistaKernel FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "./_SourceFiles.cmake" )

add_executable( 20PythonDFNDemo ${ProjectSources} )
target_link_libraries( 20PythonDFNDemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
        ${Boost_LIBRARIES}
        ${PYTHON_LIBRARIES}
)

vista_configure_app( 20PythonDFNDemo )
vista_create_default_info_file( 20PythonDFNDemo )

