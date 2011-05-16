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

#include "VistaClusterServer.h"
#include "VistaClusterAux.h"


#include <VistaTools/VistaProfiler.h>
#include <VistaTools/VistaProtocol.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
//#include <VistaKernel/EventManager/VistaInputEvent.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
//#include <VistaKernel/InteractionManager/VistaInputDevice.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>


#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>
#include <VistaInterProcComm/IPNet/VistaIPAddress.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>


#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaSerializingToolset.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

#include "VistaKernelOut.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////

using namespace std;

/**
* we will be in trouble with the below lines when using more than one
* server in a single application.
*/

static int iPsMax = 0; //<** we need this to count the maximum packet size **/
static int iSCount = 0; //<** we need this to count the number of packets sent **/

//static VistaProtocol rSyncTimings;
//static VistaProtocol rSendTimings;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

VistaClusterServer::VistaClusterServer(VistaEventManager *pEvMgr, 
										 std::string ServerName) :
	m_ServerName(ServerName),
	m_ServerSectionName(ServerName)
{
	m_ServerPort = 6678;
	m_NumberOfClients = 0;
	m_nWaitingPort = 0;
	m_nEventCounter = 0;
	m_iPacketNum = 0;
	m_nDelayAfterSend = 20;
	m_bDelayAfterSend = true;
	m_bWakeupDone = false;
	m_bDoSync = true;
	m_pEventBuffer = new VistaByteBufferSerializer;

	m_pSyncSocket = NULL;
	m_pClusterAux = NULL;

	//    rSyncTimings.SetFileName("sync-time.csv");
	//    rSyncTimings.SetSeparator(',');

	//    rSendTimings.SetFileName("send-time.csv");
	//    rSendTimings.SetSeparator(',');


	m_pTimer = new VistaTimer;

	m_bDelayBetweenACK = false;
	m_iDelayBetweenACK = 0;

	m_bDelayAfterBC = false;
	m_iDelayAfterBC = 0;

	// register as observer
	m_pEvMgr = pEvMgr;

	m_pEvMgr->RegisterObserver(this, VistaSystemEvent::GetTypeId());
	m_pEvMgr->RegisterObserver(this, VistaInteractionEvent::GetTypeId());
//	m_pEvMgr->RegisterObserver(this, VistaInputEvent::GetTypeId());
	m_pEvMgr->RegisterObserver(this, VistaExternalMsgEvent::GetTypeId());

}


VistaClusterServer::~VistaClusterServer()
{
	m_pEvMgr->UnregisterObserver(this, VistaEventManager::NVET_ALL);

	delete m_pTimer;

	if(m_pSyncSocket)
		m_pSyncSocket->CloseSocket();
	delete m_pSyncSocket;


	std::vector<VistaConnectionIP*>::iterator it;
	for(it = m_pClientAdminChannels.begin(); it != m_pClientAdminChannels.end(); ++it)
	{
		(*it)->Close();
		delete *it;
	}

	delete m_pEventBuffer;

	/*
	cout << "Writing timing file.\n";

	rSyncTimings.SetPrintHeader(false);
	rSyncTimings.SetOverwrite(true);
	rSyncTimings.SetWriteKey(false);
	rSyncTimings.WriteProtocol();

	rSendTimings.SetPrintHeader(false);
	rSendTimings.SetOverwrite(true);
	rSendTimings.SetWriteKey(false);
	rSendTimings.WriteProtocol();
	*/
}


int VistaClusterServer::GetPacketNum() const
{
	return m_iPacketNum;
}

bool VistaClusterServer::Init(const std::string &strConfigFileName)
{
	std::string     sTempString; //[255];
	VistaProfiler	LocalProfiler;
	if (LocalProfiler.GetTheProfileSection (m_ServerSectionName, strConfigFileName))
	{
		vkernout << "VistaClusterServer::Init [" << strConfigFileName
				<< " ; " << m_ServerSectionName
				<< "]" << endl;

		// determine master name
		// defaults to section name
		m_ServerName = LocalProfiler.GetTheProfileString(m_ServerSectionName, "NAME", m_ServerSectionName, strConfigFileName);
		vkernout << "[ClServ]: Setting user name to [" << m_ServerName << "]\n";
		
		// get ServerIP
		bool bDoesSwap = LocalProfiler.GetTheProfileBool(m_ServerSectionName,"BYTESWAP", false,
			strConfigFileName);
		m_bDoSync = LocalProfiler.GetTheProfileBool(m_ServerSectionName,"SYNC", true, strConfigFileName);

		if(m_bDoSync)
		{
			int iSyncPort = LocalProfiler.GetTheProfileInt(m_ServerSectionName, "GLSYNCPORT", 8888, strConfigFileName);
			string sSyncIP;

			LocalProfiler.GetTheProfileString (m_ServerSectionName,"CLIENTSYNCIP", "10.0.0.255",
				sSyncIP, strConfigFileName);

			vkernout << "[ClServ]: Doing glsync on ip [" << sSyncIP << "] and port [" << iSyncPort << "]\n";
			VistaSocketAddress adr(sSyncIP, iSyncPort);
			m_pSyncSocket = new VistaUDPSocket;
			(*m_pSyncSocket).OpenSocket();
			(*m_pSyncSocket).SetPermitBroadcast(1);
			(*m_pSyncSocket).ConnectToAddress(adr);
		}	
		SetAfterSendDelayLength(LocalProfiler.GetTheProfileInt(m_ServerSectionName, "POSTSENDDELAY", 0, strConfigFileName));
		vkernout << "[ClServ]: POSTSENDDELAY = " << m_nDelayAfterSend << endl;

		m_iDelayBetweenACK = LocalProfiler.GetTheProfileInt(m_ServerSectionName, "ACKDELAY", 0, strConfigFileName);
		vkernout << "[ClServ]: ACKDELAY = " << m_iDelayBetweenACK << endl;
		if(m_iDelayBetweenACK > 0)
			m_bDelayBetweenACK = true;

		m_iDelayAfterBC = LocalProfiler.GetTheProfileInt(m_ServerSectionName, "AFTERBCDELAY", 0, strConfigFileName);
		vkernout << "[ClServ]: AFTERBCDELAY = " << m_iDelayAfterBC << endl;
		if(m_iDelayAfterBC > 0)
			m_bDelayAfterBC = true;

		m_pEventBuffer->SetByteorderSwapFlag(bDoesSwap);

		vkernout << "[ViCluSer]: Server " << (bDoesSwap ? "DOES" : "does NOT") << " perform byte swapping.\n" << flush;

		// get ClientNames
		LocalProfiler.GetTheProfileList (m_ServerSectionName,"Clients", "",
			m_ClientNames, strConfigFileName);

		m_NumberOfClients = (int)m_ClientNames.size();
		vkernout << " Number of clients: " << m_NumberOfClients << "\n" << flush;

		// get ClientIPs
		std::list<std::string>::const_iterator listIter = m_ClientNames.begin();
		while (m_NumberOfClients && listIter != m_ClientNames.end()) 
		{
			LocalProfiler.GetTheProfileString (listIter->c_str(),"ClientIP", "-1",
				sTempString, strConfigFileName.c_str());
			int iClientPort = LocalProfiler.GetTheProfileInt(*listIter,"ClientPort", 3666,
				strConfigFileName);

			vkernout << " ClientIP: " << sTempString << ", Port=" << iClientPort << "\n" << flush;
			m_ClientIPs.push_back((std::string) sTempString);
			if(iClientPort == -1)
			{
				vkernout << "ClientPort for Client [" << listIter->c_str() << "] NOT FOUND!\n";
			}
			m_ClientPorts.push_back(iClientPort);
			++listIter;
		}
	}
	return true;
}


bool VistaClusterServer::AddClient(const std::string &sIpString, int iPort)
{
	VistaConnectionIP *pClientAdmin = EstablishConnection(sIpString, iPort);
	if(pClientAdmin)
	{
		m_pClientAdminChannels.push_back(pClientAdmin);
		return true;
	}

	return false;
}

VistaConnectionIP *VistaClusterServer::EstablishConnection(const std::string &sIp, int iPort)
{
	VistaConnectionIP *pClientAdmin = new VistaConnectionIP(VistaConnectionIP::CT_TCP, sIp, iPort);
	pClientAdmin->SetByteorderSwapFlag(m_pEventBuffer->GetByteorderSwapFlag());
	if(pClientAdmin->GetIsOpen())
	{
		// connection established.
		pClientAdmin->SetIsBlocking(true);
		pClientAdmin->SetIsBuffering(false);
		// write initial frame clock
		pClientAdmin->WriteDouble(m_dFrameClock);
	}
	else
	{
		// connection failed.
		vkernerr << " FAILED TO INIT CLIENT @ip ="
			<< sIp << std::endl;

		delete pClientAdmin;
		pClientAdmin = NULL;
	}

	return pClientAdmin; // can be NULL
}

// Wake-up awaiting ViSTA-Cluster-Clients
bool VistaClusterServer::RemoteWakeUpClients( )
{
	if(m_bWakeupDone)
		return true;

	std::list<std::string>::const_iterator it;
	std::list<int>::const_iterator pit;

	m_dFrameClock = m_pTimer->GetSystemTime();
	
	assert(m_pClusterAux);

	m_pClusterAux->SetRandomSeed((int)m_dFrameClock); // reset seed for this server

	for(it = m_ClientIPs.begin(), pit=m_ClientPorts.begin(); it != m_ClientIPs.end(); ++it, ++pit)
	{
		// create admin ports
		if(!AddClient(*it, *pit))
		{
			// report error
			vkernerr << std::string("[ViClSr]: Failed to add client @ (")
				<< *it <<  ")" << std::endl;
		}
	}

	return true;
}

int VistaClusterServer::GetNumberOfClients() const
{
	return (int)m_pClientAdminChannels.size();
}

void VistaClusterServer::GetClientNames(std::list<std::string> &liStorage) const
{
	liStorage = m_ClientNames;
}

bool VistaClusterServer::ClientSync(bool bDoGLSync)
{
	int iAck = 0;
	int iRet = 0;

// 	cout << "VistaClusterServer::ClientSync()\n -- TCP ACK";
	//    VistaProtocolMeasure(rSyncTimings, m_iPacketNum);
	std::list<VistaConnectionIP*> liBrokenClient;
	for(std::vector<VistaConnectionIP *>::const_iterator cit = m_pClientAdminChannels.begin();
		cit != m_pClientAdminChannels.end(); ++cit)
	{

		iRet = (*cit)->ReadInt32(iAck);
		if(iRet <= 0)
		{
			// we should remove this client
			//throw;
			vkernout << "-- client broken, adding to remove list\n";
			liBrokenClient.push_back(*cit);
			continue;
		}

		if(m_bDelayBetweenACK)
		{
			VistaTimeUtils::Sleep( m_iDelayBetweenACK );
		}

		if(iAck != m_iPacketNum)
		{
			// we should utter a warning here
			VistaSocketAddress add = (*cit)->GetPeerAddress();
			VistaIPAddress ip;
			add.GetIPAddress(ip);
			std::string sName;
			ip.GetHostName(sName);

			vkernout << "iAck from client [" << sName.c_str() << "] WRONG ("
				<< iAck << " != " << m_iPacketNum << ")\n";

			// we do remove this client, it missed an event
			liBrokenClient.push_back(*cit);
		}
	}

	for(std::list<VistaConnectionIP*>::const_iterator lit = liBrokenClient.begin();
		lit != liBrokenClient.end(); ++lit)
	{
		std::vector<VistaConnectionIP*>::iterator vit = std::find(
			m_pClientAdminChannels.begin(),
			m_pClientAdminChannels.end(), *lit);

		if(vit != m_pClientAdminChannels.end())
		{
			VistaSocketAddress peer = (*vit)->GetPeerAddress();
			VistaIPAddress peerIP = peer.GetIPAddress();
			vkernout << "Broken Client [" << peerIP.GetIPAddressC() << "] -- REMOVE from ACK-List\n";
			m_pClientAdminChannels.erase(vit);
		}
		else
		{
			vkernout <<  "?? Client not found?\n";
		}
	}

// 	cout << "... DONE\nUDP ACK";
	if( bDoGLSync && m_pSyncSocket &&   (*m_pSyncSocket).GetIsOpen() )
	{
		//cout << ">\n";
		//int i;
		//cin >> i;

// 		cout << "SEND BC (" << m_iPacketNum << ")\n";
		try
		{
			(*m_pSyncSocket).SendRaw(&m_iPacketNum, sizeof(int)); // broadcast for buffer swap
			if(m_bDelayAfterBC)
			{
				VistaTimeUtils::Sleep(m_iDelayAfterBC);
			}
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
			delete m_pSyncSocket;
			m_pSyncSocket = NULL;
		}
	}
// 	cout << "... DONE\n";
	return true;
}

void VistaClusterServer::BroadcastEvents( )
{

	++m_iPacketNum;

	/**
	* We are writing the header to the buffer before we broadcast,
	* as we have all the information needed at hand
	*/
	VistaByteBufferSerializer pBuf;
	pBuf.SetByteorderSwapFlag(m_pEventBuffer->GetByteorderSwapFlag());

	// total number of bytes to expect
	// sizeof(m_iPacketNum)+sizeof(m_nEventCounter)+sizeof(m_dFrameClock)
	// + |buffer|
	pBuf.WriteInt32(uint32(2*sizeof(sint32)+sizeof(double)+m_pEventBuffer->GetBufferSize()));


	pBuf.WriteInt32(m_iPacketNum);
	pBuf.WriteInt32(m_nEventCounter);
	pBuf.WriteDouble(m_dFrameClock);

	// this is a memcopy, we should get rid of it
	pBuf.WriteRawBuffer((void*)m_pEventBuffer->GetBuffer(), m_pEventBuffer->GetBufferSize());

	// we create a randomly shuffled list of recipients
	// for this, we copy the original list to a temporary
	std::vector<VistaConnectionIP*> m_vecRandomList(m_pClientAdminChannels.begin(), m_pClientAdminChannels.end());
	std::random_shuffle(m_vecRandomList.begin(), m_vecRandomList.end());

	for(std::vector<VistaConnectionIP*>::const_iterator it = m_vecRandomList.begin();
		it != m_vecRandomList.end(); ++it)
	{
		// we write the complete package to the client stream
		int iRetRaw = 0;

		try
		{
			iRetRaw = (*it)->WriteRawBuffer(pBuf.GetBuffer(), pBuf.GetBufferSize());
			// do a little statistics here
			if(pBuf.GetBufferSize() > iPsMax)
				iPsMax = pBuf.GetBufferSize(); // record largest packet
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
			iRetRaw = -1; // mark client for deletion
		}

		// if iRetRaw is below zero, we have a network error for sure
		if(iRetRaw<0)
		{
			
			std::string sTmp;
			(*it)->GetPeerAddress().GetIPAddress().GetHostName(sTmp);
			vkernerr << " [ViClSv]: BROKEN PIPE TO CLIENT "
				<< sTmp << std::endl;

			std::vector<VistaConnectionIP*>::iterator it_f = std::find(m_pClientAdminChannels.begin(), m_pClientAdminChannels.end(), *it);
			if(it_f != m_pClientAdminChannels.end())
			{
				delete *it_f;
				m_pClientAdminChannels.erase(it_f);
			}
			else
			{
				if((m_pClientAdminChannels.size() == 1) && (m_pClientAdminChannels.front() == *it))
				{
					(*it)->Close();
					m_pClientAdminChannels.clear();
					delete *it; // kill channel
				}
			}
		}
	}


	ClientSync(m_bDoSync);

	// reset event count for the next frame
	m_nEventCounter = 0;

	// clear temporary storage
	(*m_pEventBuffer).ClearBuffer();


	// deley, iff needed
	if(m_bDelayAfterSend)
		VistaTimeUtils::Sleep(m_nDelayAfterSend);
}

bool VistaClusterServer::GetDoesDelayAfterSend() const
{
	return m_bDelayAfterSend;
}

void VistaClusterServer::SetDoesDelayAfterSend(bool bDelay)
{
	m_bDelayAfterSend = bDelay;
}

int  VistaClusterServer::GetAfterSendDelayLength() const
{
	return m_nDelayAfterSend;
}

void VistaClusterServer::SetAfterSendDelayLength(int nDelay)
{
	m_nDelayAfterSend = nDelay;
	SetDoesDelayAfterSend(m_nDelayAfterSend>0);
}


void VistaClusterServer::RecordEvent(const VistaEvent & ev)
{
	++m_nEventCounter;
	(*m_pEventBuffer).WriteInt32(ev.GetType());
	(*m_pEventBuffer).WriteInt32(ev.GetId());
	(*m_pEventBuffer).WriteSerializable(ev);
}

void VistaClusterServer::SetClusterAux(VistaClusterAux *pAux)
{
	m_pClusterAux = pAux;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaClusterServer::Debug ( ostream & out ) const
{
	out << " [ViCluServ] ....\n";
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetServerName                                               */
/*                                                                            */
/*============================================================================*/
std::string VistaClusterServer::GetServerSectionName() const
{
	return m_ServerSectionName;
}

std::string VistaClusterServer::GetServerName() const
{
	return m_ServerName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Notify                                                      */
/*                                                                            */
/*============================================================================*/
void VistaClusterServer::Notify(const VistaEvent *pEvent)
{
	if(pEvent->GetType() == VistaSystemEvent::GetTypeId())
		NotifySystemEvent(*pEvent);
	//else if(pEvent->GetType() == VistaInputEvent::GetTypeId())
	//	NotifyInputEvent(*pEvent);
	else if(pEvent->GetType() == VistaInteractionEvent::GetTypeId())
		RecordEvent(*pEvent);
	// IAR: note that the following can only be preliminary
	// thinks of some "more clear" way to transfer information
	// from server to clients in frame
	else if(pEvent->GetType() == VistaExternalMsgEvent::GetTypeId())
		RecordEvent(*pEvent);

}

void VistaClusterServer::WaitForClientInit()
{
	vkernout << " Server Init-Phase DONE. Waiting for ACK from Clients.\n";

	for(std::vector<VistaConnectionIP *>::const_iterator cit = m_pClientAdminChannels.begin();
		cit != m_pClientAdminChannels.end(); ++cit)
	{
		std::string sTmp;
		int iSize = 0;

		vkernout << "Waiting for alive signal from client " << (*cit)->GetPeerAddress().GetIPAddress().GetHostNameC() << std::endl;

		try
		{
			int iRet = (*cit)->ReadInt32(iSize);
			int iRet1= (*cit)->ReadString(sTmp, iSize);
			if(iRet>0 && iRet1>0)
				vkernout << "Client [" << sTmp.c_str() << "] seems to be alive (iSize=" << iSize << ").\n";
			else
			{
				vkernout << "Error while trying to get alive signal" ;
				continue;
			}
		}
		catch(VistaExceptionBase &x)
		{
			vkernout << "Error while trying to get alive signal" ;
			x.PrintException();
			continue;
		}

	}

	// we will try to init us
	vkernout << " All ViSTA-Clients specified in the vista.ini are awake.\n"
			  << "-------------------------------------------------------------\n"
			  << " -------------- Remote Wake up Section --------------- END ---\n"
			  << " -------------------------------------------------------------\n";

	m_bWakeupDone = true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   NotifySystemEvent                                           */
/*                                                                            */
/*============================================================================*/


void VistaClusterServer::NotifySystemEvent(const VistaEvent &pEvent)
{

	++iSCount;

	switch(pEvent.GetId())
	{
	case VistaSystemEvent::VSE_INIT:
		{
			vkernout << "Skipping INIT-Event: " << iSCount << endl << flush;
			break;
		}
	case VistaSystemEvent::VSE_QUIT:
		{

			vkernout << "Skipping QUIT-Event: " << iSCount << endl << flush;
			//		    RecordEvent(pEvent);
			//            BroadcastEvents();
			vkernout << "Biggest packet in bytes: [" << iPsMax << "]\n";
			VistaTimeUtils::Sleep(500);
			break;
		}
	case VistaSystemEvent::VSE_PREGRAPHICS:
		{
			RecordEvent(pEvent);
			if(m_nEventCounter)
				BroadcastEvents();
			break;
		}
	case VistaSystemEvent::VSE_POSTGRAPHICS:
		{
			m_dFrameClock = m_pTimer->GetSystemTime(); // record frame-clock
			RecordEvent(pEvent);
			break;
		}
	default:
		RecordEvent(pEvent);
		break;
	}
}

bool VistaClusterServer::GetDoesDelayAfterACK() const
{
	return m_bDelayBetweenACK;
}

void VistaClusterServer::SetDoesDelayAfterACK(bool b)
{
	m_bDelayBetweenACK = b;
}

int  VistaClusterServer::GetAfterSendACKDelayLength() const
{
	return m_iDelayBetweenACK;
}

void VistaClusterServer::SetAfterSendACKDelayLength(int iTime)
{
	m_iDelayBetweenACK = iTime;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   NotifyInputEvent                                            */
/*                                                                            */
/*============================================================================*/
void VistaClusterServer::NotifyInputEvent(const VistaEvent &pEvent)
{
	RecordEvent(pEvent);
}

double VistaClusterServer::GetFrameClock() const
{
	return m_dFrameClock;
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( ostream & out, const VistaClusterServer & device )
{
	device.Debug ( out );
	return out;
}


