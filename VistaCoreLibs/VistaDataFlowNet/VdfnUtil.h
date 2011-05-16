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

#ifndef _VDFNUTIL_H
#define _VDFNUTIL_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*
 * this file is intended to collect useful utility routines around the
 * Vdfn and related issues. So if you have a routine you use frequently, and
 * you do not know where to put it... this can be a place...
 * Routines in here are packed into the VdfnUtil namespace, as utility methods
 * tend to have generic names and can be free floating.
 */


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaDriverMap;
class VdfnObjectRegistry;

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

namespace VdfnUtil
{
	/**
	 * templated function to save typing when getting an inport.
	 */
	template<class T> T GetInPortTyped( const std::string &strInPortName,
										const IVdfnNode *pNode )
	{
		return dynamic_cast<T>( pNode->GetInPort(strInPortName) );
	}


	/**
	 * management API, registers the basically available port setters for the
	 * action node interface and other nodes that work on the introspective feature
	 * of the Vdfn. This methods is called by InitVdfn(), so usually there is no
	 * need to call it yourself, unless you have special plans and you know what you
	 * are doing.
	 * @return true
	 */
	VISTADFNAPI bool RegisterBasicPortSetters();

	/**
	 * management API, registers the basically available node creators for the
	 * available node types. Is called by InitVdfn(), so usually there is no need
	 * to call this methods on your own, use InitVdfn() instead. Nodes creators will
	 * register themselves in the VdfnNodeFactory singleton.
	 * @param a non-null pointer to a valid driver map which must outlive the whole
	          application life cycle.
	 * @param pObjRegistry the object registry to use for nodes searching entities.
	          the registry must outlive the whole application life cycle.
	 * @return true
	 */
	VISTADFNAPI bool RegisterBasicNodeCreators(VistaDriverMap *pDrivers,
							   VdfnObjectRegistry *pObjRegistry);

	/**
	 * calls RegisterBasicPortSetters() and RegisterBasicNodeCreators()
	 * @return true
	 */
	VISTADFNAPI bool InitVdfn(VistaDriverMap *pDrivers,
							   VdfnObjectRegistry *pObjRegistry);
}


#endif //_VDFNUTIL_H

