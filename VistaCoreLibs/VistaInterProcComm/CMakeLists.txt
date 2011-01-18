if(WIN32)
    add_definitions(-DVISTAINTERPROCCOMM_EXPORTS -D_USE_WIN32_MUTEX -D_USE_IPC_EXCEPTIONS)
    set(LIBRARIES winmm ws2_32)
endif(WIN32)

set(dirFiles
    VistaInterProcCommOut.cpp
    VistaInterProcCommConfig.h
    VistaInterProcCommOut.h
)
source_group("Source Files" FILES ${dirFiles})

add_subdirectory(AsyncIO)
source_group("Source Files\\AsyncIO" FILES ${dirFiles_AsyncIO})

add_subdirectory(Concurrency)
source_group("Source Files\\Concurrency" FILES ${dirFiles_Concurrency})
source_group("Source Files\\Concurrency\\Imp" FILES ${dirFiles_Concurrency_Imp})

add_subdirectory(Connections)
source_group("Source Files\\Connections" FILES ${dirFiles_Connections})

add_subdirectory(DataLaVista)
source_group("Source Files\\DataLaVista\\Base" FILES ${dirFiles_DataLaVista_Base})
source_group("Source Files\\DataLaVista\\Common" FILES ${dirFiles_DataLaVista_Common})
source_group("Source Files\\DataLaVista\\Construct" FILES ${dirFiles_DataLaVista_Construct})

add_subdirectory(Hardware)
source_group("Source Files\\Hardware" FILES ${dirFiles_Hardware})
source_group("Source Files\\Hardware\\Imp" FILES ${dirFiles_Hardware_Imp})

add_subdirectory(IPNet)
source_group("Source Files\\IPNet" FILES ${dirFiles_IPNet})

add_subdirectory(SharedMemory)
source_group("Source Files\\SharedMemory" FILES ${dirFiles_SharedMemory})

add_library(VistaInterProcComm
    ${dirFiles}
    ${dirFiles_AsyncIO}
    ${dirFiles_Concurrency}
    ${dirFiles_Concurrency_Imp}
    ${dirFiles_Connections}
    ${dirFiles_DataLaVista_Base}
    ${dirFiles_DataLaVista_Common}
    ${dirFiles_DataLaVista_Construct}
    ${dirFiles_Hardware}
    ${dirFiles_Hardware_Imp}
    ${dirFiles_IPNet}
    ${dirFiles_SharedMemory}
) 

target_link_libraries(VistaInterProcComm
	VistaBase
    VistaAspects
    ${LIBRARIES}
)

install(TARGETS VistaInterProcComm
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaInterProcComm
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
