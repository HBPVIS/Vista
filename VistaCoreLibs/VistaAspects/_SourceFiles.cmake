set( RelativeDir "." )
set( LocalSourceGroup "Source Files" )

set( DirFiles
	VistaAspectsConfig.h
	VistaAspectsOut.cpp
	VistaAspectsOut.h
	VistaAspectsUtils.cpp
	VistaAspectsUtils.h
	VistaDeSerializer.cpp
	VistaDeSerializer.h
	VistaExplicitCallbackInterface.cpp
	VistaExplicitCallbackInterface.h
	VistaLocatable.cpp
	VistaLocatable.h
	VistaNameable.cpp
	VistaNameable.h
	VistaObjectRegistry.cpp
	VistaObjectRegistry.h
	VistaObserveable.cpp
	VistaObserveable.h
	VistaObserver.cpp
	VistaObserver.h
	VistaProperty.cpp
	VistaProperty.h
	VistaPropertyAwareable.cpp
	VistaPropertyAwareable.h
	VistaPropertyFunctor.cpp
	VistaPropertyFunctor.h
	VistaPropertyFunctorRegistry.cpp
	VistaPropertyFunctorRegistry.h
	VistaPropertyList.cpp
	VistaPropertyList.h
	VistaReferenceCountable.cpp
	VistaReferenceCountable.h
	VistaReflectionable.cpp
	VistaReflectionable.h
	VistaSerializable.cpp
	VistaSerializable.h
	VistaSerializer.cpp
	VistaSerializer.h
	VistaSerializingToolset.cpp
	VistaSerializingToolset.h
	VistaTransformable.cpp
	VistaTransformable.h
)

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( "${LocalSourceGroup}" FILES ${LocalSourceGroupFiles} )

