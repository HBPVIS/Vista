set(relDir "tinyXML")
set(dirFiles_in
    CMakeLists.txt
    tinystr.cpp
    tinyxml.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
    tinystr.h
    tinyxml.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_tinyXML "${relDir}/${file}")
endforeach(file)

set(dirFiles_tinyXML ${dirFiles_tinyXML} PARENT_SCOPE)
