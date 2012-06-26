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

#ifndef _VISTANEWCLUSTERMASTER_H
#define _VISTANEWCLUSTERMASTER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/Cluster/VistaClusterMessage.h>

#include <string>
#include <list>
#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSystem;
class VistaDisplayManager;
class VistaWeightedAverageTimer;
class VistaPropertyList;
class VistaEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaNewClusterMaster : public VistaClusterMode
{
public:
	VistaNewClusterMaster( VistaSystem* pVisatSystem,
			            const std::string& sMasterName );
	~VistaNewClusterMaster();

	virtual bool Init( const std::string& sClusterSection,
						const VistaPropertyList& oConfig );
	virtual bool PostInit();

	virtual int GetClusterMode() const;
	virtual std::string GetClusterModeName() const;
	
	virtual int GetNodeType() const;
	virtual std::string GetNodeTypeName() const;
	virtual std::string GetNodeName() const;
	virtual std::string GetConfigSectionName() const;
	virtual int GetNodeID() const;

	virtual bool GetIsLeader() const;
	virtual bool GetIsFollower() const;

	virtual bool StartFrame();
	virtual bool ProcessFrame();
	virtual bool EndFrame();

	virtual void SwapSync();
	
	virtual bool CreateConnections( std::vector<VistaConnectionIP*>& vecConnections );
	virtual bool CreateNamedConnections( std::vector<std::pair<VistaConnectionIP*, std::string> >&
															vecConnections );

	virtual IVistaDataTunnel* CreateDataTunnel( IDLVistaDataPacket* pPacketProto );
	virtual IVistaClusterDataSync* CreateDataSync();
	virtual IVistaClusterDataSync* GetDefaultDataSync();
	virtual IVistaClusterBarrier* CreateBarrier();
	virtual IVistaClusterBarrier* GetDefaultBarrier();
	virtual IVistaClusterDataCollect* CreateDataCollect();

	//@CLUSTERTODO for testing
	virtual IVistaClusterBarrier* GetSwapSyncBarrier();

	virtual void Debug( std::ostream& oStream ) const;


private: 
	class Slave;
	class EventObserver;
	class SyncEntityObserver;
	friend class EventObserver;
	friend class SyncEntityObserver;

	int GetNextFreeBroadcastPort();
	int GetNextFreeZeroMQPort();

	bool ConnectToSlave( const std::string& sName,
					const std::string &sIP, int iPort );
	void WaitForSlaveInit();
	void TransmitClusterSetupInfo();

	void DeactivateSlaveAfterDrop( const std::string& sName );
	void DeactivateSlaveAfterDrop( Slave* pSlave );

	VistaConnectionIP* CreateConnectionToSlave( Slave* pSlave );

	bool DistributeEvent( const VistaEvent* pEvent );

	IVistaClusterDataSync* CreateTypedDataSync( int nType, bool bUseDefaultConnection );
	IVistaClusterDataSync* CreateTCPIPDataSync( bool bUseDefaultConnection );
	IVistaClusterDataSync* CreateBroadcastDataSync( bool bUseDefaultConnection );
	IVistaClusterDataSync* CreateZeroMQDataSync();
	IVistaClusterDataSync* CreateInterProcDataSync();

	IVistaClusterBarrier* CreateTypedBarrier( int nType, bool bUseDefaultConnection,
												bool bIsSwapSyncBarrier );
	IVistaClusterBarrier* CreateTCPBarrier( bool bUseDefaultConnection,
												bool bIsSwapSyncBarrier );
	IVistaClusterBarrier* CreateBroadcastBarrier( bool bUseDefaultConnection,
												bool bIsSwapSyncBarrier );
	IVistaClusterBarrier* CreateInterProcBarrier( bool bIsSwapSyncBarrier);

	bool InitSwapSync();
	bool CreateDefaultSyncs();

	bool CheckSyncEntityDelay( Slave* pSlave );
	void ProcessSyncEntityDelay();

	void ParseParameters( const VistaPropertyList& oSection );
	void ParseDataSyncType( const VistaPropertyList& oSection );
	void ParseSwapSyncType( const VistaPropertyList& oSection );
	void ParseBarrierType( const VistaPropertyList& oSection);
	void PrintClusterSetupInfo();
//@CLUSTERTODO make private again
//private:
public:
	VistaSystem*			m_pVistaSystem;
	std::string				m_sMasterName;
	std::string				m_sMasterSectionName;
	
	std::string				m_sBroadcastIP;
	VistaMasterSlave::FreePortList	m_vecFreeTCPPorts;
	VistaMasterSlave::FreePortList	m_vecFreeBroadcastPorts;
	std::string				m_sZeroMQAddress;
	VistaMasterSlave::FreePortList	m_vecZeroMQPorts;
	int						m_nNumBroadcastGroups;

	IVistaClusterDataSync*	m_pDefaultDataSync;
	IVistaClusterBarrier*	m_pDefaultBarrier;
	IVistaClusterBarrier*	m_pSwapSyncBarrier;

	EventObserver*			m_pEventObserver;	
	SyncEntityObserver*		m_pSyncEntityObserver;

	std::vector<Slave*>		m_vecSlaves;
	std::vector<Slave*>		m_vecActiveSlaves;
	std::vector<Slave*>		m_vecDeadSlaves;

	std::vector<Slave*>		m_vecSyncEntityDelayedSlaves;

	VistaMasterSlave::Message	m_oMessage;

	int						m_nSwapSyncMethod;
	int						m_nBarrierWaitMethod;
	int						m_nSwapSyncTimeout;	
	int						m_nDataSyncMethod;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANEWCLUSTERMASTER_H

