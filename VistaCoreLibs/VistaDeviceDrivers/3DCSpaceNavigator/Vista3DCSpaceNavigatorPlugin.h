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

#ifndef _VISTA3DCSPACENAVIGATORPLUGIN_H
#define _VISTA3DCSPACENAVIGATORPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTA3DCSPACENAVIGATORDRIVERPLUGIN_STATIC) 
	#ifdef VISTA3DCSPACENAVIGATORDRIVERPLUGIN_EXPORTS
		#define VISTA3DCSPACENAVIGATORPLUGINAPI __declspec(dllexport)
	#else
		#define VISTA3DCSPACENAVIGATORPLUGINAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTA3DCSPACENAVIGATORPLUGINAPI
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" VISTA3DCSPACENAVIGATORPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" VISTA3DCSPACENAVIGATORPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" VISTA3DCSPACENAVIGATORPLUGINAPI const char *GetDeviceClassName();


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //__VISTA3DCSPACENAVIGATORPLUGIN_H

