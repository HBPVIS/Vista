/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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
// $Id$
#ifndef VISTAGENERICFACTORY_H
#define VISTAGENERICFACTORY_H

/*============================================================================*/
/* FORWARD DECLARATIONS														  */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES																	  */
/*============================================================================*/
#include <cstddef>
#include <map>

/*============================================================================*/
/* CLASS DEFINITION															  */
/*============================================================================*/
/**
 * This is a generic interface for object creators used with the abstract factory
 * It just defines that a generic product can be created with the given 
 * method CreateInstance.
 */
template<typename TProduct>
class IVistaCreator
{
public:
	virtual ~IVistaCreator() {}
	virtual TProduct *CreateInstance() = 0;
};

/**
 * VistaAbstractFactory defines a recurring pattern for object creation.
 * The general idea is to create objects by giving a key to the abstract
 * factory. The key uniquely identifies the base class of the object being 
 * created.
 * In order to achieve this, the factory works with so called Creator
 * objects. A Creator is basically a strategy that creates a new object of
 * a given type or sub-class thereof. Creators have to implement a method
 *
 *					TProduct *Creator::CreateInstance();
 *
 * An abstract factory can then be used to create objects that share a 
 * common base class. This base class becomes the TProduct type.
 *
 * NOTE: This basic mechanism does not make any assumptions about how
 * the factory is made known to client code or about the way in which
 * products are registered. It just implements the basic mapping.
 *
 */
template<typename TProduct, typename TKey>
class VistaGenericFactory
{
public:
	VistaGenericFactory();
	virtual ~VistaGenericFactory();

	/**
	 * Register a new (key,creator) pair for subsequent object
	 * creation via key.
	 *
	 * NOTE: The factory takes ownership for the creator i.e. 
	 *		 clients MUST NOT delete it after the register call.
	 *
	 * \return true if successful, false if rKey already exists
	 *
	 */
	bool RegisterCreator(const TKey &rKey, IVistaCreator<TProduct> *pCreator);

	/**
	 * Create an instance of TProduct of a subtype thereof as designated
	 * by rKey
	 *
	 * \return New object of TProduct or subtype, NULL if no creator was
	 *		   found for rKey.
	 */
	TProduct* CreateInstance(const TKey &rKey);

private:
	typedef std::map<TKey, IVistaCreator<TProduct>*> TCreatorMap;
	TCreatorMap m_mapKey2Creators;
};



/**
 * This is a default creator implementation that creates a new object of
 * the specific class TProduct by calling its argument-less c'tor.
 * It is assumed that the abstract factory creates products of type TProductBase.
 * Hence, in order for this to work, TProduct has to be a subtype of TProductBase.
 */
template<typename TProductBase, typename TProduct> 
class VistaVanillaCreator : public IVistaCreator<TProductBase>
{
public:
	TProductBase* CreateInstance()
	{
		return new TProduct();
	}
};

/*============================================================================*/
/* TEMPLATE IMPLEMENTATION													  */
/*============================================================================*/
template<typename TProduct, typename TKey>
VistaGenericFactory<TProduct,TKey>::VistaGenericFactory()
{

}

template<typename TProduct, typename TKey>
VistaGenericFactory<TProduct,TKey>::~VistaGenericFactory()
{
	//properly delete all the creators
	typename TCreatorMap::iterator itCurrent = m_mapKey2Creators.begin();
	typename TCreatorMap::iterator itEnd = m_mapKey2Creators.end();
	for(; itCurrent != itEnd; ++itCurrent)
	{
		delete itCurrent->second;
	}
}

template<typename TProduct, typename TKey>
bool VistaGenericFactory<TProduct,TKey>::RegisterCreator(const TKey &rKey, IVistaCreator<TProduct> *pCreator)
{
	typename TCreatorMap::iterator itFind = m_mapKey2Creators.find(rKey);
	if(itFind != m_mapKey2Creators.end())
	{
		return false;
	}
	m_mapKey2Creators.insert(std::make_pair(rKey, pCreator));
	return true;
}

template<typename TProduct, typename TKey>
TProduct *VistaGenericFactory<TProduct,TKey>::CreateInstance(const TKey &rKey)
{
	typename TCreatorMap::iterator itFind = m_mapKey2Creators.find(rKey);
	if(itFind == m_mapKey2Creators.end())
	{
		return NULL;
	}
	return static_cast<TProduct*>(itFind->second->CreateInstance());
}

#endif // Include guard.


/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
