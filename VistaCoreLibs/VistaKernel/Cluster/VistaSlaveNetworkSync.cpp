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

#include "VistaSlaveNetworkSync.h"

#include <VistaKernel/Cluster/VistaMasterNetworkSync.h>

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaSlaveNetworkSync::VistaSlaveNetworkSync()
: m_pMasterConnection( NULL )
, m_bOwnConnection( false )
, m_iSyncCounter( 0 )
, m_vecBuffer( 4096 )
, m_iBufferReadSize( 0 )
, m_iReadSyncCounter( -1 )
, m_bPendingMessage( false )
{
}
	
VistaSlaveNetworkSync::~VistaSlaveNetworkSync()
{
	if( m_bOwnConnection )
	{
		m_pMasterConnection->WaitForSendFinish();
		m_pMasterConnection->Close();
		delete m_pMasterConnection;
	}
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaSlaveNetworkSync::InitMasterConnection(
										const std::string& strSlaveHostName,
										int iClientPort, bool bSwap )
{
	if( m_pMasterConnection != NULL )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::InitMasterConnection() --"
				<< " Cannot initialize - a connection is already present )" << std::endl;
		return false;
	}
	VistaTCPServer oServer(strSlaveHostName, iClientPort);
	if( oServer.GetIsValid() == false )
	{
		vstr::errp() << "[VistaSlaveNetworkSync::InitMasterConnection] "
				<< "Unable to open server socket @" << strSlaveHostName 
				<< ":" << iClientPort << std::endl;
		return false;
	}

	// this call will block until we get an incoming connection
	VistaTCPSocket *serverPlug = oServer.GetNextClient();
	m_pMasterConnection = new VistaConnectionIP(serverPlug);
	m_pMasterConnection->SetIsBlocking(true);
	m_pMasterConnection->SetByteorderSwapFlag( bSwap );
	m_bOwnConnection = true;

	m_oDeSerializer.SetByteorderSwapFlag( bSwap );

	//m_pMasterConnection->SetShowRawSendAndReceive( true );
	return true;
}

bool VistaSlaveNetworkSync::InitMasterConnection( VistaConnectionIP* pConnection,
											const bool bManageConnectionDelete )
{
	if( m_pMasterConnection != NULL )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::InitMasterConnection() --"
				<< " Cannot initialize - a connection is already present )" << std::endl;
		return false;
	}
	
	if( pConnection->GetIsConnected() == false)
	{
		vstr::errp() << "[VistaSlaveNetworkSync::InitMasterConnection] "
				<< "Received non-connected MasterConnection" << std::endl;
		return false;
	}
	
	m_pMasterConnection = pConnection;
	m_bOwnConnection = bManageConnectionDelete;

	m_oDeSerializer.SetByteorderSwapFlag( m_pMasterConnection->GetByteorderSwapFlag() );

	//m_pMasterConnection->SetShowRawSendAndReceive( true );
	return true;
}

bool VistaSlaveNetworkSync::Sync( int iTimeOut )
{
	++m_iSyncCounter;

	m_pMasterConnection->WriteInt32( m_iSyncCounter );	

	return true;
}

bool VistaSlaveNetworkSync::BarrierWait( int iTimeOut )
{
	// We first sync, sending our ack to the master
	Sync( iTimeOut );

	if( ReadMessageWithType( VistaMasterNetworkSync::ST_BARRIERWAIT ) == false )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::BarrierWait() failed on read" << std::endl;
		return false;
	}
	return true;
}

VistaType::systemtime VistaSlaveNetworkSync::GetSyncTime()
{
	VistaType::systemtime nTime = -1;
	SyncTime( nTime );
	return nTime;
}
bool VistaSlaveNetworkSync::SyncTime( VistaType::systemtime& nTime )
{
	if( ReadMessageWithType( VistaMasterNetworkSync::ST_TIME ) == false )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::SyncTime() failed on read" << std::endl;
		return false;
	}

	assert( m_vecBuffer.size() == sizeof(double) );

	m_oDeSerializer.SetBuffer( &m_vecBuffer[0], m_iBufferReadSize );
	m_oDeSerializer.ReadDouble( nTime );
	return true;
}

bool VistaSlaveNetworkSync::SyncData( VistaPropertyList& oList )
{
	if( ReadMessageWithType( VistaMasterNetworkSync::ST_PROPERTYLIST ) == false )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::SyncData() failed on read" << std::endl;
		return false;
	}

	m_oDeSerializer.SetBuffer( &m_vecBuffer[0], m_iBufferReadSize );

	std::string sProplistName;
	VistaPropertyList::DeSerializePropertyList( m_oDeSerializer, oList, sProplistName );
	return true;
}
bool VistaSlaveNetworkSync::SyncData( IVistaSerializable& oSerializable )
{
	if( ReadMessageWithType( VistaMasterNetworkSync::ST_SERIALIZABLE ) == false )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::SyncData() failed on read" << std::endl;
		return false;
	}

	m_oDeSerializer.SetBuffer( &m_vecBuffer[0], m_iBufferReadSize );

	m_oDeSerializer.ReadSerializable( oSerializable );	

	return true;
}
bool VistaSlaveNetworkSync::SyncData( VistaType::byte* pData, 
							const int iDataSize  )
{
	if( ReadMessageWithType( VistaMasterNetworkSync::ST_RAWDATA ) == false )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::SyncData() failed on read" << std::endl;
		return false;
	}

	if( m_iBufferReadSize != iDataSize )
	{
		vstr::errp() 
				<< "VistaMasterNetworkSync::SyncData() -- cannot sync raw data - data size [ "
				<< iDataSize << "] doesn't match received size [" << m_iBufferReadSize << "] bytes"
				<< std::endl;
		return false;
	}

	memcpy( pData, &m_vecBuffer[0], iDataSize );
	return true;
}
bool VistaSlaveNetworkSync::SyncData( VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							int& iDataSize  )
{
	if( ReadMessageWithType( VistaMasterNetworkSync::ST_RAWDATA ) == false )
	{
		vstr::warnp() << "VistaSlaveNetworkSync::SyncData() failed on read" << std::endl;
		return false;
	}

	if( m_iBufferReadSize > iBufferSize )
	{
		vstr::errp() 
				<< "VistaMasterNetworkSync::SyncData() -- cannot sync raw data - buffer size [ "
				<< iBufferSize << "] insufficient, required are [" << m_iBufferReadSize << "] bytes"
				<< std::endl;
		return false;
	}

	iDataSize = m_iBufferReadSize;
	memcpy( pDataBuffer, &m_vecBuffer[0], iDataSize );
	return true;
}


bool VistaSlaveNetworkSync::ReadMessageWithType( const VistaType::sint32 iType )
{	
	++m_iSyncCounter;

	if( ReadMessage() == false )
		return false;

	while( m_iReadSyncCounter < m_iSyncCounter )
	{
		vstr::warnp() << "VistaMasterNetworkSync::ReadMessage() -- received message with SyncCount ["
				<< m_iReadSyncCounter << "], but expecting count ["
				<< m_iSyncCounter << "] - reading next message!"
				<< std::endl;

		if( ReadMessage() == false )
			return false;
	}

	if( m_iReadSyncCounter > m_iSyncCounter )
	{
		vstr::warnp() << "VistaMasterNetworkSync::ReadMessage() -- received SyncCount ["
				<< m_iReadSyncCounter << "] which is greater than expected sync count ["
				<< m_iSyncCounter << "] - desired package has been missed"
				<< std::endl;
		return false;
	}

	// our sync - accept it!
	m_bPendingMessage = false;

	if( iType != m_iReadType )
	{
		vstr::warnp() << "VistaMasterNetworkSync::ReadMessage() -- received message of type ["
				<< m_iReadType << "] but expected type ["
				<< iType << "] - cannot parse package" << std::endl;
		return false;
	}

	
	return true;
}


bool VistaSlaveNetworkSync::ReadMessage()
{	
	m_bPendingMessage = false;

	try
	{		
		if( m_pMasterConnection->ReadInt32( m_iReadSyncCounter ) != sizeof(VistaType::sint32) )
		{
			vstr::warnp() 
					<< "VistaMasterNetworkSync::ReadMessage() -- received SyncCount ["
					<< m_iReadSyncCounter << "] which is greater than expected sync count ["
					<< m_iSyncCounter << "] - package must have been missed"
					<< std::endl;
			return false;

		}		

		if( m_pMasterConnection->ReadInt32( m_iReadType ) != sizeof(VistaType::sint32) )
		{
			vstr::warnp() << "VistaMasterNetworkSync::ReadMessage() -- could not read message type"
					<< std::endl;
			return false;

		}


		if( m_pMasterConnection->ReadInt32( m_iBufferReadSize ) != sizeof(VistaType::sint32) )
		{
			vstr::warnp() << "VistaMasterNetworkSync::ReadMessage() -- could not read data size"
					<< std::endl;
			return false;

		}

		if( m_iBufferReadSize == 0 )
		{
			m_bPendingMessage = true;
			return true;
		}

		// ensure our buffer is big enough
		if( m_iBufferReadSize > (int)m_vecBuffer.size() )
			m_vecBuffer.resize( m_iBufferReadSize );

		// fill the actual buffer
		if( m_pMasterConnection->ReadRawBuffer( &m_vecBuffer[0], m_iBufferReadSize ) != m_iBufferReadSize )
		{
			vstr::warnp()  << "VistaMasterNetworkSync::ReadMessage() -- could not read data"
					<< std::endl;
			return false;
		}

		m_bPendingMessage = true;
		return true;
	}
	catch( VistaExceptionBase* e )
	{
		vstr::errp() << "VistaMasterNetworkSync::ReadMessage() -- IPC exception while reading from conection:\n"
					<< e->GetPrintStatement() << std::endl;
		return false;
	}	
}


