/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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

#include "VistaDriverManager.h"
#include <VistaTools/VistaEnvironment.h>
#include <VistaTools/VistaFileSystemDirectory.h>
#include "VistaDeviceDriver.h"
#include "VistaConnectionUpdater.h"
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>

#include <iostream>

namespace
{
	class DriverUpdateCallback: public IVistaExplicitCallbackInterface
	{
	public:
		DriverUpdateCallback(IVistaDeviceDriver *pDriver) :
			m_pDriver(pDriver)
		{
		}

		~DriverUpdateCallback()
		{
		}

		virtual bool Do()
		{
			m_pDriver->PreUpdate();
			m_pDriver->Update();
			m_pDriver->PostUpdate();

			return true;
		}

		IVistaDeviceDriver *m_pDriver;
	};
}


VistaDriverManager::VistaDriverManager()
: m_pConnUpdater(new VistaConnectionUpdater)
, m_AsyncDispatchRunning(false)
{
	VddUtil::InitVdd();
	InitPlugins();
}

VistaDriverManager::~VistaDriverManager()
{

	if( m_pConnUpdater && m_pConnUpdater->GetIsDispatching() )
		m_pConnUpdater->ShutdownUpdaterLoop(true);

	StopThreadedDevices();


	// this will delete all the *drivers* in the driver map
	// (the driver map will only delete all the creation methods for us)
	for(VistaDriverMap::iterator it = m_pDrivers.begin();
		it != m_pDrivers.end(); ++it)
	{
		(*it).second->SetIsEnabled(false);

		if( (*it).second->GetUpdateType() == IVistaDeviceDriver::UPDATE_CONNECTION_THREADED )
		{
			VistaDriverConnectionAspect *pConAsp =
				dynamic_cast<VistaDriverConnectionAspect*>(
					(*it).second->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
					);

			if(!pConAsp)
				continue;

			IVistaExplicitCallbackInterface *pI = m_pConnUpdater->RemConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()));
			delete pI;
		}
		else if( (*it).second->GetUpdateType() == IVistaDeviceDriver::UPDATE_CUSTOM_THREADED )
		{
			VistaDriverThreadAspect *thread =
					dynamic_cast<VistaDriverThreadAspect*>(
							(*it).second->GetAspectById( VistaDriverThreadAspect::GetAspectId() )
							);
			if( !thread )
				continue;
			thread->StopProcessing();
		}
	}

	// dispose all the plugins now
	DisposePlugins();

	delete m_pConnUpdater;

	m_pDrivers.Purge();

	VddUtil::ExitVdd();
}


IVistaDeviceDriver *VistaDriverManager::CreateAndRegisterDriver( const std::string &strDriverName,
															const std::string &strDriverClassName )
{
	IVistaDriverCreationMethod *cr = m_pDrivers.GetDriverCreationMethod(strDriverClassName);
	if(cr)
	{
		IVistaDeviceDriver *drv = (*cr).CreateDriver(); // creator function
		if(drv)
		{
			m_pDrivers.AddDeviceDriver( strDriverName, drv );
			return drv;
		}
		else
		{
			vstr::errp() << "VistaDriverManager::CreateAndRegisterDriver() -- "
					<< "could not load driver ["
					<< strDriverName
					<< "] using factory ["
					<< strDriverClassName
					<< "]"
					<< std::endl;
		}
	}
	return NULL;
}

void VistaDriverManager::UnregisterDriver( IVistaDeviceDriver *pDr )
{
	m_pDrivers.RemDeviceDriver(pDr);
}

void VistaDriverManager::RegisterDriver( const std::string &strDriverName, IVistaDeviceDriver *drv )
{
	m_pDrivers.AddDeviceDriver( strDriverName, drv );
}

bool VistaDriverManager::Update( DMAP &clientMap )
{
	bool bRet = false;
	for(VistaDriverMap::iterator it = m_pDrivers.begin();
				it != m_pDrivers.end(); ++it)
	{
		IVistaDeviceDriver *pDriver = (*it).second;
		if( pDriver->GetUpdateType() == IVistaDeviceDriver::UPDATE_EXPLICIT_POLL )
		{
			if(pDriver->Update())
			{
				IDVEC &v = (clientMap[ (*it).first ] = DPAIR( pDriver, IDVEC() )).second;
				pDriver->SwapSensorMeasures(&v);
				bRet = true;
			}
		}
		else
		{
			// just call swap measures for all async devices (in case there was no
			// new data in between, the v will be empty on return
			IDVEC &v = (clientMap[ (*it).first ] = DPAIR( pDriver, IDVEC() )).second;
			pDriver->SwapSensorMeasures(&v);
			if( !v.empty() )
				bRet = true;
		}
	}
	return bRet;
}


int VistaDriverManager::InitPlugins()
{
	std::string strPlugPath = VistaEnvironment::GetEnv("VDD_DRIVERS");
	if(strPlugPath.empty())
		strPlugPath = VistaEnvironment::GetEnv("VISTA_ROOT")
	                + VistaFileSystemDirectory::GetOSSpecificSeparator()
	                + std::string("lib")
				    + VistaFileSystemDirectory::GetOSSpecificSeparator()
					+ std::string(VistaEnvironment::GetOSystem());


	vstr::outi() << "plug path resolved to: " << strPlugPath << std::endl;


	VistaFileSystemDirectory plugs(strPlugPath);
#if !defined(DEBUG)
	plugs.SetPattern("*Plugin[!D]*"); // omit the debug versions
#else
	plugs.SetPattern("*PluginD*"); // force debug version
#endif

	vstr::outi() << "found [" << plugs.GetNumberOfEntries()
			  << "] plugins."
			  << std::endl;
	for( VistaFileSystemDirectory::iterator it = plugs.begin();
			 it != plugs.end(); ++it )
	{
		vstr::outi() << "[" << (*it)->GetName() << "]" << std::endl;
		VddUtil::VistaDriverPlugin plg;

		// find default transcoder for this time...
		std::string::size_type slash = (*it)->GetName().find_last_of( VistaFileSystemDirectory::GetOSSpecificSeparator() );
		if( slash == std::string::npos )
			slash = 0;
		std::string::size_type nameP = slash+9;
		std::string::size_type plugin = (*it)->GetName().rfind( "Plugin" );
		std::string driverName = (*it)->GetName().substr( nameP, plugin-nameP );

		// try to load transcoder for this device
		std::string transcoderName = strPlugPath+VistaFileSystemDirectory::GetOSSpecificSeparator()+"libVista"+driverName+"Transcoder";
#if !defined(DEBUG)
			transcoderName += ".so";
#else
			transcoderName += "D.so";
#endif
		if( !VddUtil::LoadTranscoderFromPlugin(transcoderName, &plg ) )
		{
			vstr::errp() << "Could not load transcoder by name: [" << transcoderName << "]" << std::endl;
			continue;
		}

		if( VddUtil::LoadCreationMethodFromPlugin( (*it)->GetName(), &plg ) )
		{
			vstr::outi() << "Loaded factory from ["
					  << (*it)->GetName()
					  << "] @ "
					  << plg.m_pMethod
					  << " with name ["
					  << plg.m_strDriverClassName
					  << "]"
					  << std::endl;
			m_vecDriverPlugins.push_back(plg);
			m_pDrivers.RegisterDriverCreationMethod( plg.m_strDriverClassName, plg.m_pMethod );
		}
		else
		{
			vstr::errp() << "Failed to load factory from ["
					  << (*it)->GetName()
					  << "]"
					  << std::endl;
			VddUtil::DisposeTranscoderFromPlugin( &plg );
		}
	}
	return (int)m_vecDriverPlugins.size();
}

void VistaDriverManager::RegisterDriverPlugin( const VddUtil::VistaDriverPlugin & plug )
{
	m_vecDriverPlugins.push_back( plug );
}

int VistaDriverManager::DisposePlugins()
{
	int n=0;
	// clean up
	for( std::vector<VddUtil::VistaDriverPlugin>::iterator vit = m_vecDriverPlugins.begin();
		 vit != m_vecDriverPlugins.end(); ++vit )
	{
		vstr::outi() << "close plugin for ["
				  << (*vit).m_strDriverClassName
				  << "]: ";

		// do not kill the memory on it, as this will be done by
		// the call to DisposePlugin below (on some systems or setups,
		// the call to delete might need to call a specific delete
		// operator. So this is more safe to do.
		m_pDrivers.UnregisterDriverCreationMethod( (*vit).m_strDriverClassName, false );

		if(VddUtil::DisposePlugin( &(*vit), true ))
		{
			vstr::out() << " -- SUCCESS" << std::endl;
			++n;
		}
		else
		{
			vstr::out().flush();
			vstr::err() << " -- ERROR" << std::endl;
		}

		VddUtil::DisposeTranscoderFromPlugin( &(*vit) );
	}

	m_vecDriverPlugins.clear();

	return n;
}


IVistaDriverCreationMethod *VistaDriverManager::GetCreationMethodForClass( const std::string &strDriverClassName ) const
{
	for(std::vector<VddUtil::VistaDriverPlugin>::const_iterator it = m_vecDriverPlugins.begin();
			 it != m_vecDriverPlugins.end(); ++it )
	{
		if( (*it).m_strDriverClassName == strDriverClassName )
			return (*it).m_pMethod;
	}

	return NULL;
}


VistaDriverMap &VistaDriverManager::GetDriverMap()
{
	return m_pDrivers;
}

const VistaDriverMap &VistaDriverManager::GetDriverMap() const
{
	return m_pDrivers;
}

void VistaDriverManager::SetEnableStateOnAllDrivers( bool bState )
{
	for( VistaDriverMap::iterator it = m_pDrivers.begin(); it != m_pDrivers.end(); ++it )
		(*it).second->SetIsEnabled(bState);
}

bool VistaDriverManager::StartAsyncDriverDispatch()
{
	for(VistaDriverMap::iterator it = m_pDrivers.begin();
				it != m_pDrivers.end(); ++it)
	{
		IVistaDeviceDriver *pDriver = (*it).second;
		switch( pDriver->GetUpdateType() )
		{
		case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
		{
			VistaDriverConnectionAspect *pConAsp = dynamic_cast<VistaDriverConnectionAspect*>
			                           ( pDriver->GetAspectById( VistaDriverConnectionAspect::GetAspectId() ) );
			if( pConAsp )
				m_pConnUpdater->AddConnectionUpdate( pConAsp->GetConnection( pConAsp->GetUpdateConnectionIndex() ), new DriverUpdateCallback( pDriver ) );
			break;
		}
		case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
		{
//			VistaDriverThreadAspect *pThread = dynamic_cast<VistaDriverThreadAspect*>
//			                           (pDriver->GetAspectById( VistaDriverThreadAspect::GetAspectId() ) );
//			if( pThread )
//				pThread->StartProcessing();
			break;
		}
		case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
		default:
			break;
		}
	}

	return m_pConnUpdater->StartUpdaterLoop();
}

bool VistaDriverManager::StopAsyncDriverDispatch()
{
	StopThreadedDevices();
	m_pConnUpdater->ShutdownUpdaterLoop(true);
	return true;
}

void VistaDriverManager::StopThreadedDevices()
{
	for(VistaDriverMap::iterator it = m_pDrivers.begin();
				it != m_pDrivers.end(); ++it)
	{
		IVistaDeviceDriver *pDriver = (*it).second;
		switch( pDriver->GetUpdateType() )
		{
		case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
		{
//			VistaDriverThreadAspect *pThread = dynamic_cast<VistaDriverThreadAspect*>
//			                           (pDriver->GetAspectById( VistaDriverThreadAspect::GetAspectId() ) );
//			if( pThread )
//				pThread->StopProcessing();
			break;
		}
		default:
			break;
		}
	}

}

