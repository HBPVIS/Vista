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

#include "VistaIOHandleBasedMultiplexer.h" 
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Concurrency/VistaThreadCondition.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>


#include <cassert>
#include <cstdio>

#if !defined(WIN32)
#include <unistd.h>
#include <errno.h>
#else
#include <Windows.h>
#endif

#include <iostream>
#include <algorithm>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaIOHandleBasedIOMultiplexer::VistaIOHandleBasedIOMultiplexer()
: VistaIOMultiplexer(),
m_pActionMutex(new VistaMutex),
m_pDoneMutex(new VistaMutex),
m_pHandleMapMutex(new VistaMutex),
// we need POSIX semantics in order to push a fd to select on unix
// this has no effect on WIN32 builds, so we can safely pass true here
m_pMetaEvent(new VistaThreadEvent(true)),
m_pWaitingForAction(new VistaThreadCondition),
m_pActionDone(new VistaThreadCondition)
{
	AddPoint((*m_pMetaEvent).GetEventWaitHandle(), 0, MP_IOIN);
}

VistaIOHandleBasedIOMultiplexer::~VistaIOHandleBasedIOMultiplexer()
{
	/*	for(map<int, VistaThreadEvent*>::const_iterator cit = m_mpHandles.begin();
	cit != m_mpHandles.end(); ++cit)
	{
	// simply delete 'em all
	delete (*cit).second;
	}
	*/
	delete m_pActionDone;
	delete m_pWaitingForAction;
	delete m_pMetaEvent;
	delete m_pActionMutex;
	delete m_pDoneMutex;
	delete m_pHandleMapMutex;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaIOHandleBasedIOMultiplexer::Shutdown()
{
	VistaIOMultiplexer::Shutdown();
	(*m_pMetaEvent).SignalEvent();	
}

void VistaIOHandleBasedIOMultiplexer::Remedy()
{
	(*m_pMetaEvent).ResetThisEvent();
}

void VistaIOHandleBasedIOMultiplexer::PrintHandlemap() const
{
	for(HANDLEMAP::const_iterator cit = m_mpHandleMap.begin();
		cit != m_mpHandleMap.end(); ++cit)
	{
		vipcout << "[" << (*cit).first << "]: ("
			<< (*cit).second.first
			<< ";"
			<< (*cit).second.second
			<< ")\n";
	}
}


int VistaIOHandleBasedIOMultiplexer::Demultiplex(unsigned int nTimeout)
{
	// cout << "VistaIOHandleBasedIOMultiplexer::Demultiplex()\n";
	
	bool bDone = false;
	while(!bDone)
	{
		// first of all, prepare the handle-set
		//printf ("Demultiplex @ %x - size= %d . Entering multiplexer again.\n", this, m_veHandles.size());
		SetState(MP_PLEXING);
		(*m_pDoneMutex).Lock();
		bool bFail = false;
#ifdef WIN32	
		DWORD nRet = 0;
		DWORD dwTimeout = (nTimeout == ~0) ? INFINITE : DWORD(nTimeout);

		nRet = WaitForMultipleObjects(DWORD(m_veHandles.size()), &m_veHandles[0], FALSE, dwTimeout);
		bFail = (nRet == WAIT_FAILED ? true : false);

		if(nRet == WAIT_TIMEOUT)
		{
			SetState(MP_IDLE);
			(*m_pDoneMutex).Unlock();
			return 0;
		}
#else
		fd_set myRdSocks;
		fd_set myExSocks;
		fd_set myWrSocks;
		FD_ZERO(&myRdSocks);
		FD_ZERO(&myExSocks);
		FD_ZERO(&myWrSocks);

		// default size of fd-set is claimed to be 256, this
		// should be enough here
		HANDLE hanMax = 0;
		for(HANDLEMAP::const_iterator cit = m_mpHandleMap.begin();
			cit != m_mpHandleMap.end(); ++cit)
		{			
			if((*cit).second.second & MP_IOIN)
			{
				FD_SET((*cit).first, &myRdSocks);
			}
			if((*cit).second.second & MP_IOOUT)
			{
				FD_SET((*cit).first, &myWrSocks);
			}
			if((*cit).second.second & MP_IOERR)
			{
				FD_SET((*cit).first, &myExSocks);
			}

			hanMax = hanMax < (*cit).first ? hanMax = (*cit).first : hanMax = hanMax;
		}

		// printf("VistaIOMultiplexerIP::Demultiplex() -- before select, hanMax = %d\n", hanMax);
		struct timeval tv;
		struct timeval *etv = &tv;
		if(nTimeout != ~0)
		{
			tv.tv_sec  = nTimeout/1000;
			tv.tv_usec = (nTimeout%1000)*1000;
		}
		else
		{
			//tv.tv_sec = 0;
			//tv.tv_usec= 0;
			etv = NULL;
		}

		int iRet = 0;
		int iReattemptCount = 0;
		do
 		{
			iRet = select(hanMax+1, &myRdSocks, &myWrSocks, &myExSocks, etv);
		} while( iRet == -1 && errno == EINTR && ++iReattemptCount < 1000 );

		bFail = (iRet == -1 ? true : false);
		if(iRet == 0)
		{
			SetState(MP_IDLE);
			(*m_pDoneMutex).Unlock();
			return 0;
		}
		else if( iRet == -1 )
		{
			perror( "select" );
			vipcerr << "Call to select failed: ";
			switch( errno )
			{
				case EBADF:
					vipcerr << "Invalid File Descriptor occured" << std::endl;
					break;
				case EINVAL:
					vipcerr << "Invalid file number or timeout" << std::endl;
					break;
				case EINTR:
					vipcerr << "System interrupt call" << std::endl;
					break;
				case ENOMEM:
					vipcerr << "Could not allocate internatl memory" << std::endl;
					break;
				default:
					vipcerr << "Unknown error" << std::endl;
					break;
			}			
		}
		//		printf ("VistaIOMultiplexerIP::Demultiplex() -- nRet = %d \n", iRet);
		//		perror("VistaIOMultiplexerIP::Demultiplex() -- ");
#endif
		SetState(MP_DISPATCHING);
		(*m_pDoneMutex).Unlock();

		if(bFail)
		{
			// failed
			vipcerr <<  "[VistaIOHandleBasedIOMultiplexer] WAIT FAILED , leave loop" << std::endl;			
			bDone = true; // leave loop
			PrintHandlemap();
		}
		else
		{
			switch(m_eCom)
			{
			case MPC_SHUTDOWN:
				{
					// leave loop
					vipcout <<  ("[VistaIOHandleBasedIOMultiplexer] state SHUTDOWN , leave loop\n");
					bDone = true;
					break;
				}
			case MPC_ADDHANDLE:
			case MPC_REMHANDLE:
				{
					// ok, we got signaled by the control event
					// indicate that we are waiting
					(*m_pActionMutex).Lock();
					vipcout << "[VistaIOHandleBasedIOMultiplexer] Got COMMAND " <<  m_eCom << endl;
					// wait for action done
					(*m_pActionMutex).Unlock();
					break;
				}
			case MPC_NONE:
				{
					// we have to return the index here
					//printf("MPC_NONE--- checking handles\n");
					SetState(MP_IDLE);
#ifdef WIN32
					unsigned int iIndex = nRet - WAIT_OBJECT_0;
					assert(iIndex <= m_veHandles.size());
					return GetTicketForHandle(m_veHandles[iIndex]);						
#else
					// ##########################
					// RESOLVING HANDLES FOR UNIX
					// ##########################
					// search first fitting handle
					for(HANDLEMAP::const_iterator cit = m_mpHandleMap.begin();
						cit != m_mpHandleMap.end(); ++cit)
					{
						// determine read direction
						if((*cit).second.second & MP_IOIN)
						{
							// could be read
							if(FD_ISSET((*cit).first, &myRdSocks))
							{
								// touch on handle!
								return GetTicketForHandle((*cit).first);
							}
						}
						if((*cit).second.second & MP_IOOUT)
						{
							if(FD_ISSET((*cit).first, &myWrSocks))
							{
								return GetTicketForHandle((*cit).first);	
							}

						}

						if((*cit).second.second & MP_IOERR)
						{
							if(FD_ISSET((*cit).first, &myExSocks))
							{
								return GetTicketForHandle((*cit).first);	
							}
						}

					}

					// failed
					vipcerr << "Could not find handle for fd ["
						<< iRet << "]\n";
					return -1;
#endif
				}
			default:
				printf ("[MultiplexIP] Unknown state, leave loop");
				bDone = true; // leave loop
				break;
			}
			// reset command
			m_eCom = MPC_NONE;
		}
	}
	SetState(MP_NONE);
	//printf ("VistaIOMultiplexerIP::Demultiplex() -- LEAVING\n");
	(*m_pShutdown).SignalCondition();
	return -1;
}


bool VistaIOHandleBasedIOMultiplexer::AddMultiplexPoint(HANDLE han, int iTicket, eIODir eDir)
{
	VistaMutexLock l (*m_pHandleMapMutex);
	assert(iTicket > 1);
	bool bRet = false;
	if(GetState() == MP_PLEXING)
	{
		// ok, we are serving right now, signal multiplexer
		SetCommand(MPC_ADDHANDLE);
		(*m_pActionMutex).Lock();
		(*m_pMetaEvent).SignalEvent();
		(*m_pDoneMutex).Lock();
		// ok, IO-multiplexer is waiting for a signal on action-done
		// lets insert the handle
		vector<HANDLE>::iterator it = find(m_veHandles.begin(), m_veHandles.end(), han);
		if(it != m_veHandles.end())
		{
			// we already have a ticket for this!
		}
		else
		{
			bRet=AddPoint(han, iTicket, eDir);

		}

		(*m_pDoneMutex).Unlock();
		(*m_pActionMutex).Unlock();
	}
	else if(GetState() == MP_IDLE || GetState() == MP_NONE)
	{
		bRet = AddPoint(han, iTicket, eDir);
	}
	return bRet;
}


bool VistaIOHandleBasedIOMultiplexer::RemMultiplexPoint(HANDLE han, eIODir eDir)
{
	VistaMutexLock l (*m_pHandleMapMutex);
	bool bRet = false;
	if(GetState() == MP_PLEXING)
	{
		SetCommand(MPC_REMHANDLE);
		(*m_pActionMutex).Lock();
		(*m_pMetaEvent).SignalEvent();
		(*m_pDoneMutex).Lock();

#if 0
		printf ("VistaIOMultiplexerIP::RemMultiplexPoint() -- SIGNALLED\n");
#endif
		// ok, we can now modify the vector safely
		bRet = RemPoint(han, -1, eDir);
		(*m_pDoneMutex).Unlock();
		(*m_pActionMutex).Unlock();
	}
	else
	{
#if 0
		printf ("VistaIOMultiplexerIP::RemMultiplexPoint()\n");
#endif
		bRet = RemPoint(han, -1, eDir);
	}

	return bRet;
}

bool VistaIOHandleBasedIOMultiplexer::RemMultiplexPointByTicket(int iTicket)
{
	VistaMutexLock l (*m_pHandleMapMutex);
	TICKETMAP::iterator it = m_mpTicketMap.find(iTicket);
	if(it != m_mpTicketMap.end())
	{
		// found
		return RemPoint((*it).second, iTicket, MP_IOALL);
	}
	return false;
}


int  VistaIOHandleBasedIOMultiplexer::GetTicketForHandle(HANDLE han) const
{
	HANDLEMAP::const_iterator cit = m_mpHandleMap.find(han);
	if(cit == m_mpHandleMap.end())
	{
		//std::cerr << "Demultiplexer: handle " <<  han << " not found!\n";
		return -1;
	}

	//cout << "Demultiplexer: returning handle ["
	//        << (*cit).second.first << "]\n";

	return (*cit).second.first; 
}


bool VistaIOHandleBasedIOMultiplexer::AddPoint(HANDLE han, int iTicket, eIODir eDir)
{
#ifdef WIN32
	if(m_veHandles.size() + 1 > MAXIMUM_WAIT_OBJECTS)
	{
		vipcerr << "[VistaIOHandleBasedIOMultiplexer::AddPoint()]: WIN32 limit of ["
			<< MAXIMUM_WAIT_OBJECTS << "] reached. Not adding this point!\n";
		return false;
	}
#endif
	vector<HANDLE>::iterator it = find(m_veHandles.begin(), m_veHandles.end(), han);
	if(it == m_veHandles.end())
	{
		m_veHandles.push_back(han);
		m_mpTicketMap[iTicket] = han;
		m_mpHandleMap[han] = pair<int, eIODir>(iTicket, eDir);
		//cout << "AddPoint(" << han<< "," <<  iTicket << "," << eDir << ")\n";
		return true;
	}
	else
		return false;
}


bool VistaIOHandleBasedIOMultiplexer::RemPoint(HANDLE han, int iTicket, eIODir eDir)
{
#if 0
	vipcout << "VistaIOHandleBasedIOMultiplexer::RemPoint() -- \n";
#endif
	// remove handle from:
	//  - m_veHandles
	//  - m_TicketMap
	//  - m_mpHandles
	vector<HANDLE>::iterator it = find(m_veHandles.begin(), m_veHandles.end(), han);
	if(it == m_veHandles.end())
	{
		vipcerr << "bogus... do not find handle in handle vector upon remove?\n";
		return false;
	}

	HANDLEMAP::iterator it1 = m_mpHandleMap.find(han);
	if(it1 == m_mpHandleMap.end())
	{
		vipcerr << "bogus... do not find handle in handle map upon remove?\n";
		return false;
	}

	TICKETMAP::iterator ti = m_mpTicketMap.find((*it1).second.first);
	if( ti == m_mpTicketMap.end())
	{
		vipcerr << "bogus... do not find ticket in ticket map upon remove?\n";
		return false;
	}

	// remove from m_veHandles
	m_veHandles.erase(it); 

	// remove from ticketmap
	m_mpTicketMap.erase(ti);

	// remove from m_mpHandleMap
	m_mpHandleMap.erase(it1);

#if 0
	vipcout << "REMOVE SUCCESS\n";
#endif
	return true;	
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

