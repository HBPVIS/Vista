if(WIN32)
    add_definitions(-DVISTAKERNELOPENSGEXT_EXPORTS -DOSG_WITH_GIF -DOSG_WITH_TIF -DOSG_WITH_JPG -DOSG_BUILD_DLL -D_OSG_HAVE_CONFIGURED_H_)
    set(LIBRARIES
        optimized OSGWindowGLUT
        optimized OSGSystem
        optimized OSGBase		
        debug OSGWindowGLUTD
        debug OSGSystemD
        debug OSGBaseD
		glu32
		opengl32
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
    VistaKernelOpenSGExtOut.cpp
    VistaOpenSGGeometryTools.cpp
    VistaOpenSGNormalMapMaterial.cpp
    VistaOpenSGNormalMapMaterialShaders.cpp
    VistaOpenSGParticleManager.cpp
    VistaOpenSGParticles.cpp
	VistaOpenSGParticleManagerParticleChanger.cpp
    VistaOpenSGPerMaterialShader.cpp
    VistaOpenSGPhongShader.cpp
    VistaOpenSGShadow.cpp
    VistaOpenSGSkybox.cpp
    VistaOpenSGToonShader.cpp
    VistaKernelOpenSGExtOut.h
    VistaKernelOpenSGExtConfig.h
    VistaOpenSGGeometryTools.h
    VistaOpenSGNormalMapMaterial.h
    VistaOpenSGParticles.h
	VistaOpenSGParticleManager.h
	VistaOpenSGParticleManagerParticleChanger.h
    VistaOpenSGPerMaterialShader.h
    VistaOpenSGPhongShader.h
    VistaOpenSGShadow.h
    VistaOpenSGSkybox.h
    VistaOpenSGToonShader.h
)
source_group("Source Files" FILES ${dirFiles})

add_library(VistaKernelOpenSGExt
    ${dirFiles}
) 

target_link_libraries(VistaKernelOpenSGExt
	VistaBase
	VistaAspects
	VistaTools
    VistaKernel
    ${GLUT_glut_LIBRARY}
    ${LIBRARIES}
)

# Include link paths for OpenSG so they will be found when linking against the OpenSGExt
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

install(TARGETS VistaKernelOpenSGExt
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaKernelOpenSGExt
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
