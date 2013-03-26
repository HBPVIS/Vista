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

#include "VistaDfnViewerSinkNode.h" 
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaObserver.h>
#include <VistaAspects/VistaPropertyAwareable.h>

#include <VistaDataFlowNet/VdfnUtil.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaDfnViewerSinkNode::DispObserver : public IVistaObserver
{
public:
	DispObserver(VistaDfnViewerSinkNode *pParent)
		: IVistaObserver(),
		  m_pParent(pParent)
	{}

	~DispObserver() 
	{
		if(Observes(m_pObs))
			ReleaseObserveable(m_pObs);
	}


	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, 
										  int nTicket = IVistaObserveable::TICKET_NONE)
	{
		return true;
	}

	virtual void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE)
	{
		ReleaseObserveable(pObserveable, nTicket);
	}

	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE)
	{
		pObserveable->DetachObserver(this);
		m_pParent->SetDisplaySystem(NULL);
	}

	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
	{
	}

	virtual bool Observes(IVistaObserveable *pObserveable)
	{
		return (m_pObs == pObserveable);
	}

	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE)
	{
		if(m_pObs)
			m_pObs->DetachObserver(this);

		m_pObs = pObservable;

		if(m_pObs)
			m_pObs->AttachObserver(this, eTicket);
	}

private:
	VistaDfnViewerSinkNode *m_pParent;
	IVistaObserveable *m_pObs;

};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnViewerSinkNode::VistaDfnViewerSinkNode( VistaDisplaySystem *pSys )
										  : IVdfnNode(),
											m_pPosition(NULL),
											m_pOrientation(NULL),
											m_pLeftEye(NULL),
											m_pRightEye(NULL),
											m_pDispObs(NULL),
											m_pSystem(pSys)
{
	RegisterInPortPrototype( "position", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
	RegisterInPortPrototype( "orientation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> >);
	RegisterInPortPrototype( "left_eye", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
	RegisterInPortPrototype( "right_eye", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
}


VistaDfnViewerSinkNode::~VistaDfnViewerSinkNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaDisplaySystem *VistaDfnViewerSinkNode::GetDisplaySystem() const
{
	return m_pSystem;
}

void VistaDfnViewerSinkNode::SetDisplaySystem(VistaDisplaySystem *pSys)
{
	m_pSystem = pSys;
}

bool VistaDfnViewerSinkNode::GetIsValid() const 
{ 
	return (m_pSystem && ( m_pPosition || m_pOrientation )); // others are optional
}

bool VistaDfnViewerSinkNode::PrepareEvaluationRun()
{
	m_pPosition = VdfnUtil::GetInPortTyped<TVdfnPort<VistaVector3D>*>( "position", this );
	m_pOrientation = VdfnUtil::GetInPortTyped<TVdfnPort<VistaQuaternion>*>( "orientation", this );
	m_pLeftEye = VdfnUtil::GetInPortTyped<TVdfnPort<VistaVector3D>*>( "left_eye", this );
	m_pRightEye = VdfnUtil::GetInPortTyped<TVdfnPort<VistaVector3D>*>( "right_eye", this );


	return GetIsValid();
}


bool   VistaDfnViewerSinkNode::DoEvalNode()
{
	if(m_pPosition)
		m_pSystem->GetDisplaySystemProperties()->SetViewerPosition( m_pPosition->GetValueConstRef() );
	if(m_pOrientation)
		m_pSystem->GetDisplaySystemProperties()->SetViewerOrientation( m_pOrientation->GetValueConstRef() );

	if(m_pLeftEye)
	{
		const VistaVector3D &vL = m_pLeftEye->GetValueConstRef();
		m_pSystem->GetDisplaySystemProperties()->SetLeftEyeOffset( vL[0], vL[1], vL[2] );
	}

	if(m_pRightEye)
	{
		const VistaVector3D &vR = m_pRightEye->GetValueConstRef();
		m_pSystem->GetDisplaySystemProperties()->SetRightEyeOffset(vR[0],vR[1],vR[2]);
	}

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


