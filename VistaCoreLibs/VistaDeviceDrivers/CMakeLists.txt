# $Id$

vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_SPACENAVIGATOR	3DCSpaceNavigator		OFF )
vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_PHANTOM			PhantomDriver			OFF )
vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_VRPN			VRPNDriver				OFF )
vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_WIIMOTE			WiimoteDriver			OFF )

if( WIN32 )
	vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_DIRECTXGAMEPAD	DirectXGamepad		OFF )
endif( WIN32 )

if( UNIX )
	vista_conditional_add_subdirectory( VISTA_DRIVERS_BUILD_JSWJOYSTICK		JswJoystick			OFF )
endif( UNIX )
