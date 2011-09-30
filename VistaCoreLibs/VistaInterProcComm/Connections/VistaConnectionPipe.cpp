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
// $Id: VistaConnectionPipe.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#if !defined(WIN32)
#include <poll.h>
	#include <sys/ioctl.h>
	#include <sys/types.h>
#else

#endif

#if defined(SUNOS)
#include <unistd.h>
#endif

#include <iostream>
#include "VistaConnectionPipe.h"
#include <VistaInterProcComm/VistaInterProcCommOut.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
namespace {
const int PIPE_R = 0;
const int PIPE_W = 1;
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaConnectionPipe::VistaConnectionPipe()
{
	SetIsOpen(false);
}

VistaConnectionPipe::~VistaConnectionPipe()
{
	Close();
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaConnectionPipe::Open()
{
	if(GetIsOpen())
		return true;

#if !defined(WIN32)
	if( pipe(m_fd) == -1 )
		return false;
#else
	/**
	 * @todo implement?
	 */
#endif

	SetIsOpen(true);
	return true;
}

void VistaConnectionPipe::Close(  )
{
	if( !GetIsOpen() )
		return;

#if !defined(WIN32)
	close(m_fd[PIPE_R]);
	close(m_fd[PIPE_W]);
#else
	/**
	 * @todo implement?
	 */
#endif
}

int VistaConnectionPipe::Receive (void *buffer, const int length, int iTimeout  )
{
	if( !GetIsOpen() )
		return -1;

#if !defined(WIN32)
	int read_bytes = read( m_fd[PIPE_R], buffer, length );
	if( read_bytes != length )
	{
		/* if unable to read requested bytes but not at EOF -> return error */
		return -1 ;
	}
	return read_bytes;
#else
	/**
	 * @todo implement
	 */
#ifdef DEBUG
	vipcerr << "[VistaConnectionPipe::Receive] - NOT IMPLEMENTED for this platform!" << std::endl;
#endif
	return 0;
#endif
}

int VistaConnectionPipe::Send    (const void *buffer, const int length)
{
	if( !GetIsOpen() )
		return -1;

#if !defined(WIN32)
	int sent_bytes = write( m_fd[PIPE_W], buffer, length );
	if( sent_bytes != length )
	{
		/* if unable to write all data out -> error */
		return -1 ;
	}
	return sent_bytes ;
#else
	/**
	 * @todo implement
	 */
#ifdef DEBUG
	vipcerr << "[VistaConnectionPipe::Send] - NOT IMPLEMENTED for this platform!" << std::endl;
#endif
	return 0;
#endif
}

bool VistaConnectionPipe::HasPendingData() const
{
#if !defined(WIN32)
	struct pollfd fds;
	int timeout_msecs = 0;
	int ret;

	fds.fd = m_fd[PIPE_R];
	fds.events = POLLIN;

	ret = poll(&fds, 1, timeout_msecs);

	return (fds.revents & POLLIN);
#else
	/**
	 * @todo implement
	 */
#ifdef DEBUG
	vipcerr << "[VistaConnectionPipe::HasPendingData] - NOT IMPLEMENTED for this platform!" << std::endl;
#endif
	return false;
#endif
}

unsigned long VistaConnectionPipe::PendingDataSize() const
{
#ifdef LINUX
	int result = 0;
	if ( ioctl ( int(m_fd[PIPE_R]), FIONREAD, & result ) != -1 )
		return result;
	else
		return 0;
#elif defined(SUNOS)
	int result = 0;
	if ( ioctl ( int(m_fd[PIPE_R]), I_NREAD, & result ) > 0 )
		return result;
	else
		return 0;
#else
#ifdef DEBUG
	vipcerr << "[VistaConnectionPipe::HasPendingData] - NOT IMPLEMENTED for this platform!" << std::endl;
#endif
	return 0;
#endif
}

HANDLE VistaConnectionPipe::GetConnectionDescriptor() const
{
#if !defined(WIN32)
	return m_fd[PIPE_W];
#else
	/**
	 * @todo is this valid?
	 */
	return HANDLE(~0);
#endif
}

HANDLE VistaConnectionPipe::GetConnectionWaitForDescriptor()
{
#if !defined(WIN32)
	return m_fd[PIPE_R];
#else
	/**
	 * @todo is this valid?
	 */
	return HANDLE(~0);
#endif
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


