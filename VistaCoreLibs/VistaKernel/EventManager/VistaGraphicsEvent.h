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

#ifndef _VISTAGRAPHICSEVENT_H
#define _VISTAGRAPHICSEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaGraphicsEvent - well, guess?!
 */
class VISTAKERNELAPI VistaGraphicsEvent : public VistaEvent
{
public:
	/**
	 * Possible ids for graphics events
	 */
	enum VISTAKERNELAPI EVENT_ID
	{
		VGE_INVALID				= -1,
		VGE_LIGHT_DIRECTION		= 0,	// this is the only one, that's used, so far...
		VGE_UPPER_BOUND					// this one has to remain here for sanity checks...
	};

public:
	/**
	 * Creates a VistaGraphicsEvent object
	 *
	 */
	VistaGraphicsEvent();

	/**
	 * Destroys a VistaGraphicsEvent object
	 *
	 */
	virtual ~VistaGraphicsEvent();

	/**
	 * Returns the view direction. (FOR LIGHT_DIRECTION EVENT ONLY!!!)
	 *
	 * @RETURN  const VistaVector3D &
	 */
	const VistaVector3D & GetViewDirection() const;

	/**
	 * Sets a "light direction"-event.
	 * @param   VistaVector3D & vViewDir
	 * @RETURN  bool    true=success / false=failure (i.e. id=INVALID)
	 */
	bool SetLightDirectionEvent(const VistaVector3D & vViewDir);

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
	 * Think of this as "SAVE"
	 */
	virtual int Serialize(IVistaSerializer &) const;

	/**
	 * Think of this as "LOAD"
	 */
	virtual int DeSerialize(IVistaDeSerializer &);

	static int GetTypeId();
	static void SetTypeId(int nId);
	static std::string GetIdString(int nId);
protected:

	VistaVector3D	m_vViewDir;	// used for VGE_LIGHT_DIRECTION only...

private:
	static int m_nEventId;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/


#endif //_VISTAGRAPHICSEVENT_H
