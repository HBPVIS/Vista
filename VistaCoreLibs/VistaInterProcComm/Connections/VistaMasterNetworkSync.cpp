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

#include <VistaInterProcComm/Connections/VistaMasterNetworkSync.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>
#include <iostream>
using namespace std;
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMasterNetworkSync::VistaMasterNetworkSync()
{
}
	
VistaMasterNetworkSync::~VistaMasterNetworkSync()
{
	for(std::vector<VistaConnectionIP*>::size_type i=0; 
		i<m_vecSlaves.size(); ++i)
	{
		m_vecSlaves[i]->WaitForSendFinish();
		m_vecSlaves[i]->Close();
		delete m_vecSlaves[i];
	}
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :     AddSlave                                                  */
/*                                                                            */
/*============================================================================*/
void VistaMasterNetworkSync::AddSlave(const std::string& strSlaveHostName, int iPort)
{
	VistaConnectionIP *pCurrentCon = 
		new VistaConnectionIP(VistaConnectionIP::CT_TCP,
								strSlaveHostName,
								iPort);
	if(!pCurrentCon->GetIsConnected())
	{
		vipcout << endl << endl;
		vipcout << "*** ERROR *** [VistaMasterNetworkSync::AddSlave] ";
		vipcout << "Unable to connect to host ";
		vipcout << strSlaveHostName << ":" << iPort << endl;
		vipcout << "\tClient will not sync!" << endl << endl << endl;
		delete pCurrentCon;
		return;
	}
	m_vecSlaves.push_back(pCurrentCon);
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :                                                               */
/*                                                                            */
/*============================================================================*/
bool VistaMasterNetworkSync::Sync(int iTimeOut)
{
	//collect for incoming tokens from all clients
	char iDummy = 0;
	bool bSynched = true;
	const int iNumSlaves = (int)m_vecSlaves.size();
	for(int i=0; i<iNumSlaves; ++i)
	{
		//make sure client is still there!
		if(!m_vecSlaves[i]->GetIsConnected())
		{
			vipcout << "***WARNING*** [VistaMasterNetworkSync::Sync] ";
			vipcout << "Lost connection to client ";
			vipcout << m_vecSlaves[i]->GetPeerName() << endl;
			bSynched = false;
			continue;
		}
		bSynched &= (m_vecSlaves[i]->Receive(
			(void*)&iDummy, sizeof(char), iTimeOut) > 0);
	}
	//if we want a barrier -> send ack to slaves to proceed
	if(this->GetSyncMode() == SYNCMODE_BARRIER)
	{
		iDummy = 0;
		for(int i=0; i<iNumSlaves; ++i)
		{
			if(!m_vecSlaves[i]->GetIsConnected())
				continue;
			bSynched &= (m_vecSlaves[i]->WriteRawBuffer((void*)&iDummy, sizeof(char)) > 0);
		}
		//wait until the data has actually been sent before we proceed
		for(int i=0; i<iNumSlaves; ++i)
		{
			if(!m_vecSlaves[i]->GetIsConnected())
				continue;
			m_vecSlaves[i]->WaitForSendFinish();
		}
	}
	return bSynched;
}


