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
// $Id: VistaSystem.cpp 22674 2011-07-27 13:04:10Z dr165799 $

#include "VistaSystem.h"
#include "VistaKernelOut.h"
#include "VistaDataTunnelFactory.h"

#include "EventManager/VistaEventManager.h"
#include "EventManager/VistaSystemEvent.h"
#include "EventManager/VistaExternalMsgEvent.h"
#include "EventManager/VistaGraphicsEvent.h"
#include "EventManager/VistaCommandEvent.h"
#include "EventManager/VistaDisplayEvent.h"

#include "DisplayManager/VistaVirtualPlatform.h"

#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaDefaultTimerImp.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaTools/VistaProfiler.h>
#include <VistaTools/VistaEnvironment.h>
#include <VistaTools/VistaTopologyGraph.h>


#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>

#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaKernelDfnNodeCreators.h>

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/Drivers/VistaShallowDriver.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>

#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnUtil.h>
#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnObjectRegistry.h>
#include <VistaDataFlowNet/VdfnTimerNode.h>

#include <VistaKernel/Stuff/VistaStreamManagerExt.h>

#include <VistaKernel/InteractionManager/VistaUserPlatform.h>
#include <VistaKernel/InteractionManager/VistaVirtualPlatformAdapter.h>


#include "VistaDriverPropertyConfigurator.h"

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>

#include <VistaKernel/PickManager/VistaPickManager.h>

#include <VistaKernel/VistaClusterServer.h>
#include <VistaKernel/VistaClusterClient.h>
#include <VistaKernel/VistaClusterMaster.h>
#include <VistaKernel/VistaClusterSlave.h>

#include <VistaKernel/VistaClusterAux.h>

#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include "WindowingToolkit/VistaWindowingToolkit.h"

#include <VistaBase/VistaVersion.h>

#include "VistaSystemCommands.h"
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/InteractionManager/VistaDriverWindowAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>

#include <VistaInterProcComm/Connections/VistaConnectionSerial.h>
#include <VistaInterProcComm/Connections/VistaConnectionFileTimed.h>

#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <cstring>
using namespace std;

/// @todo remove this and replace with boost
#ifndef WIN32
#include <unistd.h>
#endif

/**
 * @todo remove from VistaSystem and put into SystemCommands, add API to the bridges!
 */
#if defined(VISTA_SYS_OPENSG)

#include <VistaKernel/OpenSG/VistaOpenSGSystemClassFactory.h>

#endif // SystemClassFactory for OpenSG

#include <VistaInterProcComm/IPNet/VistaIPComm.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>

#if defined(LINUX) || defined(WIN32)
#define DLV_IX86
#include <VistaInterProcComm/DataLaVista/Common/VistaPentiumBasedRTC.h>
#include <VistaTools/VistaCPUInfo.h>
#elif defined(_USE_HRRTC)
#include <VistaInterProcComm/DataLaVista/Common/VistaHRTimerRTC.h>
#else
#include <VistaInterProcComm/DataLaVista/Common/VistaDummyRTC.h>
#endif

#if !defined(WIN32)
#include <signal.h>
#endif


#if defined(SUNOS)
#include <sunmath.h>
#endif

#include "VistaKernelMsgPort.h"
#include "VistaSystemConfigurators.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaTools/VistaRandomNumberGenerator.h>
#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#include <set>

#if !defined(VISTAKERNELSTATIC)
#include <VistaTools/VistaDLL.h>
#endif

#include <VistaDeviceDriversBase/VistaDriverUtils.h> // needed for VddInit()


//// PROTOTYPES OF STATIC FUNCTIONS
struct _dllHlp
{
#if !defined(VISTAKERNELSTATIC)
	std::list<VddUtil::VistaDriverPlugin> m_liDevices;
#endif
};
struct _drDevHlp
{
	_drDevHlp()
		: m_nPrio(-1), m_pDriver(NULL)
		{}

	_drDevHlp(IVistaDeviceDriver *pDriver,
			  const std::string &strTypeName,
			  const std::string &strName,
			  const std::string &strSection,
			  int nPrio,
			  const std::list<std::string> &liDependsOn)
		: m_strName(strName),
		  m_strSection(strSection),
		  m_strTypeName(strTypeName),
		  m_nPrio(nPrio),
		  m_pDriver(pDriver),
		  m_liDependsOn(liDependsOn)
		{}

	std::string m_strName,
		m_strSection,
		m_strTypeName;
	int         m_nPrio;
	IVistaDeviceDriver *m_pDriver;
	std::list<std::string> m_liDependsOn;
};

static VistaInteractionContext *SetupContext(VistaInteractionManager *pInMa,
											  VistaDisplayManager *pDispMgr,
											  VistaEventManager *pEvMgr,
											  VistaClusterAux *pAux,
											  VistaKeyboardSystemControl *pCtrl,
											  VistaSystem *pSys,
											  const std::string &strContextSec,
											  const std::string &strInteractionIniFile,
											  const std::string &strConfigFileName,
											  int nPrioOverride = -1,
											  int nSlaveId = -1);

namespace {

	bool isAbsolutePath(const std::string &path)
	{
		if(path.empty() ||
#ifdef WIN32
			path.size() < 2) // cannot be absolute if that short
#else
			path.size() < 1) // cannot be absolute if that short
#endif
			return false;
#ifdef WIN32
		if(path[1] == ':') // only works with "normal" pathes, no network shares or ntfs mounts!
			return true;
#else
		if(path[0] == '/')
			return true;
#endif
		return false;
	}


	/// get absolute path, depending on current working directory
	std::string getAbsolutePath(const std::string &path)
	{
		if(isAbsolutePath(path))
		{
			return path;
		}
#ifdef WIN32
		char buf[MAX_PATH];
		char *newName = _fullpath(buf, path.c_str(), MAX_PATH);
		if(newName)
			return std::string(newName);
		else
		{
			assert(false); // _fullpath failed!
			return path;
		}
#else
		char *newName = realpath(path.c_str(), NULL);
		if(newName)
		{
			std::string ret(newName);
			free(newName);
			return ret;
		}
		else
		{
			string msg("[ViSystem] \"realpath\" syscall failed for ");
			msg.append(path);
			perror(msg.c_str());
			vkernerr <<    "           temporal workaround: try to build a pseudo-path on our own..." << endl;
			char *pwd = getcwd(NULL, 0);
			string newPath(pwd);
			free(pwd);
			newPath.append("/");
			newPath.append(path);
			vkernerr << "           approximated (non-checked) path: " << newPath << endl; 
			return newPath;
		}
#endif
	}

	/// get absolute path, depending on the given working directory/file
	std::string getAbsolutePathRelativeTo(const std::string &path, std::string absWorkingDirOrFile)
	{
		if(isAbsolutePath(path) || path.empty())
			return path;

		VistaFileSystemFile f(absWorkingDirOrFile);
		if(f.IsFile())
		{
			size_t idx = absWorkingDirOrFile.find_last_of(VistaFileSystemDirectory::GetOSSpecificSeparator());
			absWorkingDirOrFile = absWorkingDirOrFile.substr(0, idx);
		}

		// concatenate and try to resolv an absolute path...
		std::string buf = absWorkingDirOrFile;
		buf.append(VistaFileSystemDirectory::GetOSSpecificSeparator());
		buf.append(path);
		std::string str = getAbsolutePath(buf);
		return str;
	}


}

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

// ==========================================
VistaSystem * global_pVistaSystem = NULL;
VistaSystem * GetVistaSystem () {return global_pVistaSystem;}
std::string gSystemSectionName = "SYSTEM";
/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaSystem::VistaSystem()
: m_pDllHlp( new _dllHlp )
, m_pWindowingToolkit( NULL )
{
	VddUtil::InitVdd();

	SetHandlerToken("SYSTEM");
	global_pVistaSystem = this;

	m_bLockSearchPath = false;

	m_bLockGraphicsIni = false;
	m_bLockDisplayIni = false;
	m_bLockInteractionIni = false;
	m_bLockClusterIni = false;

#if !defined(WIN32)
	::signal(SIGINT, VistaSystem::SIGINTHandler);
	::signal(SIGTERM, VistaSystem::SIGTERMHandler);
	::signal(SIGPIPE, VistaSystem::SIGPIPEHandler);
#else

#endif

	m_pClusterAux         = NULL;


	m_pSystemClassFactory   = NULL;
	m_pDisplayManager = NULL;
	m_pInteractionManager = NULL;
	m_pNewWorld = NULL;

	m_pEventManager = NULL;
	m_pSystemEvent = NULL;
	m_pCentralEventHandler = NULL;

	m_pPickManager = NULL;
	m_pSystemControl = NULL;


	m_strVistaConfigFile = "";
	m_strModelFile = "";
	m_nModelScale = 1.0;

	m_bInitialized = false;

	// default Vista to Standalone Mode
	m_iClusterMode = VistaSystem::VCM_STANDALONE;
	m_strSystemName = "VistaSystem";


	/**
	 * Set up IPC specific stuff. We may want to use network
	 * somewhere; UseIPComm can be called several times, so this will
	 * not hurt.
	 */
	VistaIPComm::UseIPComm();

	/**
	 * We are using DLV for cluster, so setting up the timer
	 * RTC will be a good thing.
	 */
#if defined(DLV_IX86)
	// ok, we are on a ix86-cpu, we can use the RTC clock then
	DLVistaPentiumBasedRTC *rtc = new DLVistaPentiumBasedRTC;

	// determine clock frequency
	VistaCPUInfo info;
	rtc->SetClockFrequency( info.GetSpeed() );

#elif defined(_USE_HRRTC)
	DLVistaHRTimerRTC *rtc = new DLVistaHRTimerRTC;
#else
	// no, no rtc, lets use posix/empty calls instead :(
	DLVistaDummyRTC *rtc = new DLVistaDummyRTC;
#endif

	//New timer interface: we have to create a TimerImp and set is
	// as timer's singleton
	if( IVistaTimerImp::GetSingleton( false ) == NULL )
		IVistaTimerImp::SetSingleton( new VistaDefaultTimerImp );

	// we will provide a global timer, which will be needed for
	// the frame clock (client-sync)
	m_pTimer = new VistaTimer();	

	//setting the oProfiler singleton
	VistaBasicProfiler::SetSingleton( new VistaBasicProfiler );

	std::string sEnvSearchPath = VistaEnvironment::GetEnv("VISTAINIPATH");
	if(sEnvSearchPath.empty())
	{
		std::list<std::string> liSearchPaths;
		liSearchPaths.push_back("configfiles/");
		SetIniSearchPaths(liSearchPaths);
	}
	else
	{
		list<string> liSearchPaths;
		VistaAspectsConversionStuff::ConvertToList(sEnvSearchPath, liSearchPaths);
		SetIniSearchPaths(liSearchPaths);
		vkernout << "VISTAINIPATH = " << sEnvSearchPath << endl;
	}

	// setup event manager, we really need this...
	m_pEventManager = new VistaEventManager;
	m_pEventManager->Init();

	// preliminary: setup event system

	// register new event type
	VistaEventManager::EVENTTYPE eTp = m_pEventManager->RegisterEventType("VET_SYSTEM");
	VistaSystemEvent::SetTypeId(eTp);


	// we should register all ids now
	int n;
	for(n=VistaSystemEvent::VSE_INIT; n < VistaSystemEvent::VSE_UPPER_BOUND; ++n)
	{
		m_pEventManager->RegisterEventId(eTp, VistaSystemEvent::GetIdString(n));
	}

	VistaDisplayManager::RegisterEventTypes(m_pEventManager);
	VistaGraphicsManager::RegisterEventTypes(m_pEventManager);
	VistaPickManager::RegisterEventTypes(m_pEventManager);
	VistaInteractionManager::RegisterEventTypes(m_pEventManager);

	// register and map command events

	eTp = m_pEventManager->RegisterEventType("VET_COMMAND");
	VistaCommandEvent::SetTypeId(eTp);

	for(n=VistaCommandEvent::VEIDC_CMD; n < VistaCommandEvent::VEIDC_LAST; ++n)
		m_pEventManager->RegisterEventId(eTp, VistaCommandEvent::GetIdString(n));

	eTp = m_pEventManager->RegisterEventType("VET_EXTERNAL_MESSAGE");
	VistaExternalMsgEvent::SetTypeId(eTp);

	for(n=VistaExternalMsgEvent::VEID_INCOMING_MSG; n < VistaExternalMsgEvent::VEID_LAST; ++n)
		m_pEventManager->RegisterEventId(eTp, VistaExternalMsgEvent::GetIdString(n));

	m_pCommandEvent = new VistaCommandEvent;

	// MESSAGE-PORT SPECIFIC
	m_pPort = NULL;
	m_pExternalMsg = new VistaExternalMsgEvent;


	m_pDriverMap = new VistaDriverMap;
	m_pConfigurator = new VistaDriverPropertyConfigurator;

	m_pDfnObjects = new VdfnObjectRegistry;
}

VistaSystem::~VistaSystem()
{
#ifdef DEBUG
	vkernout << " [ViSys] >> DESTRUCTOR <<" << endl;
#endif

	// remove user platforms
	for(std::map<VistaDisplaySystem*, VistaUserPlatform*>::iterator pit = m_mpUserPlatforms.begin();
		pit != m_mpUserPlatforms.end(); ++pit )
	{
		delete (*pit).second;
	}

	delete m_pTimer;
	delete m_pSystemControl;

	// reverse destruction order

	if(m_pSystemClassFactory)
	{
		std::vector<IVistaSystemClassFactory::Manager> vOrder = m_pSystemClassFactory->GetInitOrder();
		for(std::vector<IVistaSystemClassFactory::Manager>::reverse_iterator rit = vOrder.rbegin();
			rit != vOrder.rend(); ++rit)
		{
			switch(*rit)
			{
			case IVistaSystemClassFactory::DISPLAY:
				{
					delete m_pDisplayManager;
					m_pDisplayManager = NULL;
					break;
				}
			case IVistaSystemClassFactory::GRAPHICS:
				{
					delete m_pNewWorld;
					m_pNewWorld = NULL;
					break;
				}
			case IVistaSystemClassFactory::INTERACTION:
				{
					delete m_pInteractionManager;
					m_pInteractionManager = NULL;
					break;
				}
			case IVistaSystemClassFactory::PICK:
				{
					delete m_pPickManager;
					m_pPickManager = NULL;
					break;
				}
			default:
				break;
			}

		}
	}

	// 5. cluster manager
	delete m_pClusterAux;
	m_pClusterAux = NULL;

	// 6. event specific
	delete m_pEventManager;
	m_pEventManager = NULL;
	delete m_pSystemEvent;
	m_pSystemEvent = NULL;

	// delete message port stuff
	delete m_pCommandEvent;
	delete m_pPort;
	delete m_pExternalMsg;


	for(VistaDriverMap::iterator it = m_pDriverMap->begin();
		it != m_pDriverMap->end(); ++it)
	{
		(*it).second->SetIsEnabled(false);
		delete (*it).second;
	}

	delete m_pConfigurator;

#if !defined(VISTAKERNELSTATIC)
	// removed device drivers from dll loads
	for(std::list<VddUtil::VistaDriverPlugin>::iterator dllIt = m_pDllHlp->m_liDevices.begin();
		dllIt != m_pDllHlp->m_liDevices.end(); ++dllIt)
	{
		// we should unregister the stuff from the interaction manager first...
		GetDriverMap()->UnregisterDriverCreationMethod( (*dllIt).m_strDriverClassName, false ); // do not kill the memory on it
		// will be disposed here!
		VddUtil::DisposePlugin( &(*dllIt) );
	}
#endif

	delete m_pDllHlp;
	delete m_pDriverMap;

	delete m_pDfnObjects;

	// delete RTC
	delete IDLVistaRTC::GetRTCSingleton();
	IVistaTimerImp::SetSingleton(NULL);

	VistaIPComm::CloseIPComm();

	//delete data tunnel factory singleton
	VistaDataTunnelFactory::DestroyFactory();

	delete m_pSystemClassFactory;
	global_pVistaSystem = NULL;

	m_liDriverPluginPathes.push_back( "." );
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

static inline void TestAndSetOnEmpty(const std::string &sNewValue,
									 std::string &oldAndSetValue,
									 const std::string &compareTo = "")
{
	if(oldAndSetValue == compareTo)
		oldAndSetValue = sNewValue;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetIniFile                                                  */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetIniFile (const std::string &strNewIni)
{
	string absolutePath = FindFileInIniSearchPath(strNewIni);
	if(absolutePath.empty())
		return false;
	m_strVistaConfigFile = absolutePath;
	return true;
}

bool VistaSystem::SetDisplayIniFile(const std::string &strNewIni)
{
	string absolutePath = FindFileInIniSearchPath(strNewIni);
	if(absolutePath.empty())
		return false;
	m_strDisplayConfigFile = absolutePath;
	return true;
}

bool VistaSystem::SetGraphicsIniFile(const std::string &strNewIni)
{
	string absolutePath = FindFileInIniSearchPath(strNewIni);
	if(absolutePath.empty())
		return false;
	m_strGraphicsConfigFile = absolutePath;
	return true;
}

bool VistaSystem::SetInteractionIniFile(const std::string &strNewIni)
{
	string absolutePath = FindFileInIniSearchPath(strNewIni);
	if(absolutePath.empty())
		return false;
	m_strInteractionConfigFile = absolutePath;
	return true;

}

bool VistaSystem::SetClusterIniFile(const std::string &strNewIni)
{
	string absolutePath = FindFileInIniSearchPath(strNewIni);
	if(absolutePath.empty())
		return false;
	m_strClusterConfigFile = absolutePath;
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetIniFile                                                  */
/*                                                                            */
/*============================================================================*/
std::string VistaSystem::GetIniFile() const
{
	return m_strVistaConfigFile;
}

std::string VistaSystem::GetDisplayIniFile() const
{
	return m_strDisplayConfigFile;
}

std::string VistaSystem::GetGraphicsIniFile() const
{
	return m_strGraphicsConfigFile;
}

std::string VistaSystem::GetInteractionIniFile() const
{
	return m_strInteractionConfigFile;
}

std::string VistaSystem::GetClusterIniFile() const
{
	return m_strClusterConfigFile;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::Init (int argc, char *argv[])
{
	// we have to have at LEAST the application name
	// (argc==1) and a valid entry in argv[0]
	if(argc>=1 && argv[0])
	{
		m_strApplicationName = argv[0];
		if (!ArgParser (argc, argv))
			return false;
	}

	if (!this->DoInit(argc, argv))
		return false;

	// do it again: ArgParser (argc, argv);
	return true;
}

bool VistaSystem::DoInit(int argc, char **argv)
{
	VistaProfiler			 oProfiler;
	std::string             sDisplayDevice;
	PrintMsg (" ##### ViSTA SYSTEM INITIALIZATION START...##### \n");

	// ========================================================================
	// ========================================================================

	// is a ini file name already set by the application or the commandline?
	std::string     sConfigFileName = GetIniFile();
	if (sConfigFileName.length() <= 0)
	{
		// set default name; check file existence
		SetIniFile("vista.ini");
	}
	// get final name
	sConfigFileName = GetIniFile();


	// allow INI-file dispatching from the main ini file, SYSTEM section
	// if it hasn't already been set from the commandline
	if( !m_bLockDisplayIni )
	{
		std::string sIni =
			oProfiler.GetTheProfileString("SYSTEM", "DISPLAYINI", 
											  GetDisplayIniFile(),
											  sConfigFileName);
		if(sIni == "")
			sIni = sConfigFileName;
		
		SetDisplayIniFile(sIni);
	}
	
	if( !m_bLockGraphicsIni )
	{
		std::string sIni =
			oProfiler.GetTheProfileString("SYSTEM", "GRAPHICSINI", 
											  GetGraphicsIniFile(),
											  sConfigFileName);
		if(sIni == "")
			sIni = sConfigFileName;
		
		SetGraphicsIniFile(sIni);
	}

	if( !m_bLockInteractionIni )
	{
		std::string sIni =
			oProfiler.GetTheProfileString("SYSTEM", "INTERACTIONINI", 
											  GetInteractionIniFile(),
											  sConfigFileName);
		if(sIni == "")
			sIni = sConfigFileName;
		
		SetInteractionIniFile(sIni);
	}

	if( !m_bLockClusterIni )
	{
		std::string sIni =
			oProfiler.GetTheProfileString("SYSTEM", "CLUSTERINI", 
											  GetClusterIniFile(),
											  sConfigFileName);
		if(sIni == "")
			sIni = sConfigFileName;
		
		SetClusterIniFile(sIni);
	}


	PrintMsg ("[VistaSystem]: INI using: \n" );
	std::string tmp = "System-ini     : " + GetIniFile() + "\n"
		+ "Display-ini    : " + GetDisplayIniFile() + "\n"
		+ "Graphics-ini   : " + GetGraphicsIniFile() + "\n"
		+ "Interaction-ini: " + GetInteractionIniFile() + "\n"
		+ "Cluster-ini    : " + GetClusterIniFile() + "\n";
	// show configuration file
	PrintMsg (tmp);

	// ========================================================================
	// ========================================================================
	// create message Port (we might need this for progress messages)
	bool bCreateMsgPort = oProfiler.GetTheProfileBool(gSystemSectionName, "MSGPORT", false, GetIniFile());
	if(bCreateMsgPort)
	{
		PrintMsg(" [ViSystem]: Creating External TCP/IP Msg Port.\n");

		std::string sSection(gSystemSectionName);

		if(m_iClusterMode == VCM_CLIENT || m_iClusterMode == VCM_SLAVE)
			sSection = m_strSystemName;

		std::string sHost;

		oProfiler.GetTheProfileString(  sSection, "MSGPORTIP","localhost",sHost,GetIniFile());
		int iPort = oProfiler.GetTheProfileInt(sSection, "MSGPORTPORT", 6666, GetIniFile());

		bool bCreateIndicator =
			oProfiler.GetTheProfileBool( sSection, "PROGRESSINDICATOR", false, GetIniFile());
		vkernout << "[ViMspPort]: Section (" << sSection << ") in file (" << GetIniFile() << ") tells me "
			 << (bCreateIndicator ? "" : "NOT") << " to reach PROGRESSINDICATOR.\n";
		std::string sProgressHost;
		oProfiler.GetTheProfileString(sSection, "PROGRESSHOST", "localhost", sProgressHost, GetIniFile());
		int iProgressPort = oProfiler.GetTheProfileInt(sSection, "PROGRESSPORT", 6667, GetIniFile());

		m_pPort = new VistaKernelMsgPort(*this, sHost, iPort,
										  GetApplicationName(),
										  bCreateIndicator,
										  sProgressHost, iProgressPort, NULL);
	}
	else
		m_pPort = NULL;


	int iStageCount = 0;
	IndicateSystemProgress(iStageCount++, "Setup EventManager", false);
	if (!SetupEventManager())
		return false;

	IndicateSystemProgress(iStageCount++, "Setup Cluster", false);

	VistaClusterClient *pClient = NULL;
	VistaClusterServer *pServer = NULL;
	VistaClusterMaster *pMaster = NULL;
	VistaClusterSlave  *pSlave  = NULL;

	try
	{
		if (!SetupCluster(pServer, pClient, pMaster, pSlave))
			return false;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		m_iClusterMode = VCM_STANDALONE;
		delete pClient;
		delete pServer;
		m_pClusterAux = new VistaClusterAux;
		return false;
	}

	// in any case (even for the standalone case) we will create
	// a cluster aux structure to give access to the frame clock
	// needed by some components that need time
	// it is ok to pass NULL here as pointers (indicating standalone)
	if( pServer || pClient )
		m_pClusterAux = new VistaClusterAux(pServer,
											 pClient);
	else if( pMaster || pSlave )
		m_pClusterAux = new VistaClusterAux(pMaster, pSlave );

	else
		m_pClusterAux = new VistaClusterAux( (VistaClusterMaster*)NULL, (VistaClusterSlave*)NULL);

	// initialize the frame clock with a meaningful value
	// either server or client will rewrite this, but in case of standalone,
	// we have a valid frame clock (!= 0) for the first frame
	m_pClusterAux->SetFrameClock(m_pTimer->GetSystemTime());

	if(pServer)
	{
		if( !pServer->Init(GetClusterIniFile()) )
		{
			VISTA_THROW("Requested server-mode, but clusterserver init FAILED\n",
						0x00000500);
		}
		pServer->RemoteWakeUpClients();
	}
	else if(pClient)
	{
		if(!pClient->Init(GetClusterIniFile()))
		{
			VISTA_THROW("Requested clusterclient-mode, but clusterclient init FAILED\n",
						0x00000501);
		}
		// clients should not rewrite the time stamps of the received events
		m_pEventManager->SetResetEventTimeToLocalTime( false );
	}
	else if(pMaster)
	{
		if(!pMaster->Init(GetClusterIniFile(), m_strSystemName))
			VISTA_THROW("Requested clustermaster-mode, but clustermaster init FAILED", 0x00000503);
		pMaster->BeginClusterProcessing();
	}
	else if(pSlave)
	{
		if(!pSlave->Init(GetClusterIniFile(), m_strSystemName))
			VISTA_THROW("Requested clusterslave-mode, but clusterslave init FAILED", 0x00000502);
		m_pEventManager->SetResetEventTimeToLocalTime(false);
	}
	else
	{
		//Standalone: Set random seed to frame clock
		m_pClusterAux->SetRandomSeed( int(GetFrameClock()) );
	}

	// ========================================================================
	// ========================================================================
	// Create SystemClassFactory
#if defined(VISTA_SYS_OPENSG)
	m_pSystemClassFactory = new VistaOpenSGSystemClassFactory(this, argc, argv);
#else
	VISTA_THROW("NO SYSTEM TAG DURING COMPILE TIME GIVEN?", 0x0000000);
#endif


	if(!m_pSystemClassFactory)
	{
		PrintMsg(" [ViSystem] Unable to initialize the system - program exit!\n");
		m_bInitialized = false;
		return false;
	}

	// create WindowingToolkit only in case there is none
	if( !m_pWindowingToolkit )
	{
		vkernout << "[VistaSystem::DoInit] Creating WindowingToolkit" << std::endl;
		m_pWindowingToolkit = m_pSystemClassFactory->CreateWindowingToolkit(
				oProfiler.GetTheProfileString("SYSTEM", "WINDOWINGTOOLKIT",
															  "GLUT",
															  GetDisplayIniFile())
				);
	}

	unsigned int i;
	std::vector<IVistaSystemClassFactory::Manager> vOrder = m_pSystemClassFactory->GetInitOrder();
	for( i = 0; i < vOrder.size() ; ++i )
		switch(vOrder[i])
		{
		case IVistaSystemClassFactory::DISPLAY:
			IndicateSystemProgress(iStageCount++, "Setup DisplayManager", false);
			if (!SetupDisplayManager())
				return false;
			break;
		case IVistaSystemClassFactory::GRAPHICS:
			IndicateSystemProgress(iStageCount++, "Setup GraphicsManager", false);
			if (!SetupGraphicsManager())
				return false;
			break;
		case IVistaSystemClassFactory::INTERACTION:
		{
			IndicateSystemProgress(iStageCount++, "Setup InteractionManager", false);

			if(!SetupInteractionManager())
				return false;
			break;
		}
		case IVistaSystemClassFactory::PICK:
			IndicateSystemProgress(iStageCount++, "Setup PickManager", false);
			if (!SetupPickManager())
				return false;
			break;
		default:
			vkernerr << " ERROR: VistaSystem::DoInit() -> unknown init routine " << endl;
			return false;
			break;
		};

	// ========================================================================
	// ========================================================================

	std::string sDisplaySystemSectionName = "SYSTEM";
	if(IsClient())
		sDisplaySystemSectionName = GetVistaClusterClient()->GetClientSectionName();
	else if(IsServer())
		sDisplaySystemSectionName = GetVistaClusterServer()->GetServerSectionName();
	else if(GetIsMaster())
		sDisplaySystemSectionName = GetVistaClusterMaster()->GetMasterSectionName();
	else if(GetIsSlave())
		sDisplaySystemSectionName = GetVistaClusterSlave()->GetSlaveSectionName();


	// attach display manager to graphics manager
	// note that we can not do this until all
	// the managers are created
	m_pNewWorld->SetDisplayManager(m_pDisplayManager);

	// setup display systems
	if (!m_pDisplayManager->CreateDisplaySystems(sDisplaySystemSectionName, GetDisplayIniFile()))
	{
		vkernout << " [ViSystem] - unable to create display systems..." << endl;
		return false;
	}

	if(GetInteractionManager())
		if(!SetupBasicInteraction()) // try to setup new interaction iff desired
			return false;

	std::string sGraphicsSectionName = oProfiler.GetTheProfileString(GetSystemSectionName(),
																		 "GRAPHICSSECTION",
																		 "GRAPHICS",
																		 GetGraphicsIniFile());

	bool bShowCursor = oProfiler.GetTheProfileBool(sGraphicsSectionName,
													   "ShowCursor",
													   ((IsClient() || GetIsSlave()) ? false : true),
													   GetGraphicsIniFile());

	m_pNewWorld->GetDisplayManager()->GetDisplayBridge()->SetShowCursor(bShowCursor);


	// now as all is set up, we can set the background
	{
		std::string strColor;
		oProfiler.GetTheProfileString(sGraphicsSectionName,
										"BackgroundColor",
										"0.0, 0.4, 0.8",
										strColor,
										GetGraphicsIniFile());

		float bgColor[3] = {0.0f, 0.04f, 0.8f};
		if(!VistaAspectsConversionStuff::ConvertStringTo3Float(strColor,
			bgColor[0], bgColor[1], bgColor[2]))
		{
			vkernerr << "[VistaSystem] - Error while parsing BackgroundColor from "
					  << GetGraphicsIniFile() << ":\n"
					  << "               '" << strColor << "' has an unrecognized format!\n"
					  << "                using color "
					  << bgColor[0] << ", " << bgColor[1] << ", " << bgColor[2] << std::endl;
		}
		GetGraphicsManager()->SetBackgroundColor(VistaColorRGB(bgColor));
	}

	// setup WebInterface if requested
	{
		int port = oProfiler.GetTheProfileInt(
			sGraphicsSectionName,
			"WebInterface",
			-1,
			GetGraphicsIniFile());
		if(port != -1)
		{
#if defined (VISTA_SYS_OPENSG)
			vkernout << "[VistaSystem] Opening WebInterface on port " << port << std::endl;
			((VistaOpenSGSystemClassFactory*)m_pSystemClassFactory)
				->SetWebInterfaceEnabled(true, port);
#else
			vkernerr << "[VistaSystem] WebInterface is not supported." << std::endl;
#endif
		}
	}

	// ========================================================================
	// ========================================================================

	// call event to signal the initialization process
	PrintMsg(" [ViSystem]   Application init ...\n[ViSystem]---------------------------------------\n");


	// note that we treat init differently when running in cluster mode
	// it will be triggered be the ClusterServer

	IndicateSystemProgress(iStageCount++, "Application Init-phase", false);
	// in either case we will run through the ini loop
	// (client/server and standalone)
	m_pSystemEvent->SetId(VistaSystemEvent::VSE_INIT);
	m_pEventManager->ProcessEvent(m_pSystemEvent);


	if(IsClient())
	{
		// right, we finished init, so we bring in any hack we
		// might need, concerning our presence as being a client
		GetVistaClusterClient()->PostInitSection(m_pInteractionManager);
	}

	if(IsServer())
	{
		// the server may or may not be faster than the clients
		// but we will wait until any client has ACK'ed its init
		GetVistaClusterServer()->WaitForClientInit();
	}

	if(GetIsMaster())
	{
		// the master may or may not be faster than the slaves.
		// wait here, until all slaves have initialized
		GetVistaClusterMaster()->WaitForSlaveInit();
	}

	if(GetIsSlave())
	{
		// init is done, post this to the master, which is probably
		// waiting right now
		GetVistaClusterSlave()->PostSystemInitEvent();
	}

	PrintMsg(" [ViSystem]---------------------------------------\n[ViSystem]   ... init done.\n");

	// preparation for the WTuniverse_go()
	if (oProfiler.GetTheProfileInt (gSystemSectionName, "displayactive",
										1, (char*)sConfigFileName.c_str()) ==1 )
	{
		if(m_pNewWorld)
		{
			if (this->m_strModelFile.size () > 0)
			{
				VistaSG* pSG = m_pNewWorld->GetSceneGraph();
				VistaGroupNode* pSGRoot = pSG->GetRoot();
				if(pSGRoot)
					pSGRoot->AddChild(pSG->LoadNode(m_strModelFile, VistaSG::OPT_NONE, m_nModelScale));
			}
		}
	}

	vstr::GetStreamManager()->SetInfoInterface( new VistaKernelStreamInfoInterface( this ) );


	PrintMsg (" ##### ViSTA SYSTEM INITIALIZATION END...##### \n");


	m_bInitialized = true;

	IndicateSystemProgress(iStageCount++, "INIT DONE", true);
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupEventManager                                           */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupEventManager ()
{
	// Create event manager and call its Init-method
	PrintMsg (" ##### ViSTA Event Manager ##### \n");

	if(!m_pEventManager)
	{
		vkernout << " [ViSystem] No EventManager generated - program exit!\n";
		m_bInitialized = false;
		return false;
	}


	m_pSystemEvent = new VistaSystemEvent;
	if (!m_pSystemEvent)
	{
		vkernout << " [ViSystem] No VistaSystemEvent generated - program exit!\n";
		m_bInitialized = false;
		return false;
	}

	// register self (i.e. VistaSystem) as system event handler

	m_pEventManager->AddEventHandler(this, VistaSystemEvent::GetTypeId(),
									 VistaEventManager::NVET_ALL,
									 VistaEventManager::PRIO_FALLBACK);

	// we want to catch interaction events in order to keep clean the msg port
	// communication!
	m_pEventManager->SetPriority( this, VistaSystemEvent::GetTypeId(),
											VistaSystemEvent::VSE_UPDATE_INTERACTION,
											2*VistaEventManager::PRIO_HIGH );

	m_pEventManager->AddEventHandler(this, VistaExternalMsgEvent::GetTypeId(),
									 VistaEventManager::NVET_ALL,
									 VistaEventManager::PRIO_FALLBACK);


	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupPickManager                                            */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupPickManager ()
{
	// Create Pick manager and call its Init-method
	PrintMsg (" ##### ViSTA Pick Manager ##### \n");
	m_pPickManager = new VistaPickManager(GetEventManager());
	if (m_pPickManager)
	{
		m_pPickManager->Init(GetGraphicsManager()->GetSceneGraph()->GetRoot());
	}
	else
	{
		vkernout << " [ViSystem] No VistaPickManager generated - program exit!\n";
		m_bInitialized = false;
		return false;
	}

	return true;
}

const std::list<std::string>& VistaSystem::GetIniSearchPaths() const
{
	return m_liSearchPath;
}

bool VistaSystem::SetIniSearchPaths(const std::list<std::string> &liSearchPaths)
{
	if(m_bLockSearchPath) // user gave -inisearchpaths option
		return false;

	m_liSearchPath.clear();
	for(list<string>::const_iterator it = liSearchPaths.begin();
		it != liSearchPaths.end();
		++it)
	{
		/// @todo check and only add existing paths!
		if(isAbsolutePath(*it))
		{
			m_liSearchPath.push_back(*it);
		}
		else
		{
			string absPath = getAbsolutePath(*it);
			// assure that paths all do end with the separator to ease searching for files later
			if(absPath[absPath.size()-1] != VistaFileSystemDirectory::GetOSSpecificSeparator().c_str()[0])
			{
				//vkernerr << absPath << " absPath[absPath.size()-1]: '" << absPath[absPath.size()-1] << "' appending separator:\n";
				absPath.append(VistaFileSystemDirectory::GetOSSpecificSeparator());
				//vkernerr << absPath << endl;
			}
			m_liSearchPath.push_back(absPath);
		}
	}
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupDisplayManager                                         */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupDisplayManager ()
{
	// create a new display manager object
	PrintMsg (" ##### ViSTA Display Manager ##### \n");
	m_pDisplayManager = m_pSystemClassFactory->CreateDisplayManager();

	if (!m_pDisplayManager)
		return false;

	// register display manager as standard display event handler
	m_pEventManager->AddEventHandler(m_pDisplayManager, VistaDisplayEvent::GetTypeId(),
									 VistaEventManager::NVET_ALL,
									 VistaEventManager::PRIO_FALLBACK);


	std::string sDisplaySystemSectionName = "SYSTEM";
	if(IsClient())
		sDisplaySystemSectionName = GetVistaClusterClient()->GetClientSectionName();
	else if(IsServer())
		sDisplaySystemSectionName = GetVistaClusterServer()->GetServerSectionName();
	else if(GetIsMaster())
		sDisplaySystemSectionName = GetVistaClusterMaster()->GetMasterSectionName();
	else if(GetIsSlave())
		sDisplaySystemSectionName = GetVistaClusterSlave()->GetSlaveSectionName();


	return m_pDisplayManager->GetDisplayBridge()->CreateDisplaysFromIniFile(GetDisplayIniFile(),
																			sDisplaySystemSectionName);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupCluster                                                */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupCluster (VistaClusterServer *&pServer,
								 VistaClusterClient *&pClient,
								 VistaClusterMaster *&pMaster,
								 VistaClusterSlave  *&pSlave )
{
	bool bRet = true;
	switch (m_iClusterMode)
	{

	case VCM_STANDALONE:
		PrintMsg( "Running ViSTA  standalone (Cluster deactivated!)\n" );
		break;
	case VCM_SERVER:
		PrintMsg( "Running ViSTA as Cluster-Server\n" );
		pServer = new VistaClusterServer( GetEventManager(), m_strSystemName);
		break;
	case VCM_MASTER:
	{
		PrintMsg( "Running ViSTA as Cluster-Master\n");
		pMaster = new VistaClusterMaster( GetEventManager(), m_strSystemName );
		break;
	}
	case VCM_SLAVE:
	{
		PrintMsg( "Running ViSTA as Cluster-Slave\n");
		pSlave = new VistaClusterSlave( GetEventManager(), m_strSystemName );
		break;
	}
	case VCM_CLIENT:
		PrintMsg( "Running ViSTA as Cluster-Client\n" );
		pClient = new VistaClusterClient( GetEventManager(), m_strSystemName);
		break;
	default:
		PrintMsg( "Selected ClusterMode is not supported by this Version of ViSTA!\n" );
		bRet = false;
		break;

	}

	return bRet;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupGraphicsManager                                        */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupGraphicsManager ()
{
	VistaProfiler    oProfiler;
	// get ini file name
	std::string       sConfigFileName = GetGraphicsIniFile();

	//*******************************************************************
	// create graphics system
	//*******************************************************************
	PrintMsg (" ##### ViSTA Graphics Manager ##### \n");

	std::string sGraphicsSection;
	std::string sGraphicsType;
	oProfiler.GetTheProfileString(      gSystemSectionName,
											"GRAPHICSSECTION",
											"GRAPHICS",
											sGraphicsSection,
											sConfigFileName);
	//Create new GraphicsManager
	m_pNewWorld = m_pSystemClassFactory->CreateGraphicsManager();
	if (m_pNewWorld)
	{
		IVistaGraphicsBridge*   pGrBridge       = m_pSystemClassFactory->CreateGraphicsBridge();
		IVistaNodeBridge*               pNodeBridge     = m_pSystemClassFactory->CreateNodeBridge();
		if(pGrBridge && pNodeBridge)
			m_pNewWorld->InitScene( pNodeBridge,
									pGrBridge,
									GetGraphicsIniFile());
		else
		{
			delete pGrBridge;
			delete pNodeBridge;
			m_bInitialized = false;
			return false;
		}

		m_pEventManager->AddEventHandler(m_pNewWorld, VistaGraphicsEvent::GetTypeId(),
										 VistaEventManager::NVET_ALL,
										 VistaEventManager::PRIO_FALLBACK);
	}
	else
	{
		//Ini-section wasn't valid
		vkernout << "[ViSystem] Init-String \"GraphicsManager\" contains no valid data!" << endl;
		m_pNewWorld = NULL;
		vkernout << "[ViSystem] No VistaGraphicsManager generated - program exit!" << endl;
		m_bInitialized = false;
		return false;
	}
	return true;
}

bool VistaSystem::SetupBasicInteraction()
{

	// this is a beast-like method, sorry for that.
	// let's go through this step by step.

	// cache the config file name
	std::string       sConfigFileName = GetInteractionIniFile();

	// setup oProfiler with proper search path
	VistaProfiler oProfiler;

	// Load Plugins from plugin directory
	// works for dll-builds only
	//LoadPlugins( sConfigFileName );

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// setup basic key-stroke support. Before that: create keyboard system control
	m_pSystemControl = new VistaKeyboardSystemControl;

	m_pSystemControl->BindAction('q',
								new VistaQuitCommand(this),
								"Quits the ViSTA updateloop and exits the application");
	m_pSystemControl->BindAction('?',
								new VistaShowAvailableCommands(m_pSystemControl,this),
								"Show avaliable keys");
	m_pSystemControl->BindAction('F',
								new VistaToggleFramerateCommand(GetGraphicsManager()),
								"Toggle framerate info");
	m_pSystemControl->BindAction('E',
								 new TVistaDebugToConsoleCommand<VistaEventManager>(GetEventManager()),
								 "Debug printout for EventManager");
	m_pSystemControl->BindAction('G',
								 new TVistaDebugToConsoleCommand<VistaGraphicsManager>(GetGraphicsManager()),
								 "Debug printout for GraphicsManager");
	m_pSystemControl->BindAction('D',
								 new TVistaDebugToConsoleCommand<VistaDisplayManager>(GetDisplayManager()),
								 "Debug printout for DisplayManager");
	m_pSystemControl->BindAction('P',
								 new TVistaDebugToConsoleCommand<VistaPickManager>(GetPickManager()),
								 "Debug printout for PickManager");
	m_pSystemControl->BindAction('C',
								 new VistaToggleCursorCommand(GetDisplayManager()),
								 "Enable / Disable Cursor");
	m_pSystemControl->BindAction( 'I',
								 new VistaPrintProfilerOutputCommand,
								 "Print Profiling Information");
	m_pSystemControl->BindAction( VISTA_KEY_DOWNARROW, VISTA_KEYMOD_CTRL,
								 new VistaChangeEyeDistanceCommand( -0.001f, GetDisplayManager() ),
								 "Decrease x eye offset by 0.001" );
	m_pSystemControl->BindAction( VISTA_KEY_UPARROW, VISTA_KEYMOD_CTRL,
								 new VistaChangeEyeDistanceCommand( 0.001f,	GetDisplayManager() ),
								 "Increase x eye offset by 0.001" );
	m_pSystemControl->BindAction( 'V', new VistaToggleVSyncCommand( GetDisplayManager() ),
									 "Toggle VSync on/off" );

	// in master OR slave mode: construct debug printout
	if(GetVistaClusterMaster())
	{
		m_pSystemControl->BindAction('*',
									new TVistaDebugToConsoleCommand<VistaClusterMaster>( GetVistaClusterMaster() ),
									"Show Cluster-MASTER debug output");
	}
	else if(GetVistaClusterSlave())
	{
		m_pSystemControl->BindAction('*',
									new TVistaDebugToConsoleCommand<VistaClusterSlave>( GetVistaClusterSlave() ),
									"Show Cluster-SLAVE debug output");

	}

	m_pSystemControl->BindAction('$',
								new VistaToggleFrustumCullingCommand(GetGraphicsManager()),
								"Enable / Disable frustum culling (OpenSG specific)" );
	m_pSystemControl->BindAction('%',
								new VistaToggleOcclusionCullingCommand(GetGraphicsManager()) ,
								"Enable / Disable occlusion culling (OpenSG specific)" );
	m_pSystemControl->BindAction('&',
								new VistaToggleBBoxDrawingCommand(GetGraphicsManager()),
								"Enable / Disable BBox drawing (OpenSG specific)" );

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// the following does register the mapping from a trigger (key) in the ini
	// to some code that takes care for a proper parsing and setup for what it does
	// mean

	// as an example: when the key "DRIVERLOGGING" is encountered in the ini file
	// the VistaDriverLoggingConfigurator is used for setup. It is passed the section
	// where the DRIVERLOGGING key was found as a start and can decide on deeper
	// ini parsing and so on.
	m_pConfigurator->RegisterConfigurator("DRIVERLOGGING", new VistaDriverLoggingConfigurator);

	// this goes for HISTORY keys.
	m_pConfigurator->RegisterConfigurator("HISTORY", new VistaGenericHistoryConfigurator);

	// this goes for generic parameters.
	m_pConfigurator->RegisterConfigurator("PARAMETERS", new VistaDriverParameterConfigurator);

	// REGISTER CONFIGURATORS AS SUPPORTED BY THE SYSTEM
	// window stuff is kernel specific, so it is passed the display manager. Otherwise
	// is can not work.
	VistaWindowConfigurator *pWC = new VistaWindowConfigurator(GetDisplayManager());

	// now... that is tricky: some keys depend on the setup of other keys BEFOREHAND.
	// for example, the PROTOCOL key needs an ATTACHONLY to be setup BEFOREHAND.
	// knowing these dependencies is not easy and has to be deduced from the code.
	// cycles are going to make a bad day here. Dependencies can be modelled as a list of strings
	// that contain all keys that have to be setup BEFORE the key that is dependent on them (simple, not?)

	std::list<std::string> liDepends;

	// add attach-only
	liDepends.push_back("ATTACHONLY");

	// liDepends is given as 3rd argument. So PROTOCOL depends on ATTACHONLY
	m_pConfigurator->RegisterConfigurator("PROTOCOL", new VistaDriverProtocolConfigurator, liDepends);

	// this is just for re-use of the container: clear dependency
	liDepends.clear();

	liDepends.push_back("DEFAULTWINDOW");
	// same as above: WINDOWS depends on DEFAULTWINDOW
	m_pConfigurator->RegisterConfigurator("WINDOWS", pWC, liDepends);

	// same as above: re-use container
	liDepends.clear();

	m_pConfigurator->RegisterConfigurator("DEFAULTWINDOW", pWC);
	liDepends.push_back("HISTORY");
	liDepends.push_back("WINDOWS"); // some sensors might only be present on the connection to a window

	// SENSORS -> { WINDOWS, HISTORY }
	m_pConfigurator->RegisterConfigurator("SENSORS", new VistaSensorMappingConfigurator, liDepends);
	liDepends.clear();

	liDepends.push_back("PROTOCOL");
	m_pConfigurator->RegisterConfigurator("REFERENCE_FRAME",
											new VistaDriverReferenceFrameConfigurator, liDepends );
	liDepends.clear();

	liDepends.push_back("PROTOCOL");
	liDepends.push_back("ATTACHONLY");
	liDepends.push_back("REFERENCE_FRAME");
	// CONNECTIONS -> { ATTACKONLY, PROTOCOL }
	// and yes... this is transitive when seen in relation to PROTOCOL
	m_pConfigurator->RegisterConfigurator("CONNECTIONS", new VistaConnectionConfigurator, liDepends);
	
	liDepends.clear();
	

	m_pConfigurator->RegisterConfigurator("ATTACHONLY", new VistaDriverAttachOnlyConfigurator);
	m_pConfigurator->RegisterConfigurator("IDENTIFICATION", new VistaDeviceIdentificationConfigurator);

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SETUP DFN
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// THIS IS UTTERLY IMPORTANT TO CALL! InitVdfn().
	VdfnUtil::InitVdfn( GetDriverMap(), GetDfnObjectRegistry() );

	// register kernel-specific nodes
	VistaKernelDfnNodeCreators::RegisterNodeCreates( this );

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// END SETUP DFN
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// tricky stuff: setting up the devices for the standalone / master mode
	// and (more below) for the slave / client mode
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	LoadDeviceDrivers();

	// SETUP INTERACTIONCONTEXTS
	std::list<std::string> liInteractionContexts;
	oProfiler.GetTheProfileList("SYSTEM", "INTERACTIONCONTEXTS", "", liInteractionContexts, GetInteractionIniFile());
	vkernout << " [ViSys]: Found " << liInteractionContexts.size() << " entries in INTERACTIONCONTEXTS\n";

	for(std::list<std::string>::const_iterator licit = liInteractionContexts.begin();
		licit != liInteractionContexts.end(); ++licit)
	{
		int nSlaveId = GetVistaClusterClient() ? GetVistaClusterClient()->GetClientIndex() : -1;

		VistaInteractionContext *pCtx = SetupContext( GetInteractionManager(),
													   GetDisplayManager(),
													   GetEventManager(),
													   GetClusterAux(),
													   GetKeyboardSystemControl(),
													   this,
													   (*licit),
													   GetInteractionIniFile(),
													   GetIniFile(),
													   VistaInteractionManager::PRIO_DONTCARE,
													   nSlaveId);
		if(pCtx)
			m_liCreateCtxs.push_back(pCtx);
	}

	// SETUP PROJECTION UPDATES (iff wanted)
	std::list<std::string> liDisplaySystems;

	std::string sDisplaySystemSectionName = "SYSTEM";
	if(IsClient())
		sDisplaySystemSectionName = GetVistaClusterClient()->GetClientSectionName();
	else if(IsServer())
		sDisplaySystemSectionName = GetVistaClusterServer()->GetServerSectionName();
	else if(GetIsSlave())
		sDisplaySystemSectionName = GetVistaClusterSlave()->GetSlaveSectionName();
	else if(GetIsMaster())
		sDisplaySystemSectionName = GetVistaClusterMaster()->GetMasterSectionName();

	oProfiler.GetTheProfileList(sDisplaySystemSectionName, "DISPLAYSYSTEMS", "", liDisplaySystems, GetDisplayIniFile());
	for(std::list<std::string>::const_iterator it = liDisplaySystems.begin();
		it != liDisplaySystems.end(); ++it)
	{
		std::string strDSysName = oProfiler.GetTheProfileString(*it, "NAME", "", GetDisplayIniFile());
		if(strDSysName.empty())
			strDSysName = *it; // copy section name?

		VistaDisplaySystem *pSys = GetDisplayManager()->GetDisplaySystemByName(strDSysName);
		if(pSys)
		{
			// create a userplatform for this dispsys
			// note that this will mean to have a non-related bijection between the
			// display system and the user platform, can lead to dangling pointers
			// for now, we assume that the display system will last throughout the
			// simulation run.
			VistaUserPlatform *pPlat = new VistaUserPlatform( GetGraphicsManager()->GetSceneGraph(), pSys );
			m_mpUserPlatforms[pSys] = pPlat; // add to memory (for later disposal during deconstruction)

			// register this with the object registry
			GetDfnObjectRegistry()->SetObject( std::string("CAM:")+pSys->GetNameForNameable(),
											   pSys->GetProperties(),
											   pPlat->GetPlatformTrans() );
		}
	}
	return true;
}

bool VistaSystem::LoadDeviceDrivers()
{
	VistaProfiler oProfiler;
	oProfiler.SetEnvironmentVariablesEnabled( true );
	std::string sConfigFileName = GetInteractionIniFile();

	// first, we extract the list of pathes to search for driver plugins
	std::string sDriverPlugins;
	oProfiler.GetTheProfileString( GetSystemSectionName(), "DRIVERPLUGINDIRS", "",
									sDriverPlugins, sConfigFileName );
	if( sDriverPlugins.empty() )
	{
		m_liDriverPluginPathes.push_back( "." );
	}
	else
	{
		std::string sTmp = VistaEnvironment::ReplaceOSEnvironemntPathSeparators( sDriverPlugins, ',' );
		VistaAspectsConversionStuff::ConvertToList( sTmp, m_liDriverPluginPathes );
	}
	
	std::list<std::string> liDriverList;
	std::map<std::string,_drDevHlp> mpCreatedDrivers;

	oProfiler.GetTheProfileList( GetSystemSectionName(), "DEVICEDRIVERS", "", 
									liDriverList, sConfigFileName );

	vkernout << "Loading Driver Plugins" << std::endl;	
	// Load Plugins for all Drivers
	for( std::list<std::string>::const_iterator itDriver = liDriverList.begin();
		itDriver != liDriverList.end(); ++itDriver )
	{
		std::string sName = oProfiler.GetTheProfileString( (*itDriver), "NAME", (*itDriver), sConfigFileName);
		std::string sType = oProfiler.GetTheProfileString( *itDriver, "TYPE", "", sConfigFileName);
		if( sType.empty() ) // but empty type is a no-go
		{
			vkernerr << "[ViSys]: ### ERROR ### no type name given for the device [" << *itDriver << "]" << std::endl;
			continue;
		}
		// check if we have a name clash
		if( m_pDriverMap->GetIsDeviceDriver(sName) )
		{
			// yes. no-go!
			PrintMsg(std::string(" [ViSys]: ### WARNING : driver of type [")
						   + *itDriver + std::string("] with name [")
						   + sName + std::string("] already registered -- SKIPPING!\n"));
			continue;
		}


		std::string sDriverPlugin = oProfiler.GetTheProfileString( (*itDriver), "DRIVERPLUGIN",
																	"", sConfigFileName );
		std::string sTranscoderPlugin = oProfiler.GetTheProfileString( (*itDriver), "TRANSCODERPLUGIN",
																	"", sConfigFileName );

		IVistaDriverCreationMethod *pM = m_pInteractionManager->GetDriverCreationMethod(sType);

		if(!pM)
		{
			if( LoadDriverPlugin( sType, sDriverPlugin, sTranscoderPlugin ) == false )
			{
				/** DRIVERTODO warning */
				vkernerr << "Could not load Plugin for Driver [" << sName 
							<<  "] of Type [" << sName << "]" << std::endl;
				continue;
			}
		}		
	}

	if(!IsClient() && !GetIsSlave())
	{
		// iterate over all devices given, but in the driver section
		for( std::list<std::string>::const_iterator itDriver = liDriverList.begin();
			itDriver != liDriverList.end(); ++itDriver )
		{
			std::string sName = oProfiler.GetTheProfileString( (*itDriver), "NAME", (*itDriver), sConfigFileName);
			std::string sType = oProfiler.GetTheProfileString( *itDriver, "TYPE", "", sConfigFileName);
			if( sType.empty() ) // but empty type is a no-go
			{
				vkernerr << "[ViSys]: ### ERROR ### no type name given for the device [" << *itDriver << "]" << std::endl;
				continue;
			}
			// check if we have a name clash
			if( m_pDriverMap->GetIsDeviceDriver(sName) )
			{
				// yes. no-go!
				PrintMsg(std::string(" [ViSys]: ### WARNING : driver of type [")
							   + *itDriver + std::string("] with name [")
							   + sName + std::string("] already registered -- SKIPPING!\n"));
				continue;
			}


			std::string sDriverPlugin = oProfiler.GetTheProfileString( (*itDriver), "DRIVERPLUGIN",
																		"", sConfigFileName );
			std::string sTranscoderPlugin = oProfiler.GetTheProfileString( (*itDriver), "TRANSCODERPLUGIN",
																		"", sConfigFileName );

			IVistaDriverCreationMethod *pM = m_pInteractionManager->GetDriverCreationMethod(sType);		

			// create driver of this type and name and
			// register with the driver map
			if(!pM)
			{
				this->PrintMsg(std::string(" [ViSys]: ### WARNING : no driver of type [")
							   + sType + std::string("] registered, check if the DRIVERPLUGINS directory is set correctly for your platform!\n"));
				continue;
			}

			// a bit stoopid to read: this creates the driver (operator call)
			IVistaDeviceDriver *pDriver = (*pM).CreateDriver();
			if(!pDriver)
			{
				this->PrintMsg(std::string(" [ViSys]: ### WARNING : could not create driver [")
							   + *itDriver + std::string("] from factory method!\n"));
				continue;
			}

			this->PrintMsg(std::string(" [ViSys]: ### ADDING driver of type [")
						   + sType
						   + std::string("] with name [")
						   + sName
						   +std::string("]\n"));

			// drivers can be sorted by priority for later use
			unsigned int nPrio = oProfiler.GetTheProfileInt( *itDriver, "PRIORITY", 0, sConfigFileName );
			std::list<std::string> liDepends;

			// compound drivers can depend on other drivers to exist and setup beforehand.
			// this is modelled by an explicit dependency. for example:
			// a compound driver collects input from MOUSE and KEYBOARD, virtually
			// resulting in a 2DOF device with a number of buttons
			// COMPOUND -> { MOUSE, KEYBOARD }. Otherwise, the setup of the COMPOUND
			// may fail, just because the user swapped the entries in the DEVICEDRIVERS
			// section of the interaction ini file. The list can be empty, so in that case,
			// the creation and setup order is arbitrary.
			oProfiler.GetTheProfileList( *itDriver, "DEPENDSON", "", liDepends, sConfigFileName );
			mpCreatedDrivers[sName] = _drDevHlp(pDriver, sType, sName, *itDriver, nPrio, liDepends);
		}

		// make a topological sort of the driver, respecting dependencies on other drivers
		VistaTopologyGraph<_drDevHlp*> depends;

		// respect dependency on other drivers:
		for(std::map<std::string, _drDevHlp>::iterator drList1 = mpCreatedDrivers.begin();
			drList1 != mpCreatedDrivers.end(); ++drList1)
		{
			depends.AddNode( &(*drList1).second  );
			std::list<std::string> &li = (*drList1).second.m_liDependsOn;
			for( std::list<std::string>::iterator c = li.begin(); c != li.end(); ++c)
			{
				std::map<std::string, _drDevHlp>::iterator o = mpCreatedDrivers.find( *c );
				if(o == mpCreatedDrivers.end())
					continue; // skip
				depends.AddDependency( &(*o).second, &(*drList1).second );
			}
		}


		// sort
		std::list<VistaTopologyGraph<_drDevHlp*>::Node*> liConfigOrder = depends.CalcTopology();

		// iterate over this order.
		for(std::list<VistaTopologyGraph<_drDevHlp*>::Node*>::const_iterator drList = liConfigOrder.begin();
			drList != liConfigOrder.end(); ++drList)
		{
			vkernout << " [ViSys]: Creation and initializing driver ["
				 << (*drList)->m_oElement->m_strName << "] from section ["
				 << (*drList)->m_oElement->m_strSection << "]\n";

			std::list<std::string> liKeys;
			oProfiler.GetTheProfileSectionEntries( (*drList)->m_oElement->m_strSection,
												 liKeys, GetInteractionIniFile());

			// get a sorted order for the handling of the keys.
			// we want to evaluate the triggers with the above given dependencies,
			// but not all drivers have the same set of configurations,
			// so first, we figure out: which keys are used and which add an
			// dependency to other keys

			// in a nutshell: the keys might have a dependency, for example we should not
			// configure the protocol, when the connection is not yet setup and running.
			// and we only want to configure those keys that are really given in the ini
			// for example a non-given ATTACHONLY key should not be configured (as it
			// might be evaluated to true or false, ruining the default setup of the
			// driver)
			VistaTopologyGraph<std::string> liConfigDeps;
			for(std::list<std::string>::const_iterator keyIT = liKeys.begin();
				keyIT != liKeys.end(); ++keyIT)
			{
				// for all keys, we look up the list of dependencies which was given
				// as a construction argument to the configurator
				std::list<std::string> liDependsOn = m_pConfigurator->GetDependsForTrigger( *keyIT );

				// for any key in thise dependency list, we check whether it really is a
				// dependency, or not
				if(liDependsOn.empty())
				{
					// this trigger is used, but has no dependency to any other key
					// we add it to the dependency graph as a single node
					if(!liConfigDeps.HasNodeValue(*keyIT)) // this checks the case when a user has given
														   // a key twice in the ini as a mistake
						liConfigDeps.AddNode( *keyIT ); // add it as lonely node in the graph
				}
				else
				{
					// this trigger has a dependency, but the other triggers must not be
					// part of this configuration, so we check
					for(std::list<std::string>::const_iterator cit = liDependsOn.begin();
						cit != liDependsOn.end(); ++cit)
					{
						// is the dependency part of this configuration?(e.g., is it given in the ini?)
						if( std::find( liKeys.begin(), liKeys.end(), *cit ) != liKeys.end() )
							liConfigDeps.AddDependency( *cit, *keyIT ); // yes, add only iff part of the configuration
						else
						{
							if(!liConfigDeps.HasNodeValue( *keyIT )) // not a node, yet...
								liConfigDeps.AddNode( *keyIT ); // add the node without dependency, as a floating node
						}
					}
				}
			}

			// calc a topological sort for the keys
			std::list<VistaTopologyGraph<std::string>::Node*> liConfig = liConfigDeps.CalcTopology();

			// iterate over the sorted list of triggers for the configuration
			for(std::list<VistaTopologyGraph<std::string>::Node*>::const_iterator keyit = liConfig.begin();
				keyit != liConfig.end(); ++keyit)
			{
				VistaDriverPropertyConfigurator::IConfigurator *pConf = m_pConfigurator->RetrieveConfigurator( (*keyit)->m_oElement );
				if(pConf)
				{
					// we have a configurator for this trigger
					VistaPropertyList oProps;

					// first build the PropertyList
					if(pConf->GeneratePropertyListFromInifile( GetInteractionIniFile(),
														(*drList)->m_oElement->m_strSection, oProps))
					{
						// that did work, go an configure using the PropertyList interface
						if(pConf->Configure((*drList)->m_oElement->m_pDriver, oProps) == false)
						{
							vkernerr << "[ViSys]: Unable to configure element type ["
								<< (*keyit)->m_oElement << "] -- WHAT NOW?" << std::endl;
						}
					}
					else
					{
						vkernerr << "[ViSys]: Ini2PROP conversion failed for [" << (*keyit)->m_oElement << "]" << std::endl;
					}
				}
				else
				{
					// in principle, this can happen and be OK (for example, there is no
					// configurator for TYPE. But in case of simple mis-spelling, this might
					// be a worthy output.
					if( (*keyit)->m_oElement != "TYPE" )
					{
						vkernerr << "[ViSys]: no configurator for key [" << (*keyit)->m_oElement << "]" << std::endl;
					}
				}
			}

			// this finally tries to ''activate'' the device
			if( (*drList)->m_oElement->m_pDriver->Connect() )
			{
				// ok, worked, add to interaction manager for update
				m_pInteractionManager->AddDeviceDriver((*drList)->m_oElement->m_strName,
														  (*drList)->m_oElement->m_pDriver,
														  (*drList)->m_oElement->m_nPrio);

				// ok, in case the driver had a sensor mapping aspect, the
				// sensor mapping configurator has taken care of setting up
				// the sensor names. In case this driver adds sensors dynamically
				// after a connect, this is not working well, so we create the proper
				// names AFTER a successful connect
				std::list<std::string> liSensorNames;

				oProfiler.GetTheProfileList( (*drList)->m_oElement->m_strSection, "SENSORS", liSensorNames, GetInteractionIniFile() );
				for(std::list<std::string>::const_iterator cit = liSensorNames.begin();
					cit != liSensorNames.end(); ++cit )
				{
					// see whether the user specified the proper index
					unsigned int nIdx = oProfiler.GetTheProfileInt( *cit, "RAWID", -1, GetInteractionIniFile() );
					if(nIdx == -1)
						continue;

					// see whether the user *wants* to set a name
					std::string strName = oProfiler.GetTheProfileString( *cit, "NAME", "", GetInteractionIniFile() );
					if(strName.empty())
						continue; // no

					// get sensor by index
					VistaDeviceSensor *pSensor = (*drList)->m_oElement->m_pDriver->GetSensorByIndex(nIdx);
					if(pSensor)
						pSensor->SetSensorName( strName ); // set name
				}
			}
			else
			{
				vkernerr << "[ViSys]: Could not sucessfully connect the driver, so it will not be enabled / registered." << std::endl;
				delete (*drList)->m_oElement->m_pDriver;
			}

		}


	} // !IsClient() && !GetIsSlave()
	else // IsClient() || GetIsSlave()
	{
		// in client mode, we need a driver that is capable of receiving the incoming sensor data during
		// event dispatching. However, we do not want to create the physical driver, as this device might
		// not even be connected to the host machine. So we create a dummy / shallow driver for every driver
		// request and set it up according to the specification.

		std::list<std::string> liDriverList;
		std::map<std::string,_drDevHlp> mpCreatedDrivers;

		if(oProfiler.GetTheProfileList(GetSystemSectionName(), "DEVICEDRIVERS", "", liDriverList, sConfigFileName))
		{
			unsigned int nDriverHistorySize = 1;
			for(std::list<std::string>::const_iterator cit = liDriverList.begin();
				cit != liDriverList.end(); ++cit)
			{

				// find proper section for the driver
				std::string sName  = oProfiler.GetTheProfileString( *cit, "NAME", *cit, sConfigFileName);
				std::string sType  = oProfiler.GetTheProfileString( *cit, "TYPE", "", sConfigFileName);
				if(sType.empty())
				{
					vkernerr << "[ViSys]: ### ERROR ### no type name given for the device [" << *cit << "]" << std::endl;
					continue;
				}

				nDriverHistorySize = oProfiler.GetTheProfileInt( *cit, "HISTORY", nDriverHistorySize, sConfigFileName);
				if(m_pDriverMap->GetIsDeviceDriver(sName))
				{
					this->PrintMsg(std::string(" [ViSys]: ### WARNING : driver of type [")
								   + *cit + std::string("] with name [")
								   + sName + std::string("] already registered -- SKIPPING!\n"));
					continue;
				}

				// create a shallow driver for this type
				VistaShallowDriver *pDriver = dynamic_cast<VistaShallowDriver*>(
							(*VistaShallowDriver::GetDriverFactoryMethod()).CreateDriver() );

				// register with the driver map
				if(!pDriver)
				{
					this->PrintMsg(std::string(" [ViSys]: ### WARNING : could not create driver [")
								   + *cit + std::string("] from factory method!\n"));
					continue;
				}

				this->PrintMsg(std::string(" [ViSys]: ### ADDING driver of type [")
							   + sType
							   + std::string("] with name [")
							   + sName
							   +std::string("]\n"));

				unsigned int nPrio = oProfiler.GetTheProfileInt( *cit, "PRIORITY", 0, sConfigFileName );
				mpCreatedDrivers[sName] = _drDevHlp(pDriver, sType, sName, *cit, nPrio, std::list<std::string>());
			}

			for(std::map<std::string,_drDevHlp>::const_iterator drList = mpCreatedDrivers.begin();
				drList != mpCreatedDrivers.end(); ++drList )
			{
				VistaDriverSensorMappingAspect *pSensorMapping = NULL;
				// for this shallow driver, rebuild the settings in the driver creation method
				// 1. claim original method
				IVistaDriverCreationMethod *pOrigin = m_pDriverMap->GetDriverCreationMethod( (*drList).second.m_strTypeName );
				if(pOrigin)
				{
					std::list<unsigned int> liTypes = pOrigin->GetTypes();
					if( !liTypes.empty() )
					{
						// this driver sensors - create a sensor mapping aspect, and register
						// the sensors (done in ctor)
						pSensorMapping = new VistaDriverSensorMappingAspect( pOrigin );
						(*drList).second.m_pDriver->RegisterAspect( pSensorMapping );
					}
					// rebuild sensors using the original sensor attributes
					std::list<std::string> liSensors;
					oProfiler.GetTheProfileList( (*drList).second.m_strSection, "SENSORS", "", liSensors, GetInteractionIniFile() );

					for(std::list<std::string>::const_iterator sit = liSensors.begin();
						sit != liSensors.end(); ++sit )
					{
						std::string sSensorType = oProfiler.GetTheProfileString( *sit, "TYPE", "", GetInteractionIniFile() );
						std::string sSensorName  = oProfiler.GetTheProfileString( *sit, "NAME", *sit, GetInteractionIniFile() );

						int nSensorId   = oProfiler.GetTheProfileInt( *sit, "RAWID", -1, GetInteractionIniFile() );
						int nHistory    = oProfiler.GetTheProfileInt( *sit, "HISTORY", nDriverHistorySize, GetInteractionIniFile() );

						unsigned int nType = pOrigin->GetTypeFor( sSensorType );
						IVistaMeasureTranscoderFactory *pTrFac = pOrigin->GetTranscoderFactoryForSensor( nType );
						if( (nType != ~0) && pTrFac )
						{
							VistaDeviceSensor *pSensor = new VistaDeviceSensor;
							pSensor->SetTypeHint( nType );
							pSensor->SetSensorName( sSensorName );

							pSensor->SetMeasureTranscode( pTrFac->CreateTranscoder() );

							unsigned int nDriverSensorId = (*drList).second.m_pDriver->AddDeviceSensor( pSensor );

							if(pSensorMapping)
							{
								// create a proper mapping
								pSensorMapping->SetSensorId(nType, nSensorId, nDriverSensorId);
							}

							VistaDriverMeasureHistoryAspect *pHist
								= dynamic_cast<VistaDriverMeasureHistoryAspect*>(
								(*drList).second.m_pDriver->GetAspectById( VistaDriverMeasureHistoryAspect::GetAspectId() ) );
							if(pHist) // should never be NULL
							{
								unsigned int nUpdateRateOfSensorInHz = pOrigin->GetUpdateEstimatorFor( pSensor->GetTypeHint() );
								if( nUpdateRateOfSensorInHz == ~0 )
									continue;

								int nNum = int( ceilf( 66.6f / ( 1000.0f / float(nUpdateRateOfSensorInHz) ) ) );
								unsigned int nMeasureSize = pOrigin->GetMeasureSizeFor( pSensor->GetTypeHint() );
								if( nMeasureSize == ~0 )
									nMeasureSize = 0;
								
								pHist->SetHistorySize( pSensor, nHistory, 2*nNum, nMeasureSize );
							}
						}
					}
				}// if pOrigin
				// add driver to the driver map with the given name
				if((*drList).second.m_pDriver->Connect())
				{
					m_pInteractionManager->AddDeviceDriver( (*drList).second.m_strName,
						(*drList).second.m_pDriver, (*drList).second.m_nPrio );

					// note that for the clustered case, we create shallow drivers
					// with a proper sensor mapping aspect, so the sensor naming is
					// supposed to be allright at this point and there is no need
					// to setup the names (was already done!)
				}
				else
				{
					vkernerr << "[ViSys]: Could not sucessfully connect the driver, so it will not be enabled / registered." << std::endl;
					delete (*drList).second.m_pDriver;
				}
			} // for all drivers
		}
	}
	return true;
}

bool VistaSystem::LoadDriverPlugin( const std::string& sDriverType,
									const std::string& sPluginName,
									const std::string& sTranscoderName )
{
#if defined( VISTAKERNELSTATIC )
	return false;
#else
	vkernout << "[Loading plugins for driver type [" << sDriverType << "]" << std::endl;
	/** DRIVERTODO indent */

	// we will later adjust the library path, so we save it so that it can be restored in the end
	std::string sCurrentPath = VistaEnvironment::GetLibraryPathEnv();

	std::string sPluginFilename, sTranscoderFilename;
	std::string sPluginPattern = "Vista*Plugin";
	std::string sTranscoderPattern = "Vista*Transcoder";

	if( sPluginName.empty() )
	{
		// derive from driver type
		sPluginFilename = "Vista" + sDriverType + "Plugin";
	}
	else
	{
		if( sPluginName.substr( 0, 5 ) != "Vista" )
			sPluginFilename = "Vista*" + sPluginName + "*";
		else
			sPluginFilename = sPluginName + "*";
	}

	if( sTranscoderName.empty() )
	{
		// derive from driver type
		sTranscoderFilename = "Vista" + sDriverType + "Transcoder";
	}
	else
	{
		if( sTranscoderName.substr( 0, 5 ) != "Vista" )
			sTranscoderFilename = "Vista*" + sTranscoderName + "*";
		else
			sTranscoderFilename = sTranscoderName + "*";
	}

#ifdef DEBUG
	sPluginFilename += "D";
	sTranscoderFilename += "D";	
	sPluginPattern += "D";
	sTranscoderPattern += "D";
#endif
#ifdef WIN32
	sPluginFilename += ".dll";
	sTranscoderFilename += ".dll";
	sPluginPattern += ".dll";
	sTranscoderPattern += ".dll";
#else
	sPluginFilename = "lib" + sPluginFilename + ".so";
	sTranscoderFilename = "lib" + sTranscoderFilename + ".so";
	sPluginPattern = "lib" + sPluginPattern + ".so";
	sTranscoderPattern = "lib" + sTranscoderPattern + ".so";
#endif

	std::string sActualPluginFile, sActualTranscoderFile;

	// we support multiple plugin directories
	for( std::list<std::string>::const_iterator itDirectory = m_liDriverPluginPathes.begin();
			itDirectory != m_liDriverPluginPathes.end(); ++itDirectory )
	{
		VistaEnvironment::AddPathToLibraryPathEnv( (*itDirectory) );
		VistaFileSystemDirectory oDirectory( (*itDirectory) );
		oDirectory.SetPattern( sPluginPattern );

		// we cant just check for the existance of the file, since the filename
		// may have different casing - we have to check all Vista'.dll/.so files
		// manually and do a case-insensitive comparison
		for( VistaFileSystemDirectory::const_iterator itFile = oDirectory.begin();
				itFile != oDirectory.end(); ++itFile )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( 
											(*itFile)->GetLocalName(), sPluginFilename ) )
			{
				sActualPluginFile = (*itFile)->GetName();				
				break;
			}			
		} // for .dll file in plugin folder

		VistaFileSystemDirectory oTransDirectory( (*itDirectory) );
		oTransDirectory.SetPattern( sTranscoderPattern );

		// we can't just check for the existence of the file, since the filename
		// may have different casing - we have to check all Vista'.dll/.so files
		// manually and do a case-insensitive comparison
		for( VistaFileSystemDirectory::const_iterator itFile = oTransDirectory.begin();
				itFile != oTransDirectory.end(); ++itFile )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( 
											(*itFile)->GetLocalName(), sTranscoderFilename ) )
			{
				sActualTranscoderFile = (*itFile)->GetName();
				break;
			}			
		} // for .dll file in plugin folder

	} // for plugin dir

	if( sActualPluginFile.empty() )
	{
		/** DRIVERTODO warning */
		return false;
	}
	if( sActualTranscoderFile.empty() )
	{
		/** DRIVERTODO warning */
		return false;
	}

	VddUtil::VistaDriverPlugin oPlugin;

	if( VddUtil::LoadTranscoderFromPlugin( sActualTranscoderFile, &oPlugin ) == false )
	{
		
		vkernerr << "[VistaSystem]: Loading of driver plugin ["
				<< sActualTranscoderFile << "] failed." << std::endl;
		return false;
	}

	if( VddUtil::LoadCreationMethodFromPlugin( sActualPluginFile, &oPlugin ) == false )
	{
		/** DRIVERTODO warning */
		vkernerr << "[VistaSystem]: Loading of plugin ["
				<< sActualPluginFile << "] failed." << std::endl;
	}

	VistaDriverMap* pDriverMap = GetDriverMap();
	// name given AND not registered with this name...
	if( oPlugin.m_strDriverClassName.empty() )
	{
		/** DRIVERTODO warning */
		VddUtil::DisposePlugin( &oPlugin );
		return false;
	}

	if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
										oPlugin.m_strDriverClassName, sDriverType ) == false )
	{
		/** DRIVERTODO warning */
		VddUtil::DisposePlugin( &oPlugin );
		return false;
	}

	if( pDriverMap->GetDriverCreationMethod( oPlugin.m_strDriverClassName ) )
	{
		/** DRIVERTODO warning */
		VddUtil::DisposePlugin( &oPlugin );
		return false;
	}


	vkernout << "Registering method for devices of class ["
						<< oPlugin.m_strDriverClassName
						<< "]" << std::endl;

	m_pDllHlp->m_liDevices.push_back( oPlugin );
	pDriverMap->RegisterDriverCreationMethod( sDriverType, oPlugin.m_pMethod );

	// restore library path
	VistaEnvironment::SetLibraryPathEnv( sCurrentPath );

	return true;
#endif // no static build
}

static bool SetupNodeWithTag( VistaClusterAux *pAux, IVdfnNode *pNode )
{
	std::string sTag;
	pNode->GetUserTag(sTag);
	// create a list of strings
	std::list<std::string> liTokens;
	VistaAspectsConversionStuff::ConvertToList( sTag, liTokens, ';' );

	for(std::list<std::string>::const_iterator cit = liTokens.begin();
		   cit != liTokens.end(); ++cit)
	{
		std::list<std::string> cmd;
		VistaAspectsConversionStuff::ConvertToList( *cit, cmd, ':' );
		if(cmd.size() != 2)
			continue;

		std::vector<std::string> ve(cmd.begin(), cmd.end());

		if( ve[0] == "enable" )
		{
			// get node list
			std::list<std::string> liNames;
			VistaAspectsConversionStuff::ConvertToList( ve[1], liNames, ',' );
			vkernout << "node name=" << pAux->GetNodeName() << std::endl;
			if( liNames.size() == 1 )
				if( liNames.front() == "-" )
				{
					pNode->SetIsEnabled(false);
					continue;
				}

			for(std::list<std::string>::const_iterator k = liNames.begin();
						 k != liNames.end(); ++k )
			{

				if(VistaAspectsComparisonStuff::StringEquals(pAux->GetNodeName(),*k))
				{
					vkernout << " ENABLE node [" << pNode->GetNameForNameable() << "]\n";
					pNode->SetIsEnabled(true);
				}
				else
				{
					pNode->SetIsEnabled(false);
					vkernout << " DISABLE node [" << pNode->GetNameForNameable() << "]\n";
				}
			}

		}
	}


	return true;
}

static VistaInteractionContext *SetupContext(VistaInteractionManager *pInMa,
											  VistaDisplayManager *pDispMgr,
											  VistaEventManager *pEvMgr,
											  VistaClusterAux *pAux,
											  VistaKeyboardSystemControl *pCtrl,
											  VistaSystem *pSys,
											  const std::string &strContextSec,
											  const std::string &strInteractionIniFile,
											  const std::string &sConfigFileName,
											  int nPrioOverride,
											  int nSlaveId)
{
	VistaProfiler oProfiler;

	VistaInteractionContext *pCtx = NULL;
	vkernout << " [ViSys]: SetupContext() ["
		 << strContextSec << "]\n";
	std::string sRole = oProfiler.GetTheProfileString( strContextSec, "ROLE", "", strInteractionIniFile);
	if(sRole.empty())
	{
		vkernerr << " [ViSys]: ### WARNING: no role for context [" << strContextSec << "] given!" << std::endl;
	}

	int nPrio = 0;
	if(nPrioOverride == -1)
		nPrio = VistaInteractionManager::PRIO_LAST;
	else
		nPrio = oProfiler.GetTheProfileInt( strContextSec, "PRIORITY", int(VistaInteractionManager::PRIO_DONTCARE), strInteractionIniFile );
	bool bDelayedUpdate = oProfiler.GetTheProfileBool( strContextSec, "DELAYED_UPDATE", false, strInteractionIniFile );


	std::string sGraphFile = oProfiler.GetTheProfileString( strContextSec, "GRAPH", "", strInteractionIniFile );
	sGraphFile = getAbsolutePathRelativeTo(sGraphFile, strInteractionIniFile);
	if(!sGraphFile.empty())
	{
		pCtx = new VistaInteractionContext(pInMa, pEvMgr);
		pCtx->SetRoleId(pInMa->RegisterRole(sRole));

		// positive on the role
		std::string sReloadKey = oProfiler.GetTheProfileString( strContextSec, "RELOADTRIGGER", "", strInteractionIniFile );
		if(!sReloadKey.empty())
		{
			// reload trigger was given by user:
			int nTrigger = int(sReloadKey[0]);

			if(pCtrl->GetActionForToken(nTrigger))
			{
				vkernerr << " [ViSys]: ### RELOADTRIGGER [" << sReloadKey[0] << "] for context ["
						  << sRole << "] ALREADY OCCUPIED" << std::endl;
				vkernerr << " [ViSys]: Current function: "
					<< pCtrl->GetHelpTextForToken(nTrigger) << std::endl;
			}
			else
			{
				bool bDumpGraphs = oProfiler.GetTheProfileBool( "SYSTEM", "DUMPGRAPHS", false, sConfigFileName );
				bool bWritePorts = oProfiler.GetTheProfileBool( "SYSTEM", "WRITEPORTS", true, sConfigFileName );


				pCtrl->BindAction( nTrigger, new VistaReloadContextGraphCommand(pSys, pCtx, sRole,
																				bDumpGraphs, bWritePorts ),
																				"reload graph for ["+sRole+"]");
				vkernerr << " [ViSys]: ### RELOADTRIGGER [" << sReloadKey[0] << "] for context ["
						  << sRole << "] APPLIED\n";

			}
		}

		bool bEnable = oProfiler.GetTheProfileBool( strContextSec, "ENABLED", true, strInteractionIniFile);
		pCtx->SetIsEnabled(bEnable);

		pInMa->AddInteractionContext(pCtx, nPrio, bDelayedUpdate);
		pCtx->SetGraphSource(sGraphFile);
	}
	else
	{
		vkernout << " [ViSys]: ### NO GRAPH FILE DEFINED FOR CONTEXT [" << sRole << "] ###\n";
	}
	return pCtx;
}

VistaUserPlatform *VistaSystem::GetPlatformFor( VistaDisplaySystem *pDSys ) const
{

	std::map<VistaDisplaySystem*, VistaUserPlatform*>::const_iterator cit = m_mpUserPlatforms.find(pDSys);
	if(cit == m_mpUserPlatforms.end())
		return NULL;
	return (*cit).second;
}

bool VistaSystem::SetupInteractionManager()
{

	// Create InteractionManager and call Init-Method to initialise
	// Trackers and InputDevices
	PrintMsg (" ##### ViSTA Interaction Manager ##### \n");
	m_pInteractionManager = m_pSystemClassFactory->CreateInteractionManager(m_pDriverMap);

	if(GetIsSlave())
	{
		GetVistaClusterSlave()->SetInteractionManager(m_pInteractionManager);
	}
	return true;
}

VistaInteractionManager *VistaSystem::GetInteractionManager() const
{
	return this->m_pInteractionManager;
}

VdfnGraph *VistaSystem::ProperGraphLoad( const std::string &strGraphFile,
										  const std::string &strGraphTag ) const
{
	VdfnGraph *pGraph = VdfnPersistence::LoadGraph( strGraphFile, strGraphTag, false, true );
	if(!pGraph )
	{
		vkernout << "[ViSys]: ERROR loading graph context!!\n";
	}
	else
	{
		vkernout << "[ViSys]: SUCCESS loading graph context!!\n";

		// before we set the graph, we search for nodes with user tag
		VdfnGraph::Nodes nd = pGraph->GetNodesWithTag();
		for(VdfnGraph::Nodes::iterator t = nd.begin(); t != nd.end(); ++t)
		{
			SetupNodeWithTag( GetClusterAux(), *t );
		}

		// note that the graph is not yet evaluated to 0, which means
		// it is not really usable. The Evaluate(0) is called *before* Run()
		if(IsClient() || GetIsSlave())
			pGraph->SetCheckMasterSim(true);
		else
			pGraph->SetCheckMasterSim(false);
	}
	return pGraph;
}

IVistaWindowingToolkit *VistaSystem::GetWindowingToolkit() const
{
	return m_pWindowingToolkit;
}

void VistaSystem::SetWindowingToolkit( IVistaWindowingToolkit* toolkit )
{
	m_pWindowingToolkit = toolkit;
}

bool VistaSystem::Run()
{
	if( !m_bInitialized )
	{
		vkernerr << "[ViSys]: Trying to Run non-initialized VistaSystem!" << std::endl;
		return false;
	}

	if(GetInteractionManager())
	{
		// initialize all graph structures
		PrintMsg( "[ViSys]: Evaluating contextsgraphs...\n" );
		for(std::list<VistaInteractionContext*>::iterator it = m_liCreateCtxs.begin();
			it != m_liCreateCtxs.end(); ++it)
		{
			std::string strGraphFile = (*it)->GetGraphSource();
			if(strGraphFile.empty())
				continue;


			vkernout << " [ViSys]: Graph file given as [" << strGraphFile << "] -- LOAD\n";
			VdfnGraph *pGraph = ProperGraphLoad( strGraphFile,
												  GetInteractionManager()->GetRoleForId((*it)->GetRoleId()) );
			if(pGraph)
				(*it)->SetTransformGraph(pGraph);
		}

		GetInteractionManager()->InitializeGraphContexts();

		VistaProfiler oProfiler;
		std::string sConfigFileName = GetInteractionIniFile();

		// for servers / master nodes, we will offer a dump on the graphs as a feature
		bool bDumpGraphs = oProfiler.GetTheProfileBool( "SYSTEM", "DUMPGRAPHS", false, sConfigFileName );
		bool bWritePorts = oProfiler.GetTheProfileBool( "SYSTEM", "WRITEPORTS", true, sConfigFileName );
		if(bDumpGraphs)
		{
			GetInteractionManager()->DumpGraphsToDot(bWritePorts);
		}

		PrintMsg( "[ViSys]: ... DONE\n[ViSys]: Starting driver threads...\n");
		GetInteractionManager()->StartDriverThread();
	}

	bool bRet = m_pSystemClassFactory->Run();

	// StopDriverThread() is called by EXIT event handling
	return bRet;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Quit                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::Quit()
{
	m_pWindowingToolkit->Quit();
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ProcessEvent                                                */
/*                                                                            */
/*============================================================================*/
void VistaSystem::HandleEvent(VistaEvent *pEvent)
{
	//cout << "*********** VSE_EXTERNAL_MESSAGE ************" << endl;
	//cout << "*********** " << ((VistaSystemEvent*)pEvent)->GetId() << "************" << endl;
	if(pEvent->GetType() == VistaSystemEvent::GetTypeId())
	{
		switch (((VistaSystemEvent*)pEvent)->GetId())
		{
		case VistaSystemEvent::VSE_POSTGRAPHICS:
		{
			m_pClusterAux->SetFrameClock( pEvent->GetTime() );
			break;
		}
		case VistaSystemEvent::VSE_UPDATE_INTERACTION:
		{
			if(m_pPort && m_pPort->GetRescheduleFlag())
			{
				// we should clean-up first
				m_pPort->Disconnect();
			}
			else if(m_pPort && m_pPort->HasMessage())
			{
				VistaMsg *p = m_pPort->GetNextMsg();
				//                std::cout << "p-msg = " << p->GetThisMsg().c_str() << std::endl;
				m_pExternalMsg->SetHandled(false); // reset state
				m_pExternalMsg->SetThisMsg(p);
				m_pExternalMsg->SetId(VistaExternalMsgEvent::VEID_INCOMING_MSG);
				GetEventManager()->ProcessEvent(m_pExternalMsg);
#if defined(DEBUG)
				if(!p->GetMsgSuccess())
					PrintMsg("[ViSys]: KernelMsg marked as non-success.\n");
#endif
				m_pPort->AnswerQuery(p);
				pEvent->SetHandled(true);
			}

			// check app msg ports
			if(!m_liAppPorts.empty())
			{
				for(std::list<VistaKernelMsgPort*>::iterator it = m_liAppPorts.begin();
					it != m_liAppPorts.end(); ++it)
				{
					if((*it)->HasMessage())
					{
						VistaMsg *p = (*it)->GetNextMsg();
						//                vkernout << "p-msg = " << p->GetThisMsg().c_str() << std::endl;
						m_pExternalMsg->SetHandled(false); // reset state
						m_pExternalMsg->SetThisMsg(p);
						m_pExternalMsg->SetId(VistaExternalMsgEvent::VEID_INCOMING_MSG);
						GetEventManager()->ProcessEvent(m_pExternalMsg);
#if defined(DEBUG)
						if(!p->GetMsgSuccess())
							PrintMsg("[ViSys]: KernelMsg marked as non-success.\n");
#endif
						(*it)->AnswerQuery(p);
						pEvent->SetHandled(true);
					}
				}
			}

			//VistaOldInteractionManager *pInMa = GetOldInteractionManager();
			//if (pInMa)
			//{
			//	pInMa->UpdateInputDevices();
			//	pEvent->SetHandled(true);
			//}
			break;
		}
		case VistaSystemEvent::VSE_UPDATE_DISPLAYS:
		{
			assert(this->m_pDisplayManager && "no display manager present...");
			m_pDisplayManager->Update();
			pEvent->SetHandled(true);
			break;
		}
		case VistaSystemEvent::VSE_UPDATE_PICKING:
		{
			assert(this->m_pPickManager && "no pick manager present...");
			m_pPickManager->UpdatePicking();
			pEvent->SetHandled(true);
			break;
		}
		case VistaSystemEvent::VSE_QUIT:
		{
			this->Quit();
			pEvent->SetHandled(true);
			break;
		}

		case VistaSystemEvent::VSE_EXIT:
		{
			if(GetInteractionManager())
			{
				GetInteractionManager()->StopDriverThread();
			}
			pEvent->SetHandled(true);
			break;
		}
		}
	}
	else if(pEvent->GetType() == VistaExternalMsgEvent::GetTypeId())
	{
		//cout << "*********** VSE_EXTERNAL_MESSAGE ************" << endl;
		VistaMsg *pMsg = ((VistaExternalMsgEvent*)pEvent)->GetThisMsg();

		switch(pMsg->GetMsgType())
		{
		case VistaKernelMsgType::VKM_KERNEL_MSG:
		{
			//cout << "*********** VKM_KERNEL_MSG ************" << endl;
			pEvent->SetHandled(true);
			try
			{
				if(m_pPort)
					m_pPort->DispatchKernelMsg(*pMsg);
			}
			catch(...)
			{
				PrintMsg("[VisSys]: ##-- Exception during HandleKernelMsg()! --##\n");
			}
			break;
		}
		case VistaKernelMsgType::VKM_PROPERTYLIST_MSG:
		{
			//PrintMsg("[ViSys]: ## -- VKM_APPLICATION_MSG -- ##\n");

			VistaByteBufferDeSerializer dsSer;
			VistaMsg::MSG vecMsg = pMsg->GetThisMsg();
			dsSer.SetBuffer((const char*)&vecMsg[0], vecMsg.size());
			// get method
			int iMethod=0;
			int iTicket =0;

			dsSer.ReadInt32(iMethod);
			dsSer.ReadInt32(iTicket);
			(*m_pCommandEvent).SetMethodToken(iMethod);
			(*m_pCommandEvent).SetMessageTicket(iTicket);

			VistaPropertyList p;
			std::string sName;
			VistaPropertyList::DeSerializePropertyList(dsSer, p, sName);
			(*m_pCommandEvent).SetPropertiesByList(p);

			//(*m_pCommandEvent).DeSerialize(dsSer);
			(*m_pCommandEvent).SetCommandMsg(pMsg); // for answers
			GetEventManager()->ProcessEvent(m_pCommandEvent);
			m_pCommandEvent->GetCommandMsg()->SetMsgSuccess(m_pCommandEvent->IsHandled());

			if(m_pCommandEvent->UsesPropertyListAnswer())
			{// set answer parameters
				VistaByteBufferSerializer ser;
				VistaPropertyList p;
				(*m_pCommandEvent).GetPropertiesByList(p);

				VistaPropertyList::SerializePropertyList(ser, p, "answer");

				//(*m_pCommandEvent).Serialize(ser);
				VistaMsg::MSG veAnswer;
				ser.GetBuffer(veAnswer);
				VistaMsg *pMsg = m_pCommandEvent->GetCommandMsg();
				if(pMsg)
					pMsg->SetMsgAnswer(veAnswer);
				else
				{
					// bogus.
				}
			}
			pEvent->SetHandled(true);
			break;
		}
		/** @todo remove me */
		case VistaKernelMsgType::VKM_SCRIPTED_MSG:
		default:
			break;
		}
	}
}

/*############################################################################*/
/*##   End of Init-Section   #################################################*/
/*############################################################################*/

VistaKernelMsgPort *VistaSystem::SetExternalMsgPort(VistaKernelMsgPort *pPort)
{
	VistaKernelMsgPort *pOldPort = m_pPort;
	m_pPort = pPort;
	return pOldPort;
}

void VistaSystem::DisconnectExternalMsgPort()
{
	if(m_pPort)
		m_pPort->Disconnect();
}

bool VistaSystem::HasExternalMsgPort() const
{
	return (m_pPort != NULL);
}

bool VistaSystem::HasExternalConnection() const
{
	return m_pPort && m_pPort->GetIsConnected();
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetGraphicsManager                                            */
/*                                                                            */
/*============================================================================*/

VistaGraphicsManager *  VistaSystem::GetGraphicsManager () const
{
	return m_pNewWorld;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayManager                                           */
/*                                                                            */
/*============================================================================*/
VistaDisplayManager *  VistaSystem::GetDisplayManager () const
{
	return m_pDisplayManager;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetEventManager                                             */
/*                                                                            */
/*============================================================================*/
VistaEventManager *  VistaSystem::GetEventManager () const
{
	return m_pEventManager;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetPickManager                                              */
/*                                                                            */
/*============================================================================*/
VistaPickManager *  VistaSystem::GetPickManager () const
{
	return m_pPickManager;
}

VistaDriverMap             *VistaSystem::GetDriverMap() const
{
	return m_pDriverMap;
}

VistaDriverPropertyConfigurator *VistaSystem::GetDriverConfigurator() const
{
	return m_pConfigurator;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetVistaClusterClient                                       */
/*                                                                            */
/*============================================================================*/
VistaClusterClient *  VistaSystem::GetVistaClusterClient () const
{
	return m_pClusterAux->GetVistaClusterClient();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetVistaClusterServer                                       */
/*                                                                            */
/*============================================================================*/
VistaClusterServer *  VistaSystem::GetVistaClusterServer () const
{
	return m_pClusterAux->GetVistaClusterServer();
}

VistaClusterMaster  *VistaSystem::GetVistaClusterMaster () const
{
	return m_pClusterAux->GetVistaClusterMaster();
}

VistaClusterSlave   *VistaSystem::GetVistaClusterSlave() const
{
	return m_pClusterAux->GetVistaClusterSlave();
}

bool VistaSystem::GetIsMaster() const
{
	return m_pClusterAux->GetIsMaster();
}

bool VistaSystem::GetIsSlave() const
{
	return m_pClusterAux->GetIsSlave();
}

VistaClusterAux *VistaSystem::GetClusterAux() const
{
	return m_pClusterAux;
}


bool VistaSystem::IndicateApplicationProgress(int iAppProgress, const std::string &sProgressMessage)
{
	if(HasExternalMsgPort())
	{
		return m_pPort->WriteProgress(VistaKernelMsgPort::PRG_APP, iAppProgress, sProgressMessage);
	}
	return false;
}


bool VistaSystem::IndicateSystemProgress(int iProgressStage, const std::string &sProgressMessageText, bool bDone)
{
	if(HasExternalMsgPort())
	{
		return m_pPort->WriteProgress((bDone ? VistaKernelMsgPort::PRG_DONE : VistaKernelMsgPort::PRG_SYSTEM),
									  iProgressStage, sProgressMessageText);
	}
	return false;
}

int VistaSystem::GenerateRandomNumber(int iLowerBound, int iMaxBound) const
{
	return m_pClusterAux->GenerateRandomNumber(iLowerBound, iMaxBound);
}

void VistaSystem::SetRandomSeed(int iSeed)
{
	m_pClusterAux->SetRandomSeed(iSeed);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   PrintMsg                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::PrintMsg ( const char * pMsg, std::ostream *pStream) const
{
	if (pMsg == NULL)
		pMsg = ""; // lets be tolerant :)

	return PrintMsg (std::string(pMsg), pStream);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   PrintMsg                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::PrintMsg ( const std::string &strMsg, std::ostream *pStream) const
{
	if (pStream == NULL)
	{
		pStream = &vkernout;
	}
	(*pStream) << strMsg;
	(*pStream).flush ();
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IntroMsg                                                    */
/*                                                                            */
/*============================================================================*/
/**
 * @todo change to mention project homepage (sourceforge) and LGPL v3 license
 */
bool VistaSystem::IntroMsg (std::ostream *pStream) const
{
	PrintMsg("\n******************************************************************************\n", pStream);
	PrintMsg("*                                                                            *\n", pStream);
	PrintMsg("* ViSTA - Virtual Reality for Scientific and Technical Applications          *\n", pStream);
	PrintMsg("* =================================================================          *\n", pStream);
	PrintMsg("*                                                                            *\n", pStream);
	PrintMsg("* Developed at:                                                              *\n", pStream);
	PrintMsg("* Virtual Reality Group, RWTH Aachen University                              *\n", pStream);
	PrintMsg("*                                                                            *\n", pStream);
	PrintMsg("* Torsten Kuhlen et al.,              1998-2010                              *\n", pStream);
	PrintMsg("*                                                                            *\n", pStream);
	PrintMsg("* use option -h for help                                                     *\n", pStream);
	PrintMsg("*                                                                            *\n", pStream);
	PrintMsg("******************************************************************************\n", pStream);
	PrintMsg("ViSTA-Version: "+std::string(VistaVersion::GetVersion())+", Release: "
			 +VistaVersion::GetReleaseName()+"\nbuild date: "
			 +VistaVersion::GetBuildDateString()+", time: "
			 +VistaVersion::GetBuildTimeString()+"\n\n", pStream);
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ArgHelpMsg                                                  */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::ArgHelpMsg (const std::string &sAppName, std::ostream *pStream) const
{
	/**
	 * @todo add registry for command-line options? (boost::program_options or alike?)
	 */
	PrintMsg("\n", pStream);
	PrintMsg(sAppName+ "\n[-vistaini <IniFile>]\n", pStream);
	PrintMsg("      [-loadmodel <ModelFile> [-scalemodel <scale>]]\n", pStream);
	PrintMsg("      [-h|help]\n", pStream);
	PrintMsg("      \n", pStream);
	PrintMsg("Possible parameters:\n", pStream);
	PrintMsg("--------------------\n");
	PrintMsg("-loadmodel <ModelFile>        : load the specified model\n", pStream);
	PrintMsg("-scalemodel <scale>           : scale the specified model\n", pStream);
	PrintMsg("-vistaini <IniFileName>       : use the specified initialization file for SYSTEMWIDE (or all) settings\n", pStream);
	PrintMsg("-displayini <IniFileName>     : use the specified initialization file for DISPLAY settings\n", pStream);
	PrintMsg("-graphicsini <IniFileName>    : use the specified initialization file for GRAPHICS settings\n", pStream);
	PrintMsg("-interactionini <IniFileName> : use the specified initialization file for INTERACTION settings\n", pStream);
	PrintMsg("-clusterini <IniFileName>     : use the specified initialization file for CLUSTER settings\n", pStream);
	PrintMsg("-inisearchpath {(<path>{,})*}<path>: use the specified, comma-separated search paths for ini files\n", pStream);

	PrintMsg("-h|help                 : this message\n", pStream);
	PrintMsg("--------------------\n", pStream);
	PrintMsg("\n\n\n", pStream);

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ArgParser                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::ArgParser (int argc, char *argv[])
{
	if( argc == 1 )
	{
		ArgHelpMsg(argv[0]);
		return true;
	}

	// 'parse' command-line arguments
	for( int arg = 1; arg < argc; ++arg )
	{
		// get argument key
		std::string  strArg;
		strArg = argv[arg];

		// check if user wants help
		if (( strArg.compare("-h") == 0 ) ||
			( strArg.compare("-help") == 0 ))
		{
			ArgHelpMsg(argv[0]);
			return false;
		}

		// select ViSTA's initialization file
		if( strArg.compare("-vistaini") == 0 )
		{
			++arg;
			if (arg < argc)
				SetIniFile(argv[arg]);
		}

		if( strArg.compare("-displayini") == 0)
		{
			++arg;
			if(arg < argc)
			{
				SetDisplayIniFile(argv[arg]);
				m_bLockDisplayIni = true;
			}
		}

		if( strArg.compare("-graphicsini") == 0)
		{
			++arg;
			if(arg < argc)
			{
				SetGraphicsIniFile(argv[arg]);
				m_bLockGraphicsIni = true;
			}
		}

		if( strArg.compare("-interactionini") == 0)
		{
			++arg;
			if(arg < argc)
			{
				SetInteractionIniFile(argv[arg]);
				m_bLockInteractionIni = true;
			}
		}

		if( strArg.compare("-clusterini") == 0)
		{
			++arg;
			if(arg < argc)
				SetClusterIniFile(argv[arg]);
		}

		// select ViSTA's initialization file
		if( strArg.compare("-loadmodel") == 0 )
		{
			++arg;
			if (arg < argc)
			{
				m_strModelFile = argv[arg];
				m_bLockClusterIni = true;
			}
		}

		if( strArg.compare("-scalemodel") == 0 )
		{
			++arg;
			if (arg < argc)
			{
				string strScale = argv[arg];
				m_nModelScale = (float)atof (strScale.c_str());
			}
		}

		// set Vistas ClusterMode
		if( strArg.compare("-standalone") == 0)
		{
			++arg;
			if( arg < argc)
			{
				m_strSystemName = argv[arg];
				m_iClusterMode = VCM_STANDALONE;
			}
		}

		if( strArg.compare("-clusterclient") == 0)
		{
			++arg;
			if( arg < argc)
			{
				m_strSystemName = argv[arg];
				m_iClusterMode = VCM_CLIENT;
			}
		}

		if( strArg.compare("-clusterserver") == 0)
		{
			++arg;
			if( arg < argc)
			{
				m_strSystemName = argv[arg];
				m_iClusterMode = VCM_SERVER;
			}
		}


		if( strArg.compare("-clustermaster") == 0)
		{
			++arg;
			if( arg < argc)
			{
				m_strSystemName = argv[arg];
				m_iClusterMode = VCM_MASTER;
			}
		}

		if( strArg.compare("-clusterslave") == 0)
		{
			++arg;
			if( arg < argc)
			{
				m_strSystemName = argv[arg];
				m_iClusterMode = VCM_SLAVE;
			}
		}


		if( strArg.compare("-inisearchpath") == 0)
		{
			vkernout << "[ViSy]: encountered argument -inisearchpath\n";
			++arg;
			if(arg < argc)
			{
				vkernout << "[ViSy]: using explicit inisearchpath ["
					 << argv[arg] << "]\n";
				m_liSearchPath.clear();
				std::string Sarg(argv[arg]);
				if(Sarg.compare("-none-")!=0)
				{
					std::list<std::string> rLiTmp;
					VistaAspectsConversionStuff::ConvertToList(argv[arg], rLiTmp);
					for(std::list<std::string>::reverse_iterator cit = rLiTmp.rbegin();
						cit != rLiTmp.rend(); ++cit)
						m_liSearchPath.push_front(*cit);
				}
				else
				{
					vkernout << "[ViSys]: Using -none-, only local path (.) is used (default).\n";
				}
				m_bLockSearchPath = true;
			}
		}
	}
	return true;

}

double VistaSystem::GetFrameClock() const
{
	return m_pClusterAux->GetFrameClock();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsClient                                                    */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::IsClient () const
{
	return m_pClusterAux->GetIsClient();
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsServer                                                    */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::IsServer () const
{
	return m_pClusterAux->GetIsServer();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsStandalone                                                */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::IsStandalone () const
{
	return (!m_pClusterAux->GetIsServer()
		 && !m_pClusterAux->GetIsClient()
		 && !m_pClusterAux->GetIsMaster()
		 && !m_pClusterAux->GetIsSlave());
}

const std::string& VistaSystem::GetApplicationName() const
{
	return m_strApplicationName;
}

bool VistaSystem::RegisterMsgPort(VistaKernelMsgPort *pPort)
{
	m_liAppPorts.push_back(pPort);
	return true;
}

VistaKernelMsgPort *VistaSystem::UnregisterMsgPort(VistaKernelMsgPort *pPort)
{
	std::list<VistaKernelMsgPort*>::iterator it = std::find(m_liAppPorts.begin(),
															 m_liAppPorts.end(), pPort);
	if(it != m_liAppPorts.end())
	{
		// found
		m_liAppPorts.erase(it); // remove iterator
		return pPort;
	}
	return NULL;
}

IVistaSystemClassFactory *VistaSystem::GetSystemClassFactory() const
{
	return m_pSystemClassFactory;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaSystem::Debug ( ostream & out ) const
{
	out << " [ViSystem] ViSTA Version : " << VistaVersion::GetVersion() << "(" << VistaVersion::GetReleaseName() << ")";
	out << " (Major = " << VistaVersion::GetMajor();
	out << " , Minor = " << VistaVersion::GetMinor();
	out << " , Revision = " << VistaVersion::GetRevision() << ")\n";
	out << " [ViSystem] ViSTA       INI: " << GetIniFile() << endl;
	out << " [ViSystem] Display     INI: " << GetDisplayIniFile() << endl;
	out << " [ViSystem] Graphics    INI: " << GetGraphicsIniFile() << endl;
	out << " [ViSystem] Interaction INI: " << GetInteractionIniFile() << endl;
	out << " [ViSystem] Cluster     INI: " << GetClusterIniFile() << endl;
	out << " [ViSystem] Loaded Model   : " << m_strModelFile << endl;
	out << " [ViSystem] initialized    : " << boolalpha << m_bInitialized << endl;
	if (GetVistaClusterServer())
	{
		out << " [ViSystem] Cluster Server detected!\n";
		out << *GetVistaClusterServer();
	}
	if (GetVistaClusterClient())
	{
		out << " [ViSystem] Cluster Client detected!\n";
		out << *GetVistaClusterClient();
	}
	if (GetVistaClusterMaster())
	{
		out << " [ViSystem] Cluster Master detected!\n";
		GetVistaClusterMaster()->Debug(out);
	}
	if (GetVistaClusterSlave())
	{
		out << " [ViSystem] Cluster Slave detected!\n";
		GetVistaClusterSlave()->Debug(out);
	}
	if(m_iClusterMode == VistaSystem::VCM_STANDALONE)
	{
		out << " [ViSystem] Standalone mode detected!\n";
	}
	
	// flush and newline
	out << endl;
}

string VistaSystem::FindFileInIniSearchPath(const string &filename, ostream *logger) const
{
#ifdef DEBUG
	if(!logger) logger = &vkernout;
#endif

	// treat absolute pathes
	if(isAbsolutePath(filename))
	{
		VistaFileSystemFile f(filename);
		if(f.Exists() && !f.IsDirectory())
		{
			if(logger)
				*logger << "[VistaSystem] '" << filename << "' found." << endl;
			return filename;
		}
		else
			return string("");
	}

	// treat relative pathes. search in all search pathes.
	if(logger)
	{
		*logger << "[VistaSystem] - searching for : '"
				<< filename
				<< "' in the following places:\n";
	}
			
	for(list<string>::const_iterator it = GetIniSearchPaths().begin();
		it != GetIniSearchPaths().end();
		++it)
	{
		string fname = *it + filename;
		fname = getAbsolutePath(fname);
		VistaFileSystemFile iniFile(fname);
		if(iniFile.Exists() && !iniFile.IsDirectory())
		{
			if(logger)
				*logger << "\t[0] '"
						<< fname
						<< "' MATCH!"
						<< endl;
			return fname;
		}
		else
		{
			if(logger)
				*logger << "\t[ ] '"
						<< fname << "'"
						<< endl;
		}
	}

	if(logger)
		*logger << "\t    *file not found*" << endl;

	return string("");
}

// ############################################################
// STATIC METHODS
// ############################################################
static int siSigIntCount=0;

void VistaSystem::SIGINTHandler(int iMask)
{
	vkernerr << "VistaSystem::SIGINTHandler(" << iMask << ") -- SIGINT caught" << std::endl;
	::GetVistaSystem()->Quit();

	++siSigIntCount;

	if(siSigIntCount == 3)
	{
		vkernerr << "VistaSystem::SIGINTHandler() -- SIGINT caught 3 times, brute force exit" << std::endl;
		delete ::GetVistaSystem();
		exit(-1);
	}

}

void VistaSystem::SIGTERMHandler(int iMask)
{
	vkernerr << "VistaSystem::SIGTERMHandler(" << iMask << ") -- SIGTERM caught" << std::endl;
	::GetVistaSystem()->Quit();
	exit(-2);
}

void VistaSystem::SIGPIPEHandler(int iMask)
{
	vkernerr << "VistaSystem::SIGPIPEHandler(" << iMask << ") -- SIGPIPE caught." << std::endl;
	vkernerr << "some socket blew it... you better check." << std::endl;
}

std::string VistaSystem::GetSystemSectionName()
{
	return gSystemSectionName;
}

VistaKeyboardSystemControl *VistaSystem::GetKeyboardSystemControl() const
{
	return m_pSystemControl;
}

VdfnObjectRegistry *VistaSystem::GetDfnObjectRegistry() const
{
	return m_pDfnObjects;
}

void VistaSystem::LoadPlugins( const std::string& sConfigFileName )
{
#if !defined(VISTAKERNELSTATIC)
	// this is for DLL builds only: allow to load new plugins at this stage
	// first: search for the plugin directory
	VistaProfiler oProfiler;

	oProfiler.SetEnvironmentVariablesEnabled( true );
	std::string strDriverDirectory = oProfiler.GetTheProfileString( 
						"SYSTEM", "DRIVERPLUGINS", "", sConfigFileName );

	if( isAbsolutePath( strDriverDirectory ) == false )
	{
		strDriverDirectory = getAbsolutePathRelativeTo( strDriverDirectory, sConfigFileName );
	}

	// maybe the user wants to omit or select certain plugins from that directory
	std::list<std::string> liExplicitPlugIns;
	oProfiler.GetTheProfileList( "SYSTEM", "LOADPLUGINS", liExplicitPlugIns, sConfigFileName );

	if( liExplicitPlugIns.empty() )
	{
		// the user wants to load *all* plugins found with a proper wildcard.
		// go to directory, scan its contents (for "*.dll/*.so") and load all plugins for
		// the driver creation method, add to driver map and go on...
		
		VistaFileSystemDirectory dir( strDriverDirectory );
		if( dir.Exists() )
		{
			vkernout << "[VistaSystem] looking for driver plugins in directory "
					  << strDriverDirectory << std::endl;
		}
		else
		{
			vkernout << "[VistaSystem::LoadPlugins]: Warning: Driver plugin path \""
						<< strDriverDirectory
						<< "\" does not exist" << std::endl;
		}

		// set wildcards, according to system (win/un*x) (Debug/Release)
#if defined(WIN32)
	#if defined(DEBUG)
			dir.SetPattern("*PluginD.dll");
	#else
			dir.SetPattern("*Plugin.dll");
	#endif
#else // non-win32
	#if defined(DEBUG)
			dir.SetPattern("*PluginD.so");
	#else
			dir.SetPattern("*Plugin.so");
	#endif
#endif

		// check, iff the directory does exist
		if(dir.Exists())
		{
			vkernout << "[VistaSystem] number of matched plugins: "
					  << dir.GetNumberOfEntries() << std::endl;

			// call LoadPlugin for_each *file* found.
			for(VistaFileSystemDirectory::const_iterator cit = dir.begin();
				cit != dir.end(); ++cit)
			{
				LoadPlugin( (*cit)->GetName() );
			}
		}
	}
	else
	{
		// only test for prefix/infix given names
		for(std::list<std::string>::const_iterator cit = liExplicitPlugIns.begin();
			cit != liExplicitPlugIns.end(); ++cit)
		{
			// for each test, we build a new directory entity (simpler than re-usage
			// and at this point, we have enough time).
			VistaFileSystemDirectory dir(strDriverDirectory);

			vkernout << "[VistaSystem] looking for driver plugins in directory "
				<< strDriverDirectory << std::endl;
			// construct "real" plugin name!
			std::string strRealName = std::string("*") + (*cit);

			// same as above: add wildcard according to system (win/un*x) (debug/release)
#if defined(WIN32)
	#if defined(DEBUG)
				strRealName += std::string("*PluginD.dll");
	#else
				strRealName += std::string("*Plugin.dll");
	#endif
#else
	#if defined(DEBUG)
				strRealName += std::string("*PluginD.so");
	#else
				strRealName += std::string("*Plugin.so");
	#endif
#endif
			// look for a match
			dir.SetPattern( strRealName );

			vkernout << "[VistaSystem] number of matched plugins [" + strRealName +"]: "
					  << dir.GetNumberOfEntries() << std::endl;

			// load plugin
			for(VistaFileSystemDirectory::const_iterator cit = dir.begin();
				cit != dir.end(); ++cit)
			{
				LoadPlugin( (*cit)->GetName() );
			}
		}
	}
#endif
}

bool VistaSystem::LoadPlugin( const std::string &sPathToPlugin )
{
#if !defined(VISTAKERNELSTATIC)
	vkernout << "[VistaSystem] loading driver plugin: " << sPathToPlugin << std::endl;
	VddUtil::VistaDriverPlugin oPlugin;

	std::string sPathToTranscodePlugin = sPathToPlugin;
	std::size_t nIndex = sPathToTranscodePlugin.rfind( "Plugin" );
	sPathToTranscodePlugin.replace( nIndex, std::size_t( 6 ), "Transcoder" );

	if( VddUtil::LoadTranscoderFromPlugin( sPathToTranscodePlugin, &oPlugin ) == false )
	{
		vkernerr << "[VistaSystem]: Loading of plugin ["
			<< sPathToPlugin << "] failed." << std::endl;
		return false;
	}

	if( VddUtil::LoadCreationMethodFromPlugin( sPathToPlugin, &oPlugin ) )
	{
		VistaDriverMap *pDm = GetDriverMap();
		// name given AND not registered with this name...
		if(!oPlugin.m_strDriverClassName.empty() && !pDm->GetDriverCreationMethod( oPlugin.m_strDriverClassName ) )
		{
			vkernout << "[VistaSystem]: Registering method for devices of class ["
				<< oPlugin.m_strDriverClassName
				<< "] from ["
				<< sPathToPlugin
				<< "]\n";


			m_pDllHlp->m_liDevices.push_back( oPlugin );
			pDm->RegisterDriverCreationMethod( oPlugin.m_strDriverClassName, oPlugin.m_pMethod );
			return true;
		}
		else
		{
			vkernout << "[VistaSystem]: Either no classname in this plugin, or a class of this type "
				<< "is already registered?\n";
			VddUtil::DisposePlugin( &oPlugin );
		}
	}
	else
	{
		vkernerr << "[VistaSystem]: Loading of plugin ["
			<< sPathToPlugin << "] failed." << std::endl;
	}

#endif // for static builds
	return false;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( ostream & out, const VistaSystem & device )
{
	device.Debug ( out );
	return out;
}

