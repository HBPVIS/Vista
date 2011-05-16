/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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

#ifndef __VISTAVRPNRIVERPLUGIN_H
#define __VISTAVRPNRIVERPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VRPNPLUGINEXPORT __declspec(dllexport)
#define VRPNPLUGINIMPORT __declspec(dllimport)

#define VRPNPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VRPNPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VRPNPLUGINEXPORT
#define VRPNPLUGINIMPORT
#endif

// Define VRPNPLUGINAPI for DLL builds
#ifdef VISTAVRPNDRIVERPLUGINDLL
#ifdef VISTAVRPNDRIVERPLUGINDLL_EXPORTS
#define VRPNPLUGINAPI VRPNPLUGINEXPORT
#define VRPNPLUGIN_EXPLICIT_TEMPLATE
#else
#define VRPNPLUGINAPI VRPNPLUGINIMPORT
#define VRPNPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VRPNPLUGINAPI
#endif



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" VRPNPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" VRPNPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" VRPNPLUGINAPI const char *GetDeviceClassName();

/**
 * the wiimote explicitly unloads the getter/setter reflectionables
 * from the parameter aspect, so be sure to call this method when
 * - using the wiimote as a plugin
 * - unloading the device during run-time
 */
extern "C" VRPNPLUGINAPI void Unload(IVistaDriverCreationMethod *);


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //____VISTAVRPNDRIVERPLUGIN_H
