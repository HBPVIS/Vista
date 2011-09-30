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
// $Id: VistaPipeComponent.cpp 21315 2011-05-16 13:47:39Z dr165799 $


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/


#include "VistaPipeComponent.h"

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaPipeComponent::IDLVistaPipeComponent()
{}

IDLVistaPipeComponent::~IDLVistaPipeComponent()
{}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


IDLVistaDataPacket *IDLVistaPipeComponent::ReturnPacket()
{ 
	// convinience... simply return de nada ;)
	return 0; 
}


int IDLVistaPipeComponent::GetInputPacketType() const
{
	// default is: we are not connected.
	return -1;
}


int IDLVistaPipeComponent::GetOutputPacketType() const
{
	// default is: we are not connected.
	return -1;
}


