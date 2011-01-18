set(relDir "Drivers")
set(dirFiles_in
    CMakeLists.txt
    VistaCyberGloveDriver.cpp
    VistaDTrackDriver.cpp
    VistaFastrakDriver.cpp
    VistaHIDDriver.cpp
    VistaHIDDriverForceFeedbackAspect.cpp
    VistaIRManDriver.cpp
    VistaMIDIDriver.cpp
    VistaPhantomServerDriver.cpp
    VistaSpaceMouseDriver.cpp
    VistaCyberGloveDriver.h
    VistaDTrackDriver.h
    VistaFastrakDriver.h
    VistaHIDDriverForceFeedbackAspect.h
    VistaHIDDriver.h
    VistaIRManDriver.h
    VistaMIDIDriver.h
    VistaPhantomServerDriver.h
    VistaSpaceMouseDriver.h
)

foreach(file ${dirFiles_in})
    list(APPEND dirFiles_Drivers "${relDir}/${file}")
endforeach(file)

set(dirFiles_Drivers ${dirFiles_Drivers} PARENT_SCOPE)
