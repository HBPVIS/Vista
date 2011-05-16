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

#ifndef _VISTASPACENAVIGATORPLUGINPLUGIN_H
#define _VISTASPACENAVIGATORPLUGINPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTASPACENAVIGATORPLUGINEXPORT __declspec(dllexport)
#define VISTASPACENAVIGATORPLUGINIMPORT __declspec(dllimport)

#define VISTASPACENAVIGATORPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTASPACENAVIGATORPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTASPACENAVIGATORPLUGINEXPORT
#define VISTASPACENAVIGATORPLUGINIMPORT
#endif

// Define VISTASPACENAVIGATORPLUGINAPI for DLL builds
#ifdef VISTASPACENAVIGATORPLUGINDLL
#ifdef VISTASPACENAVIGATORPLUGINDLL_EXPORTS
#define VISTASPACENAVIGATORPLUGINAPI VISTASPACENAVIGATORPLUGINEXPORT
#define VISTASPACENAVIGATORPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTASPACENAVIGATORPLUGINAPI VISTASPACENAVIGATORPLUGINIMPORT
#define VISTASPACENAVIGATORPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTASPACENAVIGATORPLUGINAPI
#endif



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" VISTASPACENAVIGATORPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" VISTASPACENAVIGATORPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" VISTASPACENAVIGATORPLUGINAPI const char *GetDeviceClassName();


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_VISTASPACENAVIGATORPLUGINPLUGIN_H
