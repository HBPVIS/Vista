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
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#include <cassert>
#if   defined (WIN32)
	#include <winsock2.h>
	typedef int socklen_t;
#elif defined (LINUX) || defined (DARWIN)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <time.h>
#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <time.h>
#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <time.h>
#else
	#error "You have to define the target platform in order to compile ViSTA"
#endif
#include <iostream>
using namespace std;

#include <cerrno>     // errno
#include <cstdio>
#include <fcntl.h>
#include "VistaSocket.h"
#include "VistaIPAddress.h"
#include "VistaSocketAddress.h"
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#if defined(VISTA_IPC_USE_EXCEPTIONS)
#include <VistaBase/VistaExceptionBase.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
bool IVistaSocket::PrintErrorMessage(const string &sMethodName)
{
	#ifdef WIN32
		int err = WSAGetLastError ();
	#else
		int err = errno;
	#endif
// no message text (and endline, see end of function) on  [WSA]EQOULDBLOCK
// otherwise you will get an abundance of error messagesc
// when using loops of non-blocking accepts
#ifdef WIN32
		if (err && err !=WSAEWOULDBLOCK)
#endif
#if defined(IRIX) || defined(SUNOS) || defined(LINUX)
		if (err & err !=EWOULDBLOCK)
#endif
	vipcerr << "IVistaSocket::" << sMethodName << " failed with code " << err << ": ";
#ifdef WIN32
	switch ( err )
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
		vipcerr << "Unknown error number. (" << err << ")" << endl;
		break;
	}
#elif defined ( IRIX )
	switch ( err )
	{
	case 0:
		//cerr << "An error without error?!? Our code is wrong." << endl;
		break;
	case EBADF:
		vipcerr << "[EBADF] Not a valid descriptor." << endl;
		break;
	case ENOTSOCK:
		vipcerr << "[ENOTSOCK] Is a descriptor for a file, not a socket." << endl;
		break;
	case EADDRNOTAVAIL:
		vipcerr << "[EADDRNOTAVAIL] The specified address is not available on this ";
		vipcerr << "machine." << endl;
		break;
	case EAFNOSUPPORT:
		vipcerr << "[EAFNOSUPPORT] Addresses in the specified address family cannot be ";
		vipcerr << "used with this socket." << endl;
		break;
	case EISCONN:
		vipcerr << "[EISCONN] The socket is already connected." << endl;
		break;
	case ETIMEDOUT:
		vipcerr << "[ETIMEDOUT] Connection establishment timed out without ";
		vipcerr << "establishing a connection." << endl;
		break;
	case ECONNREFUSED:
		vipcerr << "[ECONNREFUSED] The attempt to connect was forcefully rejected." << endl;
		break;
	case ENETUNREACH:
		vipcerr << "[ENETUNREACH] The network isn't reachable from this host." << endl;
		break;
	case EADDRINUSE:
		vipcerr << "[EADDRINUSE] The address is already in use." << endl;
		break;
	case EWOULDBLOCK:
// no message on EWOULDBLOCK
//		vipcerr << "[EWOULDBLOCK] ";
//		vipcerr << "The socket is marked as non-blocking, and the ";
//		vipcerr << "requested operation would block." << endl;
		break;
	default:
		vipcerr << "Unknown error number. (" << err << ")" << endl;
		break;
	}
#elif defined ( SUNOS )
	switch ( err )
	{
	case 0:
		//cerr << "An error without error?!? Our code is wrong." << endl;
		break;
	case EACCES:
		vipcerr << "[EACCES] ";
		vipcerr << "Search permission is denied for a component of the ";
		vipcerr << "path prefix of the pathname in name." << endl;
		break;
	case EADDRINUSE:
		vipcerr << "[EADDRINUSE] ";
		vipcerr << "The address is already in use." << endl;
		break;
	case EADDRNOTAVAIL:
		vipcerr << "[EADDRNOTAVAIL] ";
		vipcerr << "The specified address is not available on the remote ";
		vipcerr << "machine." << endl;
		break;
	case EAFNOSUPPORT:
		vipcerr << "[EAFNOSUPPORT] ";
		vipcerr << "Addresses in the specified address family cannot be ";
		vipcerr << "used with this socket." << endl;
		break;
	case EALREADY:
		vipcerr << "[EALREADY] ";
		vipcerr << "The socket is non-blocking, and a previous connection ";
		vipcerr << "attempt has not yet been completed." << endl;
		break;
	case EBADF:
		vipcerr << "[EBADF] ";
		vipcerr << "Is not a valid descriptor." << endl;
		break;
	case ECONNREFUSED:
		vipcerr << "[ECONNREFUSED] ";
		vipcerr << "The attempt to connect was forcefully rejected. The ";
		vipcerr << "calling program should close(2) the socket descriptor, ";
		vipcerr << "and issue another socket(3SOCKET) call to obtain a new ";
		vipcerr << "descriptor before attempting another connect() call." << endl;
		break;
	case EINPROGRESS:
		vipcerr << "[EINPROGRESS] ";
		vipcerr << "The socket is non-blocking, and the connection  cannot ";
		vipcerr << "be completed immediately. You can use select(3C) to ";
		vipcerr << "complete the connection by selecting the socket for ";
		vipcerr << "writing." << endl;
		break;
	case EINTR:
		vipcerr << "[EINTR] ";
		vipcerr << "The connection attempt was interrupted before any data ";
		vipcerr << "arrived by the delivery of a signal." << endl;
		break;
	case EINVAL:
		vipcerr << "[EINVAL] ";
		vipcerr << "namelen is not the size of a valid address for the ";
		vipcerr << "specified address family." << endl;
		break;
	case EIO:
		vipcerr << "[EIO] ";
		vipcerr << "An I/O error occurred while reading from or writing to ";
		vipcerr << "the file system." << endl;
		break;
	case EISCONN:
		vipcerr << "[EISCONN] ";
		vipcerr << "The socket is already connected." << endl;
		break;
	case ELOOP:
		vipcerr << "[ELOOP] ";
		vipcerr << "Too many symbolic links were encountered in translat";
		vipcerr << "ing the pathname in name." << endl;
		break;
	case ENETUNREACH:
		vipcerr << "[ENETUNREACH] ";
		vipcerr << "The network is not reachable from this host." << endl;
		break;
	case EHOSTUNREACH:
		vipcerr << "[EHOSTUNREACH] ";
		vipcerr << "The remote host is not reachable from this host." << endl;
		break;
	case ENOENT:
		vipcerr << "[ENOENT] ";
		vipcerr << "A component of the path prefix of the pathname in name ";
		vipcerr << "does not exist." << endl;
		break;
	case ENOSR:
		vipcerr << "[ENOSR] ";
		vipcerr << "There were insufficient STREAMS resources available to ";
		vipcerr << "complete the operation." << endl;
		break;
	case ENXIO:
		vipcerr << "[ENXIO] ";
		vipcerr << " The server exited before the connection was complete." << endl;
		break;
	case ETIMEDOUT:
		vipcerr << "[ETIMEDOUT] ";
		vipcerr << "Connection establishment timed out without establish";
		vipcerr << "ing a connection." << endl;
		break;
	case EWOULDBLOCK:
// no message on EWOULDBLOCK
//		cerr << "[EWOULDBLOCK] ";
//		cerr << "The socket is marked as non-blocking, and the ";
//		cerr << "requested operation would block." << endl;
		break;
	// The following errors are specific to connecting names in the
	// UNIX  domain.   These  errors might not apply in future ver-
	// sions of the UNIX  IPC domain.
	case ENOTDIR:
		vipcerr << "[ENOTDIR] ";
		vipcerr << "A component of the path prefix of the pathname in name ";
		vipcerr << "is not a directory." << endl;
		break;
	case ENOTSOCK:
		vipcerr << "[ENOTSOCK] ";
		vipcerr << "Is not a socket." << endl;
		break;
	case EPROTOTYPE:
		vipcerr << "[EPROTOTYPE] ";
		vipcerr << "The file that is referred to by name is a socket of a ";
		vipcerr << "type other than type s. For example, s is a SOCK_DGRAM ";
		vipcerr << "socket, while name refers to a SOCK_STREAM socket." << endl;
		break;
	default:
		vipcerr << "Unknown error number. (" << err << ")" << endl;
		break;
	}
#elif defined ( LINUX )
	switch ( err )
	{
	case 0:
		vipcerr << "An error without error?!? Our code is wrong." << endl;
		break;
	case EBADF:
		vipcerr << "[EBADF] ";
		vipcerr << "The  file  descriptor is not a valid index in the descriptor ta-";
		vipcerr << "ble." << endl;
		break;
	case EFAULT:
		vipcerr << "[EFAULT] ";
		vipcerr << "The socket structure  address  is  outside  the  user's  address";
		vipcerr << "space." << endl;
		break;
	case ENOTSOCK:
		vipcerr << "[ENOTSOCK] ";
		vipcerr << " The file descriptor is not associated with a socket."<< endl;
		break;
	case EISCONN:
		vipcerr << "[EISCONN] ";
		vipcerr << "The socket is already connected. " << endl;
		break;
	case ECONNREFUSED:
		vipcerr << "[ECONNREFUSED] ";
		vipcerr << "No one listening on the remote address." << endl;
		break;
	case ETIMEDOUT:
		vipcerr << "[ETIMEDOUT] ";
		vipcerr << "Timeout  while attempting connection. The server may be too busy";
		vipcerr << "to accept new connections. Note that for IP sockets the  timeout";
		vipcerr << "may be very long when syncookies are enabled on the server." << endl;
		break;
	case ENETUNREACH:
		vipcerr << "[ENETUNREACH] ";
		vipcerr << "Network is unreachable." << endl;
		break;
	case EADDRINUSE:
		vipcerr << "[EADDRINUSE] ";
		vipcerr << "Local address is already in use." << endl;
		break;
	case EINPROGRESS:
		vipcerr << "[EINPROGRESS] ";
		vipcerr << "The  socket  is  non-blocking  and the connection cannot be com-";
		vipcerr << "pleted immediately.  It is possible to select(2) or poll(2)  for";
		vipcerr << "completion  by  selecting  the  socket for writing. After select";
		vipcerr << "indicates writability, use getsockopt(2) to  read  the  SO_ERROR";
		vipcerr << "option  at  level  SOL_SOCKET  to determine whether connect com-";
		vipcerr << "pleted  successfully  (SO_ERROR  is  zero)   or   unsuccessfully";
		vipcerr << "(SO_ERROR  is one of the usual error codes listed here, explain-";
		vipcerr << "ing the reason for the failure)." << endl;
		break;
	case EALREADY:
		vipcerr << "[EALREADY] ";
		vipcerr << "The socket is non-blocking and a previous connection attempt has";
		vipcerr << "not yet been completed." << endl;
		break;
	case EAGAIN:
/*		cerr << "[EAGAIN] ";
		cerr << "No  more free local ports or insufficient entries in the routing";
		cerr << "cache. For PF_INET see the  net.ipv4.ip_local_port_range  sysctl";
		cerr << "in ip(7) on how to increase the number of local ports." << endl;
		*/
		break;
	case EAFNOSUPPORT:
		vipcerr << "[EAFNOSUPPORT] ";
		vipcerr << "The passed address didn't have the correct address family in its";
		vipcerr << "sa_family field." << endl;
		break;
	case EACCES:
		vipcerr << "[EACCES] ";
		vipcerr << "The user tried to connect to a broadcast address without  having";
		vipcerr << "the  socket  broadcast  flag  enabled  or the connection request";
		vipcerr << "failed because of a local firewall rule." << endl;
		break;
	case EADDRNOTAVAIL:
	{
		vipcerr << "[EADDRNOTAVAIL] ";
		vipcerr << "Cannot assign requested address.\n";
		break;
	}
	case ENOTCONN:
	{
		vipcerr << "[ENOTCONN] ";
		vipcerr << "Transport endpoint is not connected\n";
		break;
	}
	case ECONNABORTED:
	{
		vipcerr << "[ECONNABORTED] "
		     << "Software caused connection abort.\n";
		break;
	}
	default:
		vipcerr << "Unknown error number. (" << err << ")" << endl;
		break;
	}
#else
	switch ( err )
	{
	default:
		vipcerr << "Unknown error number. (" << err << ")" << endl;
				vipcerr << "try /usr/include/asm/errno.h for more help\n";
		break;
	}
#endif
#ifdef WIN32
	if (err !=WSAEWOULDBLOCK){ return false; }
#endif
#ifdef IRIX
	if (err !=EWOULDBLOCK){ return false; }
#endif
#if defined(SUNOS) || defined(LINUX)
	if (err !=EWOULDBLOCK){ return false; }
#endif
	return true;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaSocket::IVistaSocket(int iDomain, int iType, int iProtocol)
{
	m_iDomain = iDomain;
	m_iType   = iType;
	m_iProtocol = iProtocol;
	m_bIsBound = false;
	m_bIsConnected = false;
	m_bIsBlocking = true;
	m_bIsBuffering = false;
	m_bShowRaw = false;
	m_bErrorState = false;
	m_iSocketID = (HANDLE)~0;
	//m_bMulticast = false;
#if defined(WIN32)
	m_pWaitEvent = new VistaThreadEvent(false); // no posix event on UNIX
#else
	m_pWaitEvent = NULL;
#endif
}

IVistaSocket::~IVistaSocket()
{
	delete m_pWaitEvent;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void IVistaSocket::SetIsConnected(bool bConnected)
{
	m_bIsConnected = bConnected;
}

bool IVistaSocket::GetSockOpt(int iCodeLevel, int iOptionName, void *pOptionData, int &iDataLength) const
{
#if !defined(HPUX)
	if (getsockopt(SOCKET(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, (socklen_t*)&iDataLength) < 0)
#else
	if (getsockopt(int(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, &iDataLength) < 0)
#endif
	{
		// error.
		return false;
	}
	return true;
}

bool IVistaSocket::SetSockOpt(int iCodeLevel, int iOptionName, void *pOptionData, int iDataLength)
{
	if(setsockopt(SOCKET(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, iDataLength) < 0)
	{
		return false;
	}
	return true;
}

bool IVistaSocket::OpenSocket()
{
	if((m_iSocketID = HANDLE(socket (m_iDomain, m_iType, m_iProtocol))) <= 0)
	{
		// error
		m_iSocketID = HANDLE(~0);
		PrintErrorMessage("OpenSocket()");
		return false;
	}
	SetIsBuffering(m_bIsBuffering);
	SetIsBlocking(m_bIsBlocking);
	SetErrorState(false);
	//printf("IVistaSocket::OpenSocket() -- %d\n", m_iSocketID);
	return true;
}

bool IVistaSocket::CloseSocket(bool bSkipRead)
{
	if ( this->m_iSocketID != HANDLE(~0) )
	{
		//printf("IVistaSocket::CloseSocket() -- %d\n", m_iSocketID);
		// close socket
		#ifdef WIN32
			closesocket ( SOCKET(this->m_iSocketID) );
		#else
			close       ( int(this->m_iSocketID) );
		#endif
			m_iSocketID = HANDLE(~0);
			m_bIsConnected=false;
			m_bIsBound=false;
	}
	SetErrorState(false);
	return true; // a closed socket stays closed
}

bool IVistaSocket::GetIsBuffering() const
{
	return m_bIsBuffering;
}

bool IVistaSocket::GetIsBlocking() const
{
	return m_bIsBlocking;
}

bool IVistaSocket::VerifyBlocking()
{
#if !defined(WIN32)
	if(!GetIsValid())
		return true; // no sockets verify to true

	int iRes = fcntl(int(m_iSocketID), F_GETFD);
	if(iRes & O_NONBLOCK == O_NONBLOCK)
	{
		// ok, marked non-blocking
		if(GetIsBlocking())
			{
				// internal flag says we are blocking
				// we swap this
				m_bIsBlocking = false;
			}
	}
	else
	{
		// ok, desc is marked blocking
		if(!GetIsBlocking())
		{
			// internal flag says, we are non-blocking
			// we flip this
			m_bIsBlocking = true;
		}
	}

	return m_bIsBlocking;
#else
	if(!GetIsValid())
		return true;
	if(GetIsBlocking())
	{
		// ok, we are supposed to be blocking
		u_long isNonBlocking=0;
		if (ioctlsocket ( SOCKET(m_iSocketID), FIONBIO, & isNonBlocking) == SOCKET_ERROR)
		{
			int iErr = WSAGetLastError();
			if(WSAGetLastError() == WSAEINVAL)
			{
				// setting socket to blocket state returned
				// WSAEINVAL
				// according to the MSDN, this socket was touched
				// by WSAAsyncSelect/WSAEventSelect, and marked non-blocking
				m_bIsBlocking = true;
			}
			else
			{
				vipcerr << "IVistaSocket::VerifyBlocking() -- WSA error (" << iErr << ")\n";
			}
		}
	}
	return m_bIsBlocking;
#endif
}

bool IVistaSocket::GetIsBoundToAddress() const
{
	return m_bIsBound;

}

bool IVistaSocket::GetIsConnected() const
{
	return m_bIsConnected;
}

bool IVistaSocket::GetIsOpen() const
{
	return (m_iSocketID !=  HANDLE(~0));
}

bool IVistaSocket::GetIsValid() const
{
	return (m_iSocketID != HANDLE(~0));
}

void IVistaSocket::SetIsBuffering(bool bBuffering)
{
   m_bIsBuffering = bBuffering;
}

void IVistaSocket::SetIsBlocking(bool bBlocking)
{
	if(GetIsValid())
	{

		#if defined( WIN32 )
				unsigned long isNonBlocking = (bBlocking ? 0 : 1);
			if (ioctlsocket ( SOCKET(this->m_iSocketID), FIONBIO, & isNonBlocking) == SOCKET_ERROR)
		#else
			int isNonBlocking = (bBlocking ? 0:1);
			if (ioctl ( int(this->m_iSocketID), FIONBIO, & isNonBlocking))
		#endif
			{
				// display error message
				this->PrintErrorMessage ("SetBlocking");
				return;
			}
	}
	// everything went alright
	m_bIsBlocking = bBlocking;
}

bool IVistaSocket::BindToAddress(const VistaSocketAddress &rMyAddress)
{
	if ( (bind   ( SOCKET(m_iSocketID), (sockaddr *)rMyAddress.GetINAddress(), rMyAddress.GetINAddressLength() )) != 0)
	{
		PrintErrorMessage("BindToAddress()");
		return false;
	}
	m_bIsBound = true; // toggle
	return true;
}

bool IVistaSocket::ConnectToAddress(const VistaSocketAddress &rAddress)
{
	if ( connect ( SOCKET(this->m_iSocketID), (sockaddr *)rAddress.GetINAddress(), rAddress.GetINAddressLength() ) == -1 )
	{
		//m_iSocketID = -1;
		if(PrintErrorMessage("ConnectToAddress()")==false)
		{
			vipcerr << "IVistaSocket::ConnectToAddress("
				<< rAddress.GetIPAddress().GetHostNameC()
				<< " [" << rAddress.GetIPAddress().GetIPAddressC()
				<< "] -- error during connect\n";
			SetErrorState(true);
			return false;
		}
		else
		{
			if(GetIsBlocking())
				return false; // could not connect on a blocking socket

			/**
			 *@todo
			 * note: we should treat connected state as
			 * yes, no, pending (tri-nary)
			 */
		}
	}
	m_bIsConnected = true; // toggle
	return true;
}

bool IVistaSocket::GetLocalSockName(VistaSocketAddress &rSocket) const
{

#if !defined(HPUX)
	socklen_t slDummy=rSocket.GetINAddressLength();
	if (getsockname(SOCKET(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), (socklen_t*)&slDummy) < 0)
#else
	int slDummy=rSocket.GetINAddressLength();
	if (getsockname(int(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), &slDummy) < 0)
#endif
	{
		PrintErrorMessage("GetLocalSocket()");
		return false;
	}
	return true;
}

bool IVistaSocket::GetPeerSockName(VistaSocketAddress &rSocket) const
{
#if !defined(HPUX)
	socklen_t slDummy=rSocket.GetINAddressLength();
	if(getpeername(SOCKET(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), (socklen_t*)&slDummy) < 0)
#else
	int slDummy=rSocket.GetINAddressLength();
	if(getpeername(int(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), &slDummy) < 0)
#endif
	{
		PrintErrorMessage("IVistaSocket::GetPeerSockName()");
		return false;
	}
	return true;
}

unsigned long IVistaSocket::HasBytesPending() const
{
	if(!GetIsValid())
		return 0; // no socket has no data

	#ifdef WIN32
		unsigned long result = 0;
	// windows function names
		if ( ! ioctlsocket ( SOCKET(m_iSocketID), FIONREAD, & result ) )
			return result;
	#else
		int result = 0;
		if ( ioctl ( int(m_iSocketID), FIONREAD, & result ) != -1 )
			return result;
	#endif

	return 0;
}

unsigned long IVistaSocket::WaitForIncomingData(int iTimeout)
{
	assert(GetIsValid());
#if defined(WIN32)
	bool b=GetIsBlocking();
	// associate with event (socket gets non-blocking!)
	WSAEventSelect(SOCKET(GetSocketID()),
			   (*m_pWaitEvent).GetEventSignalHandle(),
			   FD_READ|FD_CLOSE);
	DWORD iRet = WaitForSingleObject((*m_pWaitEvent).GetEventSignalHandle(), (iTimeout>0 ? iTimeout : INFINITE));
	// remove event association
	WSAEventSelect(SOCKET(GetSocketID()),
			   (*m_pWaitEvent).GetEventSignalHandle(),
			   0);
	if(b) // set to blocking again
		SetIsBlocking(true);
	switch(iRet)
	{
	case WAIT_ABANDONED:
		{
		#if defined(VISTA_IPC_USE_EXCEPTIONS)
			VISTA_THROW("WaitForIncomingData -- Exception", 0x00000104)
		#endif
			break;
		}
	case WAIT_TIMEOUT:
		{
			return ~0; // omit tcp-query for pending bytes
			break;
		}
	case WAIT_OBJECT_0: // all clear
		{
			break;
		}
	case WAIT_FAILED:
		{
		#if defined(VISTA_IPC_USE_EXCEPTIONS)
			VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000105)
		#endif
			break;
		}
	default:
		printf("WaitForIncomingData() -- %d\n", HasBytesPending());
		break;
	}
	return HasBytesPending();
#else // UNIX

  struct timeval tv;
  struct timeval *etv = &tv;
  if(iTimeout != 0)
  {
	tv.tv_sec  = iTimeout/1000;
	tv.tv_usec = (iTimeout%1000)*1000;
  }
  else
  {
	  //tv.tv_sec = 0;
	  //tv.tv_usec= 0;
	  etv = NULL;
  }
	fd_set myRdSocks;
	fd_set myExSocks;
	FD_ZERO(&myRdSocks);
	FD_ZERO(&myExSocks);
	FD_SET(int(m_iSocketID), &myRdSocks);
	FD_SET(int(m_iSocketID), &myExSocks);
	int iRet = select(int(m_iSocketID)+1, &myRdSocks, NULL, &myExSocks, etv);
	if(iRet == 0)
	{
		// timeout
		return ~0;
	}
	if(iRet == -1)
	{
		printf("-- WARNING -- socket-select returned -1 (exception) during WaitForIncomingData(%ld, %d).\n",
				long(m_iSocketID), iTimeout);
#if defined(VISTA_IPC_USE_EXCEPTIONS)
		VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000104)
#endif
	}

	if(FD_ISSET(int(m_iSocketID), &myExSocks))
	{
		// whoah!
		printf("-- WARNING -- socket is in exception array.\n");
#if defined(VISTA_IPC_USE_EXCEPTIONS)
		VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000104)
#endif
	}

	// printf("IVistaSocket[%d]::WaitForIncomingData() -- %d bytes ready\n", m_iSocketID, iSize);
	return HasBytesPending();
#endif // UNIX PART
}

unsigned long IVistaSocket::WaitForSendFinish(int iTimeout)
{
	if(!GetIsValid())
		return 0;
	//assert(m_iSocketID>=0);
  struct timeval tv;
  struct timeval *etv = &tv;
  //iTimeout *= 1000;
  if(iTimeout != 0)
  {
	tv.tv_sec  = iTimeout/1000;
	tv.tv_usec = (iTimeout%1000)*1000;
  }
  else
  {
#if defined(WIN32)
	  tv.tv_sec = 0;
	  tv.tv_usec= 0;
#else
	  etv = NULL;
#endif
  }

#if defined(WIN32)
	FD_SET myWrSocks;
	FD_SET myExSocks;
#else
	fd_set myWrSocks;
	fd_set myExSocks;
#endif
	FD_ZERO(&myWrSocks);
	FD_ZERO(&myExSocks);
	FD_SET(SOCKET(m_iSocketID), &myWrSocks);
	FD_SET(SOCKET(m_iSocketID), &myExSocks);
	int iRet = select(int(SOCKET(m_iSocketID)+1), NULL, &myWrSocks, &myExSocks, etv);
	if(FD_ISSET(SOCKET(m_iSocketID), &myExSocks))
	{
		// whoah!
		printf("-- WARNING -- socket is in exception array.\n");
#if defined(VISTA_IPC_USE_EXCEPTIONS)
		VISTA_THROW("IVistaSocket::WaitForSendFinish -- Exception", 0x00000103)
#endif
		return 0;
	}
	return 1;
}

int  IVistaSocket::SendRaw(const void *pvBuffer, const int iLength, int iFlags)
{
	if(!GetIsValid())
		return -1;
	if(m_bShowRaw)
	{
		printf("IVistaSocket[%d]::SendRaw() -- [%d] bytes\n", int(m_iSocketID), iLength);
		for(int i=0; i < iLength; ++i)
		{
			printf("%02x ", ((unsigned char*)pvBuffer)[i]);
			if(i && (i%16==0))
				printf("\n");
		}
		printf("\n");
	}

	int iRec = send(SOCKET(m_iSocketID), (char *)pvBuffer, iLength, iFlags);
	if(iRec < 0)
	{
		if(PrintErrorMessage("SendRaw()") == false)
		{
			// we mark the socket close
			SetErrorState(true);
			CloseSocket();
			// idicate network failure with an exception.
			#if defined(VISTA_IPC_USE_EXCEPTIONS)
			VISTA_THROW("IVistaSocket::SendRaw -- Exception\n", 0x00000101)
			#endif
		}
		else
		{
			// PrintErrorMessage returned true, so this is a non-blocking write
			// (most probably), we will return 0 here, as this is our semantics
			// on a non-blocking partially non-data write
			return 0;
		}
	}
	return iRec;
}


int  IVistaSocket::ReceiveRaw(void *pvBuffer, const int iLength, int iTimeout, int iFlags)
{
	if(!GetIsValid())
		return -1;


	int iReadLength = iLength;
	if(iTimeout)
	{
		int iReady = WaitForIncomingData(iTimeout);
		if(iReady == ~0) // timeout == ~0
			return 0; // the user provided timeout, so 0 is a good guess ;)

		// a failure results in an exception, so we will not get here (hopefully)
		// go a catch the exception in an upper level

		if(iReady < iLength)
			iReadLength = iReady; // adjust without buffer-size-violence
		if(iReadLength == 0)
			return (iReadLength != iLength ? -1 : iLength); // iff we could not read the desired value, we return -1
	}
	int iRet = recv(SOCKET(m_iSocketID), (char *)pvBuffer, iReadLength, iFlags);
	if(m_bShowRaw)
	{
		printf("ReceiveRaw() -- [%d] bytes\n", iRet);
		for(int i=0; i < iRet; ++i)
		{
			printf("%02x ", ((unsigned char*)pvBuffer)[i]);
			if(i && (i%16==0))
				printf("\n");
		}
		printf("\n");
   }
	if(iRet < 0)
	{
		if(PrintErrorMessage("ReceiveRaw") == false)
		{
			// it's a real error and _not_ WOULDBLOCK
			if(GetSocketTypeString() != string("UDP"))
			{
				// we have a real error on a tcp socket
				SetErrorState(true);
				CloseSocket();
				#if defined(VISTA_IPC_USE_EXCEPTIONS)
				VISTA_THROW("IVistaSocket::ReceiveRaw -- Exception", 0x00000102)
				#endif
			}
			// we are less strict with UPD sockets, so we return the -1 in iRet below
		}
		else
		{
			// ok, recv returned -1, PrintErrorMessage() returned true,
			// this indicates a WOULDBLOCK for non blocking read
			// we will return 0, as this is our sematics for
			// sucessful, but no-data non-blocking receive
			return 0;
		}

	}
	return iRet;
}

void IVistaSocket::SetShowRawSendAndReceive(bool bShowRaw)
{
	m_bShowRaw = bShowRaw;
}

bool IVistaSocket::GetShowRawSendAndReceive() const
{
	return m_bShowRaw;
}

HANDLE  IVistaSocket::GetSocketID() const
{
	return m_iSocketID;
}

bool IVistaSocket::SetSocketID(HANDLE iSockID)
{
	if(GetIsValid())
		return false;
	m_iSocketID = iSockID;
	return true;
}

bool IVistaSocket::Flush()
{
	return false;
}

int IVistaSocket::GetPermitBroadcast() const
{
	int iOn;      // 0 disables broadcast else enables
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_BROADCAST, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

void IVistaSocket::SetPermitBroadcast(int iOn)
{   // 0 disables broadcast else enables
	int iSize = sizeof(iOn);
	if(!SetSockOpt(SOL_SOCKET, SO_BROADCAST, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetDebugInfoRecord() const
{
	int iOn;
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_DEBUG, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

int IVistaSocket::GetDontRoute() const
{
	int iOn;
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_DONTROUTE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

void IVistaSocket::SetDontRoute(int iOn)
{   // A nonzero value enables SO_DONTROUTE. A value of 0 disables SO_DONTROUTE.
	int iSize = sizeof(iOn);
	if(!SetSockOpt(SOL_SOCKET, SO_DONTROUTE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetErrorStatus() const
{
	int iValue;
	int iLength = sizeof(iValue);
	if(!GetSockOpt(SOL_SOCKET, SO_ERROR, (void*)&iValue, iLength))
	{
	  // we should utter a warning here...
	}
	return iValue;
}

int IVistaSocket::GetKeepConnectionsAlive() const
{
	int iOn;
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_KEEPALIVE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

void IVistaSocket::SetKeepConnectionsAlive(int iOn)
{   //A nonzero value enables keepalives. A value of 0 disables keepalives.
	int iSize = sizeof(iOn);
	if(!SetSockOpt(SOL_SOCKET, SO_KEEPALIVE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetLingerOnClose() const
{ //When the l_onoff field is nonzero, SO_LINGER is enabled. When it is 0, SO_LINGER is disabled.
  //If SO_LINGER is being enabled, the l_linger field specifies the timeout period, in seconds
	linger ling;
	int iSize = sizeof(linger*);
	if(!GetSockOpt(SOL_SOCKET, SO_LINGER, &ling, iSize))
	{
	  // we should utter a warning here...
	}
	if((ling.l_onoff > 0) && (ling.l_linger > 0))
		return ling.l_linger;
	return 0;
}

void IVistaSocket::SetLingerOnClose(int iLingerTimeInSecs)
{
	struct linger ling;
	ling.l_linger = iLingerTimeInSecs;
	ling.l_onoff =  iLingerTimeInSecs;
	int iSize = sizeof(struct linger*);
	if(!SetSockOpt(SOL_SOCKET, SO_LINGER, &ling, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetReceiveBufferSize() const
{
	int iBufSize=-1;
	int iSize = sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_RCVBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
	return iBufSize;
}

void IVistaSocket::SetReceiveBufferSize(int iBufSize)
{
	int iSize = sizeof(int);
	if(!SetSockOpt(SOL_SOCKET, SO_RCVBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetSendBufferSize() const
{
	int iBufSize=-1;
	int iSize = sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_SNDBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
	return iBufSize;
}

void IVistaSocket::SetSendBufferSize(int iBufSize)
{
	int iSize = sizeof(int);
	if(!SetSockOpt(SOL_SOCKET, SO_SNDBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetSocketType() const
{
	int iValue=0;
	int iSize=sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_TYPE, (void*)&iValue,iSize))
	{
	  // we should utter a warning here...
	}
	return iValue;
}

bool IVistaSocket::GetSocketReuse() const
{
	int iValue=0;
	int iSize = sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_REUSEADDR, (void*)&iValue,iSize))
	{
	  // we should utter a warning here...
	}
	return (iValue!=0);
}

void IVistaSocket::SetSocketReuse(bool bReuse)
{
	int iDo = bReuse ? 1:0;
	int iSize = sizeof(int);
	if(!SetSockOpt(SOL_SOCKET, SO_REUSEADDR, (void*)&iDo, iSize))
	{
	  // we should utter a warning here...
	}
}

void IVistaSocket::SetErrorState(bool bState)
{
	m_bErrorState = bState;
}

bool IVistaSocket::GetErrorState() const
{
	return m_bErrorState;
}

#if defined(WIN32)


void IVistaSocket::EnableEventSelect(VistaThreadEvent *pEvent, long lEvents)
{
	if(GetIsOpen())
	{
		WSAEventSelect( SOCKET(GetSocketID()),
						(*pEvent).GetEventSignalHandle(),
						lEvents);
	}
}

void IVistaSocket::DisableEventSelect(VistaThreadEvent *pEvent)
{
	if(GetIsOpen())
	{
		WSAEventSelect( SOCKET(GetSocketID()),
						(*pEvent).GetEventSignalHandle(),
						0);
	}
}

VistaThreadEvent *IVistaSocket::GetWaitEvent() const
{
	return m_pWaitEvent;
}


#endif


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
