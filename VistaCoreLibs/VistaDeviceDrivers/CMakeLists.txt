# $Id$

vista_conditional_add_subdirectory( VISTADEVICEDRIVERS_BUILD_SPACENAVIGATOR		3DCSpaceNavigator		OFF )
vista_conditional_add_subdirectory( VISTADEVICEDRIVERS_BUILD_PHANTOM			PhantomDriver			OFF )
vista_conditional_add_subdirectory( VISTADEVICEDRIVERS_BUILD_VRPN				VRPNDriver				OFF )
vista_conditional_add_subdirectory( VISTADEVICEDRIVERS_BUILD_WIIMOTE			WiimoteDriver			OFF )

if( WIN32 )
	vista_conditional_add_subdirectory( VISTADEVICEDRIVERS_BUILD_DIRECTXGAMEPAD	DirectXGamepad		OFF )
elseif( UNIX )
	vista_conditional_add_subdirectory( VISTADEVICEDRIVERS_BUILD_JSWJOYSTICK	JswJoystick			OFF )
endif( WIN32 )
