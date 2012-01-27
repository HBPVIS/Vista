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
// $Id: VistaStandaloneNetworkSync.h 22143 2011-07-01 15:07:00Z dr165799 $

#ifndef _VISTASTANDALONENETWORKSYNC_H
#define _VISTASTANDALONENETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaInterProcComm/Connections/VistaNetworkSync.h>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaStandaloneNetworkSync : public IVistaNetworkSync
{
public:
	VistaStandaloneNetworkSync();
	virtual ~VistaStandaloneNetworkSync();

	virtual bool Sync( int iTimeOut = 0 );
	virtual bool BarrierWait( int iTimeOut = 0 );

	virtual VistaType::systemtime GetSyncTime();
	virtual bool SyncTime( VistaType::systemtime& nTime );
	virtual bool SyncData( VistaPropertyList& oList );
	virtual bool SyncData( IVistaSerializable& oSerializable );
	virtual bool SyncData( VistaType::byte* pData, 
							const int irSize );
	virtual bool SyncData( VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							int& iDataSize );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASTANDALONENETWORKSYNC_H
