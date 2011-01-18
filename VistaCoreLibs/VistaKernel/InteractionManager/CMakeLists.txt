set(relDir "InteractionManager")
set(dirFiles_in
    CMakeLists.txt
	VistaDriverWindowAspect.cpp
	VistaIntentionSelect.cpp
    VistaInteractionContext.cpp
    VistaInteractionEvent.cpp
    VistaInteractionManager.cpp
    VistaKeyboardSystemControl.cpp
    VistaObjectSetMove.cpp
    VistaUserPlatform.cpp
    VistaVirtualPlatformAdapter.cpp
    VistaDriverWindowAspect.h
	VistaIntentionSelect.h
    VistaInteractionContext.h
    VistaInteractionEvent.h
    VistaInteractionManager.h
    VistaKeyboardSystemControl.h
    VistaObjectSetMove.h
    VistaUserPlatform.h
    VistaVirtualPlatformAdapter.h
	 
)



foreach(file ${dirFiles_in})
    list(APPEND dirFiles_InteractionManager "${relDir}/${file}")
endforeach(file)

add_subdirectory(DfnNodes)


set(dirFiles_InteractionManager ${dirFiles_InteractionManager} PARENT_SCOPE)
set(dirFiles_InteractionManager_DfnNodes ${dirFiles_InteractionManager_DfnNodes} PARENT_SCOPE)
