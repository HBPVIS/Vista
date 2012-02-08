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
// $Id$

#ifndef _VISTASLAVENETWORKSYNC_H
#define _VISTASLAVENETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaInterProcComm/Connections/VistaNetworkSync.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>

// Standard C++ includes
#include <string>

#include <iostream>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaSlaveNetworkSync is the slave implementation of VistaNetworkSync
 * It is used to sync a slave node to some master node.
 */
class VISTAKERNELAPI VistaSlaveNetworkSync : public IVistaNetworkSync
{
public:
	VistaSlaveNetworkSync();
	virtual ~VistaSlaveNetworkSync();

	/**
	* Init the connection to the master by establishing a server
	* socket @ the given address and listen for incoming connections
	* The first such connection will be taken.
	*/
	bool InitMasterConnection( const std::string& strSlaveHostName,
								int iClientPort, bool bSwap );
	/**
	* Init the connection to the master to the parameter connection
	* If bManageConnectionDelete is true, the NetSync will handle its memory
	*/
	bool InitMasterConnection( VistaConnectionIP* pConnection,
							const bool bManageConnectionDelete = false );
	/**
	* Send an ack token to the master in order to indicate that a certain
	* state has been reached. If sync-mode is barrier wait for ack from
	* master before proceeding.
	*/
	virtual bool Sync( int iTimeOut = 0 );
	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual VistaType::systemtime GetSyncTime();
	virtual bool SyncTime( VistaType::systemtime& nTime );
	virtual bool SyncData( VistaPropertyList& oList );
	virtual bool SyncData( IVistaSerializable& oSerializable );
	virtual bool SyncData( VistaType::byte* pData, 
							const int iSize );
	virtual bool SyncData( VistaType::byte* pDataBuffer,
							const int iBufferSize,
							int& iDataSize );
private:
	bool ReadMessageWithType( const VistaType::sint32 nType );
	bool ReadMessage();

private:
	VistaConnectionIP*	m_pMasterConnection;
	bool				m_bOwnConnection;
	VistaType::sint32	m_iSyncCounter;

	// message info
	bool				m_bPendingMessage;
	std::vector<VistaType::byte>	m_vecBuffer;
	VistaByteBufferDeSerializer		m_oDeSerializer;
	int					m_iBufferReadSize;
	VistaType::sint32	m_iReadSyncCounter;
	VistaType::sint32	m_iReadType;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMASTERNETWORKSYNC_H
