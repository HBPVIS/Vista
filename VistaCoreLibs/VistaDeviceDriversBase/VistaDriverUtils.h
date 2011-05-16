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

#ifndef _VISTADRIVERUTILS_H
#define _VISTADRIVERUTILS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"
#include <string>
#include <VistaTools/VistaDLL.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;
class VistaDLL;
class VistaDeviceSensor;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a collection of tools, mostly for driver plugins
 */
namespace VddUtil
{
	/**
	 * a helper class to manage driver plugins.
	 */
	class VISTADEVICEDRIVERSAPI VistaDriverPlugin
	{
	public:
		VistaDriverPlugin()
			: m_pDriver(NULL),
			  m_pMethod(NULL),
			  m_Plugin(NULL)
		{
		}

		IVistaDeviceDriver         *m_pDriver; /**< a pointer to the driver this plugin describes
		                                            @todo check whether this is outdated! */
		IVistaDriverCreationMethod *m_pMethod; /**< a pointer to the creation methods for drivers of the type */
		VistaDLL::DLLHANDLE        m_Plugin;  /**< a handle of the DLL that is managing the driver */
		std::string                 m_strDriverClassName; /**< symbolic name of the type of devices that can be created */
	};


	/**
	 * utility function to load a driver from a dll or so. Note that this method will try
	 * to create a single driver from a DLL. In case you do only want to create a single
	 * driver from the dll, this is ok, in case you want to create more than one instance
	 * of the same driver, use LoadCreationMethodFromPlugin() instead.
	 * @param the //absolute// path to the dll or so to load
	 * @param pStoreTo a pointer to a VistaDriverPlugin instance, may not be NULL
	 * @return false if
	           - the dll could not be loaded (path?)
	           - the dll does not contain a C symbol "GetDeviceClassName"
	           - the dll does not contain a C symbol "GetCreationMethod"
	           - the dll failed to create a driver
	   in all these cases, the dll is closed and the value of the members of pStoreTo are undefined.
	 */
	bool VISTADEVICEDRIVERSAPI LoadDriverFromPlugin( const std::string &strPathToPlugin,
		                       VistaDriverPlugin *pStoreTo );


	/**
	 * opens a dll or so and claims a creation method from the shared resource.
	 * this can be used to create a number of devices of the same type.
	 * @param strPathToPlugin the //absolute// path of the dll or so to open
	 * @param pStoreTo a non-NULL pointer to a storage for the DLL info
	 * @return false if
	           - the dll could not be loaded (path?)
	           - the dll does not contain a C symbol "GetDeviceClassName"
	           - the dll does not contain a C symbol "GetCreationMethod"
	           - the dll failed to create a creation method
	   in all these cases, the dll is closed and the value of the members of pStoreTo are undefined.
	 */
	bool VISTADEVICEDRIVERSAPI LoadCreationMethodFromPlugin( const std::string &strPathToPlugin,
		                               VistaDriverPlugin *pStoreTo );

	/**
	 * releases memory from a DLL and associated resources. call this method, when cleaning up,
	 * or when absolutely sure that //NO// ressource allocated by the dll or so is still in use.
	 * this can sometimes be non-trivial! The method will call the OnUnload() method of the
	 * creation method, which might deregister some global stuff and lead to crashes.
	 * In that case: report that! and fix it!
	 * @param a pointer to a driver plugin (non-NULL)
	 * @param bDeleteCm indicates whether the creation method is to be deleted
	          note that the plugin will be unloaded, any subsequent call to
	          destruct the creation method will fail, as the memory is no
	          longer part of the application.
	 * @return true
	 */
	bool VISTADEVICEDRIVERSAPI DisposePlugin( VistaDriverPlugin *, bool bDeleteCm = false );

	/**
	 * Utility function to pick the right arguments for user defined history
	 * setup. Use this function when you do not want to deal with the interfaces
	 * of aspects, but merely want to create a properly sized history for
	 * your sensor. Note that you have to know the correct creation method,
	 * which might need to have access to a static member of a specialized
	 * driver class.
	 * @param pDriver the driver that holds pSensor and is created by pCrMethod
	 * @param pSensor the sensor to setup
	 * @param strSensorTypeName the type name of the sensor
	 * @param nDesiredReadSlots the read slots to access (maximum) during read length
	 * @param nReadLengthInMsecs the maximum read time on a history for n desired slots
	 * @return false in case of error (the history of the sensor is unchanged)
	 */
	bool VISTADEVICEDRIVERSAPI SetupSensorHistory( IVistaDeviceDriver *pDriver,
			                                VistaDeviceSensor *pSensor,
			          					  IVistaDriverCreationMethod *pCrMethod,
										    const std::string &strSensorTypeName,
											unsigned int nDesiredReadSlots,
											unsigned int nReadLengthInMsec );
	bool VISTADEVICEDRIVERSAPI InitVdd();
	bool VISTADEVICEDRIVERSAPI ExitVdd();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERUTILS_H

