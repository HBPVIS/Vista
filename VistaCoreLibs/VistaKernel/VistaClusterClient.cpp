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

#include "VistaClusterClient.h"
#include "VistaClusterAux.h"

#include <VistaTools/VistaProfiler.h>
#include <VistaTools/VistaProtocol.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
//#include <VistaKernel/EventManager/VistaInputEvent.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>

//#include <VistaKernel/InteractionManager/VistaOldInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>

#include <VistaInterProcComm/DataLaVista/Base/VistaDataSource.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaActiveDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaByteBufferPacket.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaRamQueuePipe.h>

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>

#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>

#include "VistaKernelOut.h"

#include <cassert>

using namespace std;

//static VistaProtocol rRecTime, rLoopTime;

class VistaVector3D;
class VistaTransformMatrix;


#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif 

class VistaEventSucker : public DLVistaDataSource
{
private:
	VistaConnectionIP *m_pConnection;

	std::string m_sServerHostName;
	int         m_iServerPortNumber;
	double       m_dInitialFrameClock;
protected:
public:
	VistaEventSucker( const std::string &sHostName, int iPort, bool bDoesByteSwap);

	virtual ~VistaEventSucker()
	{
		if(m_pConnection)
			m_pConnection->Close();
		delete m_pConnection;
	}

	IDLVistaDataPacket *CreatePacket()
	{
		return new DLVistaByteBufferPacket(this);
	}

	void FillPacket(IDLVistaDataPacket *p);

	bool HasPacket() const;

	void DeletePacket(IDLVistaDataPacket *p) { delete p; }

	VistaConnectionIP *GetConnection() const { return m_pConnection; }

	double GetInitialFrameClock() const
	{
		return m_dInitialFrameClock;
	}
};


class VistaOpenGLSync{
public:
	VistaOpenGLSync(int &rPacketCount,
						bool bDoGlFinish,
						int iAfterBCDelay);
	virtual ~VistaOpenGLSync();

	void NotifyUpdate();
	void CommitUpdate();
	bool NeedsUpdate();

	void SetBB(const VistaVector3D&, const VistaVector3D&);
	void SetTransform(const VistaTransformMatrix& );

	void SetSyncSocket(VistaUDPSocket *);
	VistaUDPSocket *GetSyncSocket() const;

	void SetAckConnection(VistaConnectionIP *);
	VistaConnectionIP *GetAckConnection() const;

	void SetAfterBCDelay(int iDelay);
	int  GetAfterBCDelay() const;

	void SetDoGLFinish(bool bDoIt);
	bool GetDoGLFinish() const;
	static void OGLCallback(void* pData);
protected:


	bool	m_bNeedsUpdate;
	bool    m_bDoGLFinish;
	int    &m_rPacketCount;
	int     m_iAfterBCDelay;

	VistaUDPSocket *m_pSyncSocket;
	VistaConnectionIP *m_pAckConnection;
};

VistaOpenGLSync::VistaOpenGLSync(int &rPacketCount,
					bool bDoGLFinish, int iAfterBCDelay)
:	m_bNeedsUpdate(true),
	m_pSyncSocket(NULL),
	m_pAckConnection(NULL),
	m_bDoGLFinish(bDoGLFinish),
	m_iAfterBCDelay(iAfterBCDelay),
	m_rPacketCount(rPacketCount)
{
}

VistaOpenGLSync::~VistaOpenGLSync()
{

}

void VistaOpenGLSync::SetSyncSocket(VistaUDPSocket *pCon)
{
	m_pSyncSocket = pCon;
}

VistaUDPSocket *VistaOpenGLSync::GetSyncSocket() const
{
	return m_pSyncSocket;
}

void VistaOpenGLSync::SetAckConnection(VistaConnectionIP *pCon)
{
	m_pAckConnection = pCon;
}

VistaConnectionIP *VistaOpenGLSync::GetAckConnection() const
{
	return m_pAckConnection;
}


void VistaOpenGLSync::NotifyUpdate()
{
	m_bNeedsUpdate = true;
}

void VistaOpenGLSync::CommitUpdate()
{
	m_bNeedsUpdate = false;
}

bool VistaOpenGLSync::NeedsUpdate()
{
	return m_bNeedsUpdate;
}

void VistaOpenGLSync::OGLCallback(void* pData)
{
	VistaOpenGLSync* pSyncNode = (VistaOpenGLSync*) pData;

	int iSync=0;
	if(pSyncNode->m_bDoGLFinish)
		glFinish(); // flush as best as can

//	cout << "TCP-ACK.\n";
	if(pSyncNode->GetAckConnection())
		pSyncNode->GetAckConnection()->WriteInt32(pSyncNode->m_rPacketCount);
//	cout << "... DONE\nUDP-ACK, pending\n";
	int iDummy = 0;
	if(pSyncNode->GetSyncSocket())
	{
		pSyncNode->GetSyncSocket()->ReceiveRaw(&iDummy, sizeof(int));
	}

	if(pSyncNode->m_iAfterBCDelay > 0)
		VistaTimeUtils::Sleep(pSyncNode->m_iAfterBCDelay);

	//cout << "... DONE (" << pSyncNode->m_rPacketCount << ", "
//		<< iDummy << ")\n";


#if defined(DEBUG)
	if(pSyncNode->GetSyncSocket())
	{
		if(pSyncNode->m_rPacketCount != iDummy)
		{
			vkernerr << "WARNING! PC(" << pSyncNode->m_rPacketCount << ") != DM ("
				<< iDummy << ") -- client out of sync?\n";
		}
	}
#endif
}

void VistaOpenGLSync::SetAfterBCDelay(int iDelay)
{
	m_iAfterBCDelay = iDelay;
}

int  VistaOpenGLSync::GetAfterBCDelay() const
{
	return m_iAfterBCDelay;
}

void VistaOpenGLSync::SetDoGLFinish(bool bDoIt)
{
	m_bDoGLFinish = bDoIt;
}

bool VistaOpenGLSync::GetDoGLFinish() const
{
	return m_bDoGLFinish;
}

class VistaEventPacket : public IDLVistaDataPacket
{
private:
	VistaEvent *m_pEvent;
	bool bValid;
protected:
public:
	VistaEventPacket(IDLVistaPipeComponent *p)
		: IDLVistaDataPacket(p)
	{
		//m_pEvent = new VistaInputEvent(pMgr);
        m_pEvent = NULL;
		bValid = false;
	}

	virtual ~VistaEventPacket()
	{
		delete m_pEvent;
		//m_pEvent = (VistaEvent*)0xDEADBEEF;
	}

	int GetPacketType() const { return -1; }

	int GetPacketSize() const { return sizeof(VistaEvent*)+sizeof(VistaEvent); }

	void SetDataSize(int) {}
	int GetDataSize() const { return -1; }

	IDLVistaDataPacket * Clone() const
	{
		return 0;
	}

	virtual std::string GetClName() const
	{
		return "VistaEventPacket";
	}

	VistaEvent *GetEvent() const { return m_pEvent; }

	bool IsValid() const { return bValid; }

	void SetValidFlag(bool bNew) {bValid = bNew; }
};

VistaEventSucker::VistaEventSucker(const std::string &sHostName, int iPort, bool bDoesByteSwap)
:  m_sServerHostName(sHostName), m_iServerPortNumber(iPort)
{
	SetQueueSize(500);
	VistaTCPServer server(sHostName, iPort);
	if(server.GetIsValid())
	{
		// this call will block
		VistaTCPSocket *serverPlug = server.GetNextClient();

		if(serverPlug)
		{
			m_pConnection = new VistaConnectionIP(serverPlug);
			m_pConnection->SetIsBuffering(true);
			m_pConnection->SetIsBlocking(true);
			m_pConnection->SetByteorderSwapFlag(bDoesByteSwap);

			m_pConnection->ReadDouble(m_dInitialFrameClock);
			vkernout << "EventSucker: Initial fram clock = " << m_dInitialFrameClock
				<< endl;
		}
		else
			VISTA_THROW("VistaEventSucker, accept delivered NULL client.\n", 0x00000201);
	}

	else
	{
		vkernout << "Server-Port error, good bye.\n";
		VISTA_THROW("VistaEventSucker, could not bind cluster-client-plug", 0x00000101);
	}

}


void VistaEventSucker::FillPacket(IDLVistaDataPacket *p)
{
	DLVistaByteBufferPacket *pPack = static_cast<DLVistaByteBufferPacket*>(p);

//	m_pConnection->SetShowRawSendAndReceive(true);
	//VistaProtocolMeasure m(rRecTime, GetPacketCount());
	int iBufferSize = 1024;
//    printf("VistaEventSucker::FillPacket()\n");
	m_pConnection->ReadInt32(iBufferSize);
//    printf("iBufferSize = %d\n", iBufferSize);

	pPack->SetDataSize(iBufferSize);

//    printf("VistaEventSucker::ReadingRawBuffer()\n");
	if(m_pConnection->ReadRawBuffer(pPack->GetByteBuffer(), iBufferSize) == iBufferSize)
	{
		pPack->SetIsValid(true);
	//printf("bp=%d\n", m_pConnection->PendingDataSize());
	}
	else
	{
		//printf("VistaEventSucker::FillPacket() -- failed to read packet size %d\n", iBufferSize);
		pPack->SetIsValid(false);
		VISTA_THROW("COULD NOT FILL PACKET IN EVENTSUCKER -- this is serious!", 0x0000005);
	}
}

bool VistaEventSucker::HasPacket() const
{
	return true; //m_pConnection->HasPendingData();
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

VistaClusterClient::VistaClusterClient(VistaEventManager * pEvMa, std::string& ClientName) :
	m_ClientName(ClientName),
	m_ClientSectionName(ClientName)

{
	m_pEvMgr = pEvMa;

	m_ProcessingEvents=0;
	m_PacketCount = 0;
	m_bAsyncMode = false;
	m_bDoSync = false;
	m_iClientIndex = 0;
	m_iGLSyncUDPPort  = 0;
	m_pSyncSocket = NULL;

	m_pQueue = NULL;
	m_pEventSucker = NULL;
	m_pActivator = NULL;
	m_pBuffer = new VistaByteBufferDeSerializer;
	//m_pInEvent = NULL;
	m_pIntEvent = NULL;

	m_pSysEvent = new VistaSystemEvent;
	m_pExtMsgEvent = new VistaExternalMsgEvent;
	m_pTmpMsg      = new VistaMsg;

	m_pExtMsgEvent->SetThisMsg(m_pTmpMsg);


	m_bDoesDelayAfterReceive = false;
	m_iAfterReceiveDelay = 0;
	m_pSyncNode = NULL;

	m_pSyncSocket = NULL;

	m_dFrameClock = 0.0;
	m_pClusterAux = NULL;
}


VistaClusterClient::~VistaClusterClient()
{
	if(m_bAsyncMode && m_pActivator)
		m_pActivator->StopComponentGently(true);
	delete m_pBuffer;
	delete m_pQueue;
	delete m_pActivator;
	delete m_pEventSucker;
//	delete m_pInEvent;
	delete m_pIntEvent;
	delete m_pSysEvent;
	delete m_pExtMsgEvent;
	delete m_pTmpMsg;

	if(m_pSyncSocket)
		m_pSyncSocket->CloseSocket();
	delete m_pSyncSocket;
}

bool VistaClusterClient::ConsumePacket(IDLVistaDataPacket *pPacket)
{

	DLVistaByteBufferPacket *pPack = static_cast<DLVistaByteBufferPacket*>(pPacket);
	if(pPack->IsValid())
	{
		// inject
		m_pBuffer->SetBuffer((char*)pPack->GetByteBuffer(),
			pPack->GetDataSize());
		int iEventCnt=0;

		int iTmp = m_PacketCount;
		m_pBuffer->ReadInt32(m_PacketCount);
		m_pBuffer->ReadInt32(iEventCnt);
		m_pBuffer->ReadDouble(m_dFrameClock);

		// ideally, we have the "follow-up packet", which is
		// curIdx = lastIdx + 1
		if(m_PacketCount-iTmp != 1)
			printf("### WARNING: Packet Drop! (%d, %d)\n", m_PacketCount, iTmp);

		for(int i=0; i < iEventCnt; ++i)
		{
			VistaEvent *pProcessEvent = NULL;
			int iType=0;
			int iId=0;
			if((m_pBuffer->ReadInt32(iType)<=0) || (m_pBuffer->ReadInt32(iId)<=0) )
			{
				goto recycle;
			}

			// we expect system event to be the most common
			// followed by input/interaction events
			// external messages should be few in number
			if(iType == VistaSystemEvent::GetTypeId())
			{
				pProcessEvent = m_pSysEvent;
			}
			//else if(m_pInEvent &&
			//	(iType == VistaInputEvent::GetTypeId()))
			//{
			//	pProcessEvent = m_pInEvent;
			//}
			else if(m_pIntEvent &&
				(iType == VistaInteractionEvent::GetTypeId()))
			{
				pProcessEvent = m_pIntEvent;
			}
			else if(iType == VistaExternalMsgEvent::GetTypeId())
			{
				pProcessEvent = m_pExtMsgEvent;
			}
			else
			{
				printf("received event type %d -- UNKNOWN or UNHANDLED\n", iType);
				goto recycle;
			}

			if(!pProcessEvent
						 || m_pBuffer->ReadSerializable(*(pProcessEvent))==-1)
				goto recycle;

			// process the received event
			m_pEvMgr->ProcessEvent(pProcessEvent);
		}
	}
	else
	{
		// we should stop processing!! one invalid packet means
		// total loss of control!
	}
recycle:
	if(GetDoesDelayAfterReceive() && m_iAfterReceiveDelay)
	{
		// this is for testing purposes (simulate different delays on
		// different clients)
		VistaTimeUtils::Sleep(m_iAfterReceiveDelay);
	}
	m_pDataInput->RecycleDataPacket(pPacket, this, true);
	return true;
}

int & VistaClusterClient::GetPacketCount()
{
	return m_PacketCount;
}

int VistaClusterClient::GetClientIndex() const
{
	return m_iClientIndex;
}

bool VistaClusterClient::Init(const std::string &strConfigFileName)
{
	VistaProfiler	LocalProfiler;

	vkernout << "ClusterClient::Init [" << strConfigFileName
	     << " ; " << m_ClientSectionName
		 << "]" << endl;

	if (LocalProfiler.GetTheProfileSection(m_ClientSectionName, strConfigFileName))
	{

		m_ClientName = LocalProfiler.GetTheProfileString( m_ClientSectionName, "NAME", m_ClientSectionName, strConfigFileName );
		vkernout << "[ClClient]: Setting user name to [" << m_ClientName << "]\n";

		// get ServerName
		LocalProfiler.GetTheProfileString (m_ClientSectionName,"ClientIP", "-1",
			m_ClientIP, strConfigFileName);
		vkernout << " ClientIP: " << m_ClientIP.c_str() << "\n" << flush;

		m_bAsyncMode = LocalProfiler.GetTheProfileBool (m_ClientSectionName,"ASYNCMODE", false, strConfigFileName);
		m_bDoSync    = LocalProfiler.GetTheProfileBool (m_ClientSectionName,"SYNC", true, strConfigFileName);
		m_iClientIndex = LocalProfiler.GetTheProfileInt(m_ClientSectionName,"INDEX", -1, strConfigFileName);

		m_ClientPort = LocalProfiler.GetTheProfileInt (m_ClientSectionName,"ClientPort", 3666, strConfigFileName);

		if(m_bDoSync)
		{
	#if defined(VISTA_SYS_OPENSG)
				m_pSyncNode = new VistaOpenGLSync(m_PacketCount, true, 0);
		m_pSyncNode->SetAfterBCDelay(LocalProfiler.GetTheProfileInt(m_ClientSectionName, "AFTERBCDELAY", 0, strConfigFileName));
		vkernout << "AFTERBDDELAY = " << m_pSyncNode->GetAfterBCDelay() << endl;
		m_pSyncNode->SetDoGLFinish(LocalProfiler.GetTheProfileBool(m_ClientSectionName, "DOGLFINISH", true, strConfigFileName));
		vkernout << "DOGLFINISH = " << (m_pSyncNode->GetDoGLFinish() ? "TRUE" : "FALSE") << endl;
	#endif

			m_iGLSyncUDPPort = LocalProfiler.GetTheProfileInt(m_ClientSectionName, "GLSYNCPORT", 8888, strConfigFileName);
			LocalProfiler.GetTheProfileString (m_ClientSectionName,"CLIENTSYNCIP", "10.0.0.255",
				m_ClientSyncIP, strConfigFileName);

			vkernout << "Client [" << m_ClientName.c_str() << "] @ "
				<< m_iClientIndex << " is in " << (m_bAsyncMode ? "ASYNCMODE" : "SYNCMODE") << endl;


			vkernout << " ClientPort: " << m_ClientPort << "\n"
				<< "ClientGLSyncIP:" << m_ClientSyncIP <<
				", ClientGLSyncPort: " << m_iGLSyncUDPPort << endl << flush;

			m_pSyncSocket = new VistaUDPSocket;
			if(m_pSyncSocket->OpenSocket())
			{
				m_pSyncSocket->SetPermitBroadcast(1);

				VistaSocketAddress adr(m_ClientSyncIP, m_iGLSyncUDPPort);
				if(!m_pSyncSocket->BindToAddress(adr))
				{
					vkernout << "[ViClCl]: Could not bind SYNC-Socket, check ip and port\n";
					delete m_pSyncSocket;
					m_pSyncSocket = NULL;
					return false;
				}
			}
			else
			{
				delete m_pSyncSocket;
				m_pSyncSocket = NULL;
				return false;
			}
		}
		else
		{
			vkernout << "[ViClCl]: Could (or should) not open SYNC-Socket, strange... \n";
		}

		bool bDoesSwapping = LocalProfiler.GetTheProfileBool (m_ClientSectionName,"BYTESWAP", false, strConfigFileName);
		vkernout << "Client [" << m_ClientName.c_str() << "] " << (bDoesSwapping ? "DOES" : "does NOT") << " swap bytes on receive" << endl;
		if(m_ClientPort != -1 && m_ClientIP != "-1")
		{
			try
			{
				m_pEventSucker = new VistaEventSucker(m_ClientIP, m_ClientPort, bDoesSwapping);
				m_pBuffer->SetByteorderSwapFlag(bDoesSwapping);

				m_dFrameClock = m_pEventSucker->GetInitialFrameClock();

				assert(m_pClusterAux);

				m_pClusterAux->SetRandomSeed((int)m_dFrameClock); // set random seed for the system
				if(m_bAsyncMode)
				{
					m_pActivator = new DLVistaActiveDataProducer(m_pEventSucker);
					m_pQueue = new DLVistaRamQueuePipe;

					this->AttachInputComponent(m_pQueue);
					m_pQueue->AttachOutputComponent(this);

					m_pQueue->AttachInputComponent(m_pActivator);
					m_pActivator->AttachOutputComponent(m_pQueue);
				}
				else
				{
					// connect sucker and this
					this->AttachInputComponent(m_pEventSucker);
					m_pEventSucker->AttachOutputComponent(this);
				}
			}
			catch(VistaExceptionBase &x)
			{
				// we will olny clean up here
				delete m_pEventSucker;
				delete m_pActivator;
				delete m_pQueue;
				m_pEventSucker = NULL;
				m_pActivator = NULL;
				m_pQueue = NULL;

				delete m_pSyncSocket;
				m_pSyncSocket = NULL;
				throw x; // rethrow
			}
		}
		else
			vkernout << "WRONG SERVER CREDENTIALS PASSED IN INI: " << m_ClientIP << ", port= " << m_ClientPort << endl;

		return true;
	}
	else
		vkernout << "[ViClCl]: No Cluster-client section found. skipping init, cluster-client will not work.\n";
   return false;
}

bool VistaClusterClient::GetDoesDelayAfterReceive() const
{
	return m_bDoesDelayAfterReceive;
}

void VistaClusterClient::SetDoesDelayAfterReceive(bool bDoesIt)
{
	m_bDoesDelayAfterReceive = bDoesIt;
}

int  VistaClusterClient::GetAfterReceiveDelayLength() const
{
	return m_iAfterReceiveDelay;
}

void VistaClusterClient::SetAfterReceiveDelayLength(int iDelay)
{
	m_iAfterReceiveDelay = iDelay;
}

bool VistaClusterClient::GetIsProcessingEvents() const
{
	return m_ProcessingEvents;
}


bool VistaClusterClient::Sync()
{
	//printf("packetCount = %d\n", m_PacketCount);
	m_pEventSucker->GetConnection()->WriteInt32(m_PacketCount);
	//if(this->m_pSyncNode)
	//	VistaOpenGLSync::OGLCallback(m_pSyncNode);

	return true;
}

void VistaClusterClient::SwapSync()
{
	if(m_pSyncNode == NULL)
		return;

	int iSync=0;
	if(m_pSyncNode->GetDoGLFinish())
		glFinish(); // flush as best as can

//	cout << "TCP-ACK.\n";
	VistaConnection *pAckConnection = m_pSyncNode->GetAckConnection();
	IVistaSocket *pSyncSocket = m_pSyncNode->GetSyncSocket();


	if(pAckConnection)
		pAckConnection->WriteInt32(m_PacketCount);
//	cout << "... DONE\nUDP-ACK, pending\n";
	int iDummy = 0;
	if(pSyncSocket)
	{
		pSyncSocket->ReceiveRaw(&iDummy, sizeof(int));
	}

	if(m_pSyncNode->GetAfterBCDelay() > 0)
		VistaTimeUtils::Sleep(m_pSyncNode->GetAfterBCDelay());

	//cout << "... DONE (" << pSyncNode->m_rPacketCount << ", "
//		<< iDummy << ")\n";


#if defined(DEBUG)
	if(m_pSyncNode->GetSyncSocket())
	{
		if(m_PacketCount != iDummy)
		{
			vkernerr << "WARNING! PC(" << m_PacketCount << ") != DM ("
				<< iDummy << ") -- client out of sync?\n";
		}
	}
#endif
	//		if(m_pAckConnection)
	//			m_pAckConnection->WriteInt32(*m_rPacketCount);
	//
	//		if(m_pSyncSocket)
	//		{
	//			int iDummy = 0;
	//			int iVal = m_pSyncSocket->ReceiveRaw(&iDummy, sizeof(int));
	//		}
}

VistaConnection *VistaClusterClient::GetEventConnection() const
{
	if(m_pEventSucker)
		return m_pEventSucker->GetConnection();

	return NULL;
}

IVistaSocket     *VistaClusterClient::GetSyncSocket() const
{
	return m_pSyncSocket;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaClusterClient::PrintDebug ( ostream & out ) const
{
	out << " [ViCluClie] ....\n";
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetClientName                                               */
/*                                                                            */
/*============================================================================*/
std::string VistaClusterClient::GetClientSectionName() const
{
	return m_ClientSectionName;
}

std::string VistaClusterClient::GetClientName() const
{
	return m_ClientName;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   PreLoop                                                     */
/*                                                                            */
/*============================================================================*/


void VistaClusterClient::Notify(const VistaEvent *pEvent)
{
	switch(pEvent->GetId())
	{
	 case VistaSystemEvent::VSE_INIT:
	{
		printf("VistaClusterClient::Notify() -- STARTING PIPE\n");
		if(m_bAsyncMode && !m_pActivator->StartComponent())
			printf("FAILED TO START PIPE!!\n");

	vkernerr << "VistaClusterClient::Notify() -- Written Alive-ACK\n";
		break;
	}
	case VistaSystemEvent::VSE_EXIT:
	{
		#if defined(VISTA_SYS_OPENSG)
		delete m_pSyncNode;
		m_pSyncNode = NULL;
		#endif

		printf("VistaClusterClient::Notify() -- STOPPING PIPE\n");
		if(m_bAsyncMode && !m_pActivator->StopComponentGently(true))
			printf("FAILED TO STOP PIPE!!\n");
		vkernout << "VistaClusterClient::Notify(QUIT) -- Last packet number: " << m_PacketCount << endl;

		break;
	}
	case VistaSystemEvent::VSE_PREGRAPHICS:
	case VistaSystemEvent::VSE_POSTGRAPHICS:
		break;
	default:
		vkernout << "UNHANDLED: [Notify]\n==========\n" << *pEvent << endl;
		break;
	}
}

bool VistaClusterClient::PostInitSection(VistaInteractionManager *pNewIA)
{
#if defined(VISTA_SYS_OPENSG)
	if(m_pSyncNode)
	{
		m_pSyncNode->SetAckConnection(m_pEventSucker->GetConnection());
		m_pSyncNode->SetSyncSocket(m_pSyncSocket);
	}
#endif
		m_pEventSucker->GetConnection()->WriteInt32(uint32(m_ClientName.length()));
		m_pEventSucker->GetConnection()->WriteString(m_ClientName);

    if(pNewIA)
	{
		printf(" [ViClClient]: NEW INTERACTION!\n");
		m_pIntEvent = new VistaInteractionEvent(pNewIA);
	}

	return true;
}

 int VistaClusterClient::GetGLSyncPort() const
 {
	return m_iGLSyncUDPPort;
 }


 string VistaClusterClient::GetClientSyncIP() const
 {
	return m_ClientSyncIP;
 }


 double VistaClusterClient::GetFrameClock() const
 {
	//cout << "FrCl: " << m_dFrameClock << endl;
	 return m_dFrameClock;
 }

void VistaClusterClient::SetClusterAux(VistaClusterAux *pAux)
{
	m_pClusterAux = pAux;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( ostream & out, const VistaClusterClient & device )
{
	device.PrintDebug ( out );
	return out;
}


