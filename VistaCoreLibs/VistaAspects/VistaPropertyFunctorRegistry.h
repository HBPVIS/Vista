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

#ifndef _VISTAPROPERTYFUNCTORREGISTRY_H
#define _VISTAPROPERTYFUNCTORREGISTRY_H

#include "VistaAspectsConfig.h"

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaReferenceCountable.h"
#include <map>
#include <list>
#include <set>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaPropertyGetFunctor;
class IVistaPropertySetFunctor;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAASPECTSAPI VistaPropertyFunctorRegistry : public IVistaReferenceCountable
{
	friend class CleanupHelper;

	public:
	static VistaPropertyFunctorRegistry *GetSingleton();

	bool RegisterGetter(const std::string &sPropertyName,
						const std::string &sClassType,
						IVistaPropertyGetFunctor *pFunctor,
						bool bForce = false);
	bool UnregisterGetter(const std::string &sPropName,
						  const std::string &sClassType);

	IVistaPropertyGetFunctor *GetGetFunctor(const std::string &sPropName,
										const std::string &sClassType,
										const std::list<std::string> &rLiBaseCl);


	bool RegisterSetter(const std::string &sPropertyName,
						const std::string &sClassType,
						IVistaPropertySetFunctor *pFunctor,
						bool bForce = false);
	bool UnregisterSetter(const std::string &sPropName,
						  const std::string &sClassType);

	IVistaPropertySetFunctor *GetSetFunctor(const std::string &sPropName,
										const std::string &sClassType,
										const std::list<std::string> &rLiBaseCl);

	int GetGetterSymbolSet(std::set<std::string> &setStore,
						   const std::list<std::string> &rLiBaseCl) const;
	int GetSetterSymbolSet(std::set<std::string> &setStore,
						   const std::list<std::string> &rLiBaseCl) const;

	int GetSymbolSet(std::set<std::string> &setStore,
					 const std::list<std::string> &rLiBaseCl) const;

	std::list<IVistaPropertyGetFunctor*> GetGetterByClass(const std::string &strExactMatch) const;
	std::list<IVistaPropertySetFunctor*> GetSetterByClass(const std::string &strExactMatch) const;

	bool GetTalkativeFlag() const;
	void SetTalkativeFlag(bool bTalkative);

	protected:
	private:
		VistaPropertyFunctorRegistry();
		~VistaPropertyFunctorRegistry();


	static VistaPropertyFunctorRegistry *m_pSingleton;

	typedef std::pair<std::string, std::string> STRPAIR;
	typedef std::multimap<std::string, std::set<std::string> > STSTMAP;

	typedef std::map<STRPAIR, IVistaPropertyGetFunctor*> GETFMAP;
	typedef std::map<STRPAIR, IVistaPropertySetFunctor*> SETFMAP;

	STSTMAP m_mpGProps; /**< the gprop-name to class map */
	STSTMAP m_mpSProps; /**< the gprop-name to class map */

	GETFMAP m_mpGetters, m_mpGetterCache;
	SETFMAP m_mpSetters, m_mpSetterCache;

	bool m_bTalkative;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYFUNCTORREGISTRY_H

