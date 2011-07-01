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

#include "VistaClusterMaster.h"

#include "VistaClusterAux.h"
#include "EventManager/VistaEventManager.h"
#include "EventManager/VistaEventObserver.h"
#include "EventManager/VistaSystemEvent.h"
#include "EventManager/VistaExternalMsgEvent.h"
#include "InteractionManager/VistaInteractionEvent.h"
#include "Stuff/VistaKernelProfiling.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>
#include <VistaInterProcComm/IPNet/VistaIPAddress.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaSerializingToolset.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

#include <VistaTools/VistaProfiler.h>

#include "VistaKernelOut.h"



#include <cassert>
#include <algorithm>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * helper class to hold a buffer for a serialized event. As the events are sent
 * using the asynchronous ip socket calls, the buffer to send has to remain
 * valid until finally sent. This class also defines a serialization routine
 * CreateBufferFromEvent() that is utilized by the cluster master below.
 * Note that this implicitly defines the header and part of the protocol.
 */
class EventBuffer
{
public:
	/**
	 * @param decide about byte-swapping upon serialization of an event
	 */
	EventBuffer( bool bDoSwap )
		: m_pSer( new VistaByteBufferSerializer( 32*1024 ) ),
		  m_nEventCount(0),
		  m_nCounter(0)
		{
			m_pSer->SetByteorderSwapFlag(bDoSwap);
		}

	/**
	 * deletes the serializer attached to this buffer
	 */
	~EventBuffer()
		{
			delete m_pSer;
		}

	/**
	 * this method effectively defines the header for each event,
	 * thus some kind of protocol to follow.
	 * @param the event to serialize
	 */
	void CreateBufferFromEvent( const VistaEvent &ev )
		{
			// reset write head
			(*m_pSer).ClearBuffer();

			// first we write a dummy header here
			// this will be later filled by the cluster master upon
			// the send of the packet to the slaves
			int  nDummy = 0;
			bool bDummy = false;
			(*m_pSer).WriteInt32( nDummy ); // value will be re-written
			(*m_pSer).WriteBool( bDummy );  // value will be re-written
			(*m_pSer).WriteInt32( nDummy ); // value will be re-written

			// now the event 'header'
			(*m_pSer).WriteInt32 ( m_nEventCount++ );
			(*m_pSer).WriteInt32 ( m_nCounter++    );
			(*m_pSer).WriteDouble( ev.GetTime()    );
			(*m_pSer).WriteInt32 ( ev.GetType()    );
			(*m_pSer).WriteInt32 ( ev.GetId()      );

			// now the event itself
			(*m_pSer).WriteSerializable( ev );
		}


	/**
	 * get a pointer to the first address of the serialized event buffer.
	 */
	void *GetBuffer() const
		{
			return reinterpret_cast<void*>((*m_pSer).GetBuffer());
		}

	/**
	 * returns the number of bytes stored in this buffer
	 */
	unsigned int GetBufferSize() const
		{
			return (*m_pSer).GetBufferSize();
		}


	// public API, user with care.

	VistaByteBufferSerializer *m_pSer; /**< can grow and shrink, the constructor
	                                         decides to set it to 32k, which should
	                                         be fine for most events, even devices
	                                         with a large history. */
	VistaType::sint32 m_nEventCount;              /**< some statistics, the number of events sent
											 counted for each (type/id) */
	VistaType::sint32 m_nCounter;                 /**< @todo check whether this is needed */
};

/**
 * some helper routines that are not really bound to a class
 */
namespace
{
	/**
	 * needed for call to for_each() functors, plainly calles delete on the pointer.
	 * Hey, can we not use boost for that line? They surely have more man-like
	 * templates for that...
	 * @param pBuffer a pointer to the buffer to delete
	 */
	void deleteBuffer( EventBuffer *pBuffer )
	{
		delete pBuffer;
	}

	/**
	 * creates a number of buffers and stores them in the mBuffer resulting vector
	 */
	void setupBuffer( std::vector<EventBuffer *> &mBuffer, int nNum, bool bDoSwap )
	{
		for(int n=0; n < nNum; ++n)
		{
			mBuffer.push_back( new EventBuffer(bDoSwap) );
		}
	}

	/**
	 * create a connection to the ip/port given and transmits the first
	 * frame clock as a handshake. It returns two things
	 * - the connection describing the event channel
	 * - the connection describing the ack channel
	 * the other side decides about the port number of the ack channel, this
	 * is returned after receiving the handshake frame clock
	 * @return true when the connection was established and evChannel and ackChannel
	           are both valid
	 */
	bool EstablishConnection( const std::string &strIp, int nPort, bool bDoSwap,
			                  double dFrameClock, VistaConnectionIP *&evChannel,
			                  VistaConnectionIP *&ackChannel )
	{
		evChannel = new VistaConnectionIP(VistaConnectionIP::CT_TCP,
										   strIp, nPort);
		evChannel->SetByteorderSwapFlag(bDoSwap);
		if(evChannel->GetIsOpen())
		{
			// connection established.
			// write initial frame clock
			evChannel->WriteDouble(dFrameClock);
			int nAckPort = 0;
			VistaType::sint32 iRet = evChannel->ReadInt32( nAckPort );
			assert( iRet == sizeof(VistaType::sint32) );

			evChannel->SetIsBlocking(false);
			evChannel->SetIsBuffering(false);

			vkernout << "[ViClMaster]: Slave tells to have ACK port @ ["
			          << strIp << " ; " << nAckPort << "]\n";

			ackChannel = new VistaConnectionIP(VistaConnectionIP::CT_TCP,
												strIp, nAckPort );
			if(ackChannel->GetIsOpen())
			{
				ackChannel->SetIsBlocking(true);
				ackChannel->SetIsBuffering(false);
				ackChannel->SetByteorderSwapFlag(bDoSwap);
				vkernout << "[ViClMaster]: ACK channel established.\n";
			}
			else
			{
				// connection failed.
				vkernerr << " FAILED TO INIT SLAVE @ ip ="
					      << strIp << " with port [" << nPort << "]" << std::endl;

				delete evChannel;
				evChannel = NULL;

				delete ackChannel;
				ackChannel = NULL;
				return false;
			}

		}
		else
		{
			// connection failed.
			vkernerr << " FAILED TO INIT SLAVE @ ip ="
				      << strIp << " with port [" << nPort << "]" << std::endl;

			delete evChannel;
			evChannel = NULL;
			return false;
		}
		return true;
	}
}


/**
 * does the bulk load of the network communication in the master/slave scheme.
 * it collects events from the event bus and decides to forward the to all
 * slaves attached. It creates and maintains a number of buffers for each possible
 * event type that can be transmitted.
 */
class VistaClusterMaster::EventObserver : public VistaEventObserver
{
public:
	EventObserver()
		: VistaEventObserver(),
		  m_nFrameIndex(0),
		  m_pSyncBC(NULL),
		  m_oSer( 0 ),
		  m_pAvgSend( new VistaWeightedAverageTimer )
		{
			setupBuffer( m_vecSystemBuffers,
						 VistaSystemEvent::VSE_UPPER_BOUND - VistaSystemEvent::VSE_FIRST,
						 false );
			setupBuffer( m_vecInteractionBuffers,
						 VistaInteractionEvent::VEID_LAST - VistaInteractionEvent::VEID_FIRST,
						 false);
			setupBuffer( m_vecExternalBuffers,
						 VistaExternalMsgEvent::VEID_LAST - VistaExternalMsgEvent::VEID_FIRST,
						 false);
		}

	~EventObserver()
		{

			delete m_pAvgSend;
			if(m_pSyncBC)
			{
				SyncBC();
				m_pSyncBC->CloseSocket();
			}
			delete m_pSyncBC;

			for(std::list<Slave*>::iterator it = m_liSlaves.begin();
				it != m_liSlaves.end(); ++it)
			{
				delete *it;
			}

			std::for_each( m_vecSystemBuffers.begin(),
						   m_vecSystemBuffers.end(),
						   deleteBuffer);

			std::for_each( m_vecExternalBuffers.begin(),
						   m_vecExternalBuffers.end(),
						   deleteBuffer);
			std::for_each( m_vecInteractionBuffers.begin(),
						   m_vecInteractionBuffers.end(),
						   deleteBuffer);
		}


	/**
	 * event notification. the cluster master intercepts:
	 * - INIT: this is NOT transferred to the slaves, instead,
	     every slave utters and dispatches its own init event.
	 * - EXIT: sets a special flag which causes event dispatching on
	     the slaves to end.
	 * - VSE_PREGRAPHICS: shuffles the send list, in order to "trick"
	     TCP congestion control a bit or other possible switch optimization.
	     The effect of this is not really known...
	 * - interaction/GRAPH_UPDATE: is the only interaction event forward to
	     slaves, contains the current DFN graph (in the event) and causes
	     event dispatching and graph evaluation on slave nodes.
	 * - EXTERNAL_MESSAGE: one way to transmit data from master to slave in
	     frame. Is, for example used  in conjunction with external applications
	     that share simulation results.
	 * all events are collected (serialized) and immediately sent (asynchronously,
	 * so serialization should be the only noticeable effect on latency for each event)
	 */
	virtual void Notify( const VistaEvent *pEvent )
		{
			EventBuffer *pBuffer = NULL;
			bool bEndOfFrame = false;
			if(pEvent->GetType() == VistaSystemEvent::GetTypeId())
			{
				pBuffer = m_vecSystemBuffers[pEvent->GetId()];
				switch(pEvent->GetId())
				{
				case VistaSystemEvent::VSE_INIT:
				{
					vkernout << "Skipping Init event.\n";
					return;
				}
				case VistaSystemEvent::VSE_PREGRAPHICS:
				{
					std::random_shuffle(m_veSendList.begin(), m_veSendList.end());
					bEndOfFrame = true;
					break;
				}
				case VistaSystemEvent::VSE_EXIT:
				{
					bEndOfFrame = true;
					break;
				}
				default:
					break;
				}
			}
			else if(pEvent->GetType() == VistaInteractionEvent::GetTypeId())
			{
				if(pEvent->GetId() == VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE)
					pBuffer = m_vecInteractionBuffers[pEvent->GetId()];
			}
			else if(pEvent->GetType() == VistaExternalMsgEvent::GetTypeId())
			{
				pBuffer = m_vecExternalBuffers[pEvent->GetId()];
			}

			if(pBuffer)
			{
				VistaKernelProfileScope( "CLUSTER_DIST_EVENT" );
				m_pAvgSend->StartRecording();

				pBuffer->CreateBufferFromEvent(*pEvent);
				BroadcastEvent( pBuffer, bEndOfFrame );

				m_pAvgSend->RecordTime();
			}
		}

	/**
	 * sends sync signal on the UDP sync socket. This is a broadcast to
	 * all waiting slaves. The message sent is a counter, which can be
	 * used by the slaves to see whether they are still in sync with the
	 * master node.
	 */
	void SyncBC()
		{
			if(m_pSyncBC)
			{
				// send m_nFrameIndex-1 (!) indicating that the "last"
				// frame was successfully dispatched by the master to all slaves.
				int nOldPacketIndex = m_nFrameIndex - 1;
				m_pSyncBC->SendRaw( &nOldPacketIndex, sizeof(int) );
			}
		}

	/**
	 * method to deliver the current event to all waiting slaves.
	 * Currently implemented TCP based 1:n communication.
	 * @param pBuffer the buffer to send
	 * @param bEndOfFrame marks the current buffer with the end of frame
	          token, to simplify the state processing on the slave and
	          to enable visual update (e.g., leaving the receive loop)
	 */
	void BroadcastEvent( EventBuffer *pBuffer, bool bEndOfFrame )
		{
			// take note of every slave that is broken
			std::list<Slave*> liRemoveList;
			for( std::vector<Slave*>::const_iterator it = m_veSendList.begin();
				 it != m_veSendList.end(); ++it )
			{
				VistaConnectionIP *pCon = (*it)->m_pCon;
				if(!pCon)
					continue; // should utter some error here?

				try
				{
					// first: respect the client's which to have a byte-swap
					// (can be different for each slave)
					m_oSer.SetByteorderSwapFlag( pCon->GetByteorderSwapFlag() );

					// this call 'adopts' the buffer and positions the write head
					// at index 0
					m_oSer.SetBuffer( reinterpret_cast<char*>((*pBuffer).GetBuffer()),
									  (*pBuffer).GetBufferSize(), 0 );

					// this effectively overwrites the first int32
					// which was written with a dummy value above
					m_oSer.WriteInt32( (*pBuffer).GetBufferSize() );

					// same for the bEndOfFrame flag
					m_oSer.WriteBool( bEndOfFrame );

					// now the current packet index.
					m_oSer.WriteInt32( m_nFrameIndex );

					// test whether the send buffer of the real connection
					// is free to go
					unsigned long nL = pCon->WaitForSendFinish();
					if(nL == 0)
					{
						vkernerr << "[ViClMaster]: error during WaitForSendFinish() ["
								  << (*it)->m_strSlaveName << "]." << std::endl
								  << "[ViClMaster]: removing it from send-list\n";
						liRemoveList.push_back(*it);
					}

					// write the complete buffer in a single step
					// note that the write starts at the (*pBuffer).GetBuffer()
					// and writes the complete buffer, including the header
					int nRet = pCon->WriteRawBuffer((*pBuffer).GetBuffer(),
													(*pBuffer).GetBufferSize());

					if(nRet == -1)
					{
						vkernerr << "[ViClMaster]: error during WriteRawBuffer() ["
								  << (*it)->m_strSlaveName << "]." << std::endl
								  << "[ViClMaster]: removing it from send-list\n";
						liRemoveList.push_back(*it);
					}

				}
				catch(VistaExceptionBase &x)
				{
					vkernerr << "[ViClMaster]: error during send to client ["
							  << (*it)->m_strSlaveName << "]." << std::endl
							  << "[ViClMaster]: removing it from send-list\n";
					liRemoveList.push_back(*it);
					x.PrintException();
				}
			}

			while( !liRemoveList.empty() )
			{
				std::vector<Slave*>::iterator it = std::remove(m_veSendList.begin(),
																m_veSendList.end(),
																*liRemoveList.begin() );
				m_veSendList.erase(it);
				liRemoveList.erase( liRemoveList.begin() );
				m_liSlaves.remove( *it );
			}

			if(bEndOfFrame)
				++m_nFrameIndex;
		}

	/**
	 * representing a slave unit as entity of the master.
	 * Each slave has a name, an ip and so on. Individually, a slave can sync or not.
	 * The slave instance keeps book of connections and other state variables.
	 */
	class Slave
	{
	public:
		Slave( const std::string &strSlaveName,
				const std::string &strIp, int nPort, bool bSwap, bool bDoSync )
			: m_strSlaveName(strSlaveName),
			  m_strIp(strIp), m_nPort(nPort), m_pCon(NULL),
			  m_bSwap(bSwap),
			  m_bSyncs(bDoSync),
			  m_pAckCon(NULL)
			{

			}

		Slave( const std::string &strSlaveName,
				VistaConnectionIP *pCon,
				VistaConnectionIP *pAckCon,
				bool bDoSync )
			: m_strIp(pCon->GetPeerName()), m_nPort(pCon->GetPeerPort()),
			  m_pCon(pCon), m_strSlaveName(strSlaveName),
			  m_bSwap( m_pCon->GetByteorderSwapFlag() ),
			  m_bSyncs(bDoSync),
			  m_pAckCon(pAckCon)
			{

			}


		~Slave()
			{
				if(m_pAckCon)
				{
					m_pAckCon->SetIsBlocking(true);
					m_pAckCon->WaitForSendFinish();
					m_pAckCon->Close();
				}
				delete m_pAckCon;

				if(m_pCon && m_pCon->GetIsOpen())
				{
					m_pCon->SetIsBlocking(true);
					m_pCon->WaitForSendFinish();
					m_pCon->Close();
				}
				delete m_pCon;
			}


		std::string                 m_strSlaveName;
		std::string                 m_strIp;
		int                         m_nPort;
		bool                        m_bSwap,
			m_bSyncs;
		VistaConnectionIP         *m_pCon,
								   *m_pAckCon;
	};

	/**
	 * check, whether a slave named strName exists in the list of registered slaves.
	 * @param strName case-sensitive name to check
	 * @return false no slave with name strName exists
	 */
	bool GetIsSlave( const std::string &strName ) const
		{
			for( std::list<Slave*>::const_iterator it = m_liSlaves.begin();
				 it != m_liSlaves.end(); ++it )
			{
				if( strName == (*it)->m_strSlaveName )
					return true;
			}
			return false;
		}

	// public API for master access
	std::list<Slave*>   m_liSlaves; /**< all declared slaves */
	std::vector<Slave*> m_veSendList; /**< all active slaves */
	std::vector<Slave*> m_veAckList; /**< all slaves that are required to give an ack */

	unsigned int         m_nFrameIndex; /**< the current frame index */

	VistaUDPSocket     *m_pSyncBC; /**< a UPD broadcast socket for back-linking */

	std::vector<EventBuffer*> m_vecSystemBuffers, /**< store system events */
							   m_vecInteractionBuffers, /**< store interaction events */
							   m_vecExternalBuffers; /**< store external msg events */
	VistaByteBufferSerializer m_oSer; /**< the serializer to (re-)use during dispatching */
	VistaWeightedAverageTimer  *m_pAvgSend;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterMaster::VistaClusterMaster(VistaEventManager *pEvMgr,
		            const std::string &strMasterName )
: m_pEvMgr(pEvMgr),
  m_strMasterName(strMasterName),
  m_strMasterSectionName(strMasterName),
  m_pAux(NULL),
  m_pEventObserver(NULL),
  m_bWakeupDone(false),
  m_pAvgBc( new VistaWeightedAverageTimer )
{
	m_pEventObserver = new EventObserver;
	m_pEvMgr->RegisterObserver(m_pEventObserver, VistaSystemEvent::GetTypeId());
	m_pEvMgr->RegisterObserver(m_pEventObserver, VistaInteractionEvent::GetTypeId());
	m_pEvMgr->RegisterObserver(m_pEventObserver, VistaExternalMsgEvent::GetTypeId());
}

VistaClusterMaster::~VistaClusterMaster()
{
	vkernout << "VistaClusterMaster::~VistaClusterMaster()\n";
	m_pEvMgr->UnregisterObserver(m_pEventObserver, VistaSystemEvent::GetTypeId());
	m_pEvMgr->UnregisterObserver(m_pEventObserver, VistaInteractionEvent::GetTypeId());
	m_pEvMgr->UnregisterObserver(m_pEventObserver, VistaExternalMsgEvent::GetTypeId());

	delete m_pEventObserver;
	delete m_pAvgBc;
	VistaTimeUtils::Sleep(1000); // give a few secs to flush the tcp sockets
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaClusterMaster::SetClusterAux(VistaClusterAux *pAux)
{
	m_pAux = pAux;
}


std::string VistaClusterMaster::GetMasterSectionName() const
{
	return m_strMasterSectionName;
}

std::string VistaClusterMaster::GetMasterName() const
{
	return m_strMasterName;
}

// #####################################################################
// interaction
// #####################################################################


int    VistaClusterMaster::GetCurrentFrameIndex() const
{
	return m_pEventObserver->m_nFrameIndex;
}

bool   VistaClusterMaster::Init( const std::string &strIniFile,
								  const std::string &strSectionName)
{
	VistaProfiler profile;

	m_strMasterSectionName = strSectionName;

	if( !profile.GetTheProfileString( strSectionName, "NAME", "", m_strMasterName, strIniFile ) )
		m_strMasterName = strSectionName;

	vkernout << "[ViClMaster]: Setting master name to ["
	          << m_strMasterName << "]\n";

	bool bDoSync = profile.GetTheProfileBool( strSectionName, "SYNC", true, strIniFile );
	if(bDoSync)
	{
		std::string strSyncIp = profile.GetTheProfileString( strSectionName, "SYNCIP", "", strIniFile );
		int nSyncPort = profile.GetTheProfileInt( strSectionName, "SYNCPORT", 0, strIniFile );

		if(strSyncIp.empty() || nSyncPort == 0)
		{
			vkernerr << "[ViClMaster]: invalid sync ip or port given. SNYCIP = ["
			          << strSyncIp << "], SYNCPORT = [" << nSyncPort << "]\n";
		}
		else
		{
			vkernout << "[ViClMaster]: enabling frame-sync on SYNCIP = ["
			          << strSyncIp << "], SYNCPORT = [" << nSyncPort << "]\n";

			VistaUDPSocket *pSyncSocket = new VistaUDPSocket;

			VistaSocketAddress adr(strSyncIp, nSyncPort);

			if(!adr.GetIsValid() || ((*pSyncSocket).OpenSocket() == false))
			{
				vkernerr << "[ViClMaster]: Could not open Sync socket. Sync will not work.\n";
				delete pSyncSocket;
				pSyncSocket = NULL;
			}
			else
			{
				(*pSyncSocket).SetPermitBroadcast(1);
				(*pSyncSocket).ConnectToAddress(adr);
			}
			m_pEventObserver->m_pSyncBC = pSyncSocket;
		}
	}

	std::list<std::string> strClientList;
	profile.GetTheProfileList( strSectionName, "SLAVES", strClientList, strIniFile );
	for( std::list<std::string>::const_iterator cit = strClientList.begin();
	     cit != strClientList.end(); ++cit)
	{
		std::string strClientIp;
		profile.GetTheProfileString( *cit, "SLAVEIP", "", strClientIp, strIniFile );

		if(strClientIp.empty())
		{
			vkernerr << "[ViClMaster]: no valid ip for client ["
			          << *cit << "]\n";
			continue;
		}

		int nPort = profile.GetTheProfileInt( *cit, "SLAVEPORT", 0, strIniFile );
		if(nPort <= 0)
		{
			vkernerr << "[ViClMaster]: no valid port for client ["
			          << strClientIp << "] from section ["
			          << *cit << "]\n";
			continue;
		}


		bool bDoSwap = profile.GetTheProfileBool( *cit, "BYTESWAP", false, strIniFile );
		bool bDoSync = profile.GetTheProfileBool( *cit, "SYNC", true, strIniFile );
		AddSlave( *cit, strClientIp, nPort, bDoSwap, bDoSync );
	}
	return true;
}


// #####################################################################
// CLUSTER MANAGEMENT
// #####################################################################
bool VistaClusterMaster::AddSlave(const std::string &strName,
								   const std::string &sIpString,
		                           int iPort, bool bDoSwap, bool bDoSync)
{
	if(!m_pAux)
		return false;

	if(m_pEventObserver->GetIsSlave(strName))
		return false;

	m_pEventObserver->m_liSlaves.push_back( new EventObserver::Slave( strName,
																		sIpString,
			                                                            iPort,
			                                                            bDoSwap,
			                                                            bDoSync) );
	return true;
}

int  VistaClusterMaster::GetNumberOfSlaves() const
{
	return m_pEventObserver->m_liSlaves.size();
}

void VistaClusterMaster::GetSlaveNames(std::list<std::string> &liStorage) const
{
	for(std::list<EventObserver::Slave*>::const_iterator cit = m_pEventObserver->m_liSlaves.begin();
	    cit != m_pEventObserver->m_liSlaves.end(); ++cit )
	{
		liStorage.push_back( (*cit)->m_strSlaveName );
	}
}

VistaType::microtime VistaClusterMaster::GetAvgUpdateTime() const
{
	return m_pEventObserver->m_pAvgSend->GetAverageTime();
}

VistaType::microtime VistaClusterMaster::GetAvgSwapTime() const
{
	return (*m_pAvgBc).GetAverageTime();
}

// ####################################################################
// SYSTEM API

void VistaClusterMaster::BeginClusterProcessing()
{
	assert(m_pAux);

	std::list<EventObserver::Slave*> liFailures;

	double dFrameClock = m_pAux->GetFrameClock();

	for( std::list<EventObserver::Slave*>::iterator cit = m_pEventObserver->m_liSlaves.begin();
	     cit != m_pEventObserver->m_liSlaves.end(); ++cit )
	{
		if( (*cit)->m_pCon == NULL )
		{
			 bool bSuc = EstablishConnection( (*cit)->m_strIp,
					                              (*cit)->m_nPort,
					                              (*cit)->m_bSwap,
					                              dFrameClock,
					                              (*cit)->m_pCon,
					                              (*cit)->m_pAckCon);
			if( bSuc == false )
			{
				vkernerr << "[VistaMaster]: Failed to add client ["
				          << (*cit)->m_strSlaveName << "] @ ["
				          << (*cit)->m_strIp << ", "
				          << (*cit)->m_nPort << "]\n";
				liFailures.push_back( *cit );
			}
		}
	}

	m_pAux->SetRandomSeed(int(dFrameClock));

	// removing the failures from send list
	for(std::list<EventObserver::Slave*>::const_iterator it  = liFailures.begin();
	    it != liFailures.end(); ++it)
	{
		m_pEventObserver->m_liSlaves.remove( *it );
	}


	m_pEventObserver->m_veSendList.assign(m_pEventObserver->m_liSlaves.begin(),
			                       m_pEventObserver->m_liSlaves.end());

	for( std::vector<EventObserver::Slave*>::const_iterator t = m_pEventObserver->m_veSendList.begin();
	     t != m_pEventObserver->m_veSendList.end(); ++t )
	{
//		if( (*t)->m_bSyncs )
			m_pEventObserver->m_veAckList.push_back( *t );
	}

}

void VistaClusterMaster::WaitForSlaveInit()
{
	if(m_bWakeupDone)
		return;

	vkernout << " Master Init-Phase DONE. Waiting for ACK from Slaves.\n";

	for(std::list<EventObserver::Slave*>::const_iterator cit = m_pEventObserver->m_liSlaves.begin();
	    cit != m_pEventObserver->m_liSlaves.end(); ++cit )
	{
		if( (*cit)->m_pCon == NULL )
		{
			vkernerr << " [ViClMaster]: invalid slave [" << (*cit)->m_strSlaveName << "] -- SKIP\n";
			continue;
		}

		std::string sTmp;
		int iSize = 0;

		vkernout << "[ViClMaster]: Waiting for alive signal from slave ["
		          << (*cit)->m_pCon->GetPeerAddress().GetIPAddress().GetHostNameC()
		          << "]" << std::endl;

		try
		{
			(*cit)->m_pCon->SetIsBlocking(true);
			int iRet = (*cit)->m_pCon->ReadInt32(iSize);
			int iRet1= (*cit)->m_pCon->ReadString(sTmp, iSize);
			(*cit)->m_pCon->SetIsBlocking(false);
			if(iRet>0 && iRet1>0)
				vkernout << "[ViClMaster]: slave [" << sTmp.c_str() << "] seems to be alive (iSize=" << iSize << ").\n";
			else
			{
				vkernerr << "[ViClMaster]: Error while trying to get alive signal from slave ["
				          << (*cit)->m_strSlaveName << "]\n";
				continue;
			}

		}
		catch(VistaExceptionBase &x)
		{
			vkernerr << "[ViClMaster]: Error while trying to get alive signal from slave ["
			          << (*cit)->m_strSlaveName << "]\n";
			x.PrintException();
			continue;
		}
	}

	// we will try to init us
	vkernout << " All ViSTA-Slaves specified are awake.\n"
			  << " -------------------------------------------------------------\n"
			  << " -------------- Remote Wake up Section --------------- END ---\n"
			  << " -------------------------------------------------------------\n";
	m_bWakeupDone = true;
}

/**
 * collect all acks from all active slaves, mark broken slaves as broken and remove
 * and call broadcast back-link sync after the collection of all acks.
 * @return currently true, always.
 */
bool VistaClusterMaster::Sync()
{
	VistaKernelProfileScope( "MASTER-SYNC" );
	m_pAvgBc->StartRecording();

	/** @todo think about moving this to the event observer as well... */
	std::list<EventObserver::Slave*> liRemoveList;

	for(std::vector<EventObserver::Slave*>::iterator it = m_pEventObserver->m_veAckList.begin();
	    it != m_pEventObserver->m_veAckList.end(); ++it )
	{
		try
		{
			// get the current frame index from each slave
			int nReadToken = 0;
			int nRet = (*it)->m_pAckCon->ReadInt32(nReadToken);
			if(nRet == -1 || nRet == 0) // both cases are errors from the client state
			{
				vkernerr << "[ViClMaster]: slave ["
				          << (*it)->m_strSlaveName << "] failed during ack.\n";
				liRemoveList.push_back( *it );
			}
			else if( (nReadToken + 1) != m_pEventObserver->m_nFrameIndex ) // see whether frame index matches
			{
				vkernerr << "[ViClMaster]: slave ["
				          << (*it)->m_strSlaveName << "] failed to ack correct"
				          << " frame token.\n";
				vkernerr << "[ViClMaster]: expected [" << m_pEventObserver->m_nFrameIndex << "] but was ["
				          << nReadToken+1 << "], socket: [" << nRet << "]\n";
				liRemoveList.push_back( *it );
			}
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
			liRemoveList.push_back( *it );
		}
	}

	// sync using broadcast
	m_pEventObserver->SyncBC();


	// finally: clean up, if needed
	if( !liRemoveList.empty() ) // quick check
	{
		for(std::list<EventObserver::Slave*>::iterator t = liRemoveList.begin();
			t != liRemoveList.end(); ++t )
		{
			RemSlave( (*t)->m_strSlaveName );
		}
	}

	m_pAvgBc->RecordTime();
	return true;
}

bool VistaClusterMaster::RemSlave( const std::string &strName )
{
	// find pointer belonging to name

	std::list<EventObserver::Slave*>::iterator it = m_pEventObserver->m_liSlaves.end();
	for( it = m_pEventObserver->m_liSlaves.begin(); it != m_pEventObserver->m_liSlaves.end(); ++it)
	{
		if( (*it)->m_strSlaveName == strName )
			break;
	}

	if(it == m_pEventObserver->m_liSlaves.end())
		return false;

	EventObserver::Slave *pSlave = *it;
	std::vector<EventObserver::Slave*>::iterator sit
	         = std::remove( m_pEventObserver->m_veAckList.begin(),
	        		        m_pEventObserver->m_veAckList.end(), pSlave );
	m_pEventObserver->m_veAckList.erase( sit, m_pEventObserver->m_veAckList.end());

	std::vector<EventObserver::Slave*>::iterator zit
	         = std::remove( m_pEventObserver->m_veSendList.begin(),
	        		        m_pEventObserver->m_veSendList.end(), pSlave );
	m_pEventObserver->m_veSendList.erase( zit, m_pEventObserver->m_veSendList.end());

	if(pSlave->m_pCon)
	{
		pSlave->m_pCon->WaitForSendFinish(0);
		pSlave->m_pCon->Close();
		delete pSlave->m_pCon;
		pSlave->m_pCon = NULL;
	}

	if(pSlave->m_pAckCon)
	{
		pSlave->m_pAckCon->WaitForSendFinish(0);
		pSlave->m_pAckCon->Close();
		delete pSlave->m_pAckCon;
		pSlave->m_pAckCon = NULL;
	}

	return true;
}

void VistaClusterMaster::Debug ( std::ostream & out ) const
{
	out << "[ViClMaster]: configured as [" << m_strMasterName << "]\n"
	    << "configured from section ["
	    << m_strMasterSectionName << "]\n"
	    << "[" << GetNumberOfSlaves() << "] attached slaves" << std::endl;

	for( std::list<EventObserver::Slave*>::iterator cit = m_pEventObserver->m_liSlaves.begin();
	     cit != m_pEventObserver->m_liSlaves.end(); ++cit )
	{
		out << "["   << (*cit)->m_strSlaveName << "]\n"
		    << "\t"  << (*cit)->m_strIp
		    << " ; " << (*cit)->m_nPort
		    << " ; "
		    << ((*cit)->m_bSwap ? "SWAP" : "NOSWAP")
		    << " ; "
		    << ((*cit)->m_pCon ? "CONNECTED" : "UNAVAILABLE")
		    << " ; "
		    << ((*cit)->m_pAckCon ? "ACKS" : "SILENT")
		    << " ; "
		    << ((*cit)->m_bSyncs ? "SYNCS BC" : "NO SYNC")
		    << std::endl;
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


