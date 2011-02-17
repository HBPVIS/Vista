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

#ifndef __VISTAWIIMOTEDRIVERPLUGIN_H
#define __VISTAWIIMOTEDRIVERPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTAWIIMOTEPLUGINEXPORT __declspec(dllexport)
#define VISTAWIIMOTEPLUGINIMPORT __declspec(dllimport)

#define VISTAWIIMOTEPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTAWIIMOTEPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAWIIMOTEPLUGINEXPORT
#define VISTAWIIMOTEPLUGINIMPORT
#endif

// Define VISTAWIIMOTEPLUGINAPI for DLL builds
#ifdef VISTAWIIMOTEPLUGINDLL
#ifdef VISTAWIIMOTEPLUGINDLL_EXPORTS
#define VISTAWIIMOTEPLUGINAPI VISTAWIIMOTEPLUGINEXPORT
#define VISTAWIIMOTEPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTAWIIMOTEPLUGINAPI VISTAWIIMOTEPLUGINIMPORT
#define VISTAWIIMOTEPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAWIIMOTEPLUGINAPI
#endif



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" VISTAWIIMOTEPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" VISTAWIIMOTEPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" VISTAWIIMOTEPLUGINAPI const char *GetDeviceClassName();

/**
 * the wiimote explicitly unloads the getter/setter reflectionables
 * from the parameter aspect, so be sure to call this method when
 * - using the wiimote as a plugin
 * - unloading the device during run-time
 */
extern "C" VISTAWIIMOTEPLUGINAPI void Unload(IVistaDriverCreationMethod *);


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //____VISTAWIIMOTEDRIVERPLUGIN_H