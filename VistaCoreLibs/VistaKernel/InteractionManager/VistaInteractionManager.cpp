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

#include "VistaInteractionManager.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>


#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaConnectionUpdater.h>
#include "VistaInteractionEvent.h"
#include "VistaInteractionContext.h"

#include <VistaDeviceDriversBase/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDriverLoggingAspect.h>
#include <VistaDeviceDriversBase/VistaDriverForceFeedbackAspect.h>
#include <VistaDeviceDriversBase/VistaDriverThreadAspect.h>
#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnGraph.h>


#include "VistaDriverWindowAspect.h"


// SUPPORTED GENERIC DRIVERS
#include <VistaDeviceDriversBase/Drivers/VistaSpaceMouseDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaDTrackDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaHIDDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaMIDIDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaIRManDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaPhantomServerDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaFastrakDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaCyberGloveDriver.h>
#if 0
#include <VistaDeviceDriversBase/Drivers/Vista3DCSpaceNavigatorDriver.h>
#endif

#include <VistaDataFlowNet/VdfnPersistence.h>


#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaAspects/VistaObserver.h>
#include <VistaBase/VistaTimer.h>
#include <VistaKernel/VistaKernelOut.h>

#include <cstdio>
#include <algorithm>

class VistaUpdateObserver : public IVistaObserver
{
public:

	class MapObj2Msg
	{
	public:
		MapObj2Msg(VistaInteractionContext *pCtx, int nMsg)
			: m_pCtx(pCtx), m_nMsg(nMsg)
			{

			}

		bool operator==(const MapObj2Msg &oOther) const
			{
				if(oOther.m_nMsg >= 0)
					return ((m_pCtx == oOther.m_pCtx)
							&& m_nMsg == oOther.m_nMsg);
				else
					return (m_pCtx == oOther.m_pCtx);
			}

		bool operator<(const MapObj2Msg &oOther) const
			{
				return (m_nMsg < oOther.m_nMsg);
			}

		VistaInteractionContext *m_pCtx;
		int                       m_nMsg;
	};


	VistaUpdateObserver()
		: IVistaObserver(),
		  m_mpUpdates(VistaInteractionEvent::VEID_LAST)
		{

		}

	virtual ~VistaUpdateObserver()
		{
			for(std::vector<VistaInteractionContext*>::size_type z = 0;
				z < m_vecObsVec.size(); ++z)
			{
				m_vecObsVec[z]->DetachObserver(this);
			}
		}

	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, int)
		{
			return true;
		}

	virtual void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket)
		{
			ReleaseObserveable(pObserveable, nTicket);
		}

	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, int)
		{
			// d-cast to correct pointer type (hopefully)
			VistaInteractionContext *pObj = dynamic_cast<VistaInteractionContext*>(pObserveable);

			// find observeable in all event type lists
			for(std::vector< MapObj2Msg >::size_type n = 0;
				n < m_mpUpdates.size(); ++n)
			{
				std::vector<MapObj2Msg> &v = m_mpUpdates[n];

				std::vector<MapObj2Msg>::iterator it =
					std::remove(v.begin(), v.end(), MapObj2Msg(pObj,-1));

				if(it != v.end())
					v.erase(it, v.end());
			}

			// remove from observer list
			std::vector<VistaInteractionContext *>::iterator vit = std::remove(m_vecObsVec.begin(),
																				m_vecObsVec.end(), pObj);

			if(vit != m_vecObsVec.end())
				m_vecObsVec.erase(vit, m_vecObsVec.end());


			// finally, detach from the object itself
			pObserveable->DetachObserver(this);
		}

	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
		{
		}

	virtual bool Observes(IVistaObserveable *pObserveable)
		{
			VistaInteractionContext *pObj = dynamic_cast<VistaInteractionContext*>(pObserveable);
			if(!pObj)
				return false;

			const std::vector<VistaInteractionContext *>::iterator vit = std::remove(m_vecObsVec.begin(),
																					  m_vecObsVec.end(), pObj);

			return (vit != m_vecObsVec.end());
		}

	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE)
		{
			VistaInteractionContext *pCtx = dynamic_cast<VistaInteractionContext*>(pObservable);
			if(pCtx)
			{
				m_vecObsVec.push_back(pCtx);
				pCtx->AttachObserver(this);
			}
		}


	void ResetUpdateWatch()
		{
			for(std::vector< std::vector<VistaInteractionContext*> >::size_type n=0;
				n < m_mpUpdates.size(); ++n)
			{
				std::vector<MapObj2Msg> &v = m_mpUpdates[n];
				v.clear();
			}
		}

	std::vector< std::vector< MapObj2Msg > > m_mpUpdates;
	std::vector< VistaInteractionContext * > m_vecObsVec;
protected:
private:

};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class DriverUpdateCallback : public IVistaExplicitCallbackInterface
{
public:
	DriverUpdateCallback(IVistaDeviceDriver *pDriver)
		: m_pDriver(pDriver),
		  m_pPostUpdate(NULL)
		{
		}


	~DriverUpdateCallback()
		{
			delete m_pPostUpdate;
		}

	virtual bool Do()
		{
			m_pDriver->PreUpdate();
			bool bPostUpdate = m_pDriver->Update();
			m_pDriver->PostUpdate();

			if(bPostUpdate && m_pPostUpdate)
				m_pPostUpdate->PostDriverUpdate(m_pDriver);
			return true;
		}

	IVistaDeviceDriver *m_pDriver;
	VistaInteractionManager::IVistaPostUpdateFunctor *m_pPostUpdate;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaInteractionManager::VistaInteractionManager(VistaEventManager *pMgr,
														 bool bIsServerInMa)
  : VistaEventHandler(),
	m_pEvMgr(pMgr),
	m_mpDriverMap(NULL),
	m_pUpdater(new VistaConnectionUpdater),
	// needed in order to raise events in server mode,
	// useless, else
	m_pEvObserver(bIsServerInMa ? new VistaUpdateObserver : NULL),
	m_pInteractionEvent(NULL),
	m_bProcessingEvents(false),
	m_nRoleIdCount(0),
	m_bIsServerInMa(bIsServerInMa),
	m_bSwallowEvent(true),
	m_pAvgUpd( new VistaWeightedAverageTimer )
{
	vkernout << "VistaInteractionManager() -- CREATED AS "
			  << (m_bIsServerInMa ? "MASTER/STANDALONE" : "SLAVE")
			  << " InteractionManager\n";


	m_pInteractionEvent = new VistaInteractionEvent(this);
	m_pEvMgr->AddEventHandler(this, VistaSystemEvent::GetTypeId(),
							  VistaSystemEvent::VSE_UPDATE_INTERACTION,
							  VistaEventManager::PRIO_HIGH + 100);
	m_pEvMgr->AddEventHandler(this, VistaSystemEvent::GetTypeId(),
							  VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION,
							  VistaEventManager::PRIO_HIGH + 100);

	/** @todo deactivated, not well designed at the moment, but */
	// definitely needed! So think about some incorporation of
	// hot samplers before the current rendering

//	// register for postapplication as well, as we can update
//	// hotsampler, e.g. projection update, with a newer value!
//	// when we will try to do this as late as possible, possibly
//	// components have to react on spatial input more than once,
//	// but this seems to be ok
//	m_pEvMgr->AddEventHandler(this, VistaSystemEvent::GetTypeId(),
//		VistaSystemEvent::VSE_POSTAPPLICATIONLOOP, // we will run this on post-app, before pre-gfx
//													// cluster will sync on pre-gfx
//		VistaEventManager::PRIO_LOW); // latest in the chain


	// register for events on graphs and contexts
	// will trigger a call to ''EvalContext'' on the context signature
	m_pEvMgr->AddEventHandler(this,
							  VistaInteractionEvent::GetTypeId(),
							  VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE );


}

VistaInteractionManager::~VistaInteractionManager()
{
	delete m_pAvgUpd;
	// delete and stop updater first, otherwise
	// one of the drivers being deleted might be still in use
	delete m_pUpdater;

	delete m_pEvObserver;

	// remove all instances of me in a single call
	m_pEvMgr->RemEventHandler(this, VistaEventManager::NVET_ALL,
							  VistaEventManager::NVET_ALL);

	for(std::vector<DRIVER>::iterator it = m_vecInFrameUpdates.begin();
		it != m_vecInFrameUpdates.end(); ++it)
	{
		delete (*it).m_pPostUpdate;
	}

	for(std::vector<CONTEXT>::iterator cit = m_vecLogicalDevices.begin();
		cit != m_vecLogicalDevices.end(); ++cit)
	{
		delete (*cit).m_pContext;
	}

	delete m_pInteractionEvent;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

microtime VistaInteractionManager::GetAvgUpdateTime() const
{
	return m_pAvgUpd->GetAverageTime();
}

struct VistaInteractionManager::_qCompPrio
{
	bool operator()(const VistaInteractionManager::DRIVER &left,
					const VistaInteractionManager::DRIVER &right) const
		{
			return left.m_nPrio > right.m_nPrio;
		}
};

struct VistaInteractionManager::_qCompPrioCtx
{
	bool operator()(const VistaInteractionManager::CONTEXT &left,
					const VistaInteractionManager::CONTEXT &right) const
		{
			return left.m_nPrio > right.m_nPrio;
		}
};


bool VistaInteractionManager::DRIVER::operator==(const DRIVER &other) const
{
	return this->m_pDriver == other.m_pDriver;
}

bool VistaInteractionManager::CONTEXT::operator==(const CONTEXT &other) const
{
	return m_pContext == other.m_pContext;
}

// we want an order from largest to smallest small < large
// for contexts, so we invert the normal order here
bool VistaInteractionManager::CONTEXT::operator<(const CONTEXT &other) const
{
	if( m_bDelayedUpdate == other.m_bDelayedUpdate )
		return m_nPrio > other.m_nPrio;
	else
		return !m_bDelayedUpdate;
}


bool VistaInteractionManager::AddDeviceDriver(const std::string &sName,
												  IVistaDeviceDriver *pDriver,
												  int nPrio)
{
	if(m_mpDriverMap->AddDeviceDriver(sName, pDriver))
	{
		return (m_bIsServerInMa && ActivateDeviceDriver(pDriver, nPrio));
	}
	return true;
}

bool VistaInteractionManager::ActivateDeviceDriver(IVistaDeviceDriver *pDriver,
													   int nPrio)
{
	switch(pDriver->GetUpdateType())
	{
	case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
		{
			m_vecInFrameUpdates.push_back(DRIVER(pDriver, nPrio));
			// resort the list according to the priority
			// as addition can change the update order
			std::stable_sort(m_vecInFrameUpdates.begin(),
							 m_vecInFrameUpdates.end(),
							 _qCompPrio());
			return true;
		}
	case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
		{
			VistaDriverConnectionAspect *pConAsp =
				dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));

			if(!pConAsp)
				return false;


			if(pConAsp->GetConnection())
			{
				m_pUpdater->AddConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()),
												new DriverUpdateCallback(pDriver));

				return true;
			}
			break;

		}
	case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
		{
			// record the driver in the threadupdates vector, but do not start now
			// start later
			m_vecThreadedUpdates.push_back(pDriver);
			break;
		}
	default:
		break;
	}

	return false;
}


bool VistaInteractionManager::GetIsDeviceDriver(const std::string &sName) const
{
	return m_mpDriverMap->GetIsDeviceDriver(sName);
}

IVistaDeviceDriver *VistaInteractionManager::GetDeviceDriver(const std::string &sName) const
{
	return m_mpDriverMap->GetDeviceDriver(sName);
}


bool VistaInteractionManager::DelDeviceDriver(const std::string &sName)
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sName);
	if(!pDriver)
		return false;
	switch(pDriver->GetUpdateType())
	{
	case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
		{
			RemoveFromUpdateList(pDriver);
			break;
		}
	case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
		{
			// remove from update handler!
			VistaDriverConnectionAspect *pConAsp =
				dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
					);

			if(!pConAsp)
				return false;

			IVistaExplicitCallbackInterface *pI = m_pUpdater->RemConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()));
			delete pI;
			break;
		}
	case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
		{
			VistaDriverThreadAspect *pThreadAspect =
				dynamic_cast<VistaDriverThreadAspect*>(
				pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
			if(!pThreadAspect)
				return false;

			if(pThreadAspect->StopProcessing())
			{
				std::vector<IVistaDeviceDriver*>::iterator it = std::remove(m_vecThreadedUpdates.begin(), m_vecThreadedUpdates.end(), pDriver);
				m_vecThreadedUpdates.erase(it, m_vecThreadedUpdates.end());
			}
			else
				return false;
			break;
		}
	default:
		break;
	}

	return m_mpDriverMap->DelDeviceDriver(sName);
}

bool VistaInteractionManager::RemDeviceDriver(const std::string &sName)
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sName);
	if(pDriver)
	{
		// remove devices sensors from all interaction contexts that
		// are attached to sensors of this driver
		/*for(unsigned int dk = 0; dk < pDriver->GetNumberOfSensors(); ++dk)
		{
			VistaDeviceSensor *pSen = pDriver->GetSensorByIndex(dk);

			//for(std::vector<CONTEXT>::size_type n = 0;
			//	n < m_vecLogicalDevices.size(); ++n)
			//{
			//	VistaInteractionContext *pCtx = m_vecLogicalDevices[n].m_pContext;
			//	if(pCtx->GetDeviceSensor() == pSen)
			//		// this should utter a notification to pending
			//		// obeservers
			//		pCtx->SetDeviceSensor(NULL);
			//}
		}*/

		// do the logical work
		return m_mpDriverMap->RemDeviceDriver(sName);
	}

	return false;
}

IVistaDeviceDriver *VistaInteractionManager::RemAndGetDeviceDriver(const std::string &sName)
{

	IVistaDeviceDriver *pDriver = GetDeviceDriver(sName);
	if(pDriver)
	{
		switch(pDriver->GetUpdateType())
		{
		case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
			{
				RemoveFromUpdateList(pDriver);
				break;
			}
		case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
			{
				// remove from update handler!
				VistaDriverConnectionAspect *pConAsp =
					dynamic_cast<VistaDriverConnectionAspect*>(
						pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
						);

				if(!pConAsp)
					return NULL;

				// remove from update handler!
				IVistaExplicitCallbackInterface *pI = m_pUpdater->RemConnectionUpdate(pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()));
				delete pI;
				break;
			}
		case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
			{
				VistaDriverThreadAspect *pThreadAspect =
					dynamic_cast<VistaDriverThreadAspect*>(
					pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
				if(!pThreadAspect)
					return NULL;

				if(pThreadAspect->StopProcessing())
				{
					std::vector<IVistaDeviceDriver*>::iterator it = std::remove(m_vecThreadedUpdates.begin(), m_vecThreadedUpdates.end(), pDriver);
					m_vecThreadedUpdates.erase(it, m_vecThreadedUpdates.end());
				}
				else
					return NULL;
				break;
			}
		default:
			break;
		}
	}

	return m_mpDriverMap->RemAndGetDeviceDriver(sName);
}

VistaDriverMap    *VistaInteractionManager::GetDriverMap() const
{
	return m_mpDriverMap;
}

void VistaInteractionManager::SetDriverMap(VistaDriverMap *pMap)
{
	if(!m_mpDriverMap)
		m_mpDriverMap = pMap;

	if(m_mpDriverMap)
	{
		if(!m_mpDriverMap->GetDriverCreationMethod("SPACEMOUSE"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("SPACEMOUSE",
				VistaSpaceMouseDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("DTRACK"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("DTRACK",
				VistaDTrackDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("PHANTOMSERVER"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("PHANTOMSERVER",
				VistaPhantomServerDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("IRMAN"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("IRMAN",
				VistaIRManDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("HID"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod(
				"HID",
				VistaHIDDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("MIDI"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("MIDI",
				VistaMIDIDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("FASTRAK"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("FASTRAK",
				VistaFastrakDriver::GetDriverFactoryMethod());
		}

		if(!m_mpDriverMap->GetDriverCreationMethod("CYBERGLOVE"))
		{
			m_mpDriverMap->RegisterDriverCreationMethod("CYBERGLOVE",
				VistaCyberGloveDriver::GetDriverFactoryMethod());
		}

	}
}


bool VistaInteractionManager::SetPostUpdateFunctor(const std::string &sDriverName,
													   IVistaPostUpdateFunctor *pFunctor)
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sDriverName);

	if(pDriver)
	{
		switch(pDriver->GetUpdateType())
		{
		case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
			{
				for(std::vector<DRIVER>::iterator it = m_vecInFrameUpdates.begin();
					it != m_vecInFrameUpdates.end(); ++it)
				{
					if( (*it).m_pDriver == pDriver )
					{
						(*it).m_pPostUpdate = pFunctor; // this might leak iff user is stoopid
						return true;
					}
				}
				break;
			}
		case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
			{
				// retrieve connection callback
				VistaDriverConnectionAspect *pConAsp =
					dynamic_cast<VistaDriverConnectionAspect*>(
					pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
					);
				// ok,
				if(pConAsp && pConAsp->GetConnection())
				{
					DriverUpdateCallback *pCb = dynamic_cast<DriverUpdateCallback*>(m_pUpdater->GetConnectionUpdate(pConAsp->GetConnection()));
					if(pCb)
					{
						pCb->m_pPostUpdate = pFunctor;
						return true;
					}
				}
				break;
			}
		case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
			{
				VistaDriverThreadAspect *pThreadAspect =
					dynamic_cast<VistaDriverThreadAspect*>(
					pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
				if(!pThreadAspect)
					return false;
				pThreadAspect->StopProcessing();

				/** @todo post update functor setting */


				pThreadAspect->StartProcessing();
				return true;
			}
		default:
			break;
		}
	}
	return false;
}

VistaInteractionManager::IVistaPostUpdateFunctor *VistaInteractionManager::GetPostUpdateFunctor(const std::string &sDriverName) const
{
	IVistaDeviceDriver *pDriver = GetDeviceDriver(sDriverName);

	if(pDriver)
	{
		switch(pDriver->GetUpdateType())
		{
			case IVistaDeviceDriver::UPDATE_EXPLICIT_POLL:
				{
					for(std::vector<DRIVER>::const_iterator it = m_vecInFrameUpdates.begin();
						it != m_vecInFrameUpdates.end(); ++it)
					{
						if( (*it).m_pDriver == pDriver )
						{
							return (*it).m_pPostUpdate;
						}
					}
					break;
				}
			case IVistaDeviceDriver::UPDATE_CONNECTION_THREADED:
				{
					// retrieve connection callback
					VistaDriverConnectionAspect *pConAsp =
						dynamic_cast<VistaDriverConnectionAspect*>(
						pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId())
						);
					// ok,
					if(pConAsp && pConAsp->GetConnection())
					{
						DriverUpdateCallback *pCb = dynamic_cast<DriverUpdateCallback*>(m_pUpdater->GetConnectionUpdate(pConAsp->GetConnection()));
						if(pCb)
						{
							return pCb->m_pPostUpdate;
						}
					}
					break;
				}
			case IVistaDeviceDriver::UPDATE_CUSTOM_THREADED:
				{
					break;
				}
			default:
				break;
		}
	}

	return NULL;
}

bool VistaInteractionManager::GetSwallowUpdateEvent() const
{
	return m_bSwallowEvent;
}

void VistaInteractionManager::SetSwallowUpdateEvent(bool bDone)
{
	m_bSwallowEvent = bDone;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool VistaInteractionManager::AddInteractionContext(VistaInteractionContext *pCtx,
													 int nPriority,
													 bool bDelayedUpdate)
{
	if(pCtx->m_bRegistered)
		return false;

	if(pCtx->GetRoleId() == ~0)
	{
		// create dummy role
		/** @todo this should be smarter somehow */
		char buffer[256];
		sprintf(buffer, "__dummyrole_%d", m_nRoleIdCount);
		pCtx->SetRoleId(RegisterRole(buffer));
	}

	if(GetInteractionContextByRoleId(pCtx->GetRoleId()))
	{
		vkernerr << " [NewIAM]: Context with role ["
				  << GetRoleForId(pCtx->GetRoleId())
				  << "] -- ALREADY REGISTERED\n";
		return false; // no way to assign a context when the role is already there
	}

	m_vecLogicalDevices.push_back(CONTEXT(pCtx, nPriority, bDelayedUpdate));
	// sort vector
	std::stable_sort(m_vecLogicalDevices.begin(), m_vecLogicalDevices.end());

	if(m_pEvObserver) // master mode
		m_pEvObserver->Observe(pCtx);

	pCtx->m_bRegistered = true;

	return true;
}

void VistaInteractionManager::InitializeGraphContexts()
{
	for(std::vector<CONTEXT>::iterator it = m_vecLogicalDevices.begin();
		it != m_vecLogicalDevices.end(); ++it)
	{
		(*it).m_pContext->Evaluate(0); // calls "init" on graph
	}
}

void VistaInteractionManager::DumpGraphsToDot(bool bWritePorts) const
{
	for(std::vector<CONTEXT>::const_iterator it = m_vecLogicalDevices.begin();
		it != m_vecLogicalDevices.end(); ++it)
	{
        VdfnGraph *pGraph = (*it).m_pContext->GetTransformGraph();
        if(pGraph)
        {
            pGraph->EvaluateGraph(0);
            std::string strGraphName = GetRoleForId( (*it).m_pContext->GetRoleId() );
            std::string strFileName = strGraphName + std::string(".dot");
            VdfnPersistence::SaveAsDot( pGraph, strFileName, strGraphName, bWritePorts, true );
        }
	}
}


void VistaInteractionManager::HandleEvent(VistaEvent *pEvent)
{
	if(pEvent->GetType() == VistaInteractionEvent::GetTypeId()
		&& pEvent->GetId() == VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE)
	{
		if(!m_pEvObserver) // slave mode?
		{
			m_pAvgUpd->StartRecording();
			// slaves will call evaluate on the context upon receival of this event
			VistaInteractionEvent *pInEv = static_cast<VistaInteractionEvent*>(pEvent);
			VistaInteractionContext *pCtx = pInEv->GetInteractionContext();
			if(pCtx->Evaluate(pEvent->GetTime())==false)
			{
				// something went definitely wrong!

			}
			pEvent->SetHandled(m_bSwallowEvent); // swallow event			
			m_pAvgUpd->RecordTime();
		}
		return;
	}
	else if( (pEvent->GetType() == VistaSystemEvent::GetTypeId())
		&& ( (pEvent->GetId() == VistaSystemEvent::VSE_UPDATE_INTERACTION)
		|| (pEvent->GetId() == VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION) ))
	{
		// "update interaction" event caught
		// this is *always* before POSTAPP, as POSTAPP is the last
		// event before the highway

		if(!m_pEvObserver) // slave mode?
		{
			// yes
			pEvent->SetHandled(m_bSwallowEvent); // swallow event
									  // (other events will be injected by the cluster)
			return;
		}


		m_pAvgUpd->StartRecording();
		// this is master-code below

		// update in-frame device drivers
		VistaKernelProfileStartSection( "DRIVERS_UPDATE" );
		for(std::vector<DRIVER>::iterator it = m_vecInFrameUpdates.begin();
			it != m_vecInFrameUpdates.end(); ++it)
		{
			if((*it).m_pDriver->Update() == true)
			{
				if((*it).m_pPostUpdate)
					(*it).m_pPostUpdate->PostDriverUpdate( (*it).m_pDriver );
			}
		}
		VistaKernelProfileStopSection();

		// swap measures for all the sensors
		// this is also important for in-frame devices,
		// as it will swap the current-read-head
		VistaKernelProfileStartSection( "DRIVERS_SENSORSWAP" );
		for(VistaDriverMap::iterator dm_it = m_mpDriverMap->begin();
			dm_it != m_mpDriverMap->end(); ++dm_it)
		{
			(*dm_it).second->SwapSensorMeasures();
		}
		VistaKernelProfileStopSection();

		// update logical devices aka contexts
		bool bDelayed = ( pEvent->GetId() == VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION );
		UpdateWithEventHandling( pEvent->GetTime(), bDelayed );
		// swallow events according to flag set
		pEvent->SetHandled( m_bSwallowEvent );
	}
//	else
//	if(pEvent->GetType() == VistaSystemEvent::GetTypeId()
//		&& pEvent->GetId() == VistaSystemEvent::VSE_POSTAPPLICATIONLOOP)
//	{
//		// @todo check hot sampling with a more elaborated design,
//		// this one simply does not work and it is not necessary to
//		// evaluate graphs more than once per frame right now!
//
////		// special mode for hot sampling (untested)
////		if(!m_pEvObserver) // slave mode?
////		{
////			// yes
////			pEvent->SetHandled(m_bSwallowEvent); // swallow event
////									  // (other events will be injected by the cluster)
////			return;
////		}
////
////		// update as usual
////		// update logical devices aka contexts
////		UpdateInteractionContexts(pEvent->GetTime());
////		pEvent->SetHandled(m_bSwallowEvent);
//	}
}

void VistaInteractionManager::UpdateWithEventHandling(double dTs, bool bDelayedUpdate)
{
	VistaKernelProfileScope( "INTERACTIONCTX_UPDATE" );
	for(std::vector<CONTEXT>::iterator ldit = m_vecLogicalDevices.begin();
		ldit != m_vecLogicalDevices.end(); ++ldit)
	{
		if( (*ldit).m_bDelayedUpdate != bDelayedUpdate )
			continue;
		VistaKernelProfileScope( "CTX_" + GetRoleForId( (*ldit).m_pContext->GetRoleId() ) );
		if((*ldit).m_pContext->Update(dTs) == true)
		{
			// this is master only-code, the contexts have a new state on the
			// master (return true of Update() assumes that, so throw an event
			// which is passed to slaves with the current state of the graph

			// this code should *never* be executed in case of slave processing
			m_pInteractionEvent->SetId(VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE);
			m_pInteractionEvent->SetTime(dTs);
			m_pInteractionEvent->SetInteractionContext((*ldit).m_pContext);
			m_pEvMgr->ProcessEvent(m_pInteractionEvent);
		}
	}
}

bool VistaInteractionManager::RemoveFromUpdateList(IVistaDeviceDriver *pDev)
{
	std::vector<DRIVER>::iterator it = std::remove(m_vecInFrameUpdates.begin(),
												   m_vecInFrameUpdates.end(), DRIVER(pDev, 0));

	if(it != m_vecInFrameUpdates.end())
	{
		// erase does not change the internal order
		m_vecInFrameUpdates.erase(it, m_vecInFrameUpdates.end());
		return true;
	}
	return false;
}


bool VistaInteractionManager::RemInteractionContext(VistaInteractionContext *pCtx)
{
	std::vector<CONTEXT>::iterator it = std::remove(m_vecLogicalDevices.begin(),
													m_vecLogicalDevices.end(),
													CONTEXT(pCtx,0,false));

	if(it == m_vecLogicalDevices.end())
		return false;
	if(m_bProcessingEvents == false)
	{
		m_vecLogicalDevices.erase(it, m_vecLogicalDevices.end());
		if(m_pEvObserver) // master mode
			m_pEvObserver->ReleaseObserveable(pCtx, IVistaObserveable::TICKET_NONE);
		pCtx->m_bRegistered = false;
	}
	else
	{
		pCtx->SetIsEnabled(false); // make dumb
		m_setRemoveAfterUpdate.insert(pCtx);
	}
	return true;
}


bool VistaInteractionManager::DelInteractionContext(VistaInteractionContext *pCtx)
{
	std::vector<CONTEXT>::iterator it = std::remove(m_vecLogicalDevices.begin(),
													m_vecLogicalDevices.end(),
													CONTEXT(pCtx,0,false));

	if(it == m_vecLogicalDevices.end())
		return false;

	if(m_bProcessingEvents == false)
	{
		for(std::vector<CONTEXT>::iterator cit = it;
			it != m_vecLogicalDevices.end(); ++it)
		{
			delete (*it).m_pContext;
		}

		m_vecLogicalDevices.erase(it, m_vecLogicalDevices.end());
		pCtx->m_bRegistered = false;
	}
	else
	{
		pCtx->SetIsEnabled(false); // make dumb
		m_setDeleteAfterUpdate.insert(pCtx);
	}

	return true;
}


bool VistaInteractionManager::StartDriverThread()
{
	if(!m_bIsServerInMa)
		return true;

	if(m_pUpdater->GetIsDispatching())
		return true;
	bool bRet = m_pUpdater->StartUpdaterLoop();


	// ok, let's go over all drivers and SetEnable() to true
	for(VistaDriverMap::iterator it = (*m_mpDriverMap).begin();
		it != (*m_mpDriverMap).end(); ++it)
	{
		(*it).second->SetIsEnabled(true);
	}


	// start all drivers in the threaded vector
	for(std::vector<IVistaDeviceDriver*>::iterator it = m_vecThreadedUpdates.begin();
		it != m_vecThreadedUpdates.end(); ++it)
	{
		// get thread aspect
		VistaDriverThreadAspect *pThreadAspect =
			dynamic_cast<VistaDriverThreadAspect*>(
			(*it)->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
		if(!pThreadAspect)
		{
			// no thread aspect... hmm... call enable state
			(*it)->SetIsEnabled(true);
		}
		else
			pThreadAspect->StartProcessing();
	}
	return bRet;
}

bool VistaInteractionManager::StopDriverThread()
{
	if(!m_bIsServerInMa)
		return true;

	if(m_pUpdater->ShutdownUpdaterLoop() == false)
	{
		// utter error
	}

	// ok, let's go over all drivers and SetEnable() to true
	for(VistaDriverMap::iterator it = (*m_mpDriverMap).begin();
		it != (*m_mpDriverMap).end(); ++it)
	{
		(*it).second->SetIsEnabled(false); // disable 'em all
	}

	for(std::vector<IVistaDeviceDriver*>::iterator it = m_vecThreadedUpdates.begin();
		it != m_vecThreadedUpdates.end(); ++it)
	{
		// get thread aspect
		VistaDriverThreadAspect *pThreadAspect =
			dynamic_cast<VistaDriverThreadAspect*>(
			(*it)->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
		if(!pThreadAspect)
		{
			// no thread aspect? ... hmm... call disable
			(*it)->SetIsEnabled(false);
		}
		else
			pThreadAspect->StopProcessing();
	}

	return true;
}

bool VistaInteractionManager::RegisterEventTypes(VistaEventManager *pEventMgr)
{
	VistaEventManager::EVENTTYPE eTp(pEventMgr->RegisterEventType("VET_INTERACTION"));
	VistaInteractionEvent::SetTypeId(eTp);

	for(int n = VistaInteractionEvent::VEID_CONTEXT_CHANGE;
		n < VistaInteractionEvent::VEID_LAST; ++n)
		pEventMgr->RegisterEventId(eTp, VistaInteractionEvent::GetIdString(n));

	return true;
}

unsigned int VistaInteractionManager::RegisterRole(const std::string &sRole)
{
	unsigned int nRet = 0;
	if((nRet=GetRoleId(sRole)) == ~0)
	{
		m_mapRoleIdMap[m_nRoleIdCount] = sRole;
		return m_nRoleIdCount++;
	}
	return nRet;
}

bool VistaInteractionManager::UnregisterRole(const std::string &sRole)
{
	unsigned int nRole = GetRoleId(sRole);
	if(nRole == ~0)
		return false;

	m_mapRoleIdMap.erase(m_mapRoleIdMap.find(nRole));

	return true;
}

unsigned int VistaInteractionManager::GetRoleId(const std::string &sRole) const
{
	for(ROLEMAP::const_iterator cit = m_mapRoleIdMap.begin();
		cit != m_mapRoleIdMap.end(); ++cit)
	{
		if((*cit).second == sRole)
			return (*cit).first;
	}

	return ~0;
}

bool VistaInteractionManager::GetIsRole(const std::string &sRole) const
{
	return (GetRoleId(sRole) != ~0);
}

std::string  VistaInteractionManager::GetRoleForId(unsigned int nRoleId) const
{
	ROLEMAP::const_iterator cit = m_mapRoleIdMap.find(nRoleId);
	if(cit == m_mapRoleIdMap.end())
		return "";

	return (*cit).second;
}

VistaInteractionContext *VistaInteractionManager::GetInteractionContextByRoleId(unsigned int nRoleId) const
{
	for(std::vector<CONTEXT>::const_iterator cit = m_vecLogicalDevices.begin();
		cit != m_vecLogicalDevices.end(); ++cit)
	{
		if((*cit).m_pContext->GetRoleId() == nRoleId)
			return (*cit).m_pContext;
	}
	return NULL;
}

int VistaInteractionManager::GetNumInteractionContexts() const
{
	return m_vecLogicalDevices.size();
}
VistaInteractionContext* VistaInteractionManager::GetInteractionContext( const int iIndex )
{
	return m_vecLogicalDevices[iIndex].m_pContext;
}

bool VistaInteractionManager::RegisterDriverCreationMethod(const std::string &sTypeName,
															   IVistaDriverCreationMethod *pMethod,
															   bool bForceRegistration)
{
	if(!m_mpDriverMap)
		return false;

	return m_mpDriverMap->RegisterDriverCreationMethod(sTypeName, pMethod, bForceRegistration);
}

bool VistaInteractionManager::UnregisterDriverCreationMethod(const std::string &sTypeName,
																 bool bDeleteDriverCreationMethod)
{
	if(!m_mpDriverMap)
		return false;

	return m_mpDriverMap->UnregisterDriverCreationMethod(sTypeName, bDeleteDriverCreationMethod);
}

IVistaDriverCreationMethod *VistaInteractionManager::GetDriverCreationMethod(const std::string &sTypeName) const
{
	if(!m_mpDriverMap)
		return NULL;

	return m_mpDriverMap->GetDriverCreationMethod(sTypeName);
}

