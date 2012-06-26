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

#ifndef _VISTAMASTERNETWORKSYNC_H
#define _VISTAMASTERNETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaInterProcComm/Connections/VistaNetworkSync.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>

#include <string>
#include <vector>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Master node implementation of the network synchronization mechanism defined
 * by VistaNetworkSync.
 */
class VISTAKERNELAPI VistaMasterNetworkSync : public IVistaClusterSync
{
public:
	enum SYNC_TYPE
	{
		ST_SYNC,
		ST_BARRIERWAIT,
		ST_TIME,
		ST_PROPERTYLIST,
		ST_SERIALIZABLE,
		ST_RAWDATA,
	};
public:
	VistaMasterNetworkSync();
	virtual ~VistaMasterNetworkSync();
	
	/**
	 * Try to connect to a slave @strSlaveHost:iPort and include
	 * that client into the sync procedure
	 */
	bool AddSlave( const std::string& sName,
					const std::string& strClientHost,
					int iPort );
	bool AddSlave(  const std::string& sName,
					VistaConnectionIP* pConnection,
					const bool bManageConnectionDeletion );

	virtual bool Sync( int iTimeOut = 0 );
	virtual bool BarrierWait( int iTimeOut = 0 );
	virtual VistaType::systemtime GetSyncTime();	
	virtual bool SyncTime( VistaType::systemtime& nTime );
	virtual bool SyncData( VistaPropertyList& oList );
	virtual bool SyncData( IVistaSerializable& oSerializable );
	virtual bool SyncData( VistaType::byte* pData, 
							const int iDataSize );
	virtual bool SyncData( VistaType::byte* pDataBuffer,
							const int iBufferSize,
							int& iDataSize  );

private:
	bool SendData( VistaType::sint32 bType, const bool bExternalData = false );
private:
	class SlaveInfo;
	std::vector<SlaveInfo*>		m_vecSlaves;
	VistaType::sint32			m_iSyncCounter;
	VistaByteBufferSerializer	m_oBuffer;
	VistaByteBufferSerializer	m_oHeader;
	VistaType::byte*			m_pExtBuffer;
	int							m_iExtBufferSize;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMASTERNETWORKSYNC_H
