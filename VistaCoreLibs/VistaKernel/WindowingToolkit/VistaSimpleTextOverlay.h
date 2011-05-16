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

#ifndef _VISTASIMPLETEXTOVERLAY_H
#define _VISTASIMPLETEXTOVERLAY_H

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

/**
 *  This is a simple rastered overlay that is creating an orthogonal overlay
 */
class VISTAKERNELAPI IVistaSimpleTextOverlay: public IVistaSceneOverlay
{
public:
	virtual ~IVistaSimpleTextOverlay(){};

	/**
	 * Add a new TextEntity to the overlay.
	 * If there are more than one entity to the overlay each will start in (0,0).
	 * in case you want to make two entitys readable you have to calculate the
	 * offset on your own
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

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
	virtual void SetWidth(unsigned int) = 0;
	virtual void SetHeight(unsigned int) = 0;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASIMPLETEXTOVERLAY_H
