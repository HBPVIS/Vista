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
// $Id: VistaReferenceCountable.h 22143 2011-07-01 15:07:00Z dr165799 $

#ifndef _VISTAREFERENCECOUNTABLE_H
#define _VISTAREFERENCECOUNTABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAASPECTSAPI IVistaReferenceCountable
{
public:
	virtual ~IVistaReferenceCountable();
	/**
	 * increases the reference count on this value.
	 * @param val the std::string value to reference-count
	 */
	inline static void refup(IVistaReferenceCountable *val);

	/**
	 * decreases reference counting on this value.
	 * iff refCount == 0, the instance at val will be DELETED!
	 * @param val the std::string-value to be dereferenced
	 */
	inline static bool refdown(IVistaReferenceCountable *val);

	inline int getcount() const
	{
		return m_iReferenceCount;
	};
protected:
	IVistaReferenceCountable();
		IVistaReferenceCountable(const IVistaReferenceCountable &);
private:

	int m_iReferenceCount;

};

class VISTAASPECTSAPI IVistaReferenceObj
{
public:
	IVistaReferenceObj(IVistaReferenceCountable &rCnt)
		: m_rObj(rCnt)
		{
			IVistaReferenceCountable::refup(&m_rObj);
		};

		~IVistaReferenceObj()
		{
			IVistaReferenceCountable::refdown(&m_rObj);
		};
private:
	IVistaReferenceCountable &m_rObj;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

inline void IVistaReferenceCountable::refup(IVistaReferenceCountable *val)
{
	++val->IVistaReferenceCountable::m_iReferenceCount;
};

inline bool IVistaReferenceCountable::refdown(IVistaReferenceCountable *val)
{
	--val->IVistaReferenceCountable::m_iReferenceCount;
	if(val->IVistaReferenceCountable::m_iReferenceCount <= 0)
	{
		   delete val;
		   return true;
	}
	return false;
};
#endif //_VISTAREFERENCECOUNTABLE_H

