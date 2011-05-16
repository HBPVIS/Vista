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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "Vista3DCSpaceNavigatorPlugin.h"
#include "Vista3DCSpaceNavigatorDriver.h"


#if defined(WIN32)

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif //__VISTAVISTA3DCSPACENAVIGATORCONFIG_H

extern "C" VISTA3DCSPACENAVIGATORPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int)
{
	return new Vista3DCSpaceNavigator;
}

extern "C" VISTA3DCSPACENAVIGATORPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod()
{
	return Vista3DCSpaceNavigator::GetDriverFactoryMethod();
}

extern "C" VISTA3DCSPACENAVIGATORPLUGINAPI const char *GetDeviceClassName()
{
	return "SPACENAVIGATOR";
}


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


