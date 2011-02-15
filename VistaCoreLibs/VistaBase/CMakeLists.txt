if(WIN32)
    add_definitions(-DVISTABASE_EXPORTS)
elseif(UNIX)
    set(LIBRARIES rt)
endif(WIN32)

set(dirFiles
    VistaBaseConfig.h
	VistaVersion.h
	VistaVersion.cpp
	VistaBaseOut.h
	VistaBaseOut.cpp    
	VistaExceptionBase.h
    VistaExceptionBase.cpp
    VistaBaseTypes.h
    VistaBaseMain.cpp
)
set(dirFiles_Timing
	VistaTimeUtils.h
	VistaTimeUtils.cpp
	VistaTimer.h
	VistaTimer.cpp
    VistaTimerImp.h
	VistaTimerImp.cpp    
	VistaDefaultTimerImp.h
    VistaDefaultTimerImp.cpp    
)
set(dirFiles_Streams
	VistaStreamManager.h
    VistaStreamManager.cpp
	VistaStreams.h
    VistaStreams.cpp
	VistaStreamUtils.h
    VistaStreamUtils.cpp
)
set(dirFiles_VectorMath
	VistaVectorMath.h
	VistaMathBasics.h
    VistaVector3D.h
	VistaQuaternion.h
	VistaQuaternion.cpp
	VistaTransformMatrix.h
	VistaTransformMatrix.cpp
	VistaReferenceFrame.h
	VistaReferenceFrame.cpp
)

source_group("Source Files" FILES ${dirFiles})
source_group("Source Files\\VectorMath" FILES ${dirFiles_VectorMath})
source_group("Source Files\\Timing" FILES ${dirFiles_Timing})
source_group("Source Files\\Streams" FILES ${dirFiles_Streams})


add_subdirectory(Half)
source_group("Source Files\\Half" FILES ${dirFiles_Half})

add_library(VistaBase
    ${dirFiles}
	${dirFiles_VectorMath}
	${dirFiles_Timing}
	${dirFiles_Streams}
    ${dirFiles_Half}
) 

target_link_libraries(VistaBase
    ${LIBRARIES}
)

install(TARGETS VistaBase
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION lib)
install(DIRECTORY .
         DESTINATION include/VistaBase
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
