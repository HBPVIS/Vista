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

#if !defined(_VISTACLUSTERCLIENT_H)
#define _VISTACLUSTERCLIENT_H


/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
// system includes

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSink.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataSource.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaByteBufferPacket.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>

#include <string>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaEventManager;

class VistaEventSucker;
class DLVistaActiveDataProducer;
class DLVistaRamQueuePipe;
class VistaByteBufferDeSerializer;
//class VistaInputEvent;
class VistaSystemEvent;
class VistaConnectionIP;
class VistaUDPSocket;
class VistaExternalMsgEvent;
class VistaMsg;
class VistaInteractionEvent;
class VistaClusterAux;

//class VistaOldInteractionManager;
class VistaInteractionManager;

class VistaEventManager;


// prototypes
class VistaClusterClient;

class VistaOpenGLSync;

VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaClusterClient & );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaClusterClient : public VistaEventObserver, public DLVistaDataSink
{
public:
	VistaClusterClient(VistaEventManager* pEvMgr,
						std::string& ClientName);
	virtual ~VistaClusterClient();

	// *********************************************************
	// DLV
	// *********************************************************
	virtual bool ConsumePacket(IDLVistaDataPacket *) ;

	// *********************************************************
	// VistaEventObserver
	// *********************************************************
	virtual void Notify(const VistaEvent *pEvent);

	// *********************************************************
	// VistaClusterClient
	// *********************************************************
	int & GetPacketCount();
	int   GetClientIndex() const;
	bool  GetIsProcessingEvents() const;

	bool GetDoesDelayAfterReceive() const;
	void SetDoesDelayAfterReceive(bool );
	int  GetAfterReceiveDelayLength() const;
	void SetAfterReceiveDelayLength(int);


	// *********************************************************
	// VistaStdMethods
	// *********************************************************
	bool Init(const std::string &strConfigFileName);
	virtual void PrintDebug ( std::ostream & out ) const;

	std::string     GetClientSectionName() const;

	/**
	 * @return the contents of the field NAME in the ini for this slave
	 */
	std::string     GetClientName() const;

	bool PostInitSection(VistaInteractionManager *pNewIA);

	int GetGLSyncPort() const;
	std::string GetClientSyncIP() const;

	double GetFrameClock() const;

	VistaConnection *GetEventConnection() const;
	IVistaSocket     *GetSyncSocket() const;

	bool Sync();

	void SwapSync();

	void SetClusterAux(VistaClusterAux *pAux);
private:

	VistaEventSucker *m_pEventSucker;
	VistaUDPSocket   *m_pSyncSocket;


	std::string     m_ClientSectionName, /**< section name */
					m_ClientName;  /**< NAME value */
	std::string     m_ClientIP, m_ClientSyncIP;
	int				m_ClientPort;
	int             m_PacketCount;
	int m_iClientIndex;

	int m_iGLSyncUDPPort;
	bool m_bDoesDelayAfterReceive;
	bool m_bDoSync;
	int m_iAfterReceiveDelay;

	double m_dFrameClock;

	bool   ThreadFunction();

	std::list<std::string>	m_sEventData;
	std::list<int>			m_iEventHeader;

	bool m_ProcessingEvents;


protected:
	void PreLoop();

	VistaEventManager *m_pEvMgr;

	DLVistaActiveDataProducer *m_pActivator;
	DLVistaRamQueuePipe *m_pQueue;
	VistaByteBufferDeSerializer *m_pBuffer;
	//VistaInputEvent *m_pInEvent;
	VistaInteractionEvent *m_pIntEvent;
	VistaSystemEvent *m_pSysEvent;
	VistaExternalMsgEvent *m_pExtMsgEvent;
	VistaMsg *m_pTmpMsg;
	VistaClusterAux *m_pClusterAux;

	bool m_bAsyncMode;


   VistaOpenGLSync *m_pSyncNode;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTACLUSTERCLIENT_H)


