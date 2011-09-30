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
// $Id: VistaPosixThreadEventImp.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#if !defined(VISTA_THREADING_WIN32)
// project includes

#include "VistaPosixThreadEventImp.h"

#if defined (LINUX) || defined (DARWIN)
	#include <sys/ioctl.h>

#elif defined(SUNOS) || defined(IRIX)
		#include <sys/types.h>
		#include <sys/filio.h>
#endif

# include <stdio.h>
# include <unistd.h>
# include <assert.h>
# define PIPE_R 0
# define PIPE_W 1

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
//using namespace std;

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaPosixThreadEventImp::VistaPosixThreadEventImp()
{
  int res = pipe(m_fd);
  if(!res)
  {
	  // HOUSTON, WE HAVE GOT A PROBLEM
  }
}


VistaPosixThreadEventImp::~VistaPosixThreadEventImp()
{
  close(m_fd[PIPE_R]);
  close(m_fd[PIPE_W]);
}


/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaPosixThreadEventImp::SignalEvent()
{
   VistaPosixThreadEventImp* pThis = this;
   
   int n=0;
   if(0<=ioctl(m_fd[PIPE_W],FIONREAD,(char*)&n) && n==0)
	   write(m_fd[PIPE_W], &pThis, sizeof(this));
}

long VistaPosixThreadEventImp::WaitForEvent(int iBlockTime)
{
  VistaPosixThreadEventImp *pThis;
 /* struct timeval delta;
	
  delta.tv_usec=iBlockTime%1000;
  delta.tv_sec=(int)((double)iBlockTime/1000.0);

	if(select(1, (fd_set*)&m_fd, NULL, NULL, &delta)==0)
		return -1; // nothing there.
*/
  // ok, read and block ;)
  read(m_fd[PIPE_R], (void*)&pThis, sizeof(pThis));
  assert(pThis==this);

  return 0;

}

long VistaPosixThreadEventImp::WaitForEvent(bool bBlock)
{
  VistaPosixThreadEventImp *pThis;
  if(!bBlock)
  {
/*	struct timeval delta;
	delta.tv_usec=0;
	delta.tv_sec=0;

	if(select(1, (fd_set*)&m_fd, NULL, NULL, &delta)==0)
		return -1; // nothing there.
*/
  }

  // ok, read and block ;)
  read(m_fd[PIPE_R], (void*)&pThis, sizeof(pThis));
  assert(pThis==this);
  return 0;
}

HANDLE VistaPosixThreadEventImp::GetEventSignalHandle() const
{
	return (HANDLE)m_fd[PIPE_W];
}

HANDLE VistaPosixThreadEventImp::GetEventWaitHandle() const
{
	return (HANDLE)m_fd[PIPE_R];
}

bool VistaPosixThreadEventImp::ResetThisEvent()
{
	// determine if there is a signal waiting
	int result = 0;

	// determine a snapshot of the _current_ state
	// even at the risk of loosing a signal
	if ( ! ioctl ( m_fd[PIPE_R], FIONREAD, & result ) )
	{
		// success
		VistaPosixThreadEventImp *pThis;
		
		// loop might be slower, but we avoid dynamic allocation
		// of an array of pointers to read all results at once
		// anyway... we expect to be only a few signals in the queue
		for(int i=0; i < result/sizeof(void*); ++i)
		{
			read(m_fd[PIPE_R], (void*)&pThis, sizeof(pThis));
		}
		return true;
	}

	return false;
}


#endif

// ============================================================================
// ============================================================================

