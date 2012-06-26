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

#include "VistaNetSyncedTimerImp.h"

#include "VistaClusterMode.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include "VistaBase/VistaStreamUtils.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaNetSyncedTimerImp::VistaNetSyncedTimerImp()
{
}

VistaNetSyncedTimerImp::~VistaNetSyncedTimerImp()
{
}

void VistaNetSyncedTimerImp::Sync( VistaConnectionIP* pConn, bool bIsSlave, int nIterations )
{
	if( bIsSlave )
	{
#ifdef WIN32
		// read initial system time and stamp
		pConn->ReadDouble( m_nInitialSystemTime );
		VistaType::uint64 nInitStamp;
		pConn->ReadUInt64( nInitStamp );
		m_nInitialStamp = nInitStamp;
#elif LINUX
		VistaType::systemtime nInitialMasterTime;
		// read initial system time
		pConn->ReadDouble( nInitialMasterTime );
		//m_nInitialTime = nInitialMasterTime;
#else
		VISTA_THROW_NOT_IMPLEMENTED;
#endif
		VistaType::sint32 nIterations;
		pConn->ReadInt32( nIterations );
		std::vector<VistaType::microtime> vecReceivedTimes( nIterations );
		std::vector<VistaType::microtime> vecOwnTimes( nIterations );
		std::vector<VistaType::microtime>::iterator itReceived = vecReceivedTimes.begin();
		std::vector<VistaType::microtime>::iterator itOwn = vecOwnTimes.begin();
		for( ; itReceived != vecReceivedTimes.end(); ++itReceived, ++itOwn )
		{			
			pConn->ReadDouble( (*itReceived) );
			(*itOwn) = GetMicroTime();
			pConn->WriteDouble( (*itOwn) );
		}

		// now: process the info
		std::vector<VistaType::microtime> vecOwnDeltas( nIterations - 1 );
		std::vector<VistaType::microtime> vecReceivedDeltas( nIterations - 1 );
		std::vector<VistaType::microtime> vecOffsets( nIterations - 1 );
		VistaType::microtime nAvgDeltasRec = 0.0;
		VistaType::microtime nAvgDeltasOwn = 0.0;
		VistaType::microtime nAvgOffsets = 0.0;
		for( int i = 1; i < nIterations - 1; ++i )
		{
			vecReceivedDeltas[i] = vecReceivedTimes[i+1] - vecReceivedTimes[i];
			vecOwnDeltas[i] = vecOwnTimes[i+1] - vecOwnTimes[i];
			vecOffsets[i] = vecOwnTimes[i] - vecReceivedTimes[i];
			nAvgDeltasRec += vecReceivedDeltas[i];
			nAvgDeltasOwn += vecOwnDeltas[i];
			nAvgOffsets += vecOffsets[i];
		}
		nAvgDeltasRec /= (double)( nIterations - 2 );
		nAvgDeltasOwn /= (double)( nIterations - 2 );
		nAvgOffsets /= (double)( nIterations - 2 );
		VistaType::microtime nAvgDelay = nAvgOffsets - 0.5 * ( 0.5 * ( nAvgDeltasRec + nAvgDeltasOwn ) );		
#ifdef WIN32
		VistaType::microstamp nAvgDelayStamp = (VistaType::microstamp)( nAvgDelay / m_nFrequencyDenom + 0.5 );
		m_nInitialStamp -= nAvgDelayStamp;
#else
		m_nInitialTime += nAvgDelay;
#endif		
	}
	else
	{
#ifdef WIN32
		// first, send initial system time and initial stamp
		pConn->WriteDouble( m_nInitialSystemTime );
		pConn->WriteInt64( VistaType::sint64( m_nInitialStamp ) );
#elif LINUX
		pConn->WriteDouble( m_nInitialTime );
#else
		VISTA_THROW_NOT_IMPLEMENTED;
#endif
		// write num iterations 
		pConn->WriteInt32( VistaType::sint32( nIterations ) );
		VistaType::microtime nTime;
		for( int i = 0; i < nIterations; ++i )
		{
			nTime = GetMicroTime();
			pConn->WriteDouble( nTime );
			pConn->ReadDouble( nTime );
		}
	}
}

void VistaNetSyncedTimerImp::Sync( VistaClusterMode* pCluster, int nIterations )
{
	std::vector<VistaConnectionIP*> vecConns;
	pCluster->CreateConnections( vecConns );
	if( vecConns.empty() )
		return;
	if( pCluster->GetIsLeader() )
	{
		for( std::size_t i = 0; i < vecConns.size(); ++i )
			Sync( vecConns[i], false, nIterations );
	}
	else
	{
		Sync( vecConns.front(), true, nIterations );
	}
}
