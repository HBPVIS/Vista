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

#ifndef _VISTANETWORKSYNC_H
#define _VISTANETWORKSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

// Standard C++ includes
#include <string>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaNetworkSync is an abstract base class for synchronizing several computers
 * int a network (e.g. a bunch of cluster nodes). Basically one of these nodes
 * takes on the role of a master whereas the others are mere "slaves".
 * Two different sync-modes are available:
 *
 * SYNCMODE_STATEREACHED   :	Slave nodes simply signal that they have reached
 *								a specific state and proceed immediately.
 * SYNCMODE_BARRIER			:	Slave nodes ack that they have reached a specific
 *								state but continue only after the master node has
 *								sent back a continue token.
 *
 * per default SYNCMODE_STATEREACHED will be used.
 *
 * Two concrete implementations are available for the master computer and slave
 * nodes, respectively.
 *
 */
class VISTAINTERPROCCOMMAPI VistaNetworkSync
{
public:
	/**
	*
	*/
	enum{
		SYNCMODE_STATEREACHED,
		SYNCMODE_BARRIER
	};
	/**
	*
	*/
	VistaNetworkSync();
	/**
	*
	*/
	virtual ~VistaNetworkSync();
	/**
	*
	*/
	void SetSyncMode(const int i);
	/**
	*
	*/
	int GetSyncMode() const;
	/**
	* Call this routine to sync all registered nodes according
	* to the selected sync-mode.
	*
	* @return true iff sync went on without probs
	*         false if something went wrong (e.g. timeout was hit)
	*/
	virtual bool Sync(int iTimeOut=0);

protected:
	VistaNetworkSync(const VistaNetworkSync&);
	VistaNetworkSync& operator=(const VistaNetworkSync&);

private:
	int m_iSyncMode;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANETWORKSYNC_H
