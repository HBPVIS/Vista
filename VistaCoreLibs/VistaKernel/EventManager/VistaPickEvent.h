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

#ifndef _VISTAPICKEVENT_H
#define _VISTAPICKEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include "VistaEvent.h"
#include "../PickManager/VistaPickManager.h"
#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
//class VistaPickable;
//class VistaInputDevice;
//class IVistaNode;

class VistaOldInteractionManager;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaPickEvent - well, guess?!
 */
class VISTAKERNELAPI VistaPickEvent : public VistaEvent
{
public:
	/**
	 * Possible ids for pick events.
	 */
	enum VISTAKERNELAPI EVENT_ID
	{
		//VPE_INVALID		= -1,
		VPE_PICKED		= 0,
		VPE_TOUCHED,
		VPE_UPPER_BOUND			// this one has to remain here for sanity checks...
	};

public:
	/**
	 * Creates a VistaPickEvent object
	 */
	VistaPickEvent();

	/**
	 * Destroys a VistaPickEvent object
	 */
	virtual ~VistaPickEvent();

	/**
	 * Retrieve the scene graph node, which has been picked or touched.
	 */
	IVistaNode *GetNode() const;

	/**
	 * Retrieve contact information for a pick event. This method
	 * returns NULL, if the event is not a PICKED but a TOUCHED
	 * event.
	 */
	const VistaPickManager::SContactData *GetContactData() const;

	/**
	 * Returns the VistaPickable object for the picked or touched object.
	 */
	VistaPickable *GetPickable() const;

	VistaPickManager::CPickQuery   *GetPickQuery() const;
	/**
	 * Returns the number of contacts for a TOUCHED event. For a PICKED event
	 * this method returns -1.
	 * NOTE: TOUCHED events are fired, whenever the number of contacts changes
	 * for an object. It is up to the application developer to make use of
	 * the given information (or query the pick manager for further information).
	 */
	int GetNumberOfContacts() const;

	/**
	 * Set a 'picked' event.
	 */
	void SetPickedEvent(IVistaNode *pNode, VistaPickManager::SContactData *pData,
		VistaPickable *pPickable = NULL);

	/**
	 * Sets a 'touched' event.
	 */
	void SetTouchedEvent(IVistaNode *pNode, int iNumberOfContacts,
		VistaPickManager::SContactData *pData, VistaPickable *pPickable = NULL);

	/**
	 * Returns the name of the event.
	 */
	virtual std::string GetName() const;

	/**
	 * Prints out some debug information to the given output stream.
	 */
	virtual void  Debug(std::ostream & out) const;

	static int GetTypeId();
	static void SetTypeId(int nId);
	static std::string GetIdString(int nId);
protected:
	IVistaNode *m_pNode;
	VistaPickable *m_pPickable;
	VistaPickManager::SContactData *m_pContactData;
	VistaPickManager::CPickQuery   *m_pPickQuery;
	int m_iNumberOfContacts;

private:
	static int m_nEventId;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/

inline IVistaNode *VistaPickEvent::GetNode() const
{
	return m_pNode;
}

inline const VistaPickManager::SContactData *VistaPickEvent::GetContactData() const
{
//	m_pContactData->fDistance = 0;
	return m_pContactData;
}

inline VistaPickable *VistaPickEvent::GetPickable() const
{
	return m_pPickable;
}

inline int VistaPickEvent::GetNumberOfContacts() const
{
	return m_iNumberOfContacts;
}

#endif // _VISTAPICKEVENT_H
