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
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#if defined(WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	typedef int socklen_t;
#elif defined (LINUX) || defined (DARWIN)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h> 
	#include <sys/ioctl.h>
#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
#else
	#error You have to define the target platform in order to compile ViSTA
#endif


#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

#include "VistaMcastIPAddress.h"
#include "VistaMcastSocketAddress.h"
#include "VistaMcastSocket.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMcastSocket::VistaMcastSocket()
: VistaUDPSocket()
{
	m_iTTL = 0;
	m_bLoop = true;
	m_bIsJoin = false;
	m_bIsSetMulticast = false;
}

VistaMcastSocket::~VistaMcastSocket()
{
}

 
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaMcastSocket::GetIsJoinedMulticast() const
{
	return m_bIsJoin;
}

bool VistaMcastSocket::GetIsSetMulticast() const
{
	return m_bIsSetMulticast;
}

void VistaMcastSocket::SetIsJoinedMulticast(bool bJoin)
{
	m_bIsJoin = bJoin;
}

void VistaMcastSocket::SetIsSetMulticast(bool bSet) 
{
	m_bIsSetMulticast = bSet;
}

int VistaMcastSocket::GetTTL() const
{
	return m_iTTL;
}

bool VistaMcastSocket::SetMulticastTTL( int iTTL)
{

#if defined(WIN32)
   unsigned int tmp_TTL = (unsigned int)iTTL;
#else 
   unsigned char tmp_TTL = (unsigned char)iTTL;
#endif /* defined WIN32 */

	if ( !SetSockOpt(IPPROTO_IP, IP_MULTICAST_TTL, &tmp_TTL, sizeof(tmp_TTL)) )
	{
		PrintErrorMessage("VistaMcastSocket::SetMulticastTTL(). \n");
		return false;
	}
	m_iTTL = iTTL;
	return true;
}

bool VistaMcastSocket::GetLoopBack() const
{
	return m_bLoop;
}

bool VistaMcastSocket::SetMulticastLoopBack( bool bLoop)
{
#if defined(WIN32)
	unsigned int tmp_LOOP = (unsigned int)( bLoop ? 1 : 0);
#else 
	unsigned char tmp_LOOP = (unsigned char)( bLoop ? 1 : 0);
#endif /* defined WIN32 */

	if ( !SetSockOpt(IPPROTO_IP, IP_MULTICAST_LOOP, &tmp_LOOP, sizeof(tmp_LOOP)) )
	{
		PrintErrorMessage("VistaMcastSocket::MulticastJoin(): SetMulticastLoopBack(). \n");
		return false;
	}
	
	m_bLoop = bLoop;
	return true;

}

bool VistaMcastSocket::ActiveMulticast( int iMode, const string &sIpLocalString, const string &sIpMultiString, int iPort, int iTTL, bool bLoop)
{
	if( !GetIsOpen() )
	{
		vipcerr << "VistaMcastSocket::ActiveMulticast() error: OpenSocket() not jet.\n";
		return false;
	}

	VistaIPAddress localInterface(sIpLocalString);
	VistaMcastSocketAddress remoteSocketAddr( sIpMultiString, iPort );
	if(iMode == 0)
	{
		VistaSocketAddress localSockAddr(localInterface, 0);
		
		if( !BindToAddress( localSockAddr ) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: Can't bind to "
				<< sIpLocalString.c_str()
				<< ".\n";
			return false;
		}
		
		if( !SetMulticast(remoteSocketAddr) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: SetMulticast().\n";
			return false;
		}
	
		if( !SetMulticastTTL(iTTL) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: SetMulticastTTL().\n";
			return false;
		}

		if( !SetMulticastLoopBack(bLoop) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: SetMulticastLoopBack().\n";
			return false;
		}

		struct in_addr	iface_addr;
		memcpy(&iface_addr, localInterface.GetINAddress(), localInterface.GetINAddressLength() );

		if ( !SetSockOpt( IPPROTO_IP, IP_MULTICAST_IF, (void *)&iface_addr, sizeof(iface_addr) ) )
		{
			PrintErrorMessage("VistaMcastSocket::ActiveMulticast() error: Set multicast network interface.\n");
			return false;
		}

	}else if(iMode == 1){
		
		SetSocketReuse(true);

#if defined(WIN32)

		//win32 requires to bind to ANY address and iPort
		VistaSocketAddress bindSockAddr;
		bindSockAddr.SetPortNumber(iPort);
		
#else
		//linux requires to bind to multicast group ip
		VistaMcastSocketAddress bindSockAddr;
		bindSockAddr = remoteSocketAddr;
#endif

		if( !BindToAddress( bindSockAddr ) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: Can't bind to "
#if defined(WIN32)
				<< "INADDR_ANY"
#else
				<< sIpMultiString.c_str()
#endif
				<< ", port " << iPort  << ".\n";
			return false;
		}

		if( !SetMulticast(remoteSocketAddr) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: SetMulticast().\n";
			return false;
		}

		if( !JoinMulticast(localInterface) )
		{
			vipcerr << "VistaMcastSocket::ActiveMulticast() error: JoinMulticast().\n";
			return false;
		}

	}else{

		vipcerr << "VistaMcastSocket::ActiveMulticast() error: Unknown mode.\n";
		return false;
			
	}

	return true;

}


bool VistaMcastSocket::SetMulticast(const VistaMcastSocketAddress &sAddr)
{
	if ( GetIsJoinedMulticast() )
	{
		vipcerr << "VistaMcastSocket::SetMulticast() error: Overset Multicast Group.\n";
		return false;
	}

	m_multiAddress = sAddr;
	m_bIsSetMulticast = true;

	return true;

}


int VistaMcastSocket::SendtoMulticast(void *pvBuffer, const int iLength, int flags)
{
	if ( !GetIsSetMulticast() ){

		vipcerr << "VistaMcastSocket::SendtoMulticast() error: Multicast Group is not set.\n";
		return -1;
	}

	return SendDatagramRaw( pvBuffer, iLength, m_multiAddress, flags);
}

int VistaMcastSocket::ReceivefromMulticast(void *pvBuffer, const int iLength, int iTimeout, int flags)
{
	if ( !GetIsJoinedMulticast() )
	{
		vipcerr << "VistaMcastSocket::ReceivefromMulticast() error: Not Join multicast group jet.\n";
		return -1;
	}
	int iAddrLength = m_multiAddress.GetINAddressLength();
	return recvfrom(SOCKET(GetSocketID()), (char *)pvBuffer , iLength ,0,(struct sockaddr *)(m_multiAddress.GetINAddress()),(socklen_t*)&iAddrLength  );

	  //return ReceiveDatagramRaw( pvBuffer, iLength, m_multiAddress, iTimeout, flags);
}


string VistaMcastSocket::GetSocketTypeString() const
{
	return "MULTICAST-UDP";
}


bool VistaMcastSocket::JoinMulticast( const VistaIPAddress &rNIFAddr  )
{

	if ( GetIsJoinedMulticast() )
	{
		vipcerr << "VistaMcastSocket::MulticastJoin() warn: Joined multicast group already.\n";
		return true;
	}

	if ( !GetIsSetMulticast() ){

		vipcerr << "VistaMcastSocket::MulticastJoin() error: Multicast Group is not set.\n";
		return false;
	}
	
	if ( !GetIsBoundToAddress() ){
	
		vipcerr << "VistaMcastSocket::MulticastJoin() error: Socket is not bound.\n";
		return false;
	}
	
	/*
	VistaSocketAddress localAddr;
	GetLocalSockName(localAddr);
	
	if (localAddr.GetPortNumber() != m_multiAddress.GetPortNumber())
	{		
		cerr << "VistaMcastSocket::MulticastJoin() warn: The bound port ["
			 << (int)(localAddr.GetPortNumber()) 
			 << "] is different from multicast group port ["
			 << (int)(m_multiAddress.GetPortNumber()) 
			 << "].\n";
		return false;
	}
	*/
	
	// Joins a specified multicast session
	struct ip_mreq	ipmr;

	ipmr.imr_multiaddr.s_addr = ((sockaddr_in *)m_multiAddress.GetINAddress())->sin_addr.s_addr;
	//ipmr.imr_interface.s_addr = (in_addr *)(rNIFAddr.GetINAddress())->s_addr;
	memcpy(&(ipmr.imr_interface), rNIFAddr.GetINAddress(), rNIFAddr.GetINAddressLength() );

	if ( !SetSockOpt( IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&ipmr, sizeof(ipmr)) )
	{
		PrintErrorMessage("VistaMcastSocket::MulticastJoin(): Failed to add multicast membership");
		return false;

	}
	
	/*
	if( setsockopt(GetSocketID(), IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *)&ipmr, sizeof(ipmr) ) != 0  )
	{
		PrintErrorMessage("VistaMcastSocket::MulticastJoin(): Failed to add multicast membership");
		return false;

	}
	*/

	m_bIsJoin = true;
	
	vipcout << "Join multicast!";
	return true;
}

bool VistaMcastSocket::DropMulticast( )
{
	
	if ( !GetIsJoinedMulticast() )
	{
		vipcerr << "VistaUDPSocket::MulticastDrop() error: Not join multicast group jet.\n";
		return false;
	}

	VistaSocketAddress localAddr;
	GetLocalSockName(localAddr);

	// Add socket to be a member of the multicast group
	struct ip_mreq	ipmr;
	
	ipmr.imr_multiaddr.s_addr = ((sockaddr_in *)m_multiAddress.GetINAddress())->sin_addr.s_addr;
	ipmr.imr_interface.s_addr = ((sockaddr_in *)localAddr.GetINAddress())->sin_addr.s_addr;
	
	if ( !SetSockOpt( IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&ipmr, sizeof(ipmr)) )
	{
		PrintErrorMessage("VistaMcastSocket::MulticastJoin(): Failed to drop multicast membership");
		return false;

	}

	m_bIsJoin = false;
	m_iTTL = 0;

	return true;
}


