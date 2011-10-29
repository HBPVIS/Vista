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

#include "VistaStandaloneNetworkSync.h"

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaStandaloneNetworkSync::VistaStandaloneNetworkSync()
{
}
VistaStandaloneNetworkSync::~VistaStandaloneNetworkSync()
{
}
	
/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
bool VistaStandaloneNetworkSync::Sync( int iTimeOut )
{
	return true;
}
bool VistaStandaloneNetworkSync::BarrierWait( int iTimeOut )
{
	return true;
}

VistaType::systemtime VistaStandaloneNetworkSync::GetSyncTime()
{
	return VistaTimeUtils::GetStandardTimer().GetSystemTime();
}
bool VistaStandaloneNetworkSync::SyncTime( VistaType::systemtime& nTime )
{
	return true;
}
bool VistaStandaloneNetworkSync::SyncData( VistaPropertyList& oList )
{
	return true;
}
bool VistaStandaloneNetworkSync::SyncData( IVistaSerializable& oSerializable )
{
	return true;
}

bool VistaStandaloneNetworkSync::SyncData( VistaType::byte* pData,
											const int iSize )
{
	return true;
}
bool VistaStandaloneNetworkSync::SyncData( VistaType::byte* pDataBuffer,
											const int iBufferSize,
											int& iDataSize  )
{
	return true;
}

