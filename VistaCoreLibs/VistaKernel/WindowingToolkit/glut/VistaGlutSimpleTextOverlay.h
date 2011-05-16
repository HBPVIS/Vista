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

#ifndef _VISTAGLUTSIMPLETEXTOVERLAY_H
#define _VISTAGLUTSIMPLETEXTOVERLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaSimpleTextOverlay.h"

#include "../../VistaKernelConfig.h"

#include "../../GraphicsManager/VistaGeometry.h"

#include "../../DisplayManager/VistaSceneOverlay.h"
#include "../../DisplayManager/VistaViewport.h"
#include "../../DisplayManager/VistaWindow.h"

#include "../../EventManager/VistaEventHandler.h"
#include "../../EventManager/VistaDisplayEvent.h"
#include "../../EventManager/VistaEventManager.h"

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaEventManager;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 *  This is a simple rastered overlay that is creating an orthogonal overlay
 *  for a glut font of height 18
 */
class VISTAKERNELAPI VistaGlutSimpleTextOverlay : public IVistaSimpleTextOverlay
{
public:

	VistaGlutSimpleTextOverlay(VistaDisplayManager *pMgr,
							VistaEventManager *pEvMgr,
			              unsigned int nWidth=120, unsigned int nHeight=90);
	virtual ~VistaGlutSimpleTextOverlay();

	virtual bool Do ();

	bool AddText(IVistaTextEntity *text);
	bool RemText(IVistaTextEntity *text);

	bool GetIsEnabled() const;
	void SetIsEnabled(bool bEnabled);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	/**
	 * @todo document
	 * Size is devided by 20. I am not really sure why it is
	 * like this. So if someone knows the reason please document here
	 */
	void SetWidth(unsigned int);
	void SetHeight(unsigned int);

	/**
	 * Internal EventHandler to handle VDE_RATIOCHANGE events.
	 * Those are fired, when window size changes.
	 * This Handler is not in the interface cause there maybe some
	 * other handling on the events.
	 */
	class VISTAKERNELAPI CRatioChangeHandler: public VistaEventHandler
	{
	public:
		/**
		 * The Constructor needs the EventManager and the Overlay to
		 * handle the events properly
		 * \param EventManager
		 * \param TextOverlay
		 */
		CRatioChangeHandler(VistaEventManager *pEvMgr, VistaGlutSimpleTextOverlay *pOverlay);
		virtual ~CRatioChangeHandler();

		/**
		 * Handle the fired event. Check type and send the
		 * size to the VistaSimpleTextOverlay implementation
		 * given in m_pOverlay;
		 * \param Event
		 */
		virtual void HandleEvent(VistaEvent *pEvent);

	private:
		VistaEventManager *m_pEvMgr;
		VistaGlutSimpleTextOverlay *m_pOverlay;
	};

private:

	VistaDisplayManager *m_pMgr;
	unsigned int m_nWidth,
	             m_nHeight;

	std::list<IVistaTextEntity*> m_liTexts;
	bool m_bEnabled;

	VistaGlutSimpleTextOverlay::CRatioChangeHandler *m_pObs;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAGLUTSIMPLETEXTOVERLAY_H

