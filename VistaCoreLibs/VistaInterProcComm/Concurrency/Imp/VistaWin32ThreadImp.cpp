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


#if defined(VISTA_THREADING_WIN32)

#include <string>

#include "VistaWin32ThreadImp.h"
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>

//
// Usage: SetThreadName (-1, "MainThread");
//
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

static void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = szThreadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
#ifdef WIN32
	#if defined (_MSC_VER)
		#if ( _MSC_VER >= 1400 )
	  RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info );
		#else
	  RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
		#endif
	#endif
#endif
   }
   __except(EXCEPTION_CONTINUE_EXECUTION)
   {
   }
}

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static DWORD WINAPI Win32EntryPoint ( LPVOID that )
{
	VistaThread *pThread = reinterpret_cast<VistaThread*>(that);
	
	// set name
	std::string sName = pThread->GetThreadName();
	if(!sName.empty())
	{
		DWORD threadId = GetCurrentThreadId();
		::SetThreadName(threadId, sName.c_str());
	}

	pThread->PreRun();
	pThread->ThreadBody ();
	pThread->PostRun();

	return 0;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWin32ThreadImp::VistaWin32ThreadImp(const VistaThread &thread)
: m_rThread(thread)
{
	win32Handle = 0;
	threadId = 0;
	m_bIsRunning = false;
	m_bCanBeCancelled = false;
	m_dwAffinityMask = 1;
}

VistaWin32ThreadImp::~VistaWin32ThreadImp()
{
}

/// start a process
bool     VistaWin32ThreadImp::Run( )
{
	// just to make sure
	if(win32Handle>0)
		return false;

	// create a thread without special security settings
	win32Handle = CreateThread ( 0, 0, Win32EntryPoint, (void*)&m_rThread, 0, & threadId );
	m_bIsRunning = (win32Handle!=0);
	SetProcessorAffinity(m_dwAffinityMask);

	return ( m_bIsRunning );
}


bool     VistaWin32ThreadImp::Suspend()
{
	if(::SuspendThread ( win32Handle ) != -1)
		m_bIsRunning = false;

	return ( !m_bIsRunning );
}


bool     VistaWin32ThreadImp::Resume()
{
	if(::ResumeThread  ( win32Handle ) != -1)
		m_bIsRunning = true;
	return ( m_bIsRunning );
}


bool     VistaWin32ThreadImp::Join()
{
	if(WaitForSingleObject(win32Handle, INFINITE) == WAIT_OBJECT_0)
	{
		m_bIsRunning = false;
		CloseHandle(win32Handle);
		win32Handle  = 0;
		threadId = 0;
	}


	if(win32Handle)
		return false;

	return true;
}

bool     VistaWin32ThreadImp::Abort()
{
	DWORD wCode=0;
	bool bRet = (TerminateThread(win32Handle,wCode) ? true : false);
	if(bRet)
	{
		CloseHandle(win32Handle);
		win32Handle = 0;
		threadId = 0;
	}
	return bRet;
}

bool     VistaWin32ThreadImp::SetPriority( const VistaPriority & inPrio)
{
	return (SetThreadPriority(win32Handle, inPrio.GetSystemPriority()) == TRUE);
}

void VistaWin32ThreadImp::GetPriority( VistaPriority &outPrio ) const
{
	int prio = GetThreadPriority(win32Handle);
	outPrio.SetVistaPriority(outPrio.GetVistaPriorityForSystemPriority(prio));
}

/**
 * give the processor away temporarily
 */
void VistaWin32ThreadImp::YieldThread   ()
{
	Sleep(0);
}

void VistaWin32ThreadImp::SetCancelAbility(const bool bOkToCancel)
{
}

bool VistaWin32ThreadImp::CanBeCancelled() const
{
	return false;
}

void VistaWin32ThreadImp::PreRun()
{
}

void VistaWin32ThreadImp::PostRun()
{
	if(win32Handle)
		CloseHandle(win32Handle);
	win32Handle = 0;
	threadId = 0;
}

bool VistaWin32ThreadImp::Equals(const IVistaThreadImp &oImp) const
{
	if(!win32Handle)
		return false;

	return (win32Handle == static_cast<const VistaWin32ThreadImp &>(oImp).win32Handle);
}

bool VistaWin32ThreadImp::SetProcessorAffinity(int iProcessorNum)
{
	m_dwAffinityMask = (1 << iProcessorNum);
	if(win32Handle)
	{
		if(SetThreadAffinityMask(win32Handle, m_dwAffinityMask)==0)
			// failure!
		{
			return false;
		}

		return true;
	}
	else 
		return true; // setting the mask on a non-running thread is ok
}

bool VistaWin32ThreadImp::SetThreadName(const std::string &sName)
{
	// we can not change the name when the tread is running
	if(m_bIsRunning)
		return false;
	return IVistaThreadImp::SetThreadName(sName);
}

long VistaWin32ThreadImp::GetThreadIdentity() const
{
	return (long)win32Handle;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif


