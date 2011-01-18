if(WIN32)
    add_definitions(-DVISTAMATH_EXPORTS)
endif(WIN32)

set(dirFiles
    VistaBoundingBox.cpp
    VistaBoundingSphere.cpp
    VistaGeometries.cpp
    VistaIndirectXform.cpp
    VistaLeastSquaresPlane.cpp
    VistaMathOut.cpp
    VistaMatrixDeComposer.cpp
    VistaPolynomial.cpp
    VistaBoundingBox.h
    VistaBoundingSphere.h
    VistaFilter.h
    VistaGeometries.h
    VistaIndirectXform.h
    VistaLeastSquaresPlane.h
    VistaMathConfig.h
    VistaMathOut.h
    VistaMathTools.h
    VistaMatrixDeComposer.h
    VistaMatrix.h
    VistaPolynomial.h
    VistaSquareMatrix.h
    VistaVector.h
)
source_group("Source Files" FILES ${dirFiles})

add_library(VistaMath
    ${dirFiles}
)

target_link_libraries(VistaMath
	VistaBase
    ${LIBRARIES}
)

install(TARGETS VistaMath
         LIBRARY DESTINATION ${VISTA_INSTALL_LIB_DIR}
         ARCHIVE DESTINATION ${VISTA_INSTALL_LIB_DIR}
         RUNTIME DESTINATION ${VISTA_INSTALL_LIB_DIR})
install(DIRECTORY .
         DESTINATION ${VISTA_INSTALL_INC_DIR}/VistaMath
         FILES_MATCHING PATTERN "*.h"
         PATTERN "build" EXCLUDE
         PATTERN ".svn" EXCLUDE
         PATTERN "CMakeFiles" EXCLUDE)
