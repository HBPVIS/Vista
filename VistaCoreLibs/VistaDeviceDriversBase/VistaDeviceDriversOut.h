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

#ifndef _VISTADEVICEDRIVERSOUT_H
#define _VISTADEVICEDRIVERSOUT_H

#include <ostream>
#include "VistaDeviceDriversConfig.h"
class VISTADEVICEDRIVERSAPI VistaDeviceDriversOut
{
public:
	static std::ostream& out();
	static std::ostream& err();
	static std::ostream& log();

	static void SetOutStream  (std::ostream *stream);
	static void SetErrorStream(std::ostream *stream);
	static void SetLogStream  (std::ostream *stream);


private:
	static std::ostream *m_out;
	static std::ostream *m_err;
	static std::ostream *m_log;
};

#define vddout VistaDeviceDriversOut::out()
#define vdderr VistaDeviceDriversOut::err()
#define vddlog VistaDeviceDriversOut::log()

#endif //_VISTAASPECTSOUT_H

