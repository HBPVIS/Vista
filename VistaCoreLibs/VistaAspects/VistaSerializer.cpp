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

#include "VistaSerializer.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

IVistaSerializer &IVistaSerializer::operator<< (  ushort16 val)
{
	WriteShort16(val);
	return *this;
}

IVistaSerializer &IVistaSerializer::operator<< (  sint32 val)
{
	WriteInt32(val);
	return *this;
}


IVistaSerializer &IVistaSerializer::operator<< (  uint32 val)
{
	WriteInt32(val);
	return *this;
}

IVistaSerializer &IVistaSerializer::operator<< (  sint64 val)
{
	WriteInt64(val);
	return *this;
}


IVistaSerializer &IVistaSerializer::operator<< (  uint64 val)
{
	WriteUInt64(val);
	return *this;
}

IVistaSerializer &IVistaSerializer::operator<< (  float32 val)
{
	WriteFloat32(val);
	return *this;
}

IVistaSerializer &IVistaSerializer::operator<< (  float64 val)
{
	WriteFloat64(val);
	return *this;
}

IVistaSerializer &IVistaSerializer::operator<< (  const std::string &val)
{
	WriteString(val);
	return *this;
}

IVistaSerializer &IVistaSerializer::operator<< (  bool val)
{
	WriteBool(val);
	return *this;
}

