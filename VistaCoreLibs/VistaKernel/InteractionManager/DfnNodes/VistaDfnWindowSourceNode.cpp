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

#include "VistaDfnWindowSourceNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/EventManager/VistaDisplayEvent.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaDfnWindowSourceNode::WindowObserver : public VistaEventHandler
{
public:
	WindowObserver(VistaEventManager *pMgr,
					VistaWindow *pWindow)
		: VistaEventHandler(),
		  m_pMgr(pMgr),
		  m_bRatioChange(true),
		  m_pWindow(pWindow)
	{
		m_pMgr->AddEventHandler(this, VistaDisplayEvent::GetTypeId(), VistaDisplayEvent::VDE_RATIOCHANGE);
	}

	~WindowObserver()
	{
		m_pMgr->RemEventHandler(this, VistaDisplayEvent::GetTypeId(), VistaDisplayEvent::VDE_RATIOCHANGE);
	}

	void HandleEvent(VistaEvent *pEvent)
	{
		VistaDisplayEvent *pDispEvent = static_cast<VistaDisplayEvent*>(pEvent);
		if(pDispEvent->GetViewport()->GetWindow() == m_pWindow)
		{
			m_bRatioChange = true;
		}
	}

	bool GetRatioChange()
	{
		bool bRet = m_bRatioChange;
		m_bRatioChange = false;
		return bRet;
	}

private:
	VistaEventManager *m_pMgr;
	bool m_bRatioChange;
	VistaWindow *m_pWindow;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnWindowSourceNode::VistaDfnWindowSourceNode( VistaEventManager *pMgr,
										  VistaWindow *pWindow )
										  : IVdfnNode(),
											m_pX(new TVdfnPort<int>),
											m_pY(new TVdfnPort<int>),
											m_pW(new TVdfnPort<int>),
											m_pH(new TVdfnPort<int>),
											m_pWindow(pWindow),
											m_pObs(new WindowObserver(pMgr, pWindow)),
											m_nUpdateCount(0)
{
	RegisterOutPort( "win_x", m_pX );
	RegisterOutPort( "win_y", m_pY );
	RegisterOutPort( "win_w", m_pW );
	RegisterOutPort( "win_h", m_pH );
}


VistaDfnWindowSourceNode::~VistaDfnWindowSourceNode()
{
	delete m_pObs;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDfnWindowSourceNode::GetIsValid() const
{
	return (m_pWindow != NULL);
}


bool   VistaDfnWindowSourceNode::DoEvalNode()
{
	int nX, nY, nW, nH;
	m_pWindow->GetWindowProperties()->GetSize(nW, nH);
	m_pWindow->GetWindowProperties()->GetPosition(nX, nY);

	m_pX->SetValue( nX, GetUpdateTimeStamp() );
	m_pY->SetValue( nY, GetUpdateTimeStamp() );
	m_pW->SetValue( nW, GetUpdateTimeStamp() );
	m_pH->SetValue( nH, GetUpdateTimeStamp() );

	return true;
}

unsigned int    VistaDfnWindowSourceNode::CalcUpdateNeededScore() const
{
	if(m_pObs->GetRatioChange())
	{
		return ++m_nUpdateCount;
	}
	return m_nUpdateCount;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

