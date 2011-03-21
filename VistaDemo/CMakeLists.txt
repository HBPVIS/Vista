cmake_minimum_required(VERSION 2.6)
project( VistaDemo )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )
include( VistaAppCommon )

set( VISTADEMO_VERSION_TYPE "RELEASE" )
set( VISTADEMO_VERSION_NAME "HEAD" )
set( VISTADEMO_VERSION_MAJOR 1 )
set( VISTADEMO_VERSION_MINOR 10 )
set( VISTADEMO_VERSION_REVISION 0 )

vista_set_defaultvalue( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/bin" CACHE PATH "distribution directory" FORCE )


include_directories( ${VistaDemo_SOURCE_DIR} )

# this variable indicates to sub-projects that they are build all together
set( VISTADEMO_COMMON_BUILD TRUE )

find_package_versioned( VistaCoreLibs "HEAD" )

vista_conditional_add_subdirectory( VISTADEMO_BUILD_01KEYBOARDDEMO 01KeyboardDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_02GEOMETRYDEMO 02GeometryDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_03TEXTDEMO 03TextDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_04LOADDEMO 04LoadDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_05INTERACTIONDEMO 05InteractionDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_06CAMERACONTROLDEMO 06CameraControlDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_07OVERLAYDEMO 07OverlayDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_09EVENTDEMO 09EventDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_10DISPLAYDEMO 10DisplayDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_11OGLDEMO 11OGLDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_12INTENTIONSELECTDEMO 12IntentionSelectDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_13KERNELOPENSGEXTDEMO 13KernelOpenSGExtDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_14DATAFLOWNETDEMO 14DataFlowNetDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_15VTKDEMO 15VtkDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_16PHANTOMDEMO 16PhantomDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_17MSGPORTDEMO 17MsgPortDemo ON )
vista_conditional_add_subdirectory( VISTADEMO_BUILD_18DEBUGGINGTOOLSDEMO 18DebuggingToolsDemo ON )

