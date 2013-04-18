/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#if defined(VISTA_THREADING_SPROC)
#include "VistaSPROCThreadImp.h"
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>


#include <iostream>
using namespace std;


#include <signal.h>
#include <cerrno>
#include <unistd.h>
#include <signal.h>
#include <ulocks.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <cstdio>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class sproc_hlp
{
public:
	sproc_hlp();
	~sproc_hlp();
	const VistaThread *m_pThread;
	VistaSPROCThreadImp *m_pSPROCThread;
	VistaThreadEvent *m_pEvent;
};


sproc_hlp::sproc_hlp()
{
	m_pThread = NULL;
	m_pSPROCThread = NULL;
	m_pEvent = NULL;
}

sproc_hlp::~sproc_hlp()
{
}


static void SPROCEntryPoint ( void * that )
{
	const sproc_hlp *pHlp = reinterpret_cast<const sproc_hlp *>(that);
	VistaThread *pThread = const_cast<VistaThread *>(pHlp->m_pThread);

	pThread->PreRun();
	pThread->ThreadBody ();
	pHlp->m_pEvent->SignalEvent(); // indicate that we have left this function (thread is going to exit)
	pThread->PostRun(); // thrash PID
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSPROCThreadImp::VistaSPROCThreadImp(const VistaThread &thread)
: m_rThread(thread)
{
	m_bIsRunning = false;
	m_bCanBeCancelled = false;
	irixPID = -1;
	m_pFinalizeEvent = new VistaThreadEvent(true);
	m_pHlp = new sproc_hlp;
	m_pHlp->m_pThread = &m_rThread;
	m_pHlp->m_pSPROCThread = this;
	m_pHlp->m_pEvent = m_pFinalizeEvent;
}

VistaSPROCThreadImp::~VistaSPROCThreadImp()
{
	delete m_pHlp;
	delete m_pFinalizeEvent;
}

/// start a process
bool     VistaSPROCThreadImp::Run( )
{
	// just to make sure
	if ( irixPID != -1 )
		return false;

	// create a new lightweight process that shares our address space

	irixPID = sproc ( SPROCEntryPoint, PR_SADDR, m_pHlp );

	// return if no error occured
	if ( irixPID != -1 )
	{
		m_bIsRunning = true;
		return true;
	}

	vstr::errp() << "VistaThread[IRIX]::Run() ";

	switch ( errno )
	{
		case ENOMEM:
			vstr::err() << "If there is not enough virtual space to allocate a new "
				 << "stack.  The default stack size is settable via prctl(2), "
				 << "or setrlimit(2)." << std::endl;
			break;
		case EAGAIN:
			vstr::err() << "The system-imposed limit on the total number of processes "
				 << "under execution, {NPROC} [see intro(2)], would be "
				 << "exceeded.\n\n or \n\n";
			vstr::err() << "VistaThread: The system-imposed limit on the total number of processes "
				 << "under execution by a single user {CHILD_MAX} [see "
				 << "intro(2)], would be exceeded.\n\n or \n\n";
			vstr::err() << "VistaThread: Amount of system memory required is temporarily "
				 << "unavailable." << std::endl;
			break;
		case EINVAL:
			vstr::err() << "VistaThread: sp was null and len was less than 8192." << std::endl;
			break;
		case ENOSPC:

			vstr::err() << "VistaThread: If the size of the share group exceeds the number of users "
				 << "specified via usconfig(3P) (8 by default). Any changes "
				 << "via usconfig(3P) must be done BEFORE the first sproc is "
				 << "performed." << std::endl;
			break;
		case ENOLCK:
			vstr::err() << "VistaThread: There are not enough file locks in the system." << std::endl;
			break;
		case EACCES:
			vstr::err() << "VistaThread: The shared arena file (located in /usr/tmp) used in "
				 << "conjunction with the C library could not be opened or "
				<< "created for read/write." << std::endl;
			break;
		case EPERM:
			vstr::err() << "VistaThread: No permission to create a thread. This should only "
				 << "happen when you try to mix a pthread thread model with an sproc "
				 << "thread model. Make sure you do only use sproc-linked libs to build "
				 << "your program." << std::endl;
			break;
		default:
			vstr::err() << "VistaThread: Unkown Error (" << errno << ")" << std::endl;
			break;
	}

	return false; // error here
}


bool     VistaSPROCThreadImp::Suspend()
{
	return ( kill ( irixPID, SIGSTOP ) == 0 );
}


bool     VistaSPROCThreadImp::Resume()
{
	return ( kill ( irixPID, SIGCONT ) == 0 );
}


bool     VistaSPROCThreadImp::Join()
{
	int status;

	// do we have to wait?
	if ( irixPID == -1 )
		return false;

	// wait for process to be finished, killed, etc.
	status = m_pFinalizeEvent->WaitForEvent(true); //wait(NULL); //waitpid ( irixPID, NULL, 0 ) ;
	bool yeah = ( status == irixPID );

	//printf("this=%x, yeah=%d, waiting for pid=%d, status=%d\n", this, (int)yeah, irixPID, status);

	if(status == -1)
	{
		printf("Error:");
		switch(errno)
		{
		case EINTR:
			printf("EINTR");
			break;
		case ECHILD:
			printf("ECHILD");
			break;
		case EFAULT:
			printf("EFAULT");
			break;
		case EINVAL:
			printf("EINVAL");
			break;
		default:
			printf("no error.\n");
		}
		printf("\n");
	}
	return yeah;
}

bool     VistaSPROCThreadImp::Abort()
{
	bool yeah = ( kill ( irixPID, SIGTERM ) == 0 );
	if(yeah)
		irixPID = -1; // ok, thread terminated
	return yeah;
}

bool     VistaSPROCThreadImp::SetPriority( const VistaPriority &prio )
{
	return (setpriority(PRIO_PROCESS, irixPID, (short)prio.GetSystemPriority()) != -1); 
}

void VistaSPROCThreadImp::GetPriority( VistaPriority &prio ) const
{
	prio.SetVistaPriority(prio.GetVistaPriorityForSystemPriority(getpriority(PRIO_PROCESS, irixPID)));
}

/**
 * give the processor away temporarily
 */
void VistaSPROCThreadImp::YieldThread   ()
{
	sginap ( 0 );
}

void VistaSPROCThreadImp::SetCancelAbility(const bool bOkToCancel)
{
}

bool VistaSPROCThreadImp::CanBeCancelled() const
{
	return m_bCanBeCancelled;
}

void VistaSPROCThreadImp::PreRun()
{
}

void VistaSPROCThreadImp::PostRun()
{
	irixPID = -1;
}

long VistaSPROCThreadImp::GetThreadIdentity() const
{
	return (long)irixPID;
}

long VistaSPROCThreadImp::GetCallingThreadIdentity() const
{
	VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaSPROCThreadImp::SetCallingThreadPriority( const VistaPriority& oPrio )
{
	VISTA_THROW_NOT_IMPLEMENTED;
}

bool VistaSPROCThreadImp::GetCallingThreadPriority( VistaPriority& oPrio )
{
	VISTA_THROW_NOT_IMPLEMENTED;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif


