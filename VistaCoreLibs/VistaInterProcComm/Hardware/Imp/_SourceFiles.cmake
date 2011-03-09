set( RelativeDir "./Hardware/Imp" )
set( RelativeSourceGroup "Source Files\\Hardware\\Imp" )

set( DirFiles
	VistaPosixSerialPortImp.cpp
	VistaPosixSerialPortImp.h
	VistaSerialPortImp.cpp
	VistaSerialPortImp.h
	VistaWin32SerialPortImp.cpp
	VistaWin32SerialPortImp.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

