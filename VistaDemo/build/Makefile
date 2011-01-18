SUB_DIRS = 	../01KeyboardDemo \
                ../02GeometryDemo \
                ../03TextDemo \
                ../04LoadDemo \
                ../05InteractionDemo \
                ../06CameraControlDemo \
                ../07OverlayDemo \
                ../09EventDemo \
                ../10DisplayDemo \
                ../11OGLDemo \
                ../12IntentionSelectDemo \
                ../13KernelOpenSGExtDemo/ParticlesDemo \
                ../13KernelOpenSGExtDemo/ShadowDemo \
                ../14DataFlowNetDemo \
                ../17MsgPortDemo/Alice_shellDemo \
                ../17MsgPortDemo/Bob_ApplicationDemo \
                ../18DebuggingToolsDemo \
                ../VRMLVisualizer

%:	
	for i in $(SUB_DIRS) ; \
	do \
	( WD=`pwd` && cd $$i/build && $(MAKE) $@ ) || exit ; \
	done ;
