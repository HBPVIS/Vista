/*============================================================================*/
/*                    ViSTA VR toolkit - Jsw Joystick driver                  */
/*               Copyright (c) 1997-2011 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id$

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#ifndef __VISTAJSWJOYSTICKDRIVERPLUGIN_H
#define __VISTAJSWJOYSTICKDRIVERPLUGIN_H

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTAJSWJOYSTICKPLUGINEXPORT __declspec(dllexport)
#define VISTAJSWJOYSTICKPLUGINIMPORT __declspec(dllimport)

#define VISTAJSWJOYSTICKPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTAJSWJOYSTICKPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAJSWJOYSTICKPLUGINEXPORT
#define VISTAJSWJOYSTICKPLUGINIMPORT
#endif

// Define VISTAJSWJOYSTICKPLUGINAPI for DLL builds
#ifdef VISTAJSWJOYSTICKPLUGINDLL
#ifdef VISTAJSWJOYSTICKPLUGINDLL_EXPORTS
#define VISTAJSWJOYSTICKPLUGINAPI VISTAJSWJOYSTICKPLUGINEXPORT
#define VISTAJSWJOYSTICKPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTAJSWJOYSTICKPLUGINAPI VISTAJSWJOYSTICKPLUGINIMPORT
#define VISTAJSWJOYSTICKPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAJSWJOYSTICKPLUGINAPI
#endif



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" VISTAJSWJOYSTICKPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" VISTAJSWJOYSTICKPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" VISTAJSWJOYSTICKPLUGINAPI const char *GetDeviceClassName();
extern "C" VISTAJSWJOYSTICKPLUGINAPI void Unload(IVistaDriverCreationMethod *);


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //____VISTAJSWJOYSTICKDRIVERPLUGIN_H
