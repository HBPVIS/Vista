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

#include "VdfnShallowNode.h"
#include "VdfnPort.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnShallowNode::VdfnShallowNode()
: IVdfnNode()
{
}

VdfnShallowNode::~VdfnShallowNode()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnShallowNode::SetInPort(const std::string &sName, IVdfnPort *pPort)
{
	// set in port, regardless  of type (avoid type checking)
	// the memory belongs to the shallow node afterwards (it is collected
	// upon destruction of this shallow node)
	m_mpInPrototypes[ sName ] = pPort->GetPortTypeCompare();
	return DoSetInPort(sName, pPort);
}

void VdfnShallowNode::SetOutPort( const std::string &sName, IVdfnPort *pPort )
{
	RegisterOutPort( sName, pPort );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


