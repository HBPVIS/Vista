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

#ifndef _VISTASLAVENETWORKSYNC_H
#define _VISTASLAVENETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/Connections/VistaNetworkSync.h>

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
class VISTAINTERPROCCOMMAPI VistaSlaveNetworkSync : public VistaNetworkSync
{
public:
	/**
	*
	*/
	VistaSlaveNetworkSync();
	/**
	*
	*/
	virtual ~VistaSlaveNetworkSync();
	/**
	* Init the connection to the master by establishing a server
	* socket @ the given address and listen for incoming connections
	* The first such connection will be taken.
	*/
	bool InitMasterConnection(	const std::string& strSlaveHostName,
								int iClientPort);
	/**
	* Send an ack token to the master in order to indicate that a certain
	* state has been reached. If sync-mode is barrier wait for ack from
	* master before proceeding.
	*/
	virtual bool Sync(int iTimeOut=0);
protected:

private:
	VistaConnectionIP* m_pMaster;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMASTERNETWORKSYNC_H
