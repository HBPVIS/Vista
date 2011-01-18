if(WIN32)
    add_definitions(-DVISTAKERNEL_EXPORTS -DVISTAKERNEL_USE_PROFILING -DOSG_WITH_GIF -DOSG_WITH_TIF -DOSG_WITH_JPG -DOSG_BUILD_DLL -D_OSG_HAVE_CONFIGURED_H_)
    set(LIBRARIES
        optimized OSGWindowGLUT
        optimized OSGSystem
        optimized OSGBase
        debug OSGWindowGLUTD
        debug OSGSystemD
        debug OSGBaseD
    )
    link_directories("${OPENSG_ROOT}/lib")
elseif(UNIX)
    set(LIBRARIES
        OSGWindowGLUT
        OSGSystem
        OSGBase
    )
    link_directories("${OPENSG_ROOT}/lib/opt")
endif(WIN32)

include_directories("${OPENSG_ROOT}/include")

set(dirFiles
    VistaClientDataTunnel.cpp
    VistaClusterAux.cpp
    VistaClusterClient.cpp
    VistaClusterMaster.cpp
    VistaClusterServer.cpp
    VistaClusterSlave.cpp
    VistaDataTunnel.cpp
    VistaDataTunnelFactory.cpp
    VistaDriverPropertyConfigurator.cpp
    VistaKernelMsgPort.cpp
    VistaKernelOut.cpp
    VistaMasterDataTunnel.cpp
    VistaStandaloneDataTunnel.cpp
    VistaSystemCommands.cpp
    VistaSystemConfigurators.cpp
    VistaSystem.cpp
    VistaClientDataTunnel.h
    VistaClusterAux.h
    VistaClusterClient.h
    VistaClusterMaster.h
    VistaClusterServer.h
    VistaClusterSlave.h
    VistaDataTunnelFactory.h
    VistaDataTunnel.h
    VistaDriverPropertyConfigurator.h
    VistaErrorMsgHandler.h
    VistaKernelConfig.h
    VistaKernelMsgPort.h
    VistaKernelMsgTab.h
    VistaKernelOut.h
    VistaMasterDataTunnel.h
    VistaStandaloneDataTunnel.h
    VistaSystemCommands.h
    VistaSystemConfigurators.h
    VistaSystem.h
)
source_group("Source Files" FILES ${dirFiles})

add_subdirectory(DisplayManager)
source_group("Source Files\\DisplayManager"         FILES ${dirFiles_DisplayManager})

add_subdirectory(EventManager)
source_group("Source Files\\EventManager"           FILES ${dirFiles_EventManager})

add_subdirectory(GraphicsManager)
source_group("Source Files\\GraphicsManager"        FILES ${dirFiles_GraphicsManager})

add_subdirectory(InteractionManager)
source_group("Source Files\\InteractionManager"     FILES ${dirFiles_InteractionManager})
source_group("Source Files\\InteractionManager\\DfnNodes"     FILES ${dirFiles_InteractionManager_DfnNodes})

add_subdirectory(OpenSG)
source_group("Source Files\\OpenSG"                 FILES ${dirFiles_OpenSG})

add_subdirectory(PickManager)
source_group("Source Files\\PickManager"            FILES ${dirFiles_PickManager})

add_subdirectory(Stuff)
source_group("Source Files\\Stuff"                  FILES ${dirFiles_Stuff})

add_subdirectory(WindowingToolkit)
source_group("Source Files\\WindowingToolkit"       FILES ${dirFiles_WindowingToolkit})
source_group("Source Files\\WindowingToolkit\\glut" FILES ${dirFiles_WindowingToolkit_glut})

add_library(VistaKernel
    ${dirFiles}
    ${dirFiles_DisplayManager}
    ${dirFiles_EventManager}
    ${dirFiles_GraphicsManager}
    ${dirFiles_InteractionManager}
    ${dirFiles_InteractionManager_DfnNodes}
    ${dirFiles_OpenSG}
    ${dirFiles_PickManager}
    ${dirFiles_Stuff}
    ${dirFiles_WindowingToolkit}
    ${dirFiles_WindowingToolkit_glut}
) 

target_link_libraries(VistaKernel
	VistaBase
	VistaAspects
	VistaTools
    VistaMath
	VistaInterProcComm
	VistaDeviceDriversBase
    VistaDataFlowNet    
    ${GLUT_glut_LIBRARY}
    ${LIBRARIES}
)

# Include link paths for OpenSG so they will be found when linking against the kernel
if(VISTA_USE_RPATH)
    if(UNIX)
        set_target_properties(VistaKernel PROPERTIES
            INSTALL_RPATH "${OPENSG_ROOT}/lib/opt"
        )
    elseif(WIN32)
        set_target_properties(VistaKernel PROPERTIES
            INSTALL_RPATH "${OPENSG_ROOT}/lib"
        )
    endif(UNIX)
endif(VISTA_USE_RPATH)

install(TARGETS VistaKernel
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaKernel
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
