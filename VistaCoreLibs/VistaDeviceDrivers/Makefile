SUB_DIRS = 	3DCSpaceNavigator \
			DTrackSDKDriver \
			JswJoystick \
			PhantomDriver \
			VistaSpaceNavigator \
			WiimoteDriver

all:	
	for i in $(SUB_DIRS) ; \
	do \
	( cd $$i/build && $(MAKE) $@ ) || exit ; \
	done ;
