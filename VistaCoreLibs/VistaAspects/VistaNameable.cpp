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

#include "VistaNameable.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaNameable::IVistaNameable()
{
}

IVistaNameable::~IVistaNameable()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
/*

std::size_t GetId() const
	{
		return reinterpret_cast<std::size_t>(this);
	}
*/

/**
 * @bug @todo this implementation is evil!
 */
void* IVistaNameable::GetNameableId() const
{
	// this was the old implementation:
	//return (void*)this;
	// ...a nice example how to silently violate constness with C-style castst!!
	// This is the corresponding C++ implementation, with the same violation
	// but more visible through the occurance of the evil const_cast:
	return reinterpret_cast<void*>(const_cast<IVistaNameable*>(this));
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

