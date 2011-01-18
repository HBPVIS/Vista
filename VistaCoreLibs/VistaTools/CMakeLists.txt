if(WIN32)
    add_definitions(-DVISTATOOLS_EXPORTS)
    set(LIBRARIES PowrProf dbghelp Psapi)
elseif(UNIX)
endif(WIN32)

set(dirFiles
    CommTimer.cpp
    VistaBasicProfiler.cpp
    VistaColoredConsoleMessage.cpp
    VistaCPUInfo.cpp
    VistaCRC32.cpp
    VistaDLL.cpp
    VistaEnvironment.cpp
    VistaFastMeshAdaptor.cpp
    VistaFileDataSet.cpp
    VistaFileSystemDirectory.cpp
    VistaFileSystemFile.cpp
    VistaFileSystemNode.cpp
    VistaHalfedgeAdaptor.cpp
    VistaMemoryInfo.cpp
    VistaOctree.cpp
    VistaProfiler.cpp
    VistaProgressBar.cpp
    VistaProtocol.cpp
    VistaRandomNumberGenerator.cpp
    VistaStackWalker.cpp
    VistaTimer.cpp
    VistaToolsOut.cpp
    CommTimer.h
    EncodeBinary.h
    VistaBasicProfiler.h
    VistaColoredConsoleMessage.h
    VistaCPUInfo.h
    VistaCRC32.h
    VistaDLL.h
    VistaEnvironment.h
    VistaFastMeshAdaptor.h
    VistaFileDataSet.h
    VistaFileSystemDirectory.h
    VistaFileSystemFile.h
    VistaFileSystemNode.h
    VistaHalfedgeAdaptor.h
    VistaMemoryInfo.h
    VistaOctree.h
    VistaProfiler.h
    VistaProgressBar.h
    VistaProtocol.h
    VistaRandomNumberGenerator.h
    VistaRingBuffer.h
    VistaStackWalker.h
    VistaTimer.h
    VistaToolsConfig.h
    VistaToolsOut.h
    VistaToolsStd.h
    VistaTopologyGraph.h
    VistaVoxelOctree.h
)
source_group("Source Files" FILES ${dirFiles})

add_subdirectory(tinyXML)
source_group("Source Files\\tinyXML" FILES ${dirFiles_tinyXML})

add_library(VistaTools
    ${dirFiles}
    ${dirFiles_tinyXML}
) 

target_link_libraries(VistaTools
	VistaBase
    ${LIBRARIES}
)

install(TARGETS VistaTools
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaTools
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
