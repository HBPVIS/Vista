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

#include "VistaThreadImp.h"


#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32ThreadImp.h"
#elif defined(VISTA_THREADING_POSIX)
#include "VistaPthreadsThreadImp.h"
#elif defined(VISTA_THREADING_SPROC)
#include "VistaSPROCThreadImp.h"
#else
#error "PROCESSIMPLEMENTATION UNSUPPORTED FOR THIS PLATFORM!"
#endif

#include <VistaInterProcComm/Concurrency/VistaThread.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// initialize with NULL
IVistaThreadImp::IVistaThreadImpFactory *
				 IVistaThreadImp::m_pSImpFactory = NULL;

IVistaThreadImp::IVistaThreadImpFactory::IVistaThreadImpFactory()
{
}

IVistaThreadImp::IVistaThreadImpFactory::~IVistaThreadImpFactory()
{
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaThreadImp::IVistaThreadImp()
{
}

IVistaThreadImp::~IVistaThreadImp()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVistaThreadImp *IVistaThreadImp::CreateThreadImp(const VistaThread &thread)
{
	if(m_pSImpFactory)
	{
		return m_pSImpFactory->CreateThread(thread);
	}
	else
	{
	// factory method
	#if defined(VISTA_THREADING_WIN32)
		return new VistaWin32ThreadImp(thread);
	#elif defined(VISTA_THREADING_POSIX)
		return new VistaPthreadThreadImp(thread);
	#elif defined(VISTA_THREADING_SPROC)
		return new VistaSPROCThreadImp(thread);
	#else
		return NULL;
	#endif
	}
}

void IVistaThreadImp::RegisterThreadImpFactory(IVistaThreadImpFactory *pFactory)
{
	if(m_pSImpFactory)
		vipcerr << "" << std::endl;

	m_pSImpFactory = pFactory;
}

IVistaThreadImp::IVistaThreadImpFactory *IVistaThreadImp::GetThreadImpFactory()
{
	return m_pSImpFactory;
}

bool IVistaThreadImp::Equals(const IVistaThreadImp &oImp) const
{
	return false; /** @todo define me in subclasses */
}

bool IVistaThreadImp::SetProcessorAffinity(int iProcessorNum)
{
	return false; /** @todo define me in subclasses */
}

int  IVistaThreadImp::GetCpu() const
{
	return -1; // define me in subclass
}

bool IVistaThreadImp::SetThreadName(const std::string &sName)
{
	DoSetThreadName(sName);
	return true;
}

std::string IVistaThreadImp::GetThreadName() const
{
	return m_sThreadName;
}

void IVistaThreadImp::DoSetThreadName(const std::string &sName)
{
	m_sThreadName = sName;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


