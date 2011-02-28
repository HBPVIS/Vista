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

#include <cstdio>

#include "VistaMsgChannel.h"

#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#include <cstdio>
#include <iostream>
#include <string>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMsgChannel::VistaMsgChannel()
: m_rSer(4096)
{
	m_pConnection = NULL;
}

VistaMsgChannel::~VistaMsgChannel()
{
	m_pConnection = NULL;
}


VistaMsgChannel::VistaMsgChannel(VistaMsgChannel&rOther)
: m_rSer(0)
{
	m_pConnection = (VistaConnection*)0xDEADBEEF;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// dummy vector
static std::vector<unsigned char> s_vEmpty;

static int s_iPacketCount=0;

bool VistaMsgChannel::WriteGeneralMsg( VistaMsg &rMsg)
{
	try
	{
		if(!m_pConnection)
			return false;
		m_rSer.ClearBuffer();
		rMsg.SetMsgAnswer(s_vEmpty);

		rMsg.Serialize(m_rSer);
		m_rSer.WriteInt32(++s_iPacketCount);
		m_pConnection->WriteInt32(m_rSer.GetBufferSize());
		m_pConnection->WriteRawBufferName("query", m_rSer.GetBuffer(), m_rSer.GetBufferSize());
		int iLength = 0;
		m_pConnection->ReadInt32(iLength);
		//vipcout<<"delay"<<std::endl;
		if(iLength)
		{
			std::vector<char> veTmp(iLength);

			m_pConnection->ReadRawBufferName("answer", &veTmp[0], iLength);
			m_rDeSer.SetBuffer((char*)&veTmp[0], iLength);

			rMsg.DeSerialize(m_rDeSer);
#if defined(DEBUG)
			if(!rMsg.GetMsgSuccess())
				printf("msg marked as not successful\n");
#endif
			return rMsg.GetMsgSuccess();
		}
		else
			return false;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		m_sLastError = x.GetPrintStatement();
	}
	return false;
}

std::string VistaMsgChannel::GetLastErrorString() const
{
	return m_sLastError;
}


bool VistaMsgChannel::WriteMsg(int iMethodToken, const VistaPropertyList &rList, VistaMsg::MSG *pAnswer)
{
	VistaMsg msg;
	
	m_rSer.ClearBuffer();

	// the -3 is hardcoded here
	// hmm... we should think of something here
	// but then: the number depends on the enumeration present
	// in vistakernel
	msg.SetMsgType(-3);

	msg.GetThisMsgRef().resize(4096);
	m_rSer.SetBuffer((char*)&msg.GetThisMsgRef()[0], 4096);

	m_rSer.WriteInt32(iMethodToken);
	m_rSer.WriteInt32(0);

	VistaPropertyList::SerializePropertyList(m_rSer, rList, "query");

	//m_rSer.GetBuffer(msg.GetThisMsgRef());
	//msg.SetThisMsg(veMsg);

	// vipcout << "PropertyList serialized size= " << m_rSer.GetBufferSize() << endl;

	// RELINK OLD BUFFER!
	m_rSer.SetBuffer(NULL, 4096);

	bool b=WriteGeneralMsg(msg);
	if(b && pAnswer)
	{
		*pAnswer = msg.GetMsgAnswer();
	}

	return b;
}

bool VistaMsgChannel::WriteMsg(int iMethodToken, const VistaPropertyList &rList, VistaPropertyList &rAnswer)
{
	VistaMsg::MSG rMsg;
	if(WriteMsg(iMethodToken, rList, &rMsg))
	{
		if(!rMsg.empty())
		{
			rAnswer.clear();
			m_rDeSer.SetBuffer((const char*)&rMsg[0], (int)rMsg.size());
			std::string sAnswer;
			VistaPropertyList::DeSerializePropertyList(m_rDeSer, rAnswer, sAnswer);
		}
		//rAnswer.PrintPropertyList();
		return true;
	}
	else
		return false;
}


void VistaMsgChannel::SetConnection(VistaConnection *pConnection)
{
	if(m_pConnection!=pConnection)
	{
		m_pConnection = pConnection;
		Notify(MSG_CONNECTIONCHANGED);

		if(GetIsOperational())
			Notify(MSG_CONNECTED);
	}
}

VistaConnection *VistaMsgChannel::GetConnection() const
{
	return m_pConnection;
}

bool VistaMsgChannel::DisconnectL()
{
	if(m_pConnection && m_pConnection->GetIsOpen())
	{
		m_pConnection->WaitForSendFinish();
		m_pConnection->Close();
	}
	else
		return false; // can not close no connection

	Notify(MSG_DISCONNECTED);
	return true;
}


bool VistaMsgChannel::GetIsOperational() const
{
	return (m_pConnection && m_pConnection->GetIsFine());
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


