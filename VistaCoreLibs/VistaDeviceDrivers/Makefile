SUB_DIRS = 	VistaIRManDriver \
            3DCSpaceNavigator \
            DirectXGamepad \
            JswJoystick \
            OpenALRecordDriver \
            OpenCVCaptureDriver \
            VistaGlutJoystickDriver \
            VistaGlutKeyboardDriver \
            VistaGlutMouseDriver \
            VRPNDriver \
            WiimoteDriver \
            VistaSpaceNavigatorDriver

all:	
	for i in $(SUB_DIRS) ; \
	do \
	( cd $$i/build && $(MAKE) $@ ) || exit ; \
	done ;
