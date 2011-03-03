
vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_SPACENAVIGATOR	3DCSpaceNavigator		OFF )
vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_PHANDTOM		PhantomDriver			OFF )

if( WIN32 )
	vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_DIRECTXGAMEPAD	DirectXGamepad		OFF )
endif( WIN32 )

if( UNIX )
	vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_JSWJOYSTICK		JswJoystick			OFF )
endif( UNIX )
