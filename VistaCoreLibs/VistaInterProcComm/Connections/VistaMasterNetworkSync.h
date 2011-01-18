/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
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
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/Connections/VistaNetworkSync.h>
// Standard C++ includes
#include <string>
#include <vector>

#include <iostream>

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
class VISTAINTERPROCCOMMAPI VistaMasterNetworkSync : public VistaNetworkSync
{
public:
	/**
	*
	*/
	VistaMasterNetworkSync();
	/**
	*
	*/
	virtual ~VistaMasterNetworkSync();
	/**
	* Try to connect to a slave @strSlaveHost:iPort and include
	* that client into the sync procedure
	*/
	void AddSlave(const std::string& strClientHost, int iPort);
	/**
	* the master implementation of Sync waits for incoming tokens from all
	* registered clients. If the sync-mode is set to SYNCMODE_BARRIER
	* the master will send out "go-ahead-tokens" to the clients in order
	* to tell them they may proceed.
	*/
	virtual bool Sync(int iTimeOut=0);

protected:

private:
	std::vector<VistaConnectionIP*> m_vecSlaves;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMASTERNETWORKSYNC_H
