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

#include "VistaDataTunnelFactory.h"
#include "VistaDataTunnel.h"
#include "VistaStandaloneDataTunnel.h"
#include "VistaMasterDataTunnel.h"
#include "VistaClientDataTunnel.h"
#include "VistaClusterServer.h"
#include "VistaClusterClient.h"
#include "VistaClusterMaster.h"
#include "VistaClusterSlave.h"
#include "VistaClusterAux.h"
#include "VistaKernelOut.h"

#include <VistaTools/VistaProfiler.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include <VistaInterProcComm/Connections/VistaMasterNetworkSync.h>
#include <VistaInterProcComm/Connections/VistaSlaveNetworkSync.h>

#include <vector>
#include <list>
#include <cassert>


using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//init singleton 
VistaDataTunnelFactory* VistaDataTunnelFactory::m_pSingleton = NULL;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDataTunnelFactory::VistaDataTunnelFactory(VistaClusterAux *pAux)
: m_pClusterAux(pAux)
{
	// ViSTA configuration
	if(m_pClusterAux->GetIsStandalone())
		m_iNodeId = -1; //standalone index
	else if (m_pClusterAux->GetIsServer() || m_pClusterAux->GetIsMaster())
		m_iNodeId = 0; //master indes
	else if (m_pClusterAux->GetIsClient())
		m_iNodeId = m_pClusterAux->GetVistaClusterClient()->GetClientIndex(); // client index
	else if (m_pClusterAux->GetIsSlave())
		m_iNodeId = m_pClusterAux->GetVistaClusterSlave()->GetSlaveIndex(); // slave index
}

VistaDataTunnelFactory::~VistaDataTunnelFactory()
{
	m_pSingleton = NULL;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaDataTunnelFactory* VistaDataTunnelFactory::GetFactory(VistaClusterAux *pAux)
{
	if(!m_pSingleton)
	{
		assert(pAux != NULL);
		return (m_pSingleton = new VistaDataTunnelFactory(pAux));
	}
	else
	{
		return m_pSingleton;
	}
}

IVistaDataTunnel* VistaDataTunnelFactory::CreateDataTunnel(IDLVistaDataPacket* pPacketPrototype,
															const std::string &strClusterIniFile) const
{
	//create tunnel according to roll of current machine
	IVistaDataTunnel* pNewTunnel = NULL;
	
	if(m_pClusterAux->GetIsStandalone())
	{
		pNewTunnel = new VistaStandaloneDataTunnel;
	}
	else if(m_pClusterAux->GetIsServer())
	{
		//fetch client's indices in order to know, which clients will connect to the data tunnel
		VistaProfiler localProfiler;
		
		std::vector<string> vecClientHostNames;
		std::vector<int> vecClientDTPorts;

		string strCurrentIP;
		int iCurrentPort;
		
		std::list<std::string> liClientSections;
		m_pClusterAux->GetVistaClusterServer()->GetClientNames(liClientSections);
		
		if( liClientSections.empty())
		{	
			std::string sServerName = m_pClusterAux->GetVistaClusterServer()->GetServerSectionName();
			localProfiler.GetTheProfileList (sServerName,"Clients", "", liClientSections, strClusterIniFile);
		}


		vecClientHostNames.reserve(liClientSections.size());
		vecClientDTPorts.reserve(liClientSections.size());
		
		for(std::list<std::string>::iterator itClientSection = liClientSections.begin();
			itClientSection != liClientSections.end();
			++itClientSection)
		{
			//retrieve clientIP
			localProfiler.GetTheProfileString((*itClientSection), "CLIENTIP", "", strCurrentIP, strClusterIniFile);
			if(!strCurrentIP.empty())
			{
			#ifdef DEBUG
				vkernout << "[DTFactory] Adding client @ " << strCurrentIP;
			#endif
				vecClientHostNames.push_back(strCurrentIP);
			}
			else
			{
				vkernout << endl << endl;
				vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read client IP for client section " ;
				vkernout << *itClientSection << endl;
				vkernout << "FAILED TO CREATE MASTER DATA TUNNEL!" << endl <<endl;
				return NULL;
			}
			//retrieve client's data tunnel port
			iCurrentPort = localProfiler.GetTheProfileInt((*itClientSection), "DT_PORT", -1, strClusterIniFile);
			if(iCurrentPort>0)
			{
			#ifdef DEBUG
				vkernout << ":" << iCurrentPort << " to master data tunnel" << endl;
			#endif
				vecClientDTPorts.push_back(iCurrentPort);
			}
			else
			{
				vkernout << endl << endl;
				vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read data tunnel port for client section " ;
				vkernout << *itClientSection << endl;
				vkernout << "FAILED TO CREATE MASTER DATA TUNNEL!" << endl <<endl;
				return NULL;
			}
		}
		/*
		* HACK!!! give client setup a little time ahead
		* I guess we gonna need some more sophisticated handshake mechanism here
		*
		*/
		VistaTimeUtils::Sleep(5000);
		//create the tunnel itself
		pNewTunnel = new VistaMasterDataTunnel(vecClientHostNames,vecClientDTPorts);
	}
	else if(m_pClusterAux->GetIsMaster())
	{
			//fetch client's indices in order to know, which clients will connect to the data tunnel
			VistaProfiler localProfiler;

			std::vector<string> vecClientHostNames;
			std::vector<int> vecClientDTPorts;

			string strCurrentIP;
			int iCurrentPort;

			std::list<std::string> liClientSections;
			m_pClusterAux->GetVistaClusterMaster()->GetSlaveNames(liClientSections);

			if( liClientSections.empty())
			{
				std::string sServerName = m_pClusterAux->GetVistaClusterMaster()->GetMasterSectionName();
				localProfiler.GetTheProfileList (sServerName,"SLAVES", "", liClientSections, strClusterIniFile);
			}


			vecClientHostNames.reserve(liClientSections.size());
			vecClientDTPorts.reserve(liClientSections.size());

			for(std::list<std::string>::iterator itClientSection = liClientSections.begin();
				itClientSection != liClientSections.end();
				++itClientSection)
			{
				//retrieve clientIP
				localProfiler.GetTheProfileString((*itClientSection), "SLAVEIP", "", strCurrentIP, strClusterIniFile);
				if(!strCurrentIP.empty())
				{
				#ifdef DEBUG
					vkernout << "[DTFactory] Adding slave @ " << strCurrentIP;
				#endif
					vecClientHostNames.push_back(strCurrentIP);
				}
				else
				{
					vkernout << endl << endl;
					vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read slave IP for slave section " ;
					vkernout << *itClientSection << endl;
					vkernout << "FAILED TO CREATE MASTER DATA TUNNEL!" << endl <<endl;
					return NULL;
				}
				//retrieve client's data tunnel port
				iCurrentPort = localProfiler.GetTheProfileInt((*itClientSection), "DT_PORT", -1, strClusterIniFile);
				if(iCurrentPort>0)
				{
				#ifdef DEBUG
					vkernout << ":" << iCurrentPort << " to master data tunnel" << endl;
				#endif
					vecClientDTPorts.push_back(iCurrentPort);
				}
				else
				{
					vkernout << endl << endl;
					vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read data tunnel port for slave section " ;
					vkernout << *itClientSection << endl;
					vkernout << "FAILED TO CREATE MASTER DATA TUNNEL!" << endl <<endl;
					return NULL;
				}
			}
			/*
			* HACK!!! give client setup a little time ahead
			* I guess we gonna need some more sophisticated handshake mechanism here
			*
			*/
			VistaTimeUtils::Sleep(5000);
			//create the tunnel itself
			pNewTunnel = new VistaMasterDataTunnel(vecClientHostNames,vecClientDTPorts);
		}
	else if(m_pClusterAux->GetIsSlave())
		{
			/*
			* NOTE: this ASSUMES!!! that the index returned by GetVistaSystem()->GetVistaClusterClient()->GetClientIndex()
			* corresponds to the numbering of the .ini secions in the given ini file
			*/
			string strMySection = m_pClusterAux->GetVistaClusterSlave()->GetSlaveSectionName();
			VistaProfiler localProfiler;
			string strClientIP;
			//read client IP
			localProfiler.GetTheProfileString(strMySection, "SLAVEIP", "", strClientIP, strClusterIniFile);
			if(strClientIP.empty())
			{
				vkernout << endl << endl;
				vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read ip/hostname for slave index " ;
				vkernout << m_iNodeId << endl;
				vkernout << "FAILED TO CREATE SLAVE DATA TUNNEL!" << endl <<endl;
				return NULL;
			}
			//read client DT port
			int iClientDTPort = localProfiler.GetTheProfileInt(strMySection, "DT_PORT", -1, strClusterIniFile);
			if(iClientDTPort < 0)
			{
				vkernout << endl << endl;
				vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read data tunnel port for Slave index " ;
				vkernout << m_iNodeId << endl;
				vkernout << "FAILED TO CREATE SLAVE DATA TUNNEL!" << endl <<endl;
				return NULL;
			}
			//read (optional) queue size
			int iQueueSize = localProfiler.GetTheProfileInt(strMySection, "DT_QUEUESIZE", 2, strClusterIniFile);
			if(iQueueSize < 2) // 2 is the minimal sensible value we get. if smaller => reset!
			{
				vkernout << " *** WARNING *** [VistaDataTunnelFactory::CreateDataTunnel] Queue size set to value < 2. ";
				vkernout << "This will probably result in a lot of dropped packets!" << endl;
				vkernout << "RESET QUEUE SIZE TO 2" << endl;
				iQueueSize = 2;
			}
			#ifdef DEBUG
			vkernout << "[DTFactory] Creating slave data tunnel @ " << strClientIP << ":" << iClientDTPort << " with " << iQueueSize << " queue elements" << endl;
			#endif
			pNewTunnel = new VistaClientDataTunnel(strClientIP, iClientDTPort, iQueueSize, pPacketPrototype);
		}
	else if(m_pClusterAux->GetIsClient())
	{
		/*
		* NOTE: this ASSUMES!!! that the index returned by GetVistaSystem()->GetVistaClusterClient()->GetClientIndex()
		* corresponds to the numbering of the .ini secions in the given ini file
		*/
		string strMySection = m_pClusterAux->GetVistaClusterClient()->GetClientSectionName();
		VistaProfiler localProfiler;
		string strClientIP;
		//read client IP
		localProfiler.GetTheProfileString(strMySection, "CLIENTIP", "", strClientIP, strClusterIniFile);
		if(strClientIP.empty())
		{
			vkernout << endl << endl;
			vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read hostname for client index " ;
			vkernout << m_iNodeId << endl;
			vkernout << "FAILED TO CREATE CLIENT DATA TUNNEL!" << endl <<endl;
			return NULL;
		}
		//read client DT port
		int iClientDTPort = localProfiler.GetTheProfileInt(strMySection, "DT_PORT", -1, strClusterIniFile);
		if(iClientDTPort < 0)
		{
			vkernout << endl << endl;
			vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read data tunnel port for client index " ;
			vkernout << m_iNodeId << endl;
			vkernout << "FAILED TO CREATE CLIENT DATA TUNNEL!" << endl <<endl;
			return NULL;
		}
		//read (optional) queue size
		int iQueueSize = localProfiler.GetTheProfileInt(strMySection, "DT_QUEUESIZE", 2, strClusterIniFile);
		if(iQueueSize < 2) // 2 is the minimal sensible value we get. if smaller => reset!
		{
			vkernout << " *** WARNING *** [VistaDataTunnelFactory::CreateDataTunnel] Queue size set to value < 2. ";
			vkernout << "This will probably result in a lot of dropped packets!" << endl;
			vkernout << "RESET QUEUE SIZE TO 2" << endl;
			iQueueSize = 2;
		}
		#ifdef DEBUG
		vkernout << "[DTFactory] Creating client data tunnel @ " << strClientIP << ":" << iClientDTPort << " with " << iQueueSize << " queue elements" << endl;
		#endif
		pNewTunnel = new VistaClientDataTunnel(strClientIP, iClientDTPort, iQueueSize, pPacketPrototype);
	}
	return pNewTunnel;
}

VistaNetworkSync *VistaDataTunnelFactory::CreateSync(const std::string &strClusterIniFile)
{
	VistaNetworkSync *pSync = NULL;
	
	if(m_pClusterAux->GetIsStandalone())
	{
		pSync = new VistaNetworkSync;
	}
	else if(m_pClusterAux->GetIsServer())
	{
		/*
		* HACK!!! give client setup a little time ahead
		* I guess we gonna need some more sophisticated handshake mechanism here
		*
		*/
		VistaTimeUtils::Sleep(5000);
		
		//create the sync
		VistaMasterNetworkSync *pMasterSync = new VistaMasterNetworkSync;
		
		//fetch client's indices in order to know, which clients will connect to the data tunnel
		VistaProfiler localProfiler;
		
		string strCurrentIP;
		int iCurrentPort;
		
		std::list<std::string> liClientSections;
		m_pClusterAux->GetVistaClusterServer()->GetClientNames(liClientSections);
		
		for(std::list<std::string>::iterator itClientSection = liClientSections.begin();
			itClientSection != liClientSections.end();
			++itClientSection)
		{
			//retrieve clientIP
			localProfiler.GetTheProfileString(
				(*itClientSection), "CLIENTIP", "", strCurrentIP, strClusterIniFile);
			//retrieve client's data tunnel port
			iCurrentPort = localProfiler.GetTheProfileInt(
				(*itClientSection), "DATA_SYNC_PORT", -1, strClusterIniFile);
			if(!strCurrentIP.empty() && iCurrentPort > 0)
			{
			#ifdef DEBUG
				vkernout << "[DTFactory] Adding client @ " << strCurrentIP << ":" << iCurrentPort << endl;;
			#endif
				pMasterSync->AddSlave(strCurrentIP, iCurrentPort);	
			}
			else
			{
				vkernout << endl << endl;
				vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read slave connection info for slave section " ;
				vkernout << *itClientSection << endl;
				vkernout << "FAILED TO CREATE MASTER DATA SYNC!" << endl <<endl;
				return NULL;
			}
		}
		pSync = pMasterSync;
	}
	else if(m_pClusterAux->GetIsMaster())
	{
			/*
			* HACK!!! give client setup a little time ahead
			* I guess we gonna need some more sophisticated handshake mechanism here
			*
			*/
			VistaTimeUtils::Sleep(5000);

			//create the sync
			VistaMasterNetworkSync *pMasterSync = new VistaMasterNetworkSync;

			//fetch client's indices in order to know, which clients will connect to the data tunnel
			VistaProfiler localProfiler;

			string strCurrentIP;
			int iCurrentPort;

			std::list<std::string> liClientSections;
			m_pClusterAux->GetVistaClusterMaster()->GetSlaveNames(liClientSections);

			for(std::list<std::string>::iterator itClientSection = liClientSections.begin();
				itClientSection != liClientSections.end();
				++itClientSection)
			{
				//retrieve clientIP
				localProfiler.GetTheProfileString(
					(*itClientSection), "SLAVEIP", "", strCurrentIP, strClusterIniFile);
				//retrieve client's data tunnel port
				iCurrentPort = localProfiler.GetTheProfileInt(
					(*itClientSection), "DATA_SYNC_PORT", -1, strClusterIniFile);
				if(!strCurrentIP.empty() && iCurrentPort > 0)
				{
				#ifdef DEBUG
					vkernout << "[DTFactory] Adding sync to slave @ " << strCurrentIP << ":" << iCurrentPort << endl;;
				#endif
					pMasterSync->AddSlave(strCurrentIP, iCurrentPort);
				}
				else
				{
					vkernout << endl << endl;
					vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateDataTunnel] Unable to read slave connection info for slave section " ;
					vkernout << *itClientSection << endl;
					vkernout << "FAILED TO CREATE MASTER DATA SYNC!" << endl <<endl;
					return NULL;
				}
			}
			pSync = pMasterSync;
	}
	else if(m_pClusterAux->GetIsSlave())
	{
			/*
			* NOTE: this ASSUMES!!! that the index returned by GetVistaSystem()->GetVistaClusterClient()->GetClientIndex()
			* corresponds to the numbering of the .ini secions in the given ini file
			*/
			string strMySection = m_pClusterAux->GetVistaClusterSlave()->GetSlaveSectionName();
			VistaProfiler localProfiler;
			string strSlaveIP("");
			localProfiler.GetTheProfileString(
				strMySection, "SLAVEIP", "", strSlaveIP, strClusterIniFile);
			int iSlavePort = localProfiler.GetTheProfileInt(
				strMySection, "DATA_SYNC_PORT", -1, strClusterIniFile);
			if(strSlaveIP.empty() || iSlavePort <= 0)
			{
				vkernout << endl << endl;
				vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateSync] Unable to read connection info for slave index " ;
				vkernout << m_iNodeId << endl;
				vkernout << "FAILED TO CREATE NEW SLAVE SYNC!" << endl <<endl;
				return NULL;
			}
			vkernout << "[DTFactory] Adding sync to slave @ " << strSlaveIP << ":" << iSlavePort << endl;;
			VistaSlaveNetworkSync *pSlaveSync = new VistaSlaveNetworkSync;
			pSlaveSync->InitMasterConnection(strSlaveIP, iSlavePort);
			pSync  = pSlaveSync;
	}
	else if(m_pClusterAux->GetIsClient())
	{
		/*
		* NOTE: this ASSUMES!!! that the index returned by GetVistaSystem()->GetVistaClusterClient()->GetClientIndex()
		* corresponds to the numbering of the .ini secions in the given ini file
		*/
		string strMySection = m_pClusterAux->GetVistaClusterClient()->GetClientSectionName();
		VistaProfiler localProfiler;
		string strSlaveIP("");
		localProfiler.GetTheProfileString(
			strMySection, "CLIENTIP", "", strSlaveIP, strClusterIniFile);
		int iSlavePort = localProfiler.GetTheProfileInt(
			strMySection, "DATA_SYNC_PORT", -1, strClusterIniFile);
		if(strSlaveIP.empty() || iSlavePort <= 0)
		{
			vkernout << endl << endl;
			vkernout << " *** ERROR *** [VistaDataTunnelFactory::CreateSync] Unable to read connection info for slave index " ;
			vkernout << m_iNodeId << endl;
			vkernout << "FAILED TO CREATE NEW SLAVE SYNC!" << endl <<endl;
			return NULL;
		}
		VistaSlaveNetworkSync *pSlaveSync = new VistaSlaveNetworkSync;
		pSlaveSync->InitMasterConnection(strSlaveIP, iSlavePort);
		pSync  = pSlaveSync;
	}
	return pSync;
}

void VistaDataTunnelFactory::DestroyFactory()
{
	if(m_pSingleton)
		delete m_pSingleton;
	m_pSingleton = NULL;
}
