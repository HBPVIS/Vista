/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#include <string>
using namespace std;

#include "VistaPropertyFunctor.h"
#include "VistaPropertyFunctorRegistry.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaPropertyGetFunctor::IVistaPropertyGetFunctor(const string &sPropName,
								const string &sClassName,
								const string &sDescription)
: m_sFunctorName(sPropName),
  m_sFunctorClass(sClassName),
  m_sDescription(sDescription)
{
	m_pRegistry=VistaPropertyFunctorRegistry::GetSingleton();
	// update reference count, as we store the pointer
	VistaPropertyFunctorRegistry::refup(m_pRegistry);
	m_pRegistry->RegisterGetter(sPropName, sClassName, this);
}

IVistaPropertyGetFunctor::~IVistaPropertyGetFunctor()
{
	m_pRegistry->UnregisterGetter( m_sFunctorName, m_sFunctorClass );
	VistaPropertyFunctorRegistry::refdown(m_pRegistry);
}


// #############################################################################

IVistaPropertySetFunctor::IVistaPropertySetFunctor(const string &sPropName,
						const string &sClassName,
						const string &sDescription)
: m_sFunctorName(sPropName),
  m_sFunctorClass(sClassName),
  m_sDescription(sDescription)
{
	m_pRegistry=VistaPropertyFunctorRegistry::GetSingleton();
	// update reference count, as we store the pointer
	VistaPropertyFunctorRegistry::refup(m_pRegistry);
	m_pRegistry->RegisterSetter(sPropName, sClassName, this);
}

IVistaPropertySetFunctor::~IVistaPropertySetFunctor()
{
	m_pRegistry->UnregisterSetter(m_sFunctorName, m_sFunctorClass);
	VistaPropertyFunctorRegistry::refdown(m_pRegistry);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void IVistaPropertyGetFunctor::SetNameForNameable(const string &sFunctorName)
{
	m_sFunctorName = sFunctorName;
}

string IVistaPropertyGetFunctor::GetNameForNameable() const
{
	return m_sFunctorName;
}

string IVistaPropertyGetFunctor::GetPropertyDescription() const
{
	return m_sDescription;
}

string IVistaPropertyGetFunctor::GetPropertyClassName() const
{
	return m_sFunctorClass;
}
// #############################################################################

void IVistaPropertySetFunctor::SetNameForNameable(const string &sFunctorName)
{
	m_sFunctorName = sFunctorName;
}

string IVistaPropertySetFunctor::GetNameForNameable() const
{
	return m_sFunctorName;
}

string IVistaPropertySetFunctor::GetPropertyDescription() const
{
	return m_sDescription;
}

string IVistaPropertySetFunctor::GetPropertyClassName() const
{
	return m_sFunctorClass;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

