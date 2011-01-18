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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>


#if defined(VISTA_THREADING_POSIX)

#include "VistaPthreadsThreadImp.h"
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>

#include <signal.h>


#include <iostream>
using namespace std;


#include <cstdlib>
#include <cstring>
#include <cerrno>

/** @todo check this on other platforms than unix */
// this seems to be glibc (GNU) specific and available only in >= glibc-2.6
#include <utmpx.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static void * PosixEntryPoint ( void *that )
{

	VistaThread *pThread = reinterpret_cast<VistaThread *>(that);
	pThread->PreRun();
	pThread->ThreadBody ();
	pThread->PostRun();

	pthread_exit(NULL);
	return 0;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPthreadThreadImp::VistaPthreadThreadImp(const VistaThread &thread)
: m_rThread(thread)
{
	m_bCanBeCancelled = false;
	posixThreadID = 0;
	pthread_attr_init(&m_ptAttr); // default value init
}

VistaPthreadThreadImp::~VistaPthreadThreadImp()
{
#if defined(DEBUG)
	if(posixThreadID)
	  vipcout << "VistaPthreadThreadImp::~VistaPthreadThreadImp() -- deleting non join'ed thread.\n";
#endif
	pthread_attr_destroy(&m_ptAttr);
}

/// start a process
bool VistaPthreadThreadImp::Run( )
{
	// assure only one running thread per instance
	if (posixThreadID)
		return false;

	int error = pthread_create ( & posixThreadID, &m_ptAttr, PosixEntryPoint, (void *)(&m_rThread) );
	if (error == 0)
	{
	   return true;
	}

	switch ( error )
	{

		case EAGAIN:
			vipcerr << "The maximum number of threads has been created [see NOTES, "
				 << "and setrlimit()] or there is insufficient memory to create the thread.\n";
			break;
		case ENOMEM:
			vipcerr << "The system lacked the necessary resources to create another thread.\n";
			break;
		case EINVAL:
			vipcerr << "The value specified by attr is invalid.\n";
			break;
		case EPERM:
			vipcerr << "The caller does not have appropriate permission to set "

				 << "the required scheduling parameters or scheduling policy.\n";
			break;
		case ENOSYS:
			vipcerr << "A call to pthread_create() was made from an executable "
				 << "which is not linked against the POSIX threads library. "
				 << "This typically occurs when the argument \"-lpthread\" is "
				 << "accidentally omitted during compilation of the executable.\n";
			break;
		default:
			vipcerr << "Unkown Error (" << error << ").\n";
			break;
	}

	return false;
}


bool     VistaPthreadThreadImp::Suspend()
{
	if(posixThreadID)
		return ( pthread_kill ( posixThreadID, SIGSTOP ) == 0 );
	return true; // we do consider no thread to be suspended
}


bool     VistaPthreadThreadImp::Resume()
{
	if(posixThreadID)
		return ( pthread_kill ( posixThreadID, SIGCONT ) == 0 );
	return false; // no thread can not be sumed
}


bool     VistaPthreadThreadImp::Join()
{
	if (posixThreadID)
	{
		int yeah = pthread_join ( posixThreadID, NULL );
		if(yeah == 0)
		{
			posixThreadID = 0; // ok, clear handle, thread is done
		}
		else
		{
			vipcerr << "Fail on join @ thread ["
				<< m_rThread.GetThreadName() << "]\n";

			switch(yeah)
			{
			case EDEADLK:
				{
					vipcerr << "Deadlock detected, maybe join on meself?\n";
					break;
				}
			case EINVAL:
				{
					vipcerr << "This thread is not joinable, did you detach it?\n";
					break;
				}
			case ESRCH:
				{
					vipcerr << "No thread found by that id. Already terminated?.\n";
					posixThreadID = 0;
					return true;
				}
			default:
				break;
			}
		}
		return (yeah == 0-1);
	}
	return false;
}

bool     VistaPthreadThreadImp::Abort()
{
	return ( pthread_cancel ( posixThreadID ) == 0 );
}

bool     VistaPthreadThreadImp::SetPriority( const VistaPriority &inPrio )
{
	if(!posixThreadID)
		return false;

	struct sched_param sp;

	memset (& sp, 0, sizeof (struct sched_param));
		sp.sched_priority = inPrio.GetSystemPriority();
	return (pthread_setschedparam (posixThreadID, SCHED_RR, & sp) == 0);
}

void VistaPthreadThreadImp::GetPriority( VistaPriority &outPrio ) const
{
	if(!posixThreadID)
		return;

	int                 policy;
	struct sched_param  sp;

	memset (& sp, 0, sizeof (struct sched_param));

	if (pthread_getschedparam (posixThreadID, & policy, & sp) == 0)
	{
		// fehler
	}
	else
	{
		outPrio.SetVistaPriority(outPrio.GetVistaPriorityForSystemPriority(sp.sched_priority));
	}
}

/**
 * give the processor away temporarily
 */
void VistaPthreadThreadImp::YieldThread   ()
{
	sched_yield ();
}

void VistaPthreadThreadImp::SetCancelAbility(const bool bOkToCancel)
{
}

bool VistaPthreadThreadImp::CanBeCancelled() const
{
	return m_bCanBeCancelled;
}

void VistaPthreadThreadImp::PreRun()
{
}

void VistaPthreadThreadImp::PostRun()
{
	posixThreadID = 0;
}


bool VistaPthreadThreadImp::Equals(const IVistaThreadImp &oImp) const
{
	return (pthread_equal(posixThreadID,
			static_cast<const VistaPthreadThreadImp&>(oImp).posixThreadID)!= 0);
}

long VistaPthreadThreadImp::GetThreadIdentity() const
{
	/** @todo implement me */
	return (long)pthread_self();
}

bool VistaPthreadThreadImp::SetProcessorAffinity(int iProcessorNum)
{
#if defined(__USE_GNU)
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(iProcessorNum,&mask);

	if(posixThreadID == 0)
	{
		// thread not created yet, create a thread attr to indicate the
		// cpu-set for this thread
		if(pthread_attr_setaffinity_np( &m_ptAttr, sizeof(cpu_set_t), &mask ) != 0)
		{
			vipcerr << "could not set thread affinity on attribute.\n";
			return false;
		}
		else
			return true;
	}
	else
	{
		 if(pthread_setaffinity_np(posixThreadID, sizeof(cpu_set_t), &mask)==0)
			 return true;
		 else
			 return false;
	}
#else
	return false;
#endif
}

int  VistaPthreadThreadImp::GetCpu() const
{
#if __GLIBC_MINOR__ >= 6
	/** @todo check: is there a pthreads call for this? */
	return sched_getcpu(); // define me in subclass
#else
	return -1;
#endif
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif


