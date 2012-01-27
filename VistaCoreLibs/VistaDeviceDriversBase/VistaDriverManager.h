/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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
// $Id: VistaDriverManager.h 22143 2011-07-01 15:07:00Z dr165799 $

#ifndef __VISTADRIVERMANAGER_H
#define __VISTADRIVERMANAGER_H


#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>

#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaConnectionUpdater;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a utility class for
 * - initializing Vdd
 * - managing driver plugins (adding / removing)
 * - keeping a driver map to work with
 * - Update all active devices correctly
 */
class VISTADEVICEDRIVERSAPI VistaDriverManager
{
public:
	/**
	 * The constructor sets up the Vdd for you. It will
	 * - call VddUtils::InitVdd()
	 * - initialize plugins for you
	 *
	 * The plugins are loaded from:
	 * - the path pointed to using the environment variable VDD_DRIVERS
	 * - if that one is empty, the
	 */
	VistaDriverManager();
	~VistaDriverManager();


	///@name management API
	IVistaDeviceDriver *CreateAndRegisterDriver( const std::string &strDriverName,
			                                     const std::string &strDriverClassName );

	IVistaDriverCreationMethod *GetCreationMethodForClass( const std::string &strDriverClassName ) const;

	void RegisterDriver( const std::string &strDriverName, IVistaDeviceDriver *pDriver );
	void RegisterDriverPlugin( const VddUtil::VistaDriverPlugin & oPlugin );

	void UnregisterDriver( IVistaDeviceDriver * );

	void SetEnableStateOnAllDrivers( bool bState );

	typedef std::vector<int> IDVEC;
	typedef std::pair< IVistaDeviceDriver*, IDVEC > DPAIR;
	typedef std::map< std::string, DPAIR > DMAP;

	///@name runtime-API
	bool Update( DMAP & );

	VistaDriverMap &GetDriverMap();
	const VistaDriverMap &GetDriverMap() const;

	bool StartAsyncDriverDispatch();
	bool StopAsyncDriverDispatch();
private:
	int InitPlugins();
	int DisposePlugins();


	void StopThreadedDevices();

	std::vector<VddUtil::VistaDriverPlugin> m_vecDriverPlugins;

	VistaDriverMap m_pDrivers;

	VistaConnectionUpdater *m_pConnUpdater;

	bool m_AsyncDispatchRunning;
};



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif // __VISTADRIVERMANAGER_H
