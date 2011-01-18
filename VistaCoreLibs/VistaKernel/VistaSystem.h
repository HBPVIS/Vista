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

#ifndef _VISTASYSTEM_H
#define _VISTASYSTEM_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
// system includes
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <list>
#include <iostream>
#include <vector>
#include <map>

// prototypes
class   VistaSystem;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaSystem & );


/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
//class VistaDisplay;

class VistaSensor;
class VistaGraphicsManager;
class VistaEventManager;
class VistaDisplayManager;
//class VistaOldInteractionManager;
class VistaInteractionManager;
class VistaPickManager;

class VistaClusterServer;
class VistaClusterClient;
class VistaClusterMaster;
class VistaClusterSlave;


class VistaSystemEvent;
class VistaGraphicsEvent;
class VistaTimer;
class VistaKernelMsgPort;
class VistaExternalMsgEvent;
class VistaInputEvent;
class VistaCommandEvent;
//class VistaInputDeviceObserver;
class DLVistaDataSink;
class IVistaSystemClassFactory;
class VistaKeyboardSystemControl;
class VistaClusterAux;
class VistaDriverMap;

class VistaDriverPropertyConfigurator;
class VdfnObjectRegistry;
class VistaUserPlatform;
class VistaDisplaySystem;
class VistaInteractionContext;

class VdfnGraph;

#include "WindowingToolkit/VistaWindowingToolkit.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaSystem is the baseclass which handles all functions from Vista. When
 * this class is created an the init- and run-function are called the simulation
 * scenegraph is display an the 3D-universe lives. If tracker or displaydevices
 * are specified in the applicaton/configfiles/vista.ini they are used to
 * generate the actuall configuration. But you can also start vista without
 * settings in the vista.ini, But then you to create the classes on your own.
 *
 */
class VISTAKERNELAPI VistaSystem : public VistaEventHandler
{
public:
	enum VISTA_CLUSTER_MODE
	{
	  VCM_STANDALONE = 0,
	  VCM_SERVER,
	  VCM_CLIENT,
	  VCM_MASTER,
	  VCM_SLAVE
	};

	VistaSystem();
	virtual ~VistaSystem();

	// ############################################################################
	// USER CONFIGURABLE INI FILE STRUCTURES API
	// ############################################################################
	bool        SetIniFile(const std::string &sConfigNew);
	std::string GetIniFile() const;

	bool        SetDisplayIniFile(const std::string &sDisplayConfig);
	std::string GetDisplayIniFile() const;

	bool        SetGraphicsIniFile(const std::string &sGraphicsConfig);
	std::string GetGraphicsIniFile() const;

	bool        SetInteractionIniFile(const std::string &sInterConfig);
	std::string GetInteractionIniFile() const;

	bool        SetClusterIniFile(const std::string &sClusterConfig);
	std::string GetClusterIniFile() const;

	const std::list<std::string>& GetIniSearchPaths() const;
	bool                   SetIniSearchPaths(const std::list<std::string> &liSearchPaths);
	static std::string     GetSystemSectionName();
	const std::string&     GetApplicationName() const;


	// ############################################################################
	// RUN-TIME USER API
	// ############################################################################
	bool IntroMsg (std::ostream * pStream = NULL) const;
	bool Init (int argc, char *argv[]);
	bool Run();
	bool Quit();

	// ##########################################################
	// GENERAL INTERACTION API
	// ##########################################################
	VistaKeyboardSystemControl *GetKeyboardSystemControl() const;
	VdfnObjectRegistry *GetDfnObjectRegistry() const;
	VistaUserPlatform *GetPlatformFor( VistaDisplaySystem *pDSys ) const;

	// ############################################################################
	// ACCESS TO INTERNAL STRUCTURES API
	// ############################################################################
	/**
	 * Returns the vista event manager. It serves as an entry point into the
	 * system for newly generated events. In addition it is responsible for
	 * registering and managing event types, event handlers, and event
	 * observers.
	 * The EventManager is present right after the creation of a VistaSystem
	 * instance (which is not true for the other manager instances).
	 * It defines the heart-beat of a ViSTA application.
	 */
	VistaEventManager               *GetEventManager() const;
	VistaGraphicsManager            *GetGraphicsManager () const;
	VistaDisplayManager             *GetDisplayManager () const;
	VistaInteractionManager         *GetInteractionManager() const;
	VistaPickManager                *GetPickManager() const;
	VistaDriverMap                  *GetDriverMap() const;
	VistaDriverPropertyConfigurator *GetDriverConfigurator() const;
	IVistaSystemClassFactory         *GetSystemClassFactory() const;
	VdfnGraph                       *ProperGraphLoad( const std::string &strGraphFile,
													   const std::string &strGraphTag ) const;
	IVistaWindowingToolkit	 *GetWindowingToolkit() const;
	void SetWindowingToolkit( IVistaWindowingToolkit* toolkit );


	// ############################################################################
	// CLUSTER API
	// ############################################################################
	VistaClusterClient  *    GetVistaClusterClient () const;
	VistaClusterServer  *    GetVistaClusterServer () const;
	VistaClusterMaster  *    GetVistaClusterMaster () const;
	VistaClusterSlave   *    GetVistaClusterSlave  () const;

	bool GetIsMaster() const;
	bool GetIsSlave() const;


	bool IsClient() const;
	bool IsServer() const;

	bool IsStandalone() const;

	double GetFrameClock() const;
	int    GenerateRandomNumber(int iLowerBound, int iMaxBound) const;
	void   SetRandomSeed(int iSeed);

	VistaClusterAux *GetClusterAux() const;


	// ############################################################################
	// MSGPORT API
	// ############################################################################

	/**
	 * Set the external message port. Returns the formerly set
	 * message port. By making this call, ownership of the port
	 * goes to the system, i.e. the port is beind destroyed upon
	 * system destruction.
	 */
	VistaKernelMsgPort *SetExternalMsgPort(VistaKernelMsgPort *pPort);

	void DisconnectExternalMsgPort();
	bool HasExternalMsgPort() const;
	bool HasExternalConnection() const;


	bool                 RegisterMsgPort(VistaKernelMsgPort *pPort);
	VistaKernelMsgPort *UnregisterMsgPort(VistaKernelMsgPort *);


	// ############################################################################
	// USER FEEDBACK PROGRESS API
	// ############################################################################
	bool IndicateApplicationProgress(int iAppProgress,
									 const std::string &sProgressMessage);

	// ############################################################################
	// INHERITED FROM EVENTHANDLER API
	// ############################################################################
	virtual void HandleEvent(VistaEvent *pEvent);

	// ############################################################################
	// DEBUG API
	// ############################################################################
	virtual void Debug ( std::ostream & out ) const;
	bool PrintMsg  ( const std::string &strMsg, std::ostream * pStream = 0 ) const;
	bool PrintMsg  ( const char *  pMsg,       std::ostream * pStream = 0 ) const;
	
private:
	bool ArgParser (int argc, char *argv[]);
	bool ArgHelpMsg (const std::string &sAppName, std::ostream * pStream = NULL) const;
	void LoadPlugins( const std::string& sConfigFileName );

	/**
	 * @brief	Searches for the first file in ini search path. 
	 * @param	file	The filename as absoute or relative path.
	 *                  Absolute path is not searched but only checked
	 *                  for existance.
	 * @param   logger	If non-null, the logger (e.g. cerr).
	 *          In DEBUG mode cout is used if no logger is given. 
	 * @return	The absolute path of the found file in ini search path.
	 *          Empty string on failure.
	 */
	std::string FindFileInIniSearchPath(const std::string &file,
										std::ostream *logger = NULL) const;


	IVistaSystemClassFactory    * m_pSystemClassFactory;

	IVistaWindowingToolkit	* m_pWindowingToolkit;

	VistaEventManager          * m_pEventManager;
	VistaSystemEvent           * m_pSystemEvent;
	VistaEventHandler          * m_pCentralEventHandler;

	VistaExternalMsgEvent      * m_pExternalMsg;
	VistaKernelMsgPort         * m_pPort;

	std::list<VistaKernelMsgPort*> m_liAppPorts;

	VistaDisplayManager        * m_pDisplayManager;
	VistaInteractionManager    * m_pInteractionManager;
	VistaPickManager           * m_pPickManager;
	VistaGraphicsManager	    * m_pNewWorld;
	VistaClusterAux            * m_pClusterAux;
	VistaTimer         * m_pTimer;
	VistaCommandEvent          * m_pCommandEvent;
	VistaKeyboardSystemControl * m_pSystemControl;
	VistaDriverMap             * m_pDriverMap;
	VdfnObjectRegistry         * m_pDfnObjects;
	VistaDriverPropertyConfigurator *m_pConfigurator;

	std::map<VistaDisplaySystem*, VistaUserPlatform*> m_mpUserPlatforms;
	std::list<VistaInteractionContext*> m_liCreateCtxs;

	struct _dllHlp                *m_pDllHlp;

	bool                          m_bInitialized;

	// store the absolute pathes of the ini-files
	std::string                   m_strVistaConfigFile,  /**< this is the global file */
								  m_strGraphicsConfigFile, /**< this is for the graphics */
								  m_strDisplayConfigFile,  /**< reference for the display */
								  m_strInteractionConfigFile, /**< reference for interaction, connections etc. */
								  m_strClusterConfigFile;

	// These are used to flag wether an ini file has been
	// specified via the command line. In that case the settings
	// from the main ini will be ignored (commandline has the last word)
	bool m_bLockGraphicsIni,
         m_bLockDisplayIni,
         m_bLockInteractionIni,
         m_bLockClusterIni;


	std::string                   m_strModelFile;
	std::string                   m_strSystemName;
	std::string                   m_strApplicationName;

	float                         m_nModelScale;
	int                           m_iClusterMode;

	// manage some input device / display connection data
	//std::list<VistaInputDeviceObserver *>  m_liDeviceObservers;
	//std::list<DLVistaDataSink *>           m_liAdaptors;

private:

	bool SetupEventManager ();
	bool SetupDisplayManager ();
	bool SetupCluster (VistaClusterServer *& pServer,
					   VistaClusterClient *&pClient,
					   VistaClusterMaster *&pMaster,
					   VistaClusterSlave  *&pSlave);
	bool SetupGraphicsManager ();
	bool SetupInteractionManager();
	bool SetupPickManager ();

	bool SetupBasicInteraction();

	bool IndicateSystemProgress(int iProgressStage,
								const std::string &sProgressMessageText,
								bool bDone);

	bool LoadPlugin( const std::string &strPathToPlugin );

	bool DoInit (int, char**);

	static void SIGINTHandler(int);
	static void SIGTERMHandler(int);
	static void SIGPIPEHandler(int);

	std::list<std::string> m_liSearchPath;
	bool m_bLockSearchPath;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
//extern VistaSystem * g_pVistaSystem;
VISTAKERNELAPI VistaSystem * GetVistaSystem ();

#endif // _VISTASYSTEM_H
