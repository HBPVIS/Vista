/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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


#if defined(VISTA_THREADING_POSIX)
// project includes

#include "VistaPthreadThreadEventImp.h"
#include <errno.h>

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

VistaPthreadThreadEventImp::VistaPthreadThreadEventImp()
: autoreset(true), state(0)
{
	pthread_mutex_init(&mtx, 0);
	pthread_cond_init(&cond, 0);
}


VistaPthreadThreadEventImp::~VistaPthreadThreadEventImp()
{
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mtx);
}


/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaPthreadThreadEventImp::SignalEvent()
{
	pthread_mutex_lock(&mtx);
	state = 1;
	if (autoreset)
		pthread_cond_signal(&cond);
	else
		pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mtx);
}

long VistaPthreadThreadEventImp::WaitForEvent(int msecs)
{
	pthread_mutex_lock(&mtx);
	if (state == 0)
	{
		struct timespec tv;
		tv.tv_sec  = (msecs != 0) ? (msecs / 1000000) : 0;
		tv.tv_nsec = (msecs != 0) ? (msecs % 1000000) : 0;

		if(pthread_cond_timedwait(&cond, &mtx, &tv) == ETIMEDOUT)
			return 0;
	}
	if (autoreset)
		state = 0;
	pthread_mutex_unlock(&mtx);
  return 1;

}

long VistaPthreadThreadEventImp::WaitForEvent(bool bBlock)
{
	pthread_mutex_lock(&mtx);
	if (state == 0)
		pthread_cond_wait(&cond, &mtx);
	if (autoreset)
		state = 0;
	pthread_mutex_unlock(&mtx);
  return 1;
}

HANDLE VistaPthreadThreadEventImp::GetEventSignalHandle() const
{
	// to be honest... we should throw here
	return (HANDLE)0xFFFFFFFF;
}

HANDLE VistaPthreadThreadEventImp::GetEventWaitHandle() const
{
	// to be honest... we should throw here
	return (HANDLE)0xFFFFFFFF;
}

bool VistaPthreadThreadEventImp::ResetThisEvent()
{
	pthread_mutex_lock(&mtx);
	state = 0;
	pthread_mutex_unlock(&mtx);
	return true;
}

#endif

// ============================================================================
// ============================================================================

