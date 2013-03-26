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
// $Id: VistaSixensePlugin.cpp 29136 2012-02-08 09:35:49Z dr165799 $

#include "VistaSixenseDriver.h"

#if defined(WIN32) && !defined(VISTASIXENSEDRIVERPLUGIN_STATIC)
#ifdef VISTASIXENSEPLUGIN_EXPORTS
#define VISTASIXENSEPLUGINAPI __declspec(dllexport)
#else
#define VISTASIXENSEPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASIXENSEPLUGINAPI
#endif

namespace
{
	VistaSixenseCreationMethod *SpFactory = NULL;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
extern "C" VISTASIXENSEPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaSixenseDriver(crm);
}

extern "C" VISTASIXENSEPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new VistaSixenseCreationMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTASIXENSEPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory == crm )
	{
		delete SpFactory;
		SpFactory = NULL;
	}
	else
		delete crm;
}

extern "C" VISTASIXENSEPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory != NULL )
	{
		if(IVistaReferenceCountable::refdown(SpFactory))
			SpFactory = NULL;
	}
}


extern "C" VISTASIXENSEPLUGINAPI const char *GetDeviceClassName()
{
	return "SIXENSE";
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

