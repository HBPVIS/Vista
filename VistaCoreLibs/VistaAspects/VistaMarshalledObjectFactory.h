/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2011 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id: VistaMarshalledObjectFactory.h 21315 2011-05-16 13:47:39Z dr165799 $

#ifndef _VISTAMARSHALLEDOBJECTFACTORY_H
#define _VISTAMARSHALLEDOBJECTFACTORY_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#include <map>
#include <typeinfo>
using std::type_info;

#include "VistaAspectsConfig.h"
#include "VistaGenericFactory.h"

#include <VistaBase/VistaBaseTypes.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSerializable;
class IVistaSerializer;
class IVistaDeSerializer;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaMarshalledObjectFactory is used to implement object marshalling/
 * unmarshalling (i.e. package a given object into a serialized form, including
 * not only its current state but also type information).
 *
 * This factory is implemented as a singleton in order to allow for a convenient
 * way to marshall composite objects as well, i.e. in order to avoid the 
 * necessity to pass a pointer to the factory along with the entire unmarshalling
 * process.
 *
 * Please note, that classes have to be registered with the factory before
 * use. This registration process has to be done in <bf>exactly the same order</bf>
 * for all communication partners. For example, if objects are send back and 
 * forth between two different applications, make sure that both applications
 * register their objects in exactly the same order.
 */
class VISTAASPECTSAPI VistaMarshalledObjectFactory
{
public:
	virtual ~VistaMarshalledObjectFactory();
	
	/**
	 * Singleton access method
	 */
	VistaMarshalledObjectFactory *GetSingleton();

	/**
	 * Register specific implementation sub-class of IVistaSerializable given
	 * in pType with the given object creator. Double-registration of the same
	 * type will result in failure.
	 *
	 * NOTE: The factory will take ownership of the creator object, i.e. the calling
	 *       client must not delete it!
	 *
	 * \return global type id for the newly registered type, -1 in case of failure
	 */
	VistaType::sint32 RegisterType(IVistaSerializable *pType, 
								   IVistaCreator<IVistaSerializable> *pCreator);

	/**
	 * Marshall an object i.e. pack its type and state information
	 * into the given serializer
	 *
	 * \return number of bytes written to rSer, -1 in case of failure.
	 */
	int MarshallObject(const IVistaSerializable &rObject, IVistaSerializer &rSer) const;

	/**
	 * Unmarshall an object i.e. unpack it from the byte stream provided
	 * by the given deserializer.
	 * NOTE: This effectively corresponds to a new call, i.e. the created
	 *       object is allocated herein. Therefore, clients assume responsibility
	 *       for the returned object
	 *
	 * \return newly created, unmarshalled object, NULL in case of failure
	 */
	IVistaSerializable *UnmarshallObject(IVistaDeSerializer &rDeser) const;
	
protected:
	VistaMarshalledObjectFactory();
	/**
	 * generate unique global type identifier by counting up.
	 */
	VistaType::sint32 GetNextTypeId();

	/**
	 * Helper class for type_info storage
	 * This is needed to properly store the type_info in a map container.
	 */
	class LocalTypeInfo
	{
	public:
		LocalTypeInfo(const type_info& oInfo);
		LocalTypeInfo(const LocalTypeInfo &rOther);

		bool operator<(const LocalTypeInfo &rRHS) const;
		bool operator==(const LocalTypeInfo &rRHS) const;

		const type_info& GetTypeInfo() const;
	protected:
	private:
		const type_info& m_rTypeInfo;
	};

private:
	static VistaMarshalledObjectFactory *m_pSingleton;

	typedef std::map<LocalTypeInfo, VistaType::sint32> TLocalToGlobalMap;
	/**
	 * map the local type_info-based hash key to the globally unique int32 id.
	 */
	TLocalToGlobalMap m_mapLocalTypeToGlobalType;

	typedef std::map<VistaType::sint32, LocalTypeInfo> TGlobalToLocalMap;
	/**
	 * map the globally unique type identifier to the back locally used one.
	 */
	TGlobalToLocalMap m_mapGlobalTypeToLocalType;
	/**
	 * type id counter 
	 */
	static VistaType::sint32 m_iTypeCounter;
	/**
	 * The factory here will produce serializable objects and 
	 * will do so based on their (local) type_info hash.
	 */
	VistaGenericFactory<IVistaSerializable, LocalTypeInfo> *m_pFactory;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMARSHALLEDOBJECTFACTORY_H
