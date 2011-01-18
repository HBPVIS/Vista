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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaDTrackSDKDriverPlugin.h"
#include "VistaDTrackSDKDriver.h"


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

#endif //__VISTAVISTADTrackSDKDriverCONFIG_H

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int)
{
	return new VistaDTrackSDKDriver;
}

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod()
{
	return VistaDTrackSDKDriver::GetDriverFactoryMethod();
}

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "DTRACKSDKDRIVER";
}


extern "C" VISTADTRACKSDKDRIVERPLUGINAPI void Unload(IVistaDriverCreationMethod *pMt)
{
	/**
	 * @todo: Test this!
	 */
	pMt->OnUnload();
}

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/





