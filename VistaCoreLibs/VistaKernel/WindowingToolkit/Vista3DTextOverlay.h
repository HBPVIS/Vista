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

#ifndef _VISTA3DTEXTOVERLAY_H
#define _VISTA3DTEXTOVERLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaTextEntity.h"

#include "../VistaKernelConfig.h"

#include "../DisplayManager/VistaSceneOverlay.h"

#include "../GraphicsManager/VistaGeometry.h"

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI IVista3DTextOverlay: public IVistaSceneOverlay
{
public:
	virtual ~IVista3DTextOverlay(){};

	/**
	 * \param TextEntity
	 */
	virtual bool AddText(IVistaTextEntity *text) = 0;

	/**
	 * Removes the given text entity from the overlay
	 * \param TextEntity
	 */
	virtual bool RemText(IVistaTextEntity *text) = 0;

	/**
	 * Getter and Setter
	 */
	virtual bool GetIsEnabled() const = 0;
	virtual void SetIsEnabled(bool bEnabled) = 0;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASIMPLETEXTOVERLAY_H
