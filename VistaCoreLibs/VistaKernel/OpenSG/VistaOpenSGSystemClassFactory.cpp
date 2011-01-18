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

#if defined(VISTA_SYS_OPENSG)

#include "VistaOpenSGSystemClassFactory.h"

// Vista includes
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/VistaClusterSlave.h>
#include <VistaKernel/VistaClusterMaster.h>
#include <VistaKernel/VistaClusterClient.h>
#include <VistaKernel/VistaClusterServer.h>
#include <VistaKernel/VistaKernelOut.h>

#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGThreadImp.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaGraphicsEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplayEntity.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>

#include <VistaKernel/WindowingToolkit/VistaWindowingToolkit.h>
#include <VistaKernel/WindowingToolkit/glut/VistaGlutWindowingToolkit.h>
#include <VistaKernel/WindowingToolkit/glut/VistaGlutMouseDriver.h>
#include <VistaKernel/WindowingToolkit/glut/VistaGlutKeyboardDriver.h>

#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimer.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGBaseFunctions.h>
// "name" attachments
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGConfig.h>

// webinterface
#include <OpenSG/OSGWebInterface.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif


#include <iostream>
using namespace std;

OSG_USING_NAMESPACE

static VistaOpenSGSystemClassFactory *SSysFactory = NULL;

static void ActionFunction (void)
{
	assert(SSysFactory != NULL);
	SSysFactory->Update();
}

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenSGSystemClassFactory::VistaOpenSGSystemClassFactory(VistaSystem * pViSy,
															   int argc, char **argv)
															   : IVistaSystemClassFactory(),
															   m_pVistaSystem(pViSy),
															   m_pWeb(NULL),
															   m_pAvgLoopTime( new VistaWeightedAverageTimer ),
															   m_pFrameRate( new VistaWeightedAverageTimer ),
															   m_iArgc(argc),
															   m_pArgv(argv),
															   m_iWindowingToolkit(UNKNOWN)
{
	assert(SSysFactory == NULL);
	SSysFactory = this;

	osg::osgInit(argc, argv);
	// get size of FieldContainerStore right after init (prototypes are initialized now)
	m_lFCStoreOffset = (int)osg::FieldContainerFactory::the()->getFieldContainerStore()->size();

	// register thread creation factory
	IVistaThreadImp::RegisterThreadImpFactory(new VistaOSGThreadImp::VistaOSGThreadImpFactory);

	m_pRenderAction = osg::RenderAction::create();
	m_pRenderAction->setFrustumCulling(false);
	m_pRenderAction->setUseGLFinish(false);
	m_pRenderAction->setOcclusionCullingMode( osg::RenderAction::OcclusionHierarchicalMultiFrame);
	m_pRenderAction->setOcclusionCulling(false);
	m_pRenderAction->setVolumeDrawing(false);

	m_LastTime = clock();
	m_iNumFrames = 0;

}

void VistaOpenSGSystemClassFactory::Debug(std::ostream &out, bool bVerbose) const
{
	const std::vector<FieldContainerPtr> &fcs = *FieldContainerFactory::the()->getFieldContainerStore();
	int notNull = 0;
	int suspectsCount = 0;
	for(int i = 0; i < (int)fcs.size(); ++i)
	{
		FieldContainerPtr fc = fcs[i];
		if(fc != NullFC)
		{
			++notNull;
			if(fc.getRefCount() <= 0)
				++suspectsCount;
			if(!bVerbose) continue;

			AttachmentContainerPtr ac = AttachmentContainerPtr::dcast(fc);
			if(ac == NullFC)
			{
				AttachmentPtr a = AttachmentPtr::dcast(fc);
				if(a != NullFC)
				{
					FieldContainerPtr parent = NullFC;
					if (a->getParents().size() > 0)
					{
						/** @todo make this compile again with OpenSG 1.8.0 and higher */
						parent = a->getParents().getValue(0);
					}
					ac = AttachmentContainerPtr::dcast(parent);
				}
			}
			const osg::Char8 *name = osg::getName(ac);

			if(name != NULL)
			{
				out << ((fc.getRefCount() <= 0) ? "[*SUSPECT*] " : "")
					<< ((i < m_lFCStoreOffset) ? "[prototype] " : "")
					<< "Detected living FC " << fc->getTypeName()
					<< " (" << name << ") " << fc.getCPtr()
					<< " refcount " << fc.getRefCount() << "\n";
			}
			else
			{
				out << ((fc.getRefCount() <= 0) ? "[*SUSPECT*] " : "")
					<< ((i < m_lFCStoreOffset) ? "[prototype] " : "")
					<< "Detected living FC " << fc->getTypeName()
					<< " refcount " << fc.getRefCount() << "\n";
			}
		}
	}

	out << notNull << " of " << fcs.size() << " (" << m_lFCStoreOffset
		<< " prototypes) FCs are alive, " << suspectsCount << " are suspect (refcount 0)" << std::endl;
}

VistaOpenSGSystemClassFactory::~VistaOpenSGSystemClassFactory()
{
	delete m_pAvgLoopTime;
	delete m_pFrameRate;
	IVistaThreadImp::IVistaThreadImpFactory *pF = IVistaThreadImp::GetThreadImpFactory();
	delete pF;

	delete m_pRenderAction;

	osg::Thread::getCurrentChangeList()->clearAll();

	if(m_pWeb)
	{
		delete m_pWeb;
		m_pWeb = NULL;
	}


#ifdef DEBUG
//	Debug(std::cerr);
#endif

	osg::osgExit();
	SSysFactory = NULL;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME    :	GetInitOrder                                                  */
/*                                                                            */
/*============================================================================*/
std::vector<IVistaSystemClassFactory::Manager>
VistaOpenSGSystemClassFactory::GetInitOrder() const
{
	std::vector<IVistaSystemClassFactory::Manager> vOrder;
	vOrder.clear();
	vOrder.push_back(IVistaSystemClassFactory::GRAPHICS);
	vOrder.push_back(IVistaSystemClassFactory::DISPLAY);
	vOrder.push_back(IVistaSystemClassFactory::PICK);
	vOrder.push_back(IVistaSystemClassFactory::INTERACTION);
	return vOrder;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :	CreateGraphicsBridge                                          */
/*                                                                            */
/*============================================================================*/
IVistaGraphicsBridge * VistaOpenSGSystemClassFactory::CreateGraphicsBridge()
{
	return new VistaOpenSGGraphicsBridge(m_pRenderAction,this);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :	CreateNodeBridge                                              */
/*                                                                            */
/*============================================================================*/
IVistaNodeBridge * VistaOpenSGSystemClassFactory::CreateNodeBridge()
{
	return new VistaOpenSGNodeBridge;
}

VistaGraphicsManager *VistaOpenSGSystemClassFactory::CreateGraphicsManager()
{
	return new VistaGraphicsManager(m_pVistaSystem->GetEventManager());
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   CreateDisplayManager_New                                      */
/*                                                                            */
/*============================================================================*/

VistaDisplayManager * VistaOpenSGSystemClassFactory::CreateDisplayManager()
{
	// note: init order! gm should be there!
	VistaGraphicsManager *gm = m_pVistaSystem->GetGraphicsManager();

	VistaSG *sg = gm->GetSceneGraph();
	VistaGroupNode *root = sg->GetRealRoot();
	osg::NodePtr p = (dynamic_cast<VistaOpenSGNodeData*>(root->GetData()))->GetNode();

	VistaOpenSGDisplayBridge * pDisplayBridge =
		new VistaOpenSGDisplayBridge(m_pRenderAction, p);

	pDisplayBridge->SetActionFunction(&ActionFunction);

	VistaDisplayManager *dm = new VistaDisplayManager(pDisplayBridge);

	// cross link bridge with manager
	pDisplayBridge->SetDisplayManager(dm);

	return dm;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   CreateDisplayAbstraction                                      */
/*                                                                            */
/*============================================================================*/
IVistaWindowingToolkit *VistaOpenSGSystemClassFactory::CreateWindowingToolkit( std::string toolkit )
{
	if( VistaAspectsComparisonStuff::StringEquals(toolkit,"GLUT",false) )
	{
		// compare the string once and store the result as enum
		m_iWindowingToolkit = GLUT;
		return new VistaGlutWindowingToolkit(m_pVistaSystem,m_iArgc,m_pArgv);
	}
	else
	{
		vkernerr << "[VistaOpenSGSystemClassFactory::CreateWindowingToolkit] " << toolkit << " is unknown." << std::endl;
		return NULL;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   Run						                                      */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGSystemClassFactory::Run()
{
	if(!m_pVistaSystem)
	{
		vkernerr << " ERROR: VistaOpenSGSystemClassFactory -> No VistaSystem available! " << endl;
		return false;
	}

	m_pVistaSystem->GetWindowingToolkit()->Run();

	m_oSystemEvent.SetId(VistaSystemEvent::VSE_EXIT);
	m_pVistaSystem->GetEventManager()->ProcessEvent(&m_oSystemEvent);

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   Quit					                      */
/*                                                                            */
/*============================================================================*/

microtime VistaOpenSGSystemClassFactory::GetAvgEventLoopTime() const
{
	return m_pAvgLoopTime->GetAverageTime();
}

unsigned int VistaOpenSGSystemClassFactory::GetFrameCount() const
{
	return m_iNumFrames;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Update                                                      */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGSystemClassFactory::Update()
{
	if(!m_pVistaSystem->IsClient() && !m_pVistaSystem->GetIsSlave())
	{
		VistaEventManager * pEvMa = m_pVistaSystem->GetEventManager();

		if( pEvMa == NULL )
		{
			vkernerr << " ERROR: Unable to find event Manager in VistaOpenSGSystemClassFactory::Update() " << endl;
			return;
		}

		VistaOpenSGNodeBridge *pNdBridge
			= static_cast<VistaOpenSGNodeBridge*>(m_pVistaSystem->GetGraphicsManager()->GetNodeBridge());

		if(pNdBridge->GetAmbientLightState())
		{
			// DRAW
			float aF[4] = {0,0,0,1};
			float aN[4] = {0,0,0,1};

			pNdBridge->GetLightAmbientColor( VISTA_AMBIENT_LIGHT, aF[0], aF[1], aF[2], NULL); // ok to pass NULL here
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, aF );
			((VistaOpenSGDisplayBridge*)(m_pVistaSystem->GetDisplayManager()->GetDisplayBridge()))->Draw();
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, aN );
		}
		else // no ambient light, render only...
			((VistaOpenSGDisplayBridge*)(m_pVistaSystem->GetDisplayManager()->GetDisplayBridge()))->Draw();

		if( m_iNumFrames > 0 )
		{
			VistaKernelProfileStopNamedSection( "RENDER" );
			VistaKernelProfileNewFrame();
		}


		m_pAvgLoopTime->StartRecording();

		// the master now shows the new frame.
		// do the sync with the slaves
		VistaClusterMaster *pMaster = m_pVistaSystem->GetVistaClusterMaster();
		if( pMaster && (pMaster->GetCurrentFrameIndex() > 0))
		{
			// will return when all clients show their frame
			pMaster->Sync();
		}

		// now on with the next frame!
		m_oSystemEvent.SetId(VistaSystemEvent::VSE_POSTGRAPHICS);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		m_oSystemEvent.SetId(VistaSystemEvent::VSE_PREAPPLICATIONLOOP);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		m_oSystemEvent.SetId(VistaSystemEvent::VSE_UPDATE_INTERACTION);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		m_oSystemEvent.SetId(VistaSystemEvent::VSE_UPDATE_PICKING);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		//@todo: not implemented, but in theory, displays should be updated right before draw -> move behind DELAYED_INTERACTION?
		m_oSystemEvent.SetId(VistaSystemEvent::VSE_UPDATE_DISPLAYS);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		m_oSystemEvent.SetId(VistaSystemEvent::VSE_POSTAPPLICATIONLOOP);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		m_oSystemEvent.SetId(VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		// Pre DRAW EVENT
		m_oSystemEvent.SetId(VistaSystemEvent::VSE_PREGRAPHICS);
		VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( m_oSystemEvent.GetId() ) );
		pEvMa->ProcessEvent(&m_oSystemEvent);
		VistaKernelProfileStopSection();

		//update webinterface
		if(m_pWeb)
		{
			VistaKernelProfileScope( "UPD_WEBINTERFACE" );
			m_pWeb->waitRequest(0.001);
			m_pWeb->handleRequests();
		}

		m_pAvgLoopTime->RecordTime();

		VistaKernelProfileStartSection( "RENDER" );

	}
	else
	{
		VistaKernelProfileStopNamedSection( "RENDER" );
		VistaKernelProfileNewFrame();


		m_pAvgLoopTime->StartRecording();

		if(m_pVistaSystem->IsClient())
		{
			m_pVistaSystem->GetVistaClusterClient()->PullPacket(); // suck suck
		}
		else if(m_pVistaSystem->GetIsSlave())
		{
			if(m_pVistaSystem->GetVistaClusterSlave()->ProcessCurrentFrame() == true)
			{
				m_pVistaSystem->GetWindowingToolkit()->Quit();
				return; // leave on VSE_EXIT
			}
		}

		m_pAvgLoopTime->RecordTime();

		// in OpenSG, the sync is done at the end of the window's
		// swap() method, so we do not have to deal with that here.
		((VistaOpenSGDisplayBridge*)(m_pVistaSystem->GetDisplayManager()->GetDisplayBridge()))->Draw();

		VistaKernelProfileStartSection( "RENDER" );
	}

	// ViSTA framerate measurement probably more realistic
	// than the "random" number generated in the OpenSG statistics
	++m_iNumFrames;

	m_pFrameRate->RecordTime();
	if(m_pFrameRate->GetAverageTime())
		m_fLastFPS = (float)( 1.0 / m_pFrameRate->GetAverageTime() );
}


bool VistaOpenSGSystemClassFactory::GetWebInterfaceEnabled() const
{
	if(m_pWeb != NULL)
		return true;
	else
		return false;
}

void VistaOpenSGSystemClassFactory::SetWebInterfaceEnabled(bool bState, const unsigned int &port)
{
	// if we have a webinterface, delete it
	if(m_pWeb)
		delete m_pWeb;
	m_pWeb = NULL;

	//unregister update

	// create a new one if requested
	if(bState)
	{
		m_pWeb = new osg::WebInterface(port);

		// get realroot node
		VistaNode *pRR = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph()->GetRealRoot();
		osg::NodePtr ptr = dynamic_cast<VistaOpenSGNodeData*>(pRR->GetData())->GetNode();
		m_pWeb->setRoot(ptr);
	}
}

VistaInteractionManager *VistaOpenSGSystemClassFactory::CreateInteractionManager(VistaDriverMap *pMap)
{
	VistaInteractionManager *pRet = new VistaInteractionManager(m_pVistaSystem->GetEventManager(),
		!m_pVistaSystem->IsClient() && !m_pVistaSystem->GetIsSlave() );

	pRet->SetDriverMap(pMap);

	// decide here which driver you want to use
	if( m_iWindowingToolkit == GLUT )
	{
		// register mouse creation callback
		pRet->RegisterDriverCreationMethod("MOUSE",
			VistaGlutMouseDriver::GetDriverCreationMethod(),
			true);
		pRet->RegisterDriverCreationMethod("KEYBOARD",
			VistaGlutKeyboardDriver::GetDriverCreationMethod(),
			true);
	}
	else
	{
		vkernerr << "[CreateInteractionManager] not fully supported toolkit." << std::endl;
	}


	return pRet;
}


#endif // VISTA_SYS_OPENSG
