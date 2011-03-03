set( RelativeDir "./Hardware/Imp" )
set( LocalSourceGroup "Source Files\\Hardware\\Imp" )

set( DirFiles
	VistaPosixSerialPortImp.cpp
	VistaPosixSerialPortImp.h
	VistaSerialPortImp.cpp
	VistaSerialPortImp.h
	VistaWin32SerialPortImp.cpp
	VistaWin32SerialPortImp.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

