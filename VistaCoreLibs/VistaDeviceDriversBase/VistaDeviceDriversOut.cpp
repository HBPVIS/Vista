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

#include "VistaDeviceDriversOut.h"
#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
std::ostream* VistaDeviceDriversOut::m_out = &std::cout;
std::ostream* VistaDeviceDriversOut::m_err = &std::cerr;
std::ostream* VistaDeviceDriversOut::m_log = &std::clog;

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
std::ostream& VistaDeviceDriversOut::out()
{
	return *m_out;
}

std::ostream& VistaDeviceDriversOut::err()
{
	return *m_err;
}

std::ostream& VistaDeviceDriversOut::log()
{
	return *m_log;
}

void VistaDeviceDriversOut::SetOutStream(std::ostream * ostream)
{
	m_out = ostream;
}

void VistaDeviceDriversOut::SetErrorStream(std::ostream * ostream)
{
	m_err = ostream;
}

void VistaDeviceDriversOut::SetLogStream(std::ostream * ostream)
{
	m_log = ostream;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

