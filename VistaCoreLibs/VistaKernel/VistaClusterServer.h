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

#if !defined(_VISTACLUSTERSERVER_H)
#define      _VISTACLUSTERSERVER_H

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

// ViSTA includes
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <vector>
#include <list>
#include <string>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
// prototypes
class VistaClusterServer;
class VistaUDPSocket;

class VistaByteBufferSerializer;
class VistaConnectionIP;
class VistaTimer;
class VistaEventManager;
class VistaClusterAux;

VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaClusterServer & );

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaClusterServer  : public VistaEventObserver
{

public:
	VistaClusterServer(VistaEventManager *pEvMgr,
						std::string ServerName);

	virtual ~VistaClusterServer();

	bool Init(const std::string &strConfigFileName);

	//  Remote starting ViSTA on the waiting Clients registered in the Vista.ini
	bool RemoteWakeUpClients();

	// print debug infos into stream
	virtual void Debug ( std::ostream & out ) const;

	std::string     GetServerSectionName() const;

	/**
	 * @return the value of the ini entry with the key NAME
	 */
	std::string     GetServerName() const;

	// get Events from EventManager....
	virtual void Notify(const VistaEvent* pEvent);

	int  GetPacketNum() const;
	bool GetDoesDelayAfterSend() const;
	void SetDoesDelayAfterSend(bool );
	int  GetAfterSendDelayLength() const;
	void SetAfterSendDelayLength(int);

	bool GetDoesDelayAfterACK() const;
	void SetDoesDelayAfterACK(bool );
	int  GetAfterSendACKDelayLength() const;
	void SetAfterSendACKDelayLength(int);

	int  GetNumberOfClients() const;
	void GetClientNames(std::list<std::string> &liStorage) const;
	bool AddClient(const std::string &sIpString, int iPort);

	void WaitForClientInit();

	double GetFrameClock() const;
	void SetClusterAux(VistaClusterAux *);
private:
	VistaConnectionIP *EstablishConnection(const std::string &sIp, int iPort);
	void BroadcastEvents(  );
	void RecordEvent(const VistaEvent & );
	void NotifySystemEvent(const VistaEvent &pEvent);
	void NotifyInputEvent(const VistaEvent &pEvent);

	bool ClientSync(bool bDoGlSync);

	std::string		m_ServerSectionName,
	                m_ServerName;
	std::string		m_ServerIP;
	int				m_ServerPort;
	std::list<std::string> m_ClientNames;
	int				m_NumberOfClients;

	double          m_dFrameClock;
	std::list<std::string> m_ClientIPs;
	std::list<int> m_ClientPorts;

	std::vector<VistaConnectionIP *>  m_pClientAdminChannels;

	// ViSTA-Cluster-Remote-Client TCP-Server Waiting Port
	int			 m_nWaitingPort;
	int          m_nEventCounter;
	int          m_iPacketNum;
	int          m_nDelayAfterSend;
	bool         m_bDelayAfterSend;
	bool         m_bDelayBetweenACK;
	int          m_iDelayBetweenACK;
	bool         m_bDelayAfterBC;
	int          m_iDelayAfterBC;
	bool         m_bWakeupDone;
	bool         m_bDoSync;
	VistaByteBufferSerializer *m_pEventBuffer;
	VistaEventManager *m_pEvMgr;

	//VistaConnectionIP *m_pSyncConnection;
	VistaUDPSocket *m_pSyncSocket;
	VistaClusterAux *m_pClusterAux;
protected:
	VistaTimer*  m_pTimer;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTACLUSTERSERVER_H)
