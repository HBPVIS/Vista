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

#include "VistaClusterAux.h"

#include <VistaTools/VistaRandomNumberGenerator.h>
#include "VistaClusterClient.h"
#include "VistaClusterServer.h"

#include "VistaClusterMaster.h"
#include "VistaClusterSlave.h"

#include "VistaKernelOut.h"

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterAux::VistaClusterAux()
: m_pServer(NULL),
  m_pClient(NULL),
  m_pMaster(NULL),
  m_pSlave(NULL),
  m_nFrameClock(0)
{
}


VistaClusterAux::VistaClusterAux(VistaClusterServer *pServer,
					 VistaClusterClient *pClient)
: m_pServer(pServer),
  m_pClient(pClient),
  m_pMaster(NULL),
  m_pSlave(NULL),
  m_nFrameClock(0)
{
	assert(!((pServer != 0) && (pClient != 0)));
	if(m_pServer)
		m_pServer->SetClusterAux(this);

	if(m_pClient)
		m_pClient->SetClusterAux(this);

}

VistaClusterAux::VistaClusterAux( VistaClusterMaster *pMaster,
		          VistaClusterSlave  *pSlave )
: m_pServer(NULL),
  m_pClient(NULL),
  m_pMaster(pMaster),
  m_pSlave(pSlave),
  m_nFrameClock(0)
{
	assert(!((m_pMaster != 0) && (m_pSlave != 0)));

	if( m_pMaster )
		m_pMaster->SetClusterAux(this);

	if( m_pSlave )
		m_pSlave->SetClusterAux(this);
}


VistaClusterAux::~VistaClusterAux()
{
	vkernout << "VistaClusterAux::~VistaClusterAux()\n";
	delete m_pServer;
	delete m_pClient;

	delete m_pSlave;
	delete m_pMaster;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaClusterClient *    VistaClusterAux::GetVistaClusterClient () const
{
	return m_pClient;
}

VistaClusterServer *    VistaClusterAux::GetVistaClusterServer () const
{
	return m_pServer;
}

VistaClusterMaster *    VistaClusterAux::GetVistaClusterMaster() const
{
	return m_pMaster;
}

VistaClusterSlave  *    VistaClusterAux::GetVistaClusterSlave() const
{
	return m_pSlave;
}


bool VistaClusterAux::GetIsClient() const
{
	return (m_pClient != 0);
}

bool VistaClusterAux::GetIsServer() const
{
	return (m_pServer != 0);
}

bool VistaClusterAux::GetIsStandalone() const
{
	return (m_pServer == 0
		 && m_pClient == 0
		 && m_pMaster == 0
		 && m_pSlave == 0);
}

bool VistaClusterAux::GetIsMaster() const
{
	return (m_pMaster != 0);
}

bool VistaClusterAux::GetIsSlave() const
{
	return (m_pSlave != 0);
}


double VistaClusterAux::GetFrameClock() const
{
	if(GetIsClient())
		return m_pClient->GetFrameClock();
	else if(GetIsServer())
		return m_pServer->GetFrameClock();
//	else if(GetIsMaster())
//		return m_pMaster->GetFrameClock();
//	else if(GetIsSlave())
//		return m_pSlave->GetFameClock();
	else
		return m_nFrameClock;
}

std::string VistaClusterAux::GetNodeName() const
{
	if(GetIsClient())
		return m_pClient->GetClientName();
	else if(GetIsServer())
		return m_pServer->GetServerName();
	else if(GetIsMaster())
		return m_pMaster->GetMasterName();
	else if(GetIsSlave())
		return m_pSlave->GetSlaveName();
	else
		return "standalone";
}

void VistaClusterAux::SetFrameClock(double dFrameClock)
{
	m_nFrameClock = dFrameClock;
}

int    VistaClusterAux::GenerateRandomNumber(int iLowerBound, int iMaxBound) const
{
	return VistaRandomNumberGenerator::GetSingleton()->GenerateInt32(iLowerBound, iMaxBound);
}

void   VistaClusterAux::SetRandomSeed(int iSeed)
{
#if defined(DEBUG)
	vkernout << "[ClAux]: setting random seed to [" << iSeed << "]\n";
#endif
	VistaRandomNumberGenerator::GetSingleton()->SetSeed(iSeed);
}

int VistaClusterAux::GetFrameCount() const
{
	int nRet = 0;
	if(GetIsClient())
		nRet = m_pClient->GetPacketCount();
	else if(GetIsServer())
		nRet = m_pServer->GetPacketNum();
	else if(GetIsMaster())
		nRet = m_pMaster->GetCurrentFrameIndex();
	else if(GetIsSlave())
		nRet = m_pSlave->GetCurrentFrameToken();

	return nRet;
}

microtime VistaClusterAux::GetAvgUpdateTime() const
{
	if(GetIsMaster())
		return m_pMaster->GetAvgUpdateTime();
	else if(GetIsSlave())
		return m_pSlave->GetAvgUpdateTime();

	return 0;
}

microtime VistaClusterAux::GetAvgSwapTime() const
{
	if(GetIsMaster())
		return m_pMaster->GetAvgSwapTime();
	else if(GetIsSlave())
		return m_pSlave->GetAvgSwapTime();

	return 0;

}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


