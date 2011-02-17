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

#include "VistaServiceIndicator.h"
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#include "VistaUDPSocket.h"
#include "VistaSocketAddress.h"
#include "VistaIPAddress.h"

#if defined(_USE_IPC_EXCEPTIONS)
#include <VistaBase/VistaExceptionBase.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(WIN32)
#pragma warning(disable: 4996)
#endif


#include <iostream>
using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaServiceIndicator::VistaServiceIndicator(const VistaSocketAddress &adr)
{
	m_pAdr = new VistaSocketAddress(adr);
	m_pServiceSocket = new VistaUDPSocket;
		m_cCtrl = 0x1;
}

VistaServiceIndicator::VistaServiceIndicator(const string &sHost, int iPort)
{
	m_pAdr = new VistaSocketAddress(sHost, iPort);
	m_pServiceSocket = new VistaUDPSocket;
		m_cCtrl = 0x1;
}

VistaServiceIndicator::~VistaServiceIndicator()
{
	delete m_pServiceSocket;
	delete m_pAdr;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaServiceIndicator::SetAddress(const string &sHost, int iPort)
{
	VistaIPAddress ipa(sHost);
	(*m_pAdr).SetIPAddress(ipa);
	(*m_pAdr).SetPortNumber(iPort);
}

VistaSocketAddress VistaServiceIndicator::GetSocketAddress() const
{
	return *m_pAdr;
}

bool VistaServiceIndicator::Init()
{
   if((*m_pServiceSocket).OpenSocket())
   {
	//m_oServiceSocket.ConnectToAddress(m_sAdr);
	(*m_pServiceSocket).SetIsBlocking(true);
	(*m_pServiceSocket).SetPermitBroadcast(0);

	if((*m_pServiceSocket).BindToAddress(*m_pAdr))
	{
	}
	else
	{
		(*m_pServiceSocket).CloseSocket();
		// we *should* throw here
		return false;
	}

   }
   return true;
}

void VistaServiceIndicator::PreWork()
{
	if(!Init())
			vipcerr << "VistaServiceIndicator::PreWork() -- Init FAILED\n";
}

void VistaServiceIndicator::PostWork()
{
	(*m_pServiceSocket).CloseSocket();
}

void VistaServiceIndicator::DefinedThreadWork()
{
	vipcout << "ServiceIndicator::DefinedThreadWork() -- processing.\n";
	Pong();
	vipcout << "VistaServiceIndicator::DefinedThreadWork() -- LEAVING.\n";
}

bool VistaServiceIndicator::Ping(const string &sPeerName, int iPeerPort, bool bQuit, int iWaitTime)
{
	VistaAppMsg sMsg;
	memset(&sMsg, 0, sizeof(sMsg));

	VistaIPAddress adr = (*m_pAdr).GetIPAddress();

	string sMyHostName;
	adr.GetHostName(sMyHostName);


//        assert(sMyHostName.length() < 256);

	strncpy(&sMsg.m_acHost[0], sMyHostName.c_str(), 256);
	sprintf(&sMsg.m_acPort[0], "%d", (*m_pAdr).GetPortNumber());

	VistaSocketAddress sAdr(sPeerName, iPeerPort);
/*	cout << "sending Ping to " 
		 << sPeerName << ", P=[" << iPeerPort << "] from "
		 << sMsg.m_acHost << ", P=[" << (*m_pAdr).GetPortNumber()
		 << "]\n";
*/
		if(bQuit)
			sMsg.m_acCtrl = 'q';
	(*m_pServiceSocket).SendDatagramRaw(&sMsg, sizeof(sMsg), sAdr);

	int iRet =0;

#if defined(_USE_IPC_EXCEPTIONS)
	try
	{
#endif
		if((iRet=(*m_pServiceSocket).ReceiveRaw(&sMsg, sizeof(sMsg), iWaitTime))<=0)
		{
			// fail
			return false;
		}
		else
		{
			if(iRet != sizeof(sMsg))
			{
				// fail
				return false;
			}

			if(!strncmp(&sMsg.m_acAck[0], "OK", 2))
			{
/*				cout << "Message received:\n Host: " << &sMsg.m_acHost[0]
					<< "\n Port: " << &sMsg.m_acPort[0]
					<< "\n Ack: " << &sMsg.m_acAck[0]
					<< "\n State: " << &sMsg.m_cState
					<< "\n Ctrl: " <<	&sMsg.m_acCtrl
					<<"\n";
*/
				
				VistaIPAddress addressee(&sMsg.m_acHost[0]), addresser(sPeerName);
				string s_addressee, s_addresser;
				addressee.GetAddressString(s_addressee);
				addresser.GetAddressString(s_addresser);

/*				cout	<< "addressee: "
						<< s_addressee
						<< "addresser: "
						<< s_addresser
						<<"\n";
*/
				if (s_addressee == s_addresser)
				{
					//cout<<"got packet from right sender\n";
					return (int)sMsg.m_cState>0;
				}
				else
				{
/*					cout<<"got packet from "
						<<&sMsg.m_acHost[0]
						<<" but it was send to "
						<< sPeerName
						<<"\n";
*/					
					//wait for next packets
					while((*m_pServiceSocket).ReceiveRaw(&sMsg, sizeof(sMsg), 1000)>0);

					return false;
				}
			}

			return false;
		}
		return false;
#if defined(_USE_IPC_EXCEPTIONS)
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		return false;
	}
#endif

}

bool VistaServiceIndicator::HandleMessage(const string &sMsg)
{
	if(sMsg == "q")
	{
		return HandleQuitMessage();
	}		
	return false;
}

bool VistaServiceIndicator::Pong()
{
	VistaAppMsg sMsg;
	memset(&sMsg, 0, sizeof(sMsg));


	int iRet = 0;
	while((iRet=(*m_pServiceSocket).ReceiveRaw(&sMsg, sizeof(sMsg)))>0)
	{
		if(iRet < sizeof(sMsg))
		{
			vipcout << "ServiceIndicator: Msg too short (" << iRet << ":" << sizeof(sMsg) << "!\n";
			continue;
		}

		char cTmp[2];
		cTmp[0] = sMsg.m_acCtrl;
		cTmp[1] = 0x00;
		if(HandleMessage(string(cTmp)))
		{
			// leave loop
			break;
		}
/*        if(sMsg.m_acCtrl == 'q')
		{
			cout << "ServiceIndicator: Quit msg!\n";
				HandleQuitMessage();
			break;
		}
*/		
//        cout << "ServiceIndicator::DefinedThreadWork() -- ALIVE (OK).\n";
		
		string s_hostofsender = sMsg.m_acHost;  //keep in mind for answer
		int iPort = strtol(sMsg.m_acPort, NULL, 10);//keep in mind for answer


		//get own ip-address and write to message
		VistaIPAddress adr = (*m_pAdr).GetIPAddress();
		string sMyHostName;
		adr.GetHostName(sMyHostName);
		strncpy(&sMsg.m_acHost[0], sMyHostName.c_str(), 256);
		
		sMsg.m_acAck[0] = 'O';
		sMsg.m_acAck[1] = 'K';
		sMsg.m_acAck[2] = 0x00;

		sMsg.m_cState = (char)m_cCtrl; // running?

		
/*		cout << "Sending ACK to "
			 << s_hostofsender
			 << ", P=["
			 << iPort << "]\n";
*/
		VistaSocketAddress sAdr(s_hostofsender, iPort);

		(*m_pServiceSocket).SendDatagramRaw(&sMsg, sizeof(sMsg), sAdr);
		memset(&sMsg, 0, sizeof(sMsg));
	}

	return true;
}

bool VistaServiceIndicator::HandleQuitMessage()
{
	return true;
}

void VistaServiceIndicator::SetControlFlag(char c)
{
	m_cCtrl = c;
}

char VistaServiceIndicator::GetControlFlag() const
{
	return m_cCtrl;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
