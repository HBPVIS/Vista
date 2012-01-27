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

#include "VistaObserveable.h"
#include "VistaObserver.h"
#include <algorithm>
#include <cstdio>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
bool IVistaObserveable::OBSERVER_INFO::operator==(const OBSERVER_INFO &oOther) const
{
	return (m_pObserver == oOther.m_pObserver)
		&& (m_eTicket == oOther.m_eTicket);
}

bool IVistaObserveable::OBSERVER_INFO::operator==(const IVistaObserver *pObs) const
{
	return (m_pObserver == pObs);
}

IVistaObserveable::OBSERVER_INFO::OBSERVER_INFO(IVistaObserver *pObs, int eTicket)
: m_pObserver(pObs), m_eTicket(eTicket)
{
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaObserveable::IVistaObserveable()
:m_bNotificationFlag(true)
{
}

IVistaObserveable::~IVistaObserveable()
{
	//printf("IVistaObserveable[%s]::~IVistaObserveable() @ %X, number of observers: %d\n", m_SObserveableName.c_str(), this, m_lIVistaObservers.size());
	SendDeleteMessage();
}

IVistaObserveable::IVistaObserveable(const IVistaObserveable &oOther)
: m_bNotificationFlag(oOther.m_bNotificationFlag)
{

}

IVistaObserveable &IVistaObserveable::operator=(const IVistaObserveable &oOther)
{
	m_bNotificationFlag = oOther.m_bNotificationFlag;
	return *this;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int IVistaObserveable::AttachObserver(IVistaObserver *pObserver, int eTicket)
{
	m_liVistaObservers.push_back(OBSERVER_INFO(pObserver, eTicket));
	//Notify(MSG_ATTACH);
	return int(m_liVistaObservers.size());
}

int IVistaObserveable::DetachObserver(IVistaObserver *pObserver)
{
	// locate suitable observer
	std::list<OBSERVER_INFO>::iterator it = std::remove(
			m_liVistaObservers.begin(), m_liVistaObservers.end(), pObserver );
	if(it != m_liVistaObservers.end())
	{
		m_liVistaObservers.erase(it, m_liVistaObservers.end());
		//Notify(MSG_DETACH);
	}

	return int(m_liVistaObservers.size());
}

bool IVistaObserveable::SendDeleteRequest()
{
	/**
	 * Observes will probably modify the observer list (detaching),
	 * so we can not traverse the original list.
	 * We will make a copy of the observerlist on which we will
	 * progress and afterwards forget about it
	 */
	std::list<OBSERVER_INFO> liCp = m_liVistaObservers; // we use a copy to traverse this list

	for( std::list<OBSERVER_INFO>::const_iterator it = liCp.begin(); it != liCp.end(); ++it )
	{
		if(!(*it).m_pObserver->ObserveableDeleteRequest(this, (*it).m_eTicket))
			return false;
	}

	return true;
}


void IVistaObserveable::SendDeleteMessage()
{
	/**
	 * Observes will probably modify the observer list (detaching),
	 * so we can not traverse the original list.
	 * We will make a copy of the observerlist on which we will
	 * progress and afterwards forget about it
	 */
	std::list<OBSERVER_INFO> m_liCopy = m_liVistaObservers;

	for(std::list<OBSERVER_INFO>::const_iterator it = m_liCopy.begin(); it != m_liCopy.end(); ++it)
	{
		// command to release me!
		(*it).m_pObserver->ObserveableDelete(static_cast<IVistaObserveable *>(this), (*it).m_eTicket);
	}

	m_liVistaObservers.clear();
}

bool IVistaObserveable::GetIsObservedBy(IVistaObserver *pObs) const
{
	std::list<OBSERVER_INFO>::const_iterator cit = std::find(m_liVistaObservers.begin(),
		m_liVistaObservers.end(), pObs);

	return (cit != m_liVistaObservers.end());
}


void IVistaObserveable::Notify(int msg)
{
	// in case there is no work...
	// do no work ;)
	// this, too, avoids piling of changes in
	// the change set!
	if(m_liVistaObservers.empty())
		return;


	if(!m_bNotificationFlag)
	{
		// the idea is to catch subsequent non-notified
		// messages to give the observers a chance to
		// realize what happened.
		// so we insert the msg to the set, regardless
		// of whether we really call VistaObserver::ObserverUpdate()
		// or not.
		m_sChangedSet.insert(msg);
		return; // NO NOTIFICATION (disabled on user request)!!
	}

	std::list<OBSERVER_INFO>::const_iterator it;
	for(it = m_liVistaObservers.begin(); it != m_liVistaObservers.end(); ++it)
	{
		(*it).m_pObserver->UpdateRequest(this, msg, (*it).m_eTicket);
	}

	if(!m_sChangedSet.empty())
	{
		// ok, we passed through this, so we consider the model to be
		// "unchanged"! we can clear the set now.
		m_sChangedSet.clear();
	}
}

void IVistaObserveable::SetNotificationFlag(bool bNotifyEnabled)
{
	m_bNotificationFlag = bNotifyEnabled;
}

bool IVistaObserveable::GetNotificationFlag() const
{
	return m_bNotificationFlag;
}

const std::set<int> &IVistaObserveable::GetChangedSetConstRef() const
{
	return m_sChangedSet;
}

void IVistaObserveable::ClearChangeSet()
{
	m_sChangedSet.clear();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

