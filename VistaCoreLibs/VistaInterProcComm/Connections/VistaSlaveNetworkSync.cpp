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

#include <VistaInterProcComm/Connections/VistaSlaveNetworkSync.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#include <iostream>
using namespace std;
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSlaveNetworkSync::VistaSlaveNetworkSync()
: m_pMaster(NULL)
{
	
	
}
	
VistaSlaveNetworkSync::~VistaSlaveNetworkSync()
{
	m_pMaster->WaitForSendFinish();
	m_pMaster->Close();
	delete m_pMaster;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :          InitMasterConnection                                 */
/*                                                                            */
/*============================================================================*/
bool VistaSlaveNetworkSync::InitMasterConnection(
										const std::string& strSlaveHostName,
										int iClientPort)
{
	VistaTCPServer server(strSlaveHostName, iClientPort);
	if(server.GetIsValid())
	{
#ifdef DEBUG
		vipcout << "[VistaSlaveNetworkSync] Connecting to master..." ;
#endif

		// this call will block until we get an incoming connection
		VistaTCPSocket *serverPlug = server.GetNextClient();
		m_pMaster = new VistaConnectionIP(serverPlug);
		m_pMaster->SetIsBlocking(true);

#ifdef DEBUG
		vipcout << "DONE!" << endl;
#endif
		return true;
	}
	else
	{
		vipcout << endl  << endl;
		vipcout << "*** ERROR *** [VistaSlaveNetworkSync::InitMasterConnection] ";
		vipcout << "Unable to open server socket"<< endl;
		vipcout << "\t @" << strSlaveHostName << ":" << iClientPort << endl << endl;
		return false;
	}
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :       Sync                                                    */
/*                                                                            */
/*============================================================================*/
bool VistaSlaveNetworkSync::Sync(int iTimeOut)
{
	if(m_pMaster == NULL || !m_pMaster->GetIsConnected())
	{
#ifdef DEBUG
		vipcout << "*** WARNING *** [VistaSlaveNetworkSync::Sync]";
		vipcout << "Master connection not open! Won't sync!" << endl;
#endif
		return false;
	}
	char iDummy = 0;
	bool bSynched = true;
	//ack to master that we are here!
	bSynched &= (m_pMaster->WriteRawBuffer((void*)&iDummy, sizeof(char)) > 0);

	//if we want a barrier -> wait until we get the ack to proceed
	if(this->GetSyncMode() == SYNCMODE_BARRIER)
	{
		//just to make sure ;-)
		if(!m_pMaster->GetIsConnected())
		{
#ifdef DEBUG
			vipcout << "*** WARNING *** [VistaSlaveNetworkSync::Sync]";
			vipcout << "Master connection not open! Won't sync!" << endl;
#endif 
			return false;
		}
		bSynched &= (m_pMaster->Receive(
			(void*)&iDummy, sizeof(char), iTimeOut) > 0);
	}

	return bSynched;
}

