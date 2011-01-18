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

#include "VistaClusterSlave.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

#include "EventManager/VistaSystemEvent.h"
#include "EventManager/VistaExternalMsgEvent.h"
#include "EventManager/VistaEventManager.h"
#include "InteractionManager/VistaInteractionEvent.h"
#include "Stuff/VistaKernelProfiling.h"
#include "VistaClusterAux.h"

#include <VistaTools/VistaProfiler.h>

#include "VistaKernelOut.h"

#include <cassert>


#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif 

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterSlave::VistaClusterSlave(VistaEventManager *pEvMgr,
									   const std::string &strSlaveName )
: m_pEvMgr(pEvMgr),
  m_strSlaveName(strSlaveName),
  m_strSlaveSectionName(strSlaveName),
  m_nSlaveIndex(~0),
  m_pAux(NULL),
  m_pConnection(NULL),
  m_pAckConnection(NULL),
  m_pSystemEvent( new VistaSystemEvent ),
  m_pExternalMsgEvent( new VistaExternalMsgEvent ),
  m_pMsg( new VistaMsg ),
  m_pInteractionEvent( NULL ),
  m_pInMgr(NULL),
  m_bDoSwap(false),
  m_nFrameToken(0),
  m_pSyncSocket(NULL),
  m_bDoOglFinish(false),
  m_nBeforeAckDelay(0),
  m_nBeforeSwapDelay(0),
  m_nAfterSwapSyncDelay(0),
  m_pAvgUpd( new VistaWeightedAverageTimer ),
  m_pAvgSwap( new VistaWeightedAverageTimer )
{
	m_pExternalMsgEvent->SetThisMsg(m_pMsg);
}

VistaClusterSlave::~VistaClusterSlave()
{
	delete m_pAvgUpd;
	delete m_pAvgSwap;

	vkernout << "VistaClusterSlave::~VistaClusterSlave() -- closing ACK\n";

	if(m_pAckConnection)
	{
		m_pAckConnection->SetIsBlocking(true);
		m_pAckConnection->Close();
	}
	delete m_pAckConnection;

	vkernout << "VistaClusterSlave::~VistaClusterSlave() -- closing MAIN\n";

	if(m_pConnection)
	{
		m_pConnection->SetIsBlocking(true);
		m_pConnection->Close();
	}

	delete m_pConnection;


	if(m_pSyncSocket)
		m_pSyncSocket->CloseSocket();
	delete m_pSyncSocket;

	delete m_pSystemEvent;
	delete m_pInteractionEvent;
	m_pExternalMsgEvent->SetThisMsg(NULL);
	delete m_pMsg;

	delete m_pExternalMsgEvent;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaClusterSlave::Init( const std::string &strIniFile,
		   const std::string &strSectionName)
{
	VistaProfiler profile;

	m_strSlaveSectionName = strSectionName;
	profile.GetTheProfileString( strSectionName, "NAME", strSectionName, m_strSlaveName, strIniFile);

	vkernout << "[ViClSlave]: Setting slave user name to ["
	           << m_strSlaveName << "]\n";

	std::string strIp;
	int nPort = 0;

	if( profile.GetTheProfileString( strSectionName, "SLAVEIP", "", strIp, strIniFile ) == false)
	{
		vkernerr << "[ViClSlave]: no SlaveIP given. ini file: ["
		          << strIniFile << "], section ["
		          << strSectionName << "]\n";
		return false;
	}

	nPort = profile.GetTheProfileInt( strSectionName, "SLAVEPORT", 0, strIniFile );
	if(nPort <= 0)
	{
		vkernerr << "[ViClSlave]: invalid port number given.\n"
		          << "[ViClSlave]: ini file     [" << strIniFile << "]\n"
		          << "[ViClSlave]: section name [" << strSectionName << "]\n";
		return false;
	}

	int nAckPort = profile.GetTheProfileInt( strSectionName, "ACKPORT", 0, strIniFile );
	if((nAckPort == 0) || (nAckPort == nPort))
	{
		vkernerr << "[ViClSlave]: invalid ack-port number given.\n"
		          << "[ViClSlave]: ini file     [" << strIniFile << "]\n"
		          << "[ViClSlave]: section name [" << strSectionName << "]\n";
		return false;
	}

	m_bDoSwap = profile.GetTheProfileBool( strSectionName, "BYTESWAP", false, strIniFile );


	bool bDoSync = profile.GetTheProfileBool( strSectionName, "SYNC", true, strIniFile );
	if(bDoSync)
	{
		std::string strSyncIp = profile.GetTheProfileString( strSectionName, "SYNCIP", "", strIniFile );
		int nSyncPort = profile.GetTheProfileInt( strSectionName, "SYNCPORT", 0, strIniFile );

		if(strSyncIp.empty() || nSyncPort == 0)
		{
			vkernerr << "[ViClSlave]: wrong credentials given for SNYCIP ["
			          << strSyncIp << "] or SNYCPORT [" << nSyncPort << "]\n";
			return false;
		}

		vkernout << "[ViClSlave]: slave is told to do da sync on ["
		          << strSyncIp << " ; " << nSyncPort << "]\n";

		m_pSyncSocket = new VistaUDPSocket;
		if(m_pSyncSocket->OpenSocket())
		{
			m_pSyncSocket->SetIsBlocking(true);
			m_pSyncSocket->SetPermitBroadcast(1);

			VistaSocketAddress adr(strSyncIp, nSyncPort);
			if(!m_pSyncSocket->BindToAddress(adr))
			{
				vkernerr << "[ViClSlave]: Could not bind SYNC-Socket, check ip and port\n";
				delete m_pSyncSocket;
				m_pSyncSocket = NULL;
			}
		}
		else
		{
			delete m_pSyncSocket;
			m_pSyncSocket = NULL;
			vkernerr << "[ViClSlave]: Could not open sync socket. This slave will not sync!\n";
		}

	}

	m_bDoOglFinish = profile.GetTheProfileBool( strSectionName, "DOGLFINISH", false, strIniFile );
	vkernout << "[ViClSlave]: slave is told "
	          << (m_bDoOglFinish ? "" : "NOT ")
	          << "to do glFinish() before swap" << std::endl;


	m_nBeforeAckDelay     = profile.GetTheProfileInt( strSectionName, "BEFOREACKDELAY", 0, strIniFile );
	m_nBeforeSwapDelay    = profile.GetTheProfileInt( strSectionName, "BEFORESWAPDELAY", 0, strIniFile );
	m_nAfterSwapSyncDelay = profile.GetTheProfileInt( strSectionName, "AFTERSWAPSYNCDELAY", 0, strIniFile );

	vkernout << "[ViClSlave]: BEFOREACKDELAY     : \t" << m_nBeforeAckDelay     << std::endl
			  << "[ViClSlave]: BEFORESWAPDELAY    : \t" << m_nBeforeSwapDelay    << std::endl
	          << "[ViClSlave]: AFTERSWAPSYNCDELAY : \t" << m_nAfterSwapSyncDelay << std::endl
			  << "[ViClSlave]: BYTESWAP           : \t" << m_bDoSwap             << std::endl;

	m_nSlaveIndex = profile.GetTheProfileInt( strSectionName, "SLAVEINDEX", -1, strIniFile );
	vkernout << "[VlClSlave]: SLAVEINDEX set to ["
	          << (m_nSlaveIndex == ~0 ? "not-set" : VistaAspectsConversionStuff::ConvertToString(m_nSlaveIndex))
	          << "]" << std::endl;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++ HANDSHAKE WITH MASTER
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(WaitForClusterMaster(strIp, nPort, nAckPort) == false)
	{
		vkernerr << "[ViClSlave]: WaitForClusterMaster() failed on ["
		          << strIp << " ; " << nPort << "]" << std::endl;
		return false;
	}

	vkernout << "[ViClSlave]: MASTER connected. What now, MASTER!?\n";

	return true;
}


unsigned int VistaClusterSlave::GetSlaveIndex() const
{
	return m_nSlaveIndex;
}


microtime VistaClusterSlave::GetAvgUpdateTime() const
{
	return m_pAvgUpd->GetAverageTime();
}

microtime VistaClusterSlave::GetAvgSwapTime() const
{
	return m_pAvgSwap->GetAverageTime();
}


std::string  VistaClusterSlave::GetSlaveName() const
{
	return m_strSlaveName;
}

std::string  VistaClusterSlave::GetSlaveSectionName() const
{
	return m_strSlaveSectionName;
}


unsigned int VistaClusterSlave::GetCurrentFrameToken() const
{
	return m_nFrameToken;
}

void VistaClusterSlave::SetClusterAux(VistaClusterAux *pAux)
{
	m_pAux = pAux;
}

void VistaClusterSlave::SetInteractionManager( VistaInteractionManager *pMgr )
{
	m_pInMgr = pMgr;

	if(m_pInMgr)
		m_pInteractionEvent = new VistaInteractionEvent(m_pInMgr);
}

bool VistaClusterSlave::WaitForClusterMaster( const std::string &strIp,
		                                       int nPort, int nAckPort )
{
	try
	{
		vkernout << "[ViClSlave]: Creating ACK connection on ["
		          << strIp << " ; " << nAckPort << "]\n";

		VistaTCPServer ackServer( strIp, nAckPort, 1 );
		if(!ackServer.GetIsValid())
			return false;

		vkernout << "[ViClSlave]: Waiting for master on ["
		          << strIp << " ; " << nPort << "]\n";

		VistaTCPServer server( strIp, nPort,0, true);
		if(server.GetIsValid())
		{
			VistaTCPSocket *pSocket = server.GetNextClient();

			if(pSocket == NULL)
				return false;

			m_pConnection = new VistaConnectionIP( pSocket );
			m_pConnection->SetIsBlocking(true);
			m_pConnection->SetIsBuffering(false);
			m_pConnection->SetByteorderSwapFlag(m_bDoSwap);

			double dFrameClock = 0;
			m_pConnection->ReadDouble( dFrameClock );
			m_pConnection->WriteInt32( nAckPort );

			// write initial frame clock as reported by master!
			m_pAux->SetFrameClock(dFrameClock);
			// set the random seed, just as master does
			// this is quite important
			m_pAux->SetRandomSeed(int(dFrameClock));
		}
		else
			return false;

		// get ack connection
		VistaTCPSocket *pAckSocket = ackServer.GetNextClient();
		if(pAckSocket == NULL)
		{
			m_pConnection->Close();
			delete m_pConnection;
			m_pConnection = NULL;
			return false;
		}

		m_pAckConnection = new VistaConnectionIP( pAckSocket );
		if( m_pAckConnection->GetIsOpen() )
		{
			m_pAckConnection->SetIsBlocking(true);
			m_pAckConnection->SetIsBuffering(false);
			m_pAckConnection->SetByteorderSwapFlag(m_bDoSwap);
		}
		else
		{
			vkernerr << "Could not create ACK connection. Failing here.\n";
			delete m_pAckConnection;
			m_pAckConnection = NULL;
			m_pConnection->Close();
			delete m_pConnection;
			m_pConnection = NULL;
			return false;
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		return false;
	}

	return true;
}

#define READ_W_EXP( A, B ) \
	{ \
		int n = A; \
		if(n != B) \
		{ \
			vkernerr << "expected [" << B << "] got [" << n << "]" \
			          << "pending: [" << m_pConnection->PendingDataSize() << "]"; \
			VISTA_THROW("PROTOCOL ERROR DURING FRAME PROCESSING", 0x00000000); \
		} \
	}


bool VistaClusterSlave::ProcessCurrentFrame()
{
	bool bRet = false;
	// wait blocking on the tcp socket
	if(!m_pConnection)
		return true; // exit, something is wrong.

	m_pAvgUpd->StartRecording();

	try
	{
		bool bEndOfFrame = false;

		while(!bEndOfFrame)
		{
			// create a reader for swap-correct token reading
			VistaByteBufferDeSerializer deSer;
			deSer.SetByteorderSwapFlag( m_pConnection->GetByteorderSwapFlag() );

			// this may seem strange to you, but currently, the VistaConnectionIP has some flaws:
			// it reads atoms only on fragment level, which means that it is possible to claim
			// less bytes than needed, although the interface promises to read off a complete int
			// until this is fixed, we read a raw buffer and swap by hand
			int nSizeRead, nSize = 0;
			READ_W_EXP(m_pConnection->ReadRawBuffer(&nSizeRead, sizeof(int)), sizeof(int))

			// pass it to the reader
			deSer.SetBuffer( reinterpret_cast<const char*>(&nSizeRead), sizeof(int), false );

			// read off with correct endianess
			deSer.ReadInt32( nSize );

			// important: nSize still contains the 4 bytes
			// for the message size tag
			nSize = nSize - sizeof(int);

			m_vecMessage.resize(nSize);

			// read off the complete message
			// this call is designed to read all fragments off the ip stack
			READ_W_EXP(m_pConnection->ReadRawBuffer( reinterpret_cast<void*>(&m_vecMessage[0]), nSize ),
					                                 nSize );

			// pass it to the reader
			deSer.SetBuffer( reinterpret_cast<const char*>(&m_vecMessage[0]), nSize, false );

			int nEventCount = 0;
			int nCounter    = 0;
			double dTime    = 0;

			int nType       = 0;
			int nId         = 0;

			READ_W_EXP(deSer.ReadBool(bEndOfFrame),   sizeof(bool))
			READ_W_EXP(deSer.ReadInt32(m_nFrameToken),sizeof(int))

			//std::cout << "ft: " << m_nFrameToken << std::endl;

			READ_W_EXP(deSer.ReadInt32( nEventCount ), sizeof(int));
			READ_W_EXP(deSer.ReadInt32( nCounter ), sizeof(int));
			READ_W_EXP(deSer.ReadDouble( dTime ), sizeof(double));

			READ_W_EXP(deSer.ReadInt32( nType ), sizeof(int));
			READ_W_EXP(deSer.ReadInt32( nId ), sizeof(int));

			VistaEvent *pEvent = NULL;

			if(nType == VistaSystemEvent::GetTypeId())
			{
				pEvent = m_pSystemEvent;
				VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( nId ) );
			}
			else if(nType == VistaInteractionEvent::GetTypeId())
			{
				pEvent = m_pInteractionEvent;
			}
			else if(nType == VistaExternalMsgEvent::GetTypeId())
			{
				pEvent = m_pExternalMsgEvent;
			}
			else
				VISTA_THROW( "UNSUPPORTED event type", 0x00000000);

			if(deSer.ReadSerializable( *pEvent ) <= 0)
				VISTA_THROW("PROTOCOL ERROR DURING EVENT READ", 0x00000000);

			if(pEvent->GetId() == VistaSystemEvent::VSE_EXIT)
				bRet = true;
			
			// disabled: leads to new, different frameclock for every event, different from master
			//m_pAux->SetFrameClock( dTime ); // set the current frame clock
			//                                // value as reported by the master node

			m_pEvMgr->ProcessEvent( pEvent );

			if( nType == VistaSystemEvent::GetTypeId() )
				VistaKernelProfileStopSection();
		} // only leave on error, or end of frame mark!
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		throw x;
	}

	m_pAvgUpd->RecordTime();

	return bRet;
}


bool VistaClusterSlave::PostSystemInitEvent()
{
	m_pConnection->WriteInt32( (uint32)m_strSlaveName.size() );
	m_pConnection->WriteString( m_strSlaveName );
	return true;
}

void VistaClusterSlave::SwapSync()
{
	VistaKernelProfileScope( "CLUSTER-SYNC" );
	m_pAvgSwap->StartRecording();

	// user told us to wait until glFinish() returns
	// this is a best guess that the gfx hardware is really
	// done with rendering and waits for the buffer swap
	if(m_bDoOglFinish)
		glFinish();

	// debugging: simulate a little delay
	if(m_nBeforeAckDelay)
		VistaTimeUtils::Sleep(m_nBeforeAckDelay);

	// ack that we have processed the current frame
	// and maybe wait at the barrier
	if(m_pAckConnection)
		m_pAckConnection->WriteInt32(m_nFrameToken);

	// debugging: simulate a little delay
	if(m_nBeforeSwapDelay)
		VistaTimeUtils::Sleep(m_nBeforeSwapDelay);

	// blocking read on barrier
	if(m_pSyncSocket)
	{
		int iDummy = 0;
		int iVal = m_pSyncSocket->ReceiveRaw(&iDummy, sizeof(int));
#if defined(DEBUG)
		if(iDummy != m_nFrameToken)
		{
			vkernerr << "[ViClSlave]: wait at barrier returned the wrong "
			          << "token, something is out of sync.\n"
			          << "expected [" << m_nFrameToken << "], but got: ["
			          << iDummy << "]" << " pending: " << m_pSyncSocket->HasBytesPending()
					  << std::endl;
		}
#endif
		if(m_nAfterSwapSyncDelay)
			VistaTimeUtils::Sleep(m_nAfterSwapSyncDelay);
	}

	m_pAvgSwap->RecordTime();
}


void VistaClusterSlave::Debug ( std::ostream & out ) const
{
	out << "VistaClusterSlave::Debug() -- " << std::endl;

	out << "name: [" << m_strSlaveName << "]" << std::endl
	    << "sec : [" << m_strSlaveSectionName << "]" << std::endl
	    << "acks: [" << (m_pAckConnection ? "YES" : "NO") << "]" << std::endl
	    << "sync: [" << (m_pSyncSocket ? "YES" : "NO" ) << "]" << std::endl
	    << "swap: [" << (m_bDoSwap ? "YES" : "NO") << "]" << std::endl
	    << "con : [" << (m_pConnection ? "YES" : "NO") << "]" << std::endl
	    << "cnt : [" << (m_pConnection ? m_pConnection->GetPeerName() : "<->") << "]" << std::endl
	    << "prt : [" << (m_pConnection ? m_pConnection->GetPeerPort() : -1) << "]" << std::endl
	    << "before-ack-delay: [" << m_nBeforeAckDelay << "]" << std::endl
	    << "before-swap-delay: [" << m_nBeforeSwapDelay << "]" << std::endl
	    << "after-swap-delay: [" << m_nAfterSwapSyncDelay << "]" << std::endl
	    << "oglf: [" << (m_bDoOglFinish ? "YES" : "NO") << "]" << std::endl;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


