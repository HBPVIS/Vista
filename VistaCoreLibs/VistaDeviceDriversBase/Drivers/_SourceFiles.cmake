# $Id$

set( RelativeDir "./Drivers" )
set( RelativeSourceGroup "Source Files\\Drivers" )

set( DirFiles
	VistaCyberGloveDriver.cpp
	VistaCyberGloveDriver.h
	VistaDTrackDriver.cpp
	VistaDTrackDriver.h
	VistaFastrakDriver.cpp
	VistaFastrakDriver.h
	VistaHIDDriver.cpp
	VistaHIDDriver.h
	VistaHIDDriverForceFeedbackAspect.cpp
	VistaHIDDriverForceFeedbackAspect.h
	VistaIRManDriver.cpp
	VistaIRManDriver.h
	VistaMIDIDriver.cpp
	VistaMIDIDriver.h
	VistaPhantomServerDriver.cpp
	VistaPhantomServerDriver.h
	VistaSpaceMouseDriver.cpp
	VistaSpaceMouseDriver.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

