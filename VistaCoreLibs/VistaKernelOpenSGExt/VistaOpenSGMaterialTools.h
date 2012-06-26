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

#ifndef _VISTAOPENSGMATERIALTOOLS_H
#define _VISTAOPENSGMATERIALTOOLS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaKernelOpenSGExtConfig.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaGeometry;
class IVistaNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VistaOpenSGMaterialTools
{

	VISTAKERNELOPENSGEXTAPI
	bool SetSortKey( VistaGeometry* pGeom,
									const int nSortKey );
	VISTAKERNELOPENSGEXTAPI
	bool SetSortKeyOnSubtree( IVistaNode* pNode,
									const int nSortKey );

	VISTAKERNELOPENSGEXTAPI
	bool SetColorMask( VistaGeometry* pGeom,
									const bool bDrawRed,
									const bool bDrawGreen,
									const bool bDrawBlue,
									const bool bDrawAlpha );
	VISTAKERNELOPENSGEXTAPI
	bool SetColorMaskOnSubtree( IVistaNode* pNode,
									const bool bDrawRed,
									const bool bDrawGreen,
									const bool bDrawBlue,
									const bool bDrawAlpha );

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGMATERIALTOOLS_H

