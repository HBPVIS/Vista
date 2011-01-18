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

#include "VistaReflectionable.h"
#include "VistaPropertyFunctorRegistry.h"

#include <set>
#include <iostream>
#include <cstdio>
using namespace std;
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
static const string SsReflectionType("IVistaReflectionable");

static IVistaPropertyGetFunctor *aCgFunctors[] =
{
	new TVistaPropertyGet<string, IVistaReflectionable>
		  ("NAME", SsReflectionType,
		  &IVistaReflectionable::GetNameForNameable),
	new TVistaPropertyGet<string, IVistaReflectionable>
		  ("TYPE", SsReflectionType,
		  &IVistaReflectionable::GetReflectionableType),
		  new TVistaPropertyGet<void*, IVistaReflectionable, VistaProperty::PROPT_ID>
		  ("NAME_ID", SsReflectionType,
		  &IVistaReflectionable::GetNameableId),
	NULL
};

static IVistaPropertySetFunctor *aCsFunctors[] =
{
	new TVistaPropertySet<const string &, string,
						IVistaReflectionable>
		("NAME", SsReflectionType,
		&IVistaReflectionable::SetNameProp,
		&VistaAspectsConversionStuff::ConvertToString),
	NULL
};

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaReflectionable::IVistaReflectionable()
{
}

IVistaReflectionable::~IVistaReflectionable()
{
}

IVistaReflectionable::IVistaReflectionable(const IVistaReflectionable &oOther)
: IVistaPropertyAwareable(oOther),
  IVistaObserveable(oOther),
  IVistaNameable(oOther)
{
}

IVistaReflectionable &IVistaReflectionable::operator=(const IVistaReflectionable &oOther)
{
	if(oOther.m_liBaseTypeList.empty())
		return *this; // INVALID CALL

	// build base type list for ourselves
	ValidateBaseTypeList();

	// assigning for reflectionables means to exchange
	// as much of the properties as possible
	// * note that "nameable" is not assigned (names define
	//   entities and thus must not be copied

	// we call assignment as observeable
	// because we do not know what happens there
	IVistaObserveable::operator=(oOther);


	// lets emulate the non-const property get ans set
	// (we make it const by avoiding to build the
	// base type list with oOther.ValidateBaseTypeList()).


	VistaPropertyFunctorRegistry *pR = VistaPropertyFunctorRegistry::GetSingleton();
	IVistaReferenceObj o(*pR);


	set<string> setSymbols;

	// claim the getter symbols for symbols from oOther
	pR->GetGetterSymbolSet(setSymbols,oOther.m_liBaseTypeList);

	// storage list
	bool b=GetNotificationFlag();
	bool d = false;
	SetNotificationFlag(false);
	for(set<string>::const_iterator sit = setSymbols.begin();
		sit != setSymbols.end(); ++sit)
	{
		VistaProperty pTmp(*sit);

		// try to get a functor for a property
		IVistaPropertyGetFunctor *pFunctor = pR->GetGetFunctor(*sit,
												  oOther.GetReflectionableType(),
												  oOther.m_liBaseTypeList);

		if(pFunctor && (*pFunctor)(oOther, pTmp))
		{
			d = true;
			SetProperty(pTmp);
		}
	}

	SetNotificationFlag(b);

	// we changed at least one property, so do a notify
	// for the setchange
	if(d)
		Notify(MSG_SETCHANGE);

	return *this;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void IVistaReflectionable::ValidateBaseTypeList()
{
	if(m_liBaseTypeList.empty())
	{
		// build base-type list (once is enough)
		GetBaseTypes(m_liBaseTypeList);
	}
}

int IVistaReflectionable::SetProperty(const VistaProperty &rProp)
{
	ValidateBaseTypeList();

	VistaPropertyFunctorRegistry *pR = VistaPropertyFunctorRegistry::GetSingleton();
	IVistaReferenceObj o(*pR);

	IVistaPropertySetFunctor *pFunctor = pR->GetSetFunctor(rProp.GetNameForNameable(),
													  GetReflectionableType(),
													  m_liBaseTypeList);
	if(pFunctor)
	{
		bool b=(*pFunctor)(*this, rProp);
		if(b)
			return 1;
		else
			return 0;
	}
#if defined(DEBUG)
	else
	{
		printf("IVistaReflectionable::SetProperty(%s) -- no set-functor found [%s]\n",
			   rProp.GetNameForNameable().c_str(), GetReflectionableType().c_str());
	}
#endif
	return 0;
}

int IVistaReflectionable::GetProperty(VistaProperty &rProp)
{
	ValidateBaseTypeList();
	// find functor
	VistaPropertyFunctorRegistry *pR = VistaPropertyFunctorRegistry::GetSingleton();
	IVistaReferenceObj o(*pR);

	IVistaPropertyGetFunctor *pFunctor = pR->GetGetFunctor(rProp.GetNameForNameable(),
													  GetReflectionableType(),
													  m_liBaseTypeList);
	if(pFunctor)
	{
		bool b=(*pFunctor)(*this, rProp);
		if(b)
			return 1;
		else
			return 0;
	}
#if defined(DEBUG)
	else
	{
		printf("IVistaReflectionable::GetProperty(%s) -- no get-functor found [%s]\n",
			   rProp.GetNameForNameable().c_str(), GetReflectionableType().c_str());
	}
#endif
	return 0;
}

VistaProperty IVistaReflectionable::GetPropertyByName(const string &sPropName)
{
	VistaProperty rTmp(sPropName);
	GetProperty(rTmp);
	return rTmp;
}

bool IVistaReflectionable::SetPropertyByName(const string &sPropName,
								const string &sPropValue)
{
	VistaProperty rTmp(sPropName);
	rTmp.SetValue(sPropValue);
	SetProperty(rTmp);
	return true;
}

int IVistaReflectionable::SetPropertiesByList(const VistaPropertyList &rList)
{
	bool bNotify = GetNotificationFlag();
	SetNotificationFlag(false); // disable notification during update
	int iSet = 0;
	for(VistaPropertyList::const_iterator cit = rList.begin();
	   cit != rList.end(); ++cit)
	   {
		iSet += SetProperty((*cit).second);
	   }
	SetNotificationFlag(bNotify); // re-enable notification if desired
	if(iSet>0)
	{
		Notify(MSG_SETCHANGE);
	}
	return iSet;
}

int IVistaReflectionable::GetPropertiesByList(VistaPropertyList &rList)
{
	ValidateBaseTypeList();

	int iSet = 0;
	rList.clear(); // clear container
	// iterate over list of get-functors
	VistaPropertyFunctorRegistry *pR = VistaPropertyFunctorRegistry::GetSingleton();
	IVistaReferenceObj o(*pR);


	set<string> setSymbols;
	pR->GetGetterSymbolSet(setSymbols,m_liBaseTypeList);
	for(set<string>::const_iterator sit = setSymbols.begin();
		sit != setSymbols.end(); ++sit)
		{
			VistaProperty pTmp(*sit);
			iSet += GetProperty(pTmp);
			rList[*sit] = pTmp;
		}
	return iSet;
}


int IVistaReflectionable::GetPropertySymbolList(list<string> &rStorageList)
{
	ValidateBaseTypeList();
	VistaPropertyFunctorRegistry *pR = VistaPropertyFunctorRegistry::GetSingleton();
	IVistaReferenceObj o(*pR);


	set<string> setSymbols;
	int iNum = pR->GetSymbolSet(setSymbols,m_liBaseTypeList);
	#if defined(LINUX)
	rStorageList.assign(setSymbols.begin(), setSymbols.end());
	#else
	for(set<string>::const_iterator sit = setSymbols.begin();
		sit != setSymbols.end(); ++sit)
		{
			rStorageList.push_back(*sit);
		}
	#endif
	return iNum;
}

string IVistaReflectionable::GetPropertyDescription(const string &sPropName)
{
	ValidateBaseTypeList();
	VistaPropertyFunctorRegistry *pR = VistaPropertyFunctorRegistry::GetSingleton();
	IVistaReferenceObj o(*pR);

	std::string sRet;

	IVistaPropertyGetFunctor *pGFunctor = pR->GetGetFunctor(sPropName,
													  GetReflectionableType(),
													  m_liBaseTypeList);
	if(pGFunctor)
		sRet = string("GET:") + pGFunctor->GetPropertyDescription() + string("\n");

	IVistaPropertySetFunctor *pSFunctor = pR->GetSetFunctor(sPropName,
													  GetReflectionableType(),
													  m_liBaseTypeList);
	if(pSFunctor)
		sRet += string("SET:") + pSFunctor->GetPropertyDescription() + string("\n");

	return sRet;
}


string IVistaReflectionable::GetReflectionableType() const
{
	return SsReflectionType;
}

string IVistaReflectionable::GetNameForNameable() const
{
	return m_sName;
}

bool   IVistaReflectionable::SetNameProp(const std::string &sName)
{
	std::string sOldName = GetNameForNameable();
	SetNameForNameable(sName);
	return sName != sOldName;
}

list<string> IVistaReflectionable::GetBaseTypeList() const
{
	list<string> rList;
	// this is a polymorph (maybe expensive) call
	AddToBaseTypeList(rList);
	return rList;
}

int IVistaReflectionable::GetBaseTypes(list<string> &rList) const
{
	// this is a polymorph call (maybe expensive)
	return AddToBaseTypeList(rList);
}


int IVistaReflectionable::AddToBaseTypeList(list<string> &rBtList) const
{
	rBtList.push_back(SsReflectionType);
	return 1;
}

void IVistaReflectionable::SetNameForNameable(const string &sName)
{
	//cout << "IVistaReflectionable::SetNameForNameable() -- "
 //            << sName << endl;
	if(compAndAssignFunc<string>(sName, m_sName) == 1)
	{
		Notify(MSG_NAMECHANGE);
	}
}

