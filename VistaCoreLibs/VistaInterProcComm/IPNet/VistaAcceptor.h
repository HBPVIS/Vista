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

#ifndef _VISTAACCEPTOR_H
#define _VISTAACCEPTOR_H

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThreadEvent;
class VistaTCPSocket;
class VistaTCPServerSocket;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IVistaAcceptor : public IVistaThreadPoolWorkInstance
{
public:
	IVistaAcceptor(const std::string &sInterfaceName, int iPort);
	virtual ~IVistaAcceptor();


	void SetAbortSignal();
	bool GetIsConnected() const;
protected:
	virtual void PreWork();
	virtual void DefinedThreadWork();

	virtual bool HandleIncomingClient(VistaTCPSocket *pSocket) = 0;
	virtual bool HandleAbortMessage() = 0;

	enum eSockState
	{
		SOCK_NONE=-1,
		SOCK_CLIENT,
		SOCK_EXIT
	};

private:
	VistaThreadEvent  *m_pSyncEvent;
	VistaTCPServerSocket *m_pServer;

	std::string   m_sInterfaceName;
	int           m_iInterfacePort;
	bool          m_bConnected;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

