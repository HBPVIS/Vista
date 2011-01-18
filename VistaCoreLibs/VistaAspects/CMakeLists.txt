if(WIN32)
    add_definitions(-DVISTAASPECTS_EXPORTS)
elseif(UNIX)
    set(LIBRARIES rt)
endif(WIN32)

set(dirFiles
    VistaAspectsOut.cpp
    VistaAspectsUtils.cpp
    VistaDeSerializer.cpp
    VistaExplicitCallbackInterface.cpp
    VistaNameable.cpp
    VistaObjectRegistry.cpp
    VistaObserveable.cpp
    VistaObserver.cpp
	VistaLocatable.cpp
    VistaPropertyAwareable.cpp
    VistaPropertyFunctor.cpp
    VistaPropertyFunctorRegistry.cpp
    VistaReferenceCountable.cpp
    VistaReflectionable.cpp
    VistaSerializable.cpp
    VistaSerializer.cpp
    VistaSerializingToolset.cpp
    VistaTransformable.cpp
    VistaAspectsConfig.h
    VistaAspectsOut.h
    VistaAspectsUtils.h
    VistaDeSerializer.h
    VistaExplicitCallbackInterface.h
    VistaNameable.h
    VistaObjectRegistry.h
    VistaObserveable.h
    VistaObserver.h
	VistaLocatable.h
    VistaPropertyAwareable.h
    VistaPropertyFunctor.h
    VistaPropertyFunctorRegistry.h
    VistaReferenceCountable.h
    VistaReflectionable.h
    VistaSerializable.h
    VistaSerializer.h
    VistaSerializingToolset.h
    VistaTransformable.h
	VistaPropertyList.h
	VistaPropertyList.cpp
	VistaProperty.h
	VistaProperty.cpp
)
source_group("Source Files" FILES ${dirFiles})

add_library(VistaAspects
    ${dirFiles}
) 

target_link_libraries(VistaAspects
	VistaBase
    ${LIBRARIES}
)

install(TARGETS VistaAspects
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaAspects
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
