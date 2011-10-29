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
// $Id: VistaPthreadReaderWriterLockImp.h 20730 2011-03-30 15:56:24Z dr165799 $

#if defined(VISTA_THREADING_POSIX) && defined(_USE_PTHREAD_RWLOCK)

#ifndef _VISTAPTHREADREADERWRITERLOCKIMP_H
#define _VISTAPTHREADREADERWRITERLOCKIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaReaderWriterLockImp.h"
#include <pthread.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaPthreadReaderWriterLockImp : public IVistaReaderWriterLockImp
{
public:
	VistaPthreadReaderWriterLockImp();
	virtual ~VistaPthreadReaderWriterLockImp();


	bool ReaderLock();
	bool WriterLock();
	bool ReaderUnlock();
	bool WriterUnlock();

protected:
private:
	pthread_rwlock_t m_rwLock;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPTHREADREADERWRITERLOCKIMP_H

#endif // VISTA_THREADINGPOSIX ...

