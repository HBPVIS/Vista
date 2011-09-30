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
// $Id: VistaIPAddress.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include <cstdio>
#include "VistaIPAddress.h"
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#if defined(WIN32)
#pragma warning(disable: 4996)
#endif


#if defined(WIN32)
#include <Windows.h>
#elif defined (LINUX) || defined (DARWIN)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>

#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <arpa/inet.h>

#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>

#else
	#error You have to define the target platform in order to compile ViSTA

#endif


#include <iostream>
using namespace std;

#include <cstring>
#include <cstdio>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
static void TranslateHostResolveError(int iErrno)
{
	vipcerr << "VistaIPAddress::HostResolveError:";
#ifdef WIN32
	switch ( iErrno )
	{
	case WSAEACCES:
		vipcerr << "Permission denied.";
		break;
	case WSAEADDRINUSE:
		vipcerr << "Address already in use.";
		break;
	case WSAEADDRNOTAVAIL:
		vipcerr << "Cannot assign requested address.";
		break;
	case WSAEAFNOSUPPORT:
		vipcerr << "Address family not supported by protocol family.";
		break;
	case WSAEALREADY:
		vipcerr << "Operation already in progress.";
		break;
	case WSAECONNABORTED:
		vipcerr << "Software caused connection abort.";
		break;
	case WSAECONNREFUSED:
		vipcerr << "Connection refused.";
		break;
	case WSAECONNRESET:
		vipcerr << "Connection reset by peer.";
		break;
	case WSAEDESTADDRREQ:
		vipcerr << "Destination address required.";
		break;
	case WSAEFAULT:
		vipcerr << "Bad address.";
		break;
	case WSAEHOSTDOWN:
		vipcerr << "Host is down.";
		break;
	case WSAEHOSTUNREACH:
		vipcerr << "No route to host.";
		break;
	case WSAEINPROGRESS:
		vipcerr << "Operation now in progress.";
		break;
	case WSAEINTR:
		vipcerr << "Interrupted function call.";
		break;
	case WSAEINVAL:
		vipcerr << "Invalid argument.";
		break;
	case WSAEISCONN:
		vipcerr << "Socket is already connected.";
		break;
	case WSAEMFILE:
		vipcerr << "Too many open files.";
		break;
	case WSAEMSGSIZE:
		vipcerr << "Message too long.";
		break;
	case WSAENETDOWN:
		vipcerr << "Network is down.";
		break;
	case WSAENETUNREACH:
		vipcerr << "Network is unreachable.";
		break;
	case WSAENETRESET:
		vipcerr << "Network dropped connection on reset.";
		break;
	case WSAENOBUFS:
		vipcerr << "No buffer space available.";
		break;
	case WSAENOPROTOOPT:
		vipcerr << "Bad protocol option.";
		break;
	case WSAENOTCONN:
		vipcerr << "Socket is not connected.";
		break;
	case WSAENOTSOCK:
		vipcerr << "Socket operation on non-socket.";
		break;
	case WSAEOPNOTSUPP:
		vipcerr << "Operation not supported.";
		break;
	case WSAEPFNOSUPPORT:
		vipcerr << "Protocol family not supported.";
		break;
	case WSAEPROCLIM:
		vipcerr << "Too many processes.";
		break;
	case WSAEPROTONOSUPPORT:
		vipcerr << "Protocol not supported.";
		break;
	case WSAEPROTOTYPE:
		vipcerr << "Protocol wrong type for socket.";
		break;
	case WSAESHUTDOWN:
		vipcerr << "Cannot send after socket shutdown.";
		break;
	case WSAESOCKTNOSUPPORT:
		vipcerr << "Socket type not supported.";
		break;
	case WSAETIMEDOUT:
		vipcerr << "Connection timed out.";
		break;
/*	case WSATYPE_NOT_FOUND:
		cerr << "Class type not found.";
		break;
	case WSAHOST_NOT_FOUND:
		cerr << "Host not found.";
		break;
	case WSA_INVALID_HANDLE:
		cerr << "Specified event object handle is invalid.";
		break;
	case WSA_INVALID_PARAMETER:
		cerr << "One or more parameters are invalid.";
		break;
	case WSAINVALIDPROCTABLE:
		cerr << "Invalid procedure table from service provider.";
		break;
	case WSAINVALIDPROVIDER:
		cerr << "Invalid service provider version number.";
		break;
	case WSA_IO_INCOMPLETE:
		cerr << "Overlapped I/O event object not in signaled state.";
		break;
	case WSA_IO_PENDING:
		cerr << "Overlapped operations will complete later.";
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		cerr << "Insufficient memory available.";
		break;
*/		case WSANOTINITIALISED:
		vipcerr << "Successful WSAStartup not yet performed.";
		break;
	case WSANO_DATA:
		vipcerr << "Valid name, no data record of requested type.";
		break;
	case WSANO_RECOVERY:
		vipcerr << "This is a non-recoverable error.";
		break;
/*	case WSAPROVIDERFAILEDINIT:
		cerr << "Unable to initialize a service provider.";
		break;
	case WSASYSCALLFAILURE:
		cerr << "System call failure.";
		break;
*/		case WSASYSNOTREADY:
		vipcerr << "Network subsystem is unavailable.";
		break;
	case WSATRY_AGAIN:
		vipcerr << "Non-authoritative host not found.";
		break;
	case WSAVERNOTSUPPORTED :
		vipcerr << "WINSOCK.DLL version out of range.";
		break;
	case WSAEDISCON:
		vipcerr << "Graceful shutdown in progress.";
		break;
/*		case WSA_OPERATION_ABORTED:
		cerr << "Overlapped operation aborted.";
		break;
*/	
	case WSAEWOULDBLOCK:
		break;
	default:
		vipcerr << "Unknown error number." << iErrno;
		break;
	}
#else
	switch (iErrno)
	{
		case HOST_NOT_FOUND:
			{
				vipcerr << "HOST NOT FOUND";
				break;
			}
		case NO_ADDRESS:
			{
				vipcerr << "NO IP ADDRESS";
				break;
			}
		case NO_RECOVERY:
		case TRY_AGAIN:
			{
				vipcerr << "DNS ERROR? (NO RECOVERY OR TRY AGAIN LATER)";
				break;
			}
		default:
			{
				vipcerr << "UNKNOWN ERROR (" << iErrno << ")";
				break;
			}
	}
#endif
	vipcerr << endl;
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaIPAddress::VistaIPAddress()
{
	m_iAddressType = VIPA_UNASSIGNED;
	m_bIsValid     = false;
}

VistaIPAddress::VistaIPAddress(const string &sHostName)
{
	m_iAddressType = VIPA_UNASSIGNED;
	m_bIsValid     = false;
	SetHostName(sHostName);
}

VistaIPAddress::VistaIPAddress(const VistaIPAddress &adr)
{
   m_iAddressType = adr.m_iAddressType;
   m_sHostName = adr.m_sHostName;
   m_bIsValid = adr.m_bIsValid;
   
   //SetHostName(m_sHostName);
//   m_bIsValid = adr.m_bIsValid;
//   m_sHostName = adr.m_sHostName;
   if(m_iAddressType != VIPA_UNASSIGNED)
	m_sInAddress.assign(adr.m_sInAddress.begin(), adr.m_sInAddress.end());
}

VistaIPAddress::~VistaIPAddress()
{

}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaIPAddress::ResolveHostName(const string &sHostName)
{

	struct hostent  *pheDetails;

	if(isalpha(sHostName[0]))
	{
		// this failed, this means that we have no dotted
		// notation in sHostName, we can use gethostbyname
		// for resolving the address
		pheDetails = gethostbyname(sHostName.c_str());
	}
	else
	{
		// this worked! we have a dotted notation now
		// use gethostbyaddr for hostent resolvement
		int iAddr = inet_addr(sHostName.c_str());
		pheDetails = gethostbyaddr((char*)&iAddr, sizeof(iAddr), AF_INET);
	}

	if(!pheDetails)
	{
		vipcout << "[VistaIpAddress]: gethostbyname failed on " << sHostName.c_str() << endl;
		#if !defined(HPUX)
		/** @todo fix this 'compatibility' feature */
		TranslateHostResolveError(h_errno);
		#endif
		m_bIsValid     = false;
		return false;
	}

	m_sHostName    = pheDetails->h_name; // ok, hostname resolved
	m_sInAddress.resize(pheDetails->h_length);
	m_sInAddress.assign(pheDetails->h_addr_list[0], pheDetails->h_length);
	m_iAddressType = pheDetails->h_addrtype;

	return true;
}

void VistaIPAddress::SetHostName(const string &sHostName)
{
	if(sHostName != m_sHostName)
		m_bIsValid = ResolveHostName(sHostName);   
//    string s;
//    GetAddressString(s);
	//printf("hostname = %s, addr: %s\n", m_sHostName.c_str(), s.c_str());
}

void VistaIPAddress::SetAddress(const string &sAddress)
{
	
	struct hostent  *pheDetails;

	if(!(pheDetails = gethostbyaddr(sAddress.data(), (int)sAddress.length(), AF_INET)))
	{
		#if !defined(HPUX)
		/** @todo fix this 'compatibility' feature */
		TranslateHostResolveError(h_errno);
		#endif
		m_bIsValid     = false;
		return;
	}

	m_sHostName    = pheDetails->h_name; // ok, hostname resolved
	m_sInAddress.resize(pheDetails->h_length);
	m_sInAddress.assign(pheDetails->h_addr_list[0], pheDetails->h_length);

//    string s;
//    GetAddressString(s);
	//printf("hostname = %s, addr: %s\n", m_sHostName.c_str(), s.c_str());


	m_iAddressType = pheDetails->h_addrtype;
	m_bIsValid     = true;
}

void VistaIPAddress::GetHostName(string &sHostName) const
{
	sHostName = m_sHostName;
}

const char *VistaIPAddress::GetHostNameC() const
{
	return m_sHostName.c_str();
}

string VistaIPAddress::GetIPAddressC() const
{
	if(!m_bIsValid)
		return "[INVALID]";

	char buffer[256];
	memset(buffer, 0, 256);

	sprintf(buffer, "%d.%d.%d.%d", 
		(int)((unsigned char)m_sInAddress[0]), 
		(int)((unsigned char)m_sInAddress[1]), 
		(int)((unsigned char)m_sInAddress[2]), 
		(int)((unsigned char)m_sInAddress[3]));

	return string(buffer);
}

void *VistaIPAddress::GetINAddress() const
{
	return (void*)m_sInAddress.data();
}

void VistaIPAddress::SetRawINAddress(struct in_addr *pData, int iSize, bool bSetName)
{
	m_sInAddress.resize(iSize);
	m_sInAddress.assign((const char*)pData, iSize);

	if(bSetName)
		GetAddressString(m_sHostName);

}

int   VistaIPAddress::GetINAddressLength() const
{
	return (int)m_sInAddress.size();
}


void  VistaIPAddress::GetAddressString(string &sString) const
{
	sString = inet_ntoa(*((in_addr*)(m_sInAddress.data())));
}

void  VistaIPAddress::GetAddressTypeString(string &sString) const
{
	switch(m_iAddressType)
	{
	case VIPA_UNASSIGNED:
		{
			sString = "Unassigned";
			break;
		}
	case AF_INET:
		{
			sString = "IPV4 (AF_INET)";
			break;
		}
	default:
		{
			sString = "Unknown, sorry";
			break;
		}
	}
}


int   VistaIPAddress::GetAddressType() const
{
	return m_iAddressType;
}

VistaIPAddress &VistaIPAddress::operator= (const VistaIPAddress &inAddr)
{
	//printf("VistaIPAddress::operator=\n");
	SetHostName(inAddr.GetHostNameC());
	m_iAddressType = inAddr.m_iAddressType;
//    m_bIsValid = inAddr.m_bIsValid;
//    m_sInAddress.assign(inAddr.m_sInAddress.begin(), inAddr.m_sInAddress.end());    
	return *this;
}

VistaIPAddress &VistaIPAddress::operator= (const string &sHostName)
{
	SetHostName(sHostName);
	m_iAddressType = AF_INET; // we simply assume this...
	return *this;
}


bool VistaIPAddress::GetIsValid() const
{
	return m_bIsValid;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


