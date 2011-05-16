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
// $Id$

#ifndef _VISTAPROPERTYLIST_H
#define _VISTAPROPERTYLIST_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

#include <VistaAspects/VistaProperty.h>

#include "VistaNameable.h"
#include "VistaReferenceCountable.h"
#include "VistaAspectsOut.h"

#include <map>
#include <string>
#include <list>
#include <iostream>



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSerializer;
class IVistaDeSerializer;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A PropertyList is-a map<string, VistaProperty>, as such feel free
 * to use all map-methods as you are used to.
 * A PropertyList does not maintain an internal
 * state, all the functions are "macros" is you like with additional
 * checking, if needed.
 */
class VISTAASPECTSAPI VistaPropertyList : public std::map<std::string, VistaProperty, VistaAspectsComparisonStuff::StringCompare>
{
public:
	explicit VistaPropertyList( const bool bCaseSensitive = true );
	VistaPropertyList( const VistaPropertyList& oCopy );
	virtual ~VistaPropertyList();

	bool GetIsCaseSensitive() const;
	/**
	 * @param bConvertSubPropLists - if true, Sub-PorpertyLists of Properties
	 *        will be converted too
	 * @param bFailOnNameClash - if set to true, the conversion will abort and return false
	 *        if two entries in the current list would have identical keys after the conversion,
	 *        which is not allowed in maps. If false is returned, the state is not changed at all.
	 *        If bFailOnNameClash is false, a name clash will just result in a warning output.
	 */
	bool SetIsCaseSensitive( const bool bSet,
							const bool bConvertSubPropLists = true,
							const bool bFailOnNameClash = false );
						
	// #####################################################
	// ACCESSORS
	// #####################################################

	bool	    HasProperty(const std::string &) const;


	std::string GetStringValue(const std::string &) const;
	int         GetIntValue(const std::string &sPropName) const;
	double      GetDoubleValue(const std::string &sPropName) const;
	bool        GetBoolValue(const std::string &sPropName) const;
	void *		GetIdValue(const std::string &sPropName) const;
	int         GetStringListValue(const std::string &, std::list<std::string> &) const;

	VistaProperty	      GetProperty(const std::string &) const;
	VistaProperty	     &GetPropertyRef(const std::string &);
	const VistaProperty &GetPropertyConstRef(const std::string &) const;
	VistaPropertyList    GetPropertyListValue(const std::string &sPropName) const;

	// #####################################################
	// EFFECTORS
	// #####################################################
	void		SetProperty( const VistaProperty & propVal );

	void        SetStringValue(const std::string &propName, const std::string &propValue);
	void        SetIntValue(const std::string &sPropName, int iPropValue);
	void        SetDoubleValue(const std::string &sPropName, double dPropValue);
	void        SetBoolValue(const std::string &sPropName, bool bPropValue);
	void		SetIdValue(const std::string &sPropName, void *pPropValue);
	void        SetStringListValue(const std::string &, const std::list<std::string> &);

	void        SetPropertyListValue(const std::string &sPropName, const VistaPropertyList &refList);

	/**
	 * @param sPropName the name of the property to remove
	 * @return true iff a property with name sPropName() was removed, false if such a property
	 *         does not exist.
	 */
	bool        RemoveProperty(const std::string &sPropName);


	// #####################################################
	// OPERATORS
	// #####################################################

	VistaProperty operator()(const std::string &sName) const;


	// #####################################################
	// SERIALIZATION
	// #####################################################
	static int DeSerializePropertyList( IVistaDeSerializer& rDeSer,
								   VistaPropertyList &rPropertyList,
								   std::string &sName);

	static int SerializePropertyList( IVistaSerializer& rSer,
								 const VistaPropertyList &rPropertyList,
								 const std::string &sPropertyListName);


	/**
	 * create a PropertyList that contains elements from oMaster,
	 * overwritten from oMergeIn. PropertyList elements will be recursively
	 * merged in. Elements that are only in oMergeIn will be in oMaster
	 * after this call. If oMaster has a PropertyList element named X and
	 * oMergeIn has the same PropertyList element named X, the list will
	 * be recusively copied, else, the value of oMergeIn will replace the
	 * one in oMaster. That means, if oMaster has a property Y which is
	 * non-PropertyList, and oMergeIn has a property named Y which is PropertyList,
	 * the returned PropertyList will after this call have a PropertyList property
	 * called Y which is an exact copy of oMergeIn.
	 */
	static VistaPropertyList MergePropertyLists(const VistaPropertyList &oMaster,
											 const VistaPropertyList &oMergeIn);

	/**
	 * merge THIS PropertyList with ioMergeIn.
	 */
	bool MergeWith(const VistaPropertyList &ioMergeIn);

	// #####################################################
	// DEBUGGING
	// #####################################################
	
	/**
	 * @deprecated use Print() instead.
	 */
	void PrintPropertyList(int iDepth = 0) const;
	
	/**
	 * Prints this PropertyList to out. Ignore iDepth, this is used
	 * for the recursive traversal during printouts.
	 * @param iIndentation indents a line with iIndentation number of whitespaces
	 *        resulting in more readable output.
	 */
	void Print(std::ostream &out = vaspout, int iIndentation = 0) const;

private:
	std::string GetStringValueTyped(const std::string &, VistaProperty::ePropType eType) const;
	void   SetStringValueTyped(const std::string &sPropName,
									   const std::string &sValue,
									   VistaProperty::ePropType eType,
									   VistaProperty::ePropType eListType);
	bool m_bIsCaseSensitive;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYAWARE_H

