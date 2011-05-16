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

#ifndef _VISTADISPLAYEVENT_H
#define _VISTADISPLAYEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEvent.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaViewport;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaDisplayEvent - well, guess?!
 */
class VISTAKERNELAPI VistaDisplayEvent : public VistaEvent
{
public:
	/**
	 * Possible ids for display events
	 */
	enum EVENT_ID
	{
		VDE_INVALID		= -1,
		VDE_UPDATE		= 0,
		VDE_RATIOCHANGE,
		VDE_POSITIONCHANGE,
		VDE_UPPER_BOUND			// this one has to remain here for sanity checks...
	};

public:
	/**
	 * Creates a VistaDisplayEvent object
	 *
	 */
	VistaDisplayEvent();

	/**
	 * Destroys a VistaDisplayEvent object
	 *
	 */
	virtual ~VistaDisplayEvent();


	/**
	 * Returns the name of the event.
	 *
	 *
	 * @param   --
	 * @RETURN  std::string
	 */
	virtual std::string GetName() const;

	/**
	 * Prints out some debug information to the given output stream.
	 *
	 *
	 * @param   std::ostream & out
	 * @RETURN  void
	 */
	virtual void  Debug(std::ostream & out) const;

	/**
	 * Sets the id of the event.
	 *
	 * @param   int iId
	 * @RETURN  bool    true=success / false=failure (i.e. id=INVALID)
	 */
	virtual bool	SetId(int iId);

	/**
	 *  This method is used by the invoker of this event.
	 *  EventHandlers must not use it (unless they know
	 *  what they are doing...)
	 * @param pPort the viewport that was changed
	 */
	void SetViewport(VistaViewport *pPort);

	/**
	 * Returns the viewport on which a change happened.
	 * This is valid (!= NULL for ID_RATIOCHANGED event ids
	 * @see GetId()
	 * @see SetViewport()
	 * @return a pointer to the viewport that changed, or NULL
	 */
	VistaViewport *GetViewport() const;

	static int GetTypeId();
	static void SetTypeId(int nId);
	static std::string GetIdString(int nId);
protected:
	VistaViewport *m_pViewport;
private:
	static int m_nEventId;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/


#endif //_VISTADISPLAYEVENT_H

