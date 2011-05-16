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

#ifndef _VISTADRIVERGENERICPARAMETERASPECT_H
#define _VISTADRIVERGENERICPARAMETERASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"
#include "VistaDeviceDriver.h"


#include <VistaAspects/VistaReflectionable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 *
 */
class VISTADEVICEDRIVERSAPI VistaDriverGenericParameterAspect
                          : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	class VISTADEVICEDRIVERSAPI IParameterContainer : public IVistaReflectionable
	{
		REFL_DECLARE
	public:
		~IParameterContainer();
	protected:
		IParameterContainer();
	};

	class IContainerCreate
	{
	public:
		virtual IParameterContainer *CreateContainer() = 0;
		virtual bool DeleteContainer( IParameterContainer *pContainer ) = 0;
	};


	VistaDriverGenericParameterAspect(IContainerCreate *pCreationFct);
	virtual ~VistaDriverGenericParameterAspect();


	IParameterContainer * GetParameterContainer();
	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
private:
	IContainerCreate    *m_pCreate;
	IParameterContainer *m_pParams;
	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERGENERICPARAMETERASPECT_H


