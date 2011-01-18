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

#include "VistaIPComm.h"
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>


#if !defined(HOST_NAME_MAX)
#define HOST_NAME_MAX 255
#endif

#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <errno.h>


#include <iostream>
using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static int iRefCount = 0;

#if defined WIN32
/**
 * This is a stupid hack... but then again... wsock is kind of a hack, too
 */
static VistaMutex mutex;
static VistaIPComm iDummy;

#endif

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaIPComm::VistaIPComm()
{
//    printf("VistaIPComm::VistaIPComm()\n");
	UseIPComm();
}

VistaIPComm::~VistaIPComm()
{
	//printf("VistaIPComm::~VistaIPComm()\n");
	CloseIPComm();
}

int VistaIPComm::UseIPComm()
{
#ifdef WIN32
	VistaMutexLock l(mutex); // we make this thread safe, even at the cost
							// of binding ipnet the to concurrency-modules

	if ( ! iRefCount++ )
	{
		WSADATA dummyWsaData; // not needed

		if ( WSAStartup ( MAKEWORD (1, 1), & dummyWsaData ) != 0 )
		{
			vipcerr << "VistaIPComm::WSAstart() couldn't initialise WinSock." << endl;
			--iRefCount;
		}
	}
#endif
	return iRefCount;
}

int VistaIPComm::CloseIPComm()
{
#if defined(WIN32)
	VistaMutexLock l(mutex);// we make this thread safe, even at the cost
							// of binding ipnet the to concurrency-modules
	if( ! --iRefCount )
	{
		WSACleanup();
	}
#endif
	return iRefCount;
}


string VistaIPComm::GetHostname()
{
	char buffer[HOST_NAME_MAX+1];
	if(gethostname(buffer, HOST_NAME_MAX+1) < 0)
		vipcerr << "VistaIPComm::GetHostname() -- ERROR! errno = " << errno << endl;
	return string(buffer);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


