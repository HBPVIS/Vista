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
// $Id: VistaPropertyAwareable.cpp 5616 2010-11-16 10:04:47Z dominikrausch $

#include "VistaPropertyList.h"

#include "VistaAspectsUtils.h"
#include "VistaSerializer.h"
#include "VistaDeSerializer.h"

#include <VistaBase/VistaExceptionBase.h>

#include <cstdlib>
#include <string>
#include <list>
#include <iostream>

#include "VistaAspectsOut.h"
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static VistaProperty oNilProp;

static bool ConvertPropList( const VistaPropertyList& oIn,
							VistaPropertyList& oOut,
							const bool bOutputCaseSensitive,
							const bool bConvertSubPropLists = true,
							const bool bFailOnNameClash = true )
{
	oOut = VistaPropertyList( bOutputCaseSensitive );

	if( bOutputCaseSensitive )
	{
		// easier, we know that there can be no name clash
		for( VistaPropertyList::const_iterator itEntry = oIn.begin();
				itEntry != oIn.end(); ++itEntry )
		{
			oOut.insert( (*itEntry) );
			if( bConvertSubPropLists
				&& (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
			{
				oOut[(*itEntry).first].GetPropertyListRef().SetIsCaseSensitive( bOutputCaseSensitive );
			}
		}		
	}
	else
	{
		// we need to check if insertions succeed
		for( VistaPropertyList::const_iterator itEntry = oIn.begin();
				itEntry != oIn.end(); ++itEntry )
		{
			std::pair<VistaPropertyList::iterator, bool> oReturnPair = oOut.insert( (*itEntry) );
			if( oReturnPair.second == false )
			{
				// Name clash!
				if( bFailOnNameClash )
				{
					return false;
				}
				else
				{
					vasperr << "### WARNING ### [VistaPropertyList] Name clash when converting "
						<< "PropertyList to CaseInsensitive: Entries [" << (*oReturnPair.first).first
						<< "] and [" << (*itEntry).first << "] have the same key!" << std::endl;
				}
			}

			if( bConvertSubPropLists
				&& (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST )
			{
				if( oOut[(*itEntry).first].GetPropertyListRef().SetIsCaseSensitive( bOutputCaseSensitive ) == false 
					&& bFailOnNameClash )
				{
					return false;
				}
			}
		}
	}
	return true;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaPropertyList::VistaPropertyList( const bool bCaseSensitive )
: std::map<std::string, VistaProperty, VistaAspectsComparisonStuff::StringCompare>( VistaAspectsComparisonStuff::StringCompare( bCaseSensitive ) )
, m_bIsCaseSensitive( bCaseSensitive )
{
}

VistaPropertyList::VistaPropertyList( const VistaPropertyList& oCopy )
: std::map<std::string, VistaProperty, VistaAspectsComparisonStuff::StringCompare>( oCopy )
{

}

VistaPropertyList::~VistaPropertyList()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPropertyList::GetIsCaseSensitive() const
{
	return m_bIsCaseSensitive;
}

bool VistaPropertyList::SetIsCaseSensitive( const bool bSet,
											const bool bConvertSubPropLists,
											const bool bFailOnNameClash  )
{
	if( m_bIsCaseSensitive == bSet )
		return true;

	// we need to remove and re-insert everything. For this, we first
	// copy ourselve, and then copy our content
	VistaPropertyList oTmpList;
	if( ConvertPropList( *this, oTmpList, bSet, bConvertSubPropLists, bFailOnNameClash ) )
	{
		(*this) = oTmpList;
		return true;
	}
	return false;
}


string VistaPropertyList::GetStringValueTyped(const string &sPropName, VistaProperty::ePropType eType) const
{
	VistaPropertyList::const_iterator cit = find(sPropName);
	if(cit != end())
	{
#if 0
		if((*cit).second.GetPropertyType() != eType)
			vasperr << "PropertyList::GetStringValue(" << sPropName << ") -- WARNING, "
				 << "type mismatch (requested=["
				 << VistaProperty::GetPropTypeName(eType) <<"], is=["
				 << VistaProperty::GetPropTypeName((*cit).second.GetPropertyType())
				 << "])\n";
#endif // DEBUG
		return (*cit).second.GetValue();
	}
	return "";
}

string VistaPropertyList::GetStringValue(const string &sPropName) const
{
	return GetStringValueTyped(sPropName, VistaProperty::PROPT_STRING);
}

int VistaPropertyList::GetIntValue(const string &sPropName) const
{
	string sTmp = GetStringValueTyped(sPropName, VistaProperty::PROPT_INT);
	return VistaAspectsConversionStuff::ConvertToInt(sTmp);
}

double VistaPropertyList::GetDoubleValue(const string &sPropName) const
{
	string sTmp = GetStringValueTyped(sPropName, VistaProperty::PROPT_DOUBLE);
	return VistaAspectsConversionStuff::ConvertToDouble(sTmp);
}

bool VistaPropertyList::GetBoolValue(const string &sPropName) const
{
	string sTmp = GetStringValueTyped(sPropName, VistaProperty::PROPT_BOOL);
	return VistaAspectsConversionStuff::ConvertToBool(sTmp);
}

void *VistaPropertyList::GetIdValue(const string &sPropName) const
{
	string sTmp = GetStringValueTyped(sPropName, VistaProperty::PROPT_ID);
	return VistaAspectsConversionStuff::ConvertToId(sTmp);
}

int VistaPropertyList::GetStringListValue(const string &sPropName, list<string> &liStrings) const
{
	string sTmp = GetStringValueTyped(sPropName, VistaProperty::PROPT_LIST);
	return VistaAspectsConversionStuff::ConvertToList(sTmp, liStrings);
}

VistaPropertyList VistaPropertyList::GetPropertyListValue(const string &sPropName) const
{
	VistaPropertyList::const_iterator cit = find(sPropName);
	if(cit != end())
	{
		const VistaProperty &prop = (*cit).second;
		return prop.GetPropertyListValue(); // this should do
	}
	return VistaPropertyList();
}

int VistaPropertyList::DeSerializePropertyList(IVistaDeSerializer &rDeSer, VistaPropertyList &rPropertyList, std::string &sName)
{
	int iRet = 0;
	int iCount = 0;
	int iNameSize = 0;
	bool bCaseSensitive = false;
	iRet += rDeSer.ReadInt32(iCount);
	iRet += rDeSer.ReadInt32(iNameSize);
	iRet += rDeSer.ReadString(sName, iNameSize);
	iRet += rDeSer.ReadBool( bCaseSensitive );

	for(int i=0; i < iCount; ++i)
	{
		int iKeySize, iValueSize, iType, nListSubType;
		iKeySize = iValueSize = 0;
		string key, value;

		iRet += rDeSer.ReadInt32(iType);
		if(iType == (int)VistaProperty::PROPT_PROPERTYLIST)
		{
			VistaPropertyList oList;
			std::string subName;
			iRet += DeSerializePropertyList(rDeSer, oList, subName);
			rPropertyList.SetPropertyListValue(subName, oList);
		}
		else
		{
			iRet += rDeSer.ReadInt32(iKeySize);
			iRet += rDeSer.ReadString(key, iKeySize);
			iRet += rDeSer.ReadInt32(iValueSize);
			iRet += rDeSer.ReadString(value, iValueSize);
			iRet += rDeSer.ReadInt32(nListSubType);
			rPropertyList.SetStringValueTyped(key, value,
											  VistaProperty::GetPropTypeEnum(iType),
											  VistaProperty::GetPropTypeEnum(nListSubType));
		}

	}

	return iRet;
}


int VistaPropertyList::SerializePropertyList(IVistaSerializer &rSer,
				 const VistaPropertyList &rPropertyList,
				 const std::string &sPropertyListName)
{
	int iRet = 0;
	iRet += rSer.WriteInt32(int(rPropertyList.size()));
	iRet += rSer.WriteInt32(int(sPropertyListName.size()));
	iRet += rSer.WriteString(sPropertyListName);
	iRet += rSer.WriteBool( rPropertyList.GetIsCaseSensitive() );

	for(VistaPropertyList::const_iterator cit = rPropertyList.begin();
		cit != rPropertyList.end(); ++cit)
	{
		string key = (*cit).first;
		iRet += rSer.WriteInt32((int)(*cit).second.GetPropertyType());
		if((*cit).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST)
		{
			iRet += SerializePropertyList(rSer,
				(*cit).second.GetPropertyListValue(),
				key);
		}
		else
		{
			//string value = (*cit).second;
			string value = (*cit).second.GetValue();
			iRet += rSer.WriteInt32(int(key.size()));
			iRet += rSer.WriteString(key);
			iRet += rSer.WriteInt32(int(value.size()));
			iRet += rSer.WriteString(value);
			iRet += rSer.WriteInt32((int)(*cit).second.GetPropertyListSubType());
		}
	}
	return iRet;
}

VistaProperty VistaPropertyList::GetProperty(const string &sPropName) const
{
	VistaPropertyList::const_iterator cit = find(sPropName);

	if (cit != end())
		return (*cit).second;
	else
		return VistaProperty();
}

VistaProperty &VistaPropertyList::GetPropertyRef(const string &sPropName)
{
	VistaPropertyList::iterator it = find(sPropName);

	if (it != end())
		return (*it).second;
	else
		return oNilProp;
}

const VistaProperty &VistaPropertyList::GetPropertyConstRef(const string &sPropName) const
{
	VistaPropertyList::const_iterator cit = find(sPropName);

	if (cit != end())
		return (*cit).second;
	else
		return oNilProp;
}

void VistaPropertyList::SetStringValueTyped(const string &sPropName,
									   const string &sValue,
									   VistaProperty::ePropType eType,
									   VistaProperty::ePropType eListType)
{
	VistaProperty prop(sPropName);
	prop.SetValue(sValue);
	prop.SetPropertyType(eType);
	prop.SetPropertyListSubType(eListType);

	(*this)[sPropName] = prop;
}

void VistaPropertyList::SetProperty( const VistaProperty & propVal )
{
	std::string strPropName = propVal.GetNameForNameable();
	(*this)[strPropName] = propVal;
}

void VistaPropertyList::SetStringValue(const string &propName, const string &propValue)
{
	SetStringValueTyped(propName, propValue, VistaProperty::PROPT_STRING, VistaProperty::PROPT_NIL);
}


void VistaPropertyList::SetIntValue(const string &sPropName, int iPropValue)
{
	SetStringValueTyped(sPropName, VistaAspectsConversionStuff::ConvertToString(iPropValue),
		VistaProperty::PROPT_INT, VistaProperty::PROPT_NIL);
}

void VistaPropertyList::SetDoubleValue(const string &sPropName, double dPropValue)
{
	SetStringValueTyped(sPropName, VistaAspectsConversionStuff::ConvertToString(dPropValue),
		VistaProperty::PROPT_DOUBLE, VistaProperty::PROPT_NIL);
}

void VistaPropertyList::SetBoolValue(const string &sPropName, bool bPropValue)
{
	SetStringValueTyped(sPropName, (bPropValue ? "TRUE" : "FALSE"),
		VistaProperty::PROPT_BOOL, VistaProperty::PROPT_NIL);
}

void VistaPropertyList::SetIdValue(const string &sPropName, void *pPropValue)
{
	SetStringValueTyped(sPropName, VistaAspectsConversionStuff::ConvertToString(pPropValue),
		VistaProperty::PROPT_ID, VistaProperty::PROPT_NIL);
}

void VistaPropertyList::SetStringListValue(const string &sPropName, const list<string> &liStrings)
{
	SetStringValueTyped(sPropName, VistaAspectsConversionStuff::ConvertToString(liStrings),
		VistaProperty::PROPT_LIST, VistaProperty::PROPT_STRING);
}

void VistaPropertyList::SetPropertyListValue(const string &sPropName, const VistaPropertyList &refList)
{
	VistaProperty prop(sPropName);
	(*this)[sPropName] = prop;
	VistaProperty &refProp = (*this)[sPropName];
	refProp.SetPropertyListValue(refList);
	refProp.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
}

bool VistaPropertyList::HasProperty(const string &sPropName) const
{
	return find(sPropName) != end();
}

bool VistaPropertyList::RemoveProperty(const std::string &sPropName)
{
	VistaPropertyList::iterator it = find(sPropName);
	if(it != end())
	{
		erase(it);
		return true;
	}
	return false;
}

void VistaPropertyList::PrintPropertyList(int iDepth) const
{
	Print(vaspout, iDepth);
}

void VistaPropertyList::Print(ostream &out, int iDepth) const
{
	string strPrefix;
	for (int i=0; i<iDepth; ++i)
		strPrefix += "  ";

	for (VistaPropertyList::const_iterator cit=begin(); cit!=end(); ++cit)
	{
		out << strPrefix << "[" << (*cit).second.GetNameForNameable() << "]";


		if ((*cit).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST)
		{
			out << " : TYPE [" << VistaProperty::GetPropTypeName( (*cit).second.GetPropertyType() ) << "]\n";
			out << strPrefix << "### BEGIN [" << (*cit).second.GetNameForNameable() << "] ###\n";
			const VistaPropertyList &refProps = (*cit).second.GetPropertyListConstRef();
			refProps.Print(out, iDepth+1);
			out << strPrefix << "### END [" << (*cit).second.GetNameForNameable() << "] ###\n";
		}
		else
		{
			out << " = [" << (*cit).second.GetValue() << "] : TYPE [";
			out << VistaProperty::GetPropTypeName( (*cit).second.GetPropertyType() ) << ", "
				<< VistaProperty::GetPropTypeName( (*cit).second.GetPropertyListSubType() ) << "]\n";
		}
	}
}

VistaPropertyList VistaPropertyList::MergePropertyLists(const VistaPropertyList &oMaster, const VistaPropertyList &oMergeIn)
{
	VistaPropertyList oRet = oMaster;
	for(VistaPropertyList::const_iterator cit = oMergeIn.begin(); cit != oMergeIn.end(); ++cit)
	{
		if((*cit).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST)
		{
			if(oMaster.HasProperty((*cit).first)
				&& (oMaster((*cit).first).GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST))
			{
				oRet.SetPropertyListValue((*cit).first,
									 MergePropertyLists(
									   oMaster((*cit).first).GetPropertyListConstRef(),
									   (*cit).second.GetPropertyListConstRef()));
			}
			else
			{
				oRet.SetPropertyListValue((*cit).first, (*cit).second.GetPropertyListConstRef());
			}
		}
		else
		{
			// overwrite value from oMergeIn in oMaster
			oRet[(*cit).first] = (*cit).second;
		}
	}
	return oRet;
}

bool VistaPropertyList::MergeWith(const VistaPropertyList &oMergeIn)
{
	for(VistaPropertyList::const_iterator cit = oMergeIn.begin(); cit != oMergeIn.end(); ++cit)
	{
	    if((*cit).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST)
	    {
	        if(HasProperty((*cit).first)
	            && ((*this)((*cit).first).GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST))
	        {
	            SetPropertyListValue((*cit).first,
	                                 MergePropertyLists(
	                                   (*this)((*cit).first).GetPropertyListConstRef(),
	                                   (*cit).second.GetPropertyListConstRef()));
	        }
	        else
	        {
	            SetPropertyListValue((*cit).first, (*cit).second.GetPropertyListConstRef());
	        }
	    }
	    else
	    {
	        // overwrite value from oMergeIn in oMaster
	        (*this)[(*cit).first] = (*cit).second;
	    }
	}

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

