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

#include "VistaMasterNetworkSync.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaAspects/VistaSerializingToolset.h>
#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>

#include <list>
#include <algorithm>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

class VistaMasterNetworkSync::SlaveInfo
{
public:
	std::string			m_sName;
	VistaConnectionIP*	m_pConnection;
	bool				m_bManageDeletion;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMasterNetworkSync::VistaMasterNetworkSync()
: m_iSyncCounter( 0 )
, m_oBuffer( 4096 )
, m_oHeader( 3 * sizeof( VistaType::sint32 ) )
{	
}
	
VistaMasterNetworkSync::~VistaMasterNetworkSync()
{
	for( std::vector<SlaveInfo*>::iterator itSlave = m_vecSlaves.begin();
			itSlave != m_vecSlaves.end(); ++itSlave )
	{
		if( (*itSlave)->m_bManageDeletion )
		{
			(*itSlave)->m_pConnection->WaitForSendFinish();
			(*itSlave)->m_pConnection->Close();
			delete (*itSlave)->m_pConnection;
		}
	}
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaMasterNetworkSync::AddSlave( const std::string& sSlaveName,
									  const std::string& sSlaveHostName,
									  int iPort )
{
	VistaConnectionIP *pCurrentCon = new VistaConnectionIP(VistaConnectionIP::CT_TCP,
										sSlaveHostName,
										iPort );
	if( pCurrentCon->GetIsConnected() == false )
	{
		vstr::warnp()
				<< "[VistaMasterNetworkSync::AddSlave]: Unable to connect to host ["
				<< sSlaveHostName << "] - Port [" << iPort << std::endl;
		delete pCurrentCon;
		return false;
	}
	
	return AddSlave( sSlaveName, pCurrentCon, true );
}

bool VistaMasterNetworkSync::AddSlave( const std::string& sSlaveName,
									  VistaConnectionIP* pConnection,
									  const bool bManageConnectionDeletion )
{	
	if( pConnection->GetIsConnected() == false )
	{
		vstr::warnp()
				<< "[VistaMasterNetworkSync::AddSlave]: Received non-connected IP Conn!"
				<< std::endl;
		return false;
	}
	SlaveInfo* pSlave = new SlaveInfo;
	pSlave->m_bManageDeletion = bManageConnectionDeletion;
	pSlave->m_pConnection = pConnection;
	pSlave->m_sName = sSlaveName;

	//pSlave->m_pConnection->SetShowRawSendAndReceive( true );
	
	if( m_vecSlaves.empty() )
	{
		// we only support slaves with the same endianess, so that
		// we only need one buffer
		// if this ever becomes a problem - which it shouldn't - then
		// we can think of something more elaborate
		m_oBuffer.SetByteorderSwapFlag( pSlave->m_pConnection->GetByteorderSwapFlag() );
	}
	else
	{
		if( pSlave->m_pConnection->GetByteorderSwapFlag() != m_oBuffer.GetByteorderSwapFlag() )
		{
			vstr::warnp() << "VistaMasterNetworkSync::AddSlave() -- "
					<< "Slave [" << pSlave->m_sName << "] has different ByteorderSwapFlag than "
					<< "first slave - this shouldn't happen!" << std::endl;
		}
	}
	m_vecSlaves.push_back( pSlave );

	m_oHeader.SetByteorderSwapFlag( pSlave->m_pConnection->GetByteorderSwapFlag() );
	m_oBuffer.SetByteorderSwapFlag( pSlave->m_pConnection->GetByteorderSwapFlag() );
	return true;
}

bool VistaMasterNetworkSync::Sync( int iTimeOut )
{
	++m_iSyncCounter;

	std::list<SlaveInfo*> liRemoveList;
	VistaType::sint32 nReadToken = 0;
	int nRet = 0;	
	for( std::vector<SlaveInfo*>::iterator itSlave = m_vecSlaves.begin();
	    itSlave != m_vecSlaves.end(); ++itSlave )
	{
		try
		{
			// get the current counter from each SlaveInfo
			nRet = (*itSlave)->m_pConnection->Receive( (void*)&nReadToken, sizeof(VistaType::sint32), iTimeOut );
			if( (*itSlave)->m_pConnection->GetByteorderSwapFlag() )
				VistaSerializingToolset::Swap( (void*)&nReadToken, sizeof(VistaType::sint32) );

			if( nRet == -1 || nRet == 0 ) // both cases are errors from the SlaveInfo state
			{
				vstr::errp() 
						<< "VistaMasterNetworkSync::Sync() -- SlaveInfo failed to answer" << std::endl;
				liRemoveList.push_back( *itSlave );
			}
			else if( nReadToken != m_iSyncCounter ) // see whether sync counter matches
			{
				vstr::errp() << "VistaMasterNetworkSync::Sync() -- SlaveInfo ["
				          << (*itSlave)->m_sName << "] returned wrong synccounter token.\n";
				vstr::err() << vstr::singleindent << "VistaMasterNetworkSync::Sync() -- expected [" 
						<< m_iSyncCounter << "] but was ["
						<< nReadToken + 1 << "]" << std::endl;
				liRemoveList.push_back( *itSlave );
			}
		}
		catch(VistaExceptionBase &x)
		{
			vstr::errp() << "VistaMasterNetworkSync::Sync() -- "
				<< "Excption during send to [" << (*itSlave)->m_sName << "]" << std::endl;
			x.PrintException();
			liRemoveList.push_back( *itSlave );
		}
	}

	for( std::list<SlaveInfo*>::iterator itBrokenSlave = liRemoveList.begin();
			itBrokenSlave != liRemoveList.end(); ++itBrokenSlave )
	{
		std::vector<SlaveInfo*>::iterator itStorage = 
				std::find( m_vecSlaves.begin(), m_vecSlaves.end(), (*itBrokenSlave) );
		if( itStorage == m_vecSlaves.end() )
			continue;

		if( (*itBrokenSlave)->m_bManageDeletion )
		{
			(*itBrokenSlave)->m_pConnection->WaitForSendFinish();
			(*itBrokenSlave)->m_pConnection->Close();
		}
		delete (*itBrokenSlave);
		m_vecSlaves.erase( itStorage );
	}
	return true;
}

bool VistaMasterNetworkSync::BarrierWait( int iTimeOut )
{
	// We first sync, thereby waiting for all slaves to send us an ack
	Sync( iTimeOut );

	// now, we send out 'go' back to them
	return SendData( ST_BARRIERWAIT );
}

VistaType::systemtime VistaMasterNetworkSync::GetSyncTime()
{
	VistaType::systemtime nTime = VistaTimeUtils::GetStandardTimer().GetSystemTime();
	SyncTime( nTime );
	return nTime;
}
bool VistaMasterNetworkSync::SyncTime( VistaType::systemtime& nTime )
{
	m_oBuffer.WriteDouble( nTime );
	return SendData( ST_TIME );
}
bool VistaMasterNetworkSync::SyncData( VistaPropertyList& oList )
{
	VistaPropertyList::SerializePropertyList( m_oBuffer, oList, "" );
	return SendData( ST_PROPERTYLIST );
}
bool VistaMasterNetworkSync::SyncData( IVistaSerializable& oSerializable )
{
	m_oBuffer.WriteSerializable( oSerializable );
	return SendData( ST_SERIALIZABLE );
}
bool VistaMasterNetworkSync::SyncData( VistaType::byte* pData,
									  const int iSize )
{
	m_pExtBuffer = pData;
	m_iExtBufferSize = iSize;
	return SendData( ST_RAWDATA, true );
}
bool VistaMasterNetworkSync::SyncData( VistaType::byte* pDataBuffer,
									  const int iBufferSize,
									  int& iDataSize )
{
	m_pExtBuffer = pDataBuffer;
	m_iExtBufferSize = iDataSize;
	return SendData( ST_RAWDATA, true );
}
bool VistaMasterNetworkSync::SendData( VistaType::sint32 iType, const bool bExternalData )
{
	VistaType::sint32 nDataSize = m_oBuffer.GetBufferSize();
	const VistaType::byte* pData = m_oBuffer.GetBuffer();
	if( bExternalData )
	{
		nDataSize = m_iExtBufferSize;
		pData = m_pExtBuffer;
	}

	++m_iSyncCounter;

	// prepare header to write
	m_oHeader.ClearBuffer();
	m_oHeader.WriteInt32( m_iSyncCounter );
	m_oHeader.WriteInt32( iType );
	m_oHeader.WriteInt32( nDataSize );

	// take note of every SlaveInfo that is broken
	std::list<SlaveInfo*> liRemoveList;

	for( std::vector<SlaveInfo*>::const_iterator itSlave = m_vecSlaves.begin();
		 itSlave != m_vecSlaves.end(); ++itSlave )
	{
		try
		{
			unsigned long nL = (*itSlave)->m_pConnection->WaitForSendFinish();
			if( nL == 0 )
			{
				vstr::errp() << "[VistaMasterNetworkSync]: "
							<< "error during WaitForSendFinish() ["
							<< (*itSlave)->m_sName 
							<< "] - removing it from send-list" << std::endl;
				liRemoveList.push_back(*itSlave);
			}

			VistaConnectionIP* pConn = (*itSlave)->m_pConnection;			

			if( pConn->WriteRawBuffer( m_oHeader.GetBuffer(), 3*sizeof(VistaType::sint32) )	!= 3*sizeof(VistaType::sint32) )
			{
				vstr::errp() << "[VistaMasterNetworkSync]: "
						<< "Error during Write for Slave ["
						<< (*itSlave)->m_sName
						<< "] - removing it from send-list" << std::endl;
				liRemoveList.push_back(*itSlave);
			}
			else if( nDataSize > 0 && pConn->WriteRawBuffer( pData, nDataSize ) != nDataSize )
			{
				vstr::errp() << "[VistaMasterNetworkSync]: "
						<< "Error during Write for Slave ["
						<< (*itSlave)->m_sName
						<< "] - removing it from send-list" << std::endl;
				liRemoveList.push_back(*itSlave);
			}

		}
		catch(VistaExceptionBase &x)
		{
			vstr::errp() << "[VistaMasterNetworkSync]: "
						<< "Error during send to slave ["
						<< (*itSlave)->m_sName << "]." << std::endl
						<< "[ViClMaster]: removing it from send-list" << std::endl;
			liRemoveList.push_back(*itSlave);
			x.PrintException();
		}
	}

	// prepare buffer for next message
	m_oBuffer.ClearBuffer();

	if( liRemoveList.empty() )
		return true;

	for( std::list<SlaveInfo*>::iterator itBrokenSlave = liRemoveList.begin();
			itBrokenSlave != liRemoveList.end(); ++itBrokenSlave )
	{
		std::vector<SlaveInfo*>::iterator itStorage = 
				std::find( m_vecSlaves.begin(), m_vecSlaves.end(), (*itBrokenSlave) );
		if( itStorage != m_vecSlaves.end() )
			m_vecSlaves.erase( itStorage );
		if( (*itBrokenSlave)->m_bManageDeletion )
		{
			(*itBrokenSlave)->m_pConnection->WaitForSendFinish();
			(*itBrokenSlave)->m_pConnection->Close();
			delete (*itBrokenSlave);
		}
		delete (*itBrokenSlave);
	}
	return false;
}
