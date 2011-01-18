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

#ifndef _VISTAPICKABLE_H
#define _VISTAPICKABLE_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include "VistaPickManager.h"
//#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
class VistaVector3D;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaPickable
{
public:
	virtual void Pick(IVistaNode *pNode,
					  VistaPickManager::SContactData *pData)
	{}

	virtual void Touch(IVistaNode *pNode,
					   int iNumberOfContacts,
					   VistaPickManager::SContactData *pData)
	{}

	virtual float QueryForContact(const VistaVector3D &v3Position,
								  const VistaVector3D &v3Direction)
	{
		return -1.0f;
	}
};


/*============================================================================*/
/* INLINE METHODS                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAPICKABLE_H
