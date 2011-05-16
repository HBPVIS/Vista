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

#ifndef _VISTAGLUT3DTEXTOVERLAY_H
#define _VISTAGLUT3DTEXTOVERLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../Vista3DTextOverlay.h"

#include "../../VistaKernelConfig.h"

#include "../../DisplayManager/VistaSceneOverlay.h"

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaDisplaySystem;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaGlut3DTextOverlay : public IVista3DTextOverlay
{
public:
	VistaGlut3DTextOverlay(VistaDisplayManager *pMgr);
	virtual ~VistaGlut3DTextOverlay();

	virtual bool Do ();

	bool AddText(IVistaTextEntity *text);
	bool RemText(IVistaTextEntity *text);

	bool GetIsEnabled() const;
	void SetIsEnabled(bool bEnabled);

private:
	std::list<IVistaTextEntity*> m_liTexts;
	bool m_bEnabled;
	VistaDisplayManager *m_pDisplayMgr;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAGLUTSIMPLETEXTOVERLAY_H

