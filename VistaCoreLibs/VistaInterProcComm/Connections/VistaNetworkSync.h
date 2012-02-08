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

#ifndef _VISTANETWORKSYNC_H
#define _VISTANETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
class IVistaSerializable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaNetworkSync is an abstract base class for synchronizing several computers
 * in a network (e.g. a bunch of cluster nodes). Basically one of these nodes
 * takes on the role of a master whereas the others are mere "slaves".
 */
class VISTAINTERPROCCOMMAPI IVistaNetworkSync
{
public:
	virtual ~IVistaNetworkSync();

	/**
	 * Call this routine to wait until all nodes reach the sync
	 * The network leader will wait until all nodes passed the sync call,
	 * but all follower nodes will return and continue immediately
	 * @return true if sync went on without problems
	 *         false if something went wrong (e.g. timeout was hit)
	 */
	virtual bool Sync( int iTimeOut = 0 ) = 0;
	/**
	  * Call this routine to wait until all nodes reach the sync
	 * The network leader as well as all nodes will wait until all
	 * nodes reach the sync, so that all return at about the
	 * same time
	 * @return true if sync went on without problems
	 *         false if something went wrong (e.g. timeout was hit)
	 */
	virtual bool BarrierWait( int iTimeOut = 0 ) = 0;

	/**
	 * Call this routine to get a synchronized time that corresponds
	 * to the time instance on the leader node
	 */
	virtual VistaType::systemtime GetSyncTime() = 0;
	/**
	 * Call this routine to sync data on all clients
	 * If the call returns false, Syncing failed and the data remains untouched
	 * Otherwise, the data is the same on all nodes
	 */
	virtual bool SyncTime( VistaType::systemtime& nTime ) = 0;
	virtual bool SyncData( VistaPropertyList& oList ) = 0;
	virtual bool SyncData( IVistaSerializable& oSerializable ) = 0;
	virtual bool SyncData( VistaType::byte* pData, 
							const int iDataSize ) = 0;
	virtual bool SyncData( VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							int& iDataSize ) = 0;

protected:
	IVistaNetworkSync();
	// prevent copying
private:
	IVistaNetworkSync( const IVistaNetworkSync& );
	IVistaNetworkSync& operator= ( const IVistaNetworkSync& );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANETWORKSYNC_H
