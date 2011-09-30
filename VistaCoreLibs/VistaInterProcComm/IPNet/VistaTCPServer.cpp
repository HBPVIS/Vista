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
// $Id: VistaTCPServer.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include "VistaTCPServer.h"
#include "VistaTCPServerSocket.h"
#include "VistaSocketAddress.h"
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#include <iostream>
using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaTCPServer::VistaTCPServer(const VistaSocketAddress &rAdd, int iBacklog,
								 bool bForceReuse)
: m_iBacklog(iBacklog)
{
	SetupSocket(rAdd, bForceReuse);
}

VistaTCPServer::VistaTCPServer(const string &sMyHostName, int iMyPort,
								 int iBacklog, bool bForceReuse )
: m_iBacklog(iBacklog)
{
	VistaSocketAddress myAdd(sMyHostName, iMyPort);
	SetupSocket(myAdd, bForceReuse);
}


VistaTCPServer::~VistaTCPServer()
{
	if(m_pServerSocket && m_pServerSocket->GetIsOpen())
	{
		vipcout << "VistaTCPServer::~VistaTCPServer() -- CLOSING SOCKET\n";
		m_pServerSocket->CloseSocket();
	}
		
	delete m_pServerSocket;
	m_pServerSocket = (VistaTCPServerSocket*)0xDEADBEEF;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaTCPServer::SetupSocket(const VistaSocketAddress &myAdd,
								  bool bForceReuse)
{
	m_pServerSocket = new VistaTCPServerSocket;
	if((*m_pServerSocket).OpenSocket())
	{
		if(bForceReuse)
			(*m_pServerSocket).SetSocketReuse(bForceReuse);

		if((*m_pServerSocket).BindToAddress(myAdd))
		{
			(*m_pServerSocket).SetIsBlocking(true);
			m_bValid = true;
		}
		else
		{
			vipcerr << "VistaTCPServer::VistaTCPServer(): "
					"Error opening Server-socket (BIND-TO-ADDRESS-FAILED).\n";
			(*m_pServerSocket).CloseSocket();
			m_bValid = false;
			delete m_pServerSocket;
			m_pServerSocket = NULL;
		}
	}
	else
	{
		vipcerr << "VistaTCPServer::VistaTCPServer(): "
				"Error opening Server-socket (OPEN-FAILED).\n";
		(*m_pServerSocket).CloseSocket();
		m_bValid = false;
		delete m_pServerSocket;
		m_pServerSocket = NULL;
	}

	if ( m_bValid && m_iBacklog)
		(*m_pServerSocket).Listen(m_iBacklog);
}

VistaTCPSocket *VistaTCPServer::GetNextClient(int iBacklog)
{
	if(!m_bValid)
		return NULL;

	if ( iBacklog != m_iBacklog)
		m_iBacklog = iBacklog;

	(*m_pServerSocket).Listen(m_iBacklog);
	return (*m_pServerSocket).Accept();
}


VistaSocketAddress VistaTCPServer::GetServerAddress() const
{
	VistaSocketAddress adr;
	(*m_pServerSocket).GetLocalSockName(adr);
	return adr;
}

void VistaTCPServer::StopAccept()
{
	// close the socket :(
		(*m_pServerSocket).CloseSocket();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

