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

#ifndef _VISTAEVENT_H
#define _VISTAEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaSerializable.h>
#include <VistaBase/VistaBaseTypes.h>

#include <VistaKernel/VistaKernelConfig.h>

#include <string>
#include <iostream>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEventManager;

// prototypes
class   VistaEvent;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaEvent & );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaEvent - the mother of all events ;-)
 */
class VISTAKERNELAPI VistaEvent : public IVistaSerializable
{
public:
	/**
	 * Event types, supported by Vista
	 */
	enum VISTAKERNELAPI EVENT_TYPE
	{
	VET_INVALID		= -1,
	VET_ALL			= -2
	};

	enum VISTAKERNELAPI EVENT_ID
	{
		VEID_NONE = -1,
		VEID_LAST
	};

public:
	/**
	 * Destroy the event.
	 *
	 */
	virtual ~VistaEvent();

	/**
	 * Returns the system time of the event's creation
	 *
	 * @RETURN  double    event creation time
	 */
	VistaType::microtime	GetTime() const;

	/**
	 * Returns the type of the event
	 *
	 * @RETURN  int    event type id
	 */
	int		GetType() const;

	/**
	 * Returns, whether the event has already been handled
	 *
	 * @RETURN  bool   true = already handled / false = not handled yet
	 */
	bool	IsHandled() const;

	/**
	 * Sets the handling state of the object.
	 * Use this to mark the event as being handled.
	 *
	 * @param   bool bHandled
	 * @RETURN  --
	 */
	void	SetHandled(bool bHandled);

	/**
	 * Returns the name of the event.
	 *
	 * @param   --
	 * @RETURN  std::string
	 */
	virtual std::string GetName() const;

	/**
	 * Prints out some debug information to the given output stream.
	 *
	 * @param   std::ostream & out
	 * @RETURN  void
	 */
	virtual void  Debug(std::ostream & out) const;

	/**
	 * Returns the event id
	 *
	 * @RETURN  int    event id
	 */
	int GetId() const;

	/**
	 * Sets the id of the event.
	 *
	 * @param   int iId
	 * @RETURN  bool    true=success / false=failure (i.e. id=INVALID)
	 */
	virtual bool	SetId(int iId);


	/**
	 * Think of this as "SAVE"
	 */
	virtual int Serialize(IVistaSerializer &) const;

	/**
	 * Think of this as "LOAD"
	 */
	virtual int DeSerialize(IVistaDeSerializer &);

	virtual std::string GetSignature() const;

	static int GetTypeId();
	static void SetTypeId(int nId);

	static std::string GetIdString(int nId);

	int GetCount() const;

protected:
	VistaEvent();		// avoid instantiation of this class -> use a derived class, instead!
	void SetType(int iType);

	bool	m_bHandled;	// already handled?
	VistaType::microtime	m_dTime;	// creation time of event (gets set by CEventManager)

private:
	static int  m_nEventId;
	int		m_iType;	// the type of the event
	int	    m_iId;		// the event id
	int     m_nCnt;

	friend class VistaEventManager;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/
inline VistaEvent::VistaEvent()
: m_iType(VET_INVALID), m_bHandled(false), m_dTime(0), m_iId(VEID_NONE), m_nCnt(0)
{
}

inline VistaType::microtime VistaEvent::GetTime() const
{
	return m_dTime;
}

inline bool VistaEvent::IsHandled() const
{
	return m_bHandled;
}

inline void VistaEvent::SetHandled(bool bHandled)
{
	m_bHandled = bHandled;
}

inline int VistaEvent::GetId() const
{
	return m_iId;
}

inline bool	VistaEvent::SetId(int iId)
{
	m_iId = iId;
	return true;
};


#endif //_VISTAEVENT_H

