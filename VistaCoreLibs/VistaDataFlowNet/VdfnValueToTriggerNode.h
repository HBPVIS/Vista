/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
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

#ifndef _VDFNVALUETOTRIGGERNODE_H
#define _VDFNVALUETOTRIGGERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>
#include <list>

#include "VdfnConfig.h"

#include "VdfnOut.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
#include "VdfnUtil.h"

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * @inport{value,T,mandatory,the value to switch/route}
 * @outport{many,T,forwards the value of 'value', number and name of outports
            is determined upon contruction}
 */
template<class T>
class TVdfnValueToTriggerNode : public IVdfnNode
{
public:
	typedef T (*ToValueFct)(const std::string &);
	typedef std::map<T,std::pair<std::string,TVdfnPort<bool>* > > MapType;

	TVdfnValueToTriggerNode(const MapType &map,
							const std::list<T> &liIgnore) : 
		IVdfnNode(),
		m_pIn(NULL),
		m_mMap(map),
		m_liIgnore(liIgnore)
	{
		for( typename MapType::iterator it = m_mMap.begin() ;
			 it != m_mMap.end() ; it++ )
		{
			TVdfnPort<bool> *pPort = new TVdfnPort<bool>;
			(*it).second.second = pPort;
			RegisterOutPort( (*it).second.first, pPort );
		}

		RegisterInPortPrototype( "value", new TVdfnPortTypeCompare<TVdfnPort<T> >);
	}


	bool PrepareEvaluationRun()
	{
		m_pIn = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>("value", this);
		return GetIsValid();
	}
protected:
	bool DoEvalNode()
	{
		// is value in ignore list?
		bool bIgnore = false;
		for( typename std::list<T>::iterator ign_it = m_liIgnore.begin();
			 ign_it != m_liIgnore.end() ; ++ign_it )
		{
			if(*ign_it == m_pIn->GetValueConstRef())
				bIgnore = true;
		}
		if(bIgnore)
			return true;

		for( typename MapType::iterator it = m_mMap.begin() ;
			 it != m_mMap.end() ; it++ )
		{			
//find( m_liIgnore.begin(), m_liIgnore.end(), *it );

			TVdfnPort<bool> *pPort = (*it).second.second;
			if( (*it).first == m_pIn->GetValueConstRef() )
			{
#ifdef DEBUG
//				std::cout << "dispatching value to outport " << (*it).second.first
//						  << std::endl;
#endif
				if(pPort->GetValueConstRef() == false)
					pPort->SetValue( true, GetUpdateTimeStamp() );
			}
			else
			{
				if(pPort->GetValueConstRef() == true)
					pPort->SetValue( false, GetUpdateTimeStamp() );
			}
		}
		return true;
	}
private:
	TVdfnPort<T>  *m_pIn;

    // maps values of type T to a name and corresponding outport
	MapType m_mMap;
	std::list<T> m_liIgnore;
};


/**
 * @todo document me
 */
template<class T>
class TVdfnValueToTriggerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	typedef typename TVdfnValueToTriggerNode<T>::ToValueFct ToValueFct;
	TVdfnValueToTriggerNodeCreate(ToValueFct fct)
		: m_CFct(fct)
	{}

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		typename TVdfnValueToTriggerNode<T>::MapType map;

		VistaProperty oMapping = oSubs("mapping");
		if(oMapping.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST)
		{
			for(VistaPropertyList::const_iterator cit = oMapping.GetPropertyListConstRef().begin();
				cit != oMapping.GetPropertyListConstRef().end(); ++cit)
				{
					T value = m_CFct((*cit).second.GetValue());
					std::string name = (*cit).first;

					map[ value ].first = name;

#ifdef DEBUG
					vdfnout << "[ValueToTrigger] registered outport " << name
							  << " for value " << value << std::endl;
#endif
				}
		}
		else
		{
			std::list<std::string> liStrings;
			oSubs.GetStringListValue("mapping", liStrings);


			std::list<std::string>::const_iterator it = liStrings.begin();
			while( it != liStrings.end() )
			{
				T value = m_CFct(*it);
				it++;
				map[value].first = *it;
				it++;
			}
		}

		std::list<T> liIgnore;
		VistaProperty oIgnoreList = oSubs("ignorelist");
		std::list<std::string> liStrings;
		oSubs.GetStringListValue("ignorelist", liStrings);
		
		std::list<std::string>::const_iterator it = liStrings.begin();
		while( it != liStrings.end() )
		{
			liIgnore.push_back(m_CFct(*it));
			vdfnout << "[ValueToTrigger] ignoring value: "	
					  << m_CFct(*it)
					  << std::endl;
			++it;
		}
		
		return new TVdfnValueToTriggerNode<T>(map, liIgnore);
	}

private:
	ToValueFct m_CFct;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_VDFNVALUETOTRIGGERNODE_H

