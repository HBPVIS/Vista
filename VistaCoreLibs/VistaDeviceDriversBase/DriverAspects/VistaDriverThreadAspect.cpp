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

#include "VistaDriverThreadAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>

#include <cassert>

class VistaDriverPollThread : public VistaThreadLoop
{
public:
	VistaDriverPollThread(IVistaDeviceDriver *pDriver)
		: VistaThreadLoop(),
		  m_pPoller(NULL),
		  m_pDriver(pDriver)
	{
	}

	~VistaDriverPollThread()
	{
	}

	VistaDriverThreadAspect::IDriverUpdatePrepare *GetDriverUpdate() const
	{
		return m_pPoller;
	}

	void SetDriverUpdate(VistaDriverThreadAspect::IDriverUpdatePrepare *pPoller)
	{
		if(IsRunning())
		{
			PauseThread();
			m_pPoller = pPoller;
			UnpauseThread();
		}
		else
			m_pPoller = pPoller;
	}

protected:
	bool LoopBody()
	{
		if(m_pPoller)
			m_pPoller->PrePoll();

		m_pDriver->Update();

		if(m_pPoller)
			m_pPoller->PostPoll();

		return false; // do not yield
	}
private:
	VistaDriverThreadAspect::IDriverUpdatePrepare *m_pPoller;
	IVistaDeviceDriver *m_pDriver;

};

VistaDriverThreadAspect::IDriverUpdatePrepare::~IDriverUpdatePrepare()
{
}

VistaDriverThreadAspect::IDriverUpdatePrepare::IDriverUpdatePrepare()
{
}

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverThreadAspect::m_nAspectId  = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverThreadAspect::VistaDriverThreadAspect(IVistaDeviceDriver *pDriver)
: IVistaDeviceDriver::IVistaDeviceDriverAspect(),
  m_pThread( new VistaDriverPollThread(pDriver) ),
  m_nProcAffinity(-1)
{
	if(VistaDriverThreadAspect::GetAspectId() == -1) // unregistered
		VistaDriverThreadAspect::SetAspectId( 
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("THREAD"));

	SetId(VistaDriverThreadAspect::GetAspectId());
}

VistaDriverThreadAspect::~VistaDriverThreadAspect()
{
	if(m_pThread->IsRunning())
		m_pThread->StopGently(true); // this could deadlock?!

	delete m_pThread;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverThreadAspect::GetAspectId()
{
	return VistaDriverThreadAspect::m_nAspectId;
}

void VistaDriverThreadAspect::SetAspectId(int nId)
{
	assert(VistaDriverThreadAspect::m_nAspectId == -1);
	VistaDriverThreadAspect::m_nAspectId = nId;

}

VistaDriverThreadAspect::IDriverUpdatePrepare *VistaDriverThreadAspect::GetDriverUpdatePrepare() const
{
	return m_pThread->GetDriverUpdate();
}

void VistaDriverThreadAspect::SetDriverUpdatePrepare(IDriverUpdatePrepare *pUpdate)
{
	m_pThread->SetDriverUpdate(pUpdate);
}

bool VistaDriverThreadAspect::StopProcessing()
{
	return m_pThread->StopGently(true);
}

bool VistaDriverThreadAspect::StartProcessing()
{
	if(!m_pThread->IsRunning())
		return m_pThread->Run();
	return true; // started threads remain running
}

bool VistaDriverThreadAspect::PauseProcessing()
{
	if( m_pThread->IsRunning() )
		return m_pThread->PauseThread(true);
	return true;
}

bool VistaDriverThreadAspect::UnpauseProcessing()
{
	if( m_pThread->IsRunning() )
		return m_pThread->UnpauseThread();
	else
		return m_pThread->Run();
}

bool VistaDriverThreadAspect::GetIsProcessing() const
{
	return m_pThread->IsRunning() && !m_pThread->IsPausing();
}

void VistaDriverThreadAspect::GetPriority(VistaPriority &oPrio) const
{
	return m_pThread->GetPriority(oPrio);
}

void VistaDriverThreadAspect::SetPriority( const VistaPriority &oPrio )
{
	m_pThread->SetPriority(oPrio);
}

void VistaDriverThreadAspect::SetProcessorAffinity(int nProcessorNum)
{
	if(m_pThread->IsRunning())
		return;

	if(m_pThread->SetProcessorAffinity(nProcessorNum))
	{
		m_nProcAffinity = nProcessorNum;
	}
}

int VistaDriverThreadAspect::GetProcessorAffinity() const
{
	return m_nProcAffinity;
}
