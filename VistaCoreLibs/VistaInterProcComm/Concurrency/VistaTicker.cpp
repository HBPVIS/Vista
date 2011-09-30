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
// $Id: VistaTicker.cpp 21315 2011-05-16 13:47:39Z dr165799 $

//#include "ITAStopwatch.h"

#include "VistaTicker.h" 
#include <VistaBase/VistaExceptionBase.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/VistaInterProcCommOut.h>

#include <algorithm>
#include <cassert>

#include <iostream>

//static ITAStopwatch watch;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VistaTicker::CTriggerContext::CTriggerContext(int nTimeout, bool bPulsing)
: m_nTimeout(nTimeout),
  m_nCurrent(nTimeout),
  m_bPulsing(bPulsing),
  m_pTrigger(new VistaThreadEvent(true)),
  m_bActive(true),
  m_bOwnEvent(true)
{
}

VistaTicker::CTriggerContext::CTriggerContext(int nTimeout, 
											   bool bPulsing,
											   VistaThreadEvent *pEvent)
: m_nTimeout(nTimeout),
  m_nCurrent(nTimeout),
  m_bPulsing(bPulsing),
  m_pTrigger(pEvent),
  m_bActive(true),
  m_bOwnEvent(false)
{
	assert(pEvent != NULL);
}

VistaTicker::CTriggerContext::~CTriggerContext()
{
	if(m_bOwnEvent)
		delete m_pTrigger;
}


HANDLE VistaTicker::CTriggerContext::GetEventWaitHandle() const
{
	return m_pTrigger->GetEventWaitHandle();
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

#if defined(WIN32)
int VistaTicker::m_nNumTimers = 0;

VistaTicker::VistaTicker(unsigned int nGranularity)
: m_nGranularity(nGranularity), 
   m_Ticker(NULL),
   m_bRunning(false),
   m_pAfterPulseFunctor(NULL),
   m_pFirst(NULL),
   m_nSizeCache(0)
{
	TIMECAPS caps;
	if(timeGetDevCaps(&caps, sizeof(caps)) == TIMERR_NOERROR)
	{
		if(m_nGranularity < caps.wPeriodMin || m_nGranularity > caps.wPeriodMax)
			VISTA_THROW("timeGetDevCaps error specified illegal value for granularity!\n", 0x00000000);
	}

	if(++m_nNumTimers > 16)
		VISTA_THROW("VistaTicker::CVistTicker[WIN32]: no more than 16 tickers allowed.\n", 0x00000000);
}

VistaTicker::~VistaTicker()
{
	StopTicker();
	m_Ticker = NULL;
	for(TRIGVEC::iterator it = m_vecTriggers.begin(); it != m_vecTriggers.end(); ++it)
		delete *it;

	delete m_pAfterPulseFunctor;

	--m_nNumTimers;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaTicker::StartTicker()
{
	m_Ticker = timeSetEvent(m_nGranularity, 5, &TickerCallback, (DWORD_PTR)this, 
							TIME_PERIODIC|TIME_CALLBACK_FUNCTION|TIME_KILL_SYNCHRONOUS);
	if(m_Ticker == NULL)
		VISTA_THROW("VistaTicker::StartTicker() -- could not start\n", 0x00000000);

	//watch.start();

	m_bRunning = true;
	return true;
}

bool VistaTicker::StopTicker()
{
	if(m_Ticker == NULL)
		return true;

	if( timeKillEvent(m_Ticker) == TIMERR_NOERROR )
	{
		m_Ticker = NULL;
		m_bRunning = false;
		return true;
	}
	return false;
}

//static int n=0;
void VistaTicker::TickerCallback(UINT uTimerID, 
							   UINT uMsg, 
							   DWORD_PTR dwUser, 
							   DWORD_PTR dw1, 
							   DWORD_PTR dw2)
{
	//++n;
	//if(n % 50 == 0)
	//{
	//	double dMean = watch.mean();
	//	std::cout << "n=" << n 
	//				 << ", mean = " << dMean
	//				 << " (= " << 1/dMean << " Hz)" << std::endl;
	//	watch.reset();
	//}

	//watch.start();

	VistaTicker *pT = (VistaTicker*)(dwUser);
	if(pT)
		pT->UpdateTriggerContexts();

	//watch.stop();

}


#else

/** @todo implement me for unix builds :) */
VistaTicker::VistaTicker(unsigned int nGranularity)
: m_nGranularity(nGranularity), 
   m_bRunning(false),
   m_pAfterPulseFunctor(NULL),
   m_pFirst(NULL),
   m_nSizeCache(0)
{
}

VistaTicker::~VistaTicker()
{
	StopTicker();

	for(TRIGVEC::iterator it = m_vecTriggers.begin(); it != m_vecTriggers.end(); ++it)
		delete *it;

	delete m_pAfterPulseFunctor;
}


bool VistaTicker::StartTicker()
{
	return false;
}

bool VistaTicker::StopTicker()
{
	return false;
}

#endif

void VistaTicker::UpdateTriggerContexts()
{
	if(!m_pFirst)
		return;

	CTriggerContext **p = m_pFirst;
	for(register TRIGVEC::size_type n = 0; n < m_nSizeCache; ++n, ++p)
	{
		//p = m_vecTriggers[n];
		if(!(*p)->m_bActive)
			continue;

		(*p)->m_nCurrent -= m_nGranularity;
		if((*p)->m_nCurrent <= 0)
		{
			(*p)->m_pTrigger->SignalEvent();
			if((*p)->m_bPulsing)
				(*p)->m_nCurrent = (*p)->m_nTimeout;
			else
				(*p)->m_bActive = false;
		}
	}

	if(m_pAfterPulseFunctor)
		(*m_pAfterPulseFunctor)();
}

bool VistaTicker::AddTrigger(CTriggerContext *pCtx)
{
	bool bStart = false;
	if(m_bRunning)
	{
		StopTicker();
		bStart = true;
	}

	m_vecTriggers.push_back(pCtx);

	m_nSizeCache = m_vecTriggers.size();

	m_pFirst = &m_vecTriggers[0];

	if(bStart)
		StartTicker();
	return true;
}

bool VistaTicker::RemTrigger(CTriggerContext *pCtx, bool bDelete)
{
	bool bStart = false;
	if(m_bRunning)
	{
		StopTicker();
		bStart = true;
	}

	TRIGVEC::iterator it = std::find(m_vecTriggers.begin(), m_vecTriggers.end(), pCtx);
	if(it == m_vecTriggers.end())
	{
		StartTicker();
		return false;
	}

	
	m_vecTriggers.erase(it);
	m_nSizeCache = m_vecTriggers.size();

	if(!m_vecTriggers.empty())
		m_pFirst = &m_vecTriggers[0];
	else
		m_pFirst = NULL;

	if(bStart)
		StartTicker();

	if(bDelete)
		delete pCtx;

	return true;
}


VistaTicker::AfterPulseFunctor *VistaTicker::GetAfterPulseFunctor() const
{
	return m_pAfterPulseFunctor;
}

void VistaTicker::SetAfterPulseFunctor( AfterPulseFunctor *pFnct )
{
	m_pAfterPulseFunctor = pFnct;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


