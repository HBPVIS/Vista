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

#ifndef __VISTAWIIMOTEDRIVERPLUGIN_H
#define __VISTAWIIMOTEDRIVERPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAWIIMOTEDRIVERPLUGIN_STATIC) 
	#ifdef VISTAWIIMOTEDRIVERPLUGIN_EXPORTS
		#define WIIMOTEPLUGINAPI __declspec(dllexport)
	#else
		#define WIIMOTEPLUGINAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define WIIMOTEPLUGINAPI
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" WIIMOTEPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" WIIMOTEPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" WIIMOTEPLUGINAPI const char *GetDeviceClassName();

/**
 * the wiimote explicitly unloads the getter/setter reflectionables
 * from the parameter aspect, so be sure to call this method when
 * - using the wiimote as a plugin
 * - unloading the device during run-time
 */
extern "C" WIIMOTEPLUGINAPI void Unload(IVistaDriverCreationMethod *);


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //____VISTAWIIMOTEDRIVERPLUGIN_H
