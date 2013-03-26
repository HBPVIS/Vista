/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#if defined(WIN32) && !defined(VISTADIRECTXGAMEPADPLUGIN_STATIC)
	#ifdef VISTADIRECTXGAMEPADPLUGIN_EXPORTS
		#define VISTADIRECTXGAMEPADPLUGINAPI __declspec(dllexport)
	#else
		#define VISTADIRECTXGAMEPADPLUGINAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTADIRECTXGAMEPADPLUGINAPI
#endif

#include "VistaDirectXGamepadDriver.h"
#include "VistaDirectXGamepadSensorMeasures.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>


namespace
{
	CreateProxy *SpFactory = NULL;
}


extern "C" VISTADIRECTXGAMEPADPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if(SpFactory == NULL)
	{
		SpFactory = new CreateProxy( fac );
	}
	return SpFactory;
}


extern "C" VISTADIRECTXGAMEPADPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory == crm )
	{
		delete SpFactory;
		SpFactory = NULL;
	}
	else
		delete crm;
}

extern "C" VISTADIRECTXGAMEPADPLUGINAPI const char *GetDeviceClassName()
{
	return "DIRECTXGAMEPAD";
}


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


