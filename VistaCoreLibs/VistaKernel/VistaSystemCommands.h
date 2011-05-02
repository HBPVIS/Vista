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

#ifndef _VISTASYSTEMCOMMANDS_H
#define _VISTASYSTEMCOMMANDS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/VistaKernelOut.h>
#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <map>
#include <string>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaGraphicsManager;
class VistaDisplayManager;
class VistaPickManager;
class VistaKeyboardSystemControl;
class VistaEventManager;
class VistaInteractionManager;
class VistaInteractionContext;
class VistaClusterAux;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAKERNELAPI VistaQuitCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaQuitCommand(VistaSystem *pSys);
	virtual bool Do();

	VistaSystem *m_pSys;
};

class VISTAKERNELAPI VistaToggleFramerateCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleFramerateCommand(VistaGraphicsManager *pGrMgr);

	virtual bool Do();

	VistaGraphicsManager *m_pGrMgr;
};

class VISTAKERNELAPI VistaToggleCursorCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleCursorCommand( VistaDisplayManager *pDisMgr );
	virtual bool Do();

	VistaDisplayManager *m_pDisMgr;
};

class VISTAKERNELAPI VistaShowAvailableCommands : public IVistaExplicitCallbackInterface
{
public:
	VistaShowAvailableCommands(VistaKeyboardSystemControl *pSysCtrl,
								VistaSystem *pSys);
	virtual bool Do();

	VistaKeyboardSystemControl *m_pCtrl;
	VistaSystem *m_pSys;
};


class VISTAKERNELAPI VistaReloadContextGraphCommand : public IVistaExplicitCallbackInterface
{
public:
    VistaReloadContextGraphCommand( VistaSystem *pSys,
                                     VistaInteractionContext *pCtx,
                                     const std::string &strRoleId,
                                     bool bDumpGraph, bool bWritePorts );

    virtual bool Do();
private:
    VistaInteractionContext    *m_pCtx;
    VistaSystem                *m_pSys;
    std::string m_strRoleId;
    bool m_bDumpGraph,
         m_bWritePorts;
};

template<class T>
class TVistaDebugToConsoleCommand : public IVistaExplicitCallbackInterface
{
public:
	TVistaDebugToConsoleCommand( T *pObj )
	: IVistaExplicitCallbackInterface(),
	  m_pObj(pObj)
	  {

	  }

	bool Do()
	{
		(*m_pObj).Debug(vkernout);
		return true;
	}

	T *m_pObj;
};

class VISTAKERNELAPI VistaPrintProfilerOutputCommand : public IVistaExplicitCallbackInterface
{
public:
    virtual bool Do();
private:   
};


class VistaToggleFrustumCullingCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleFrustumCullingCommand( VistaGraphicsManager *pGfxMgr );

	bool Do();
private:
	VistaGraphicsManager *m_pGfxMgr;
};

class VistaToggleOcclusionCullingCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleOcclusionCullingCommand( VistaGraphicsManager *pGfxMgr );

	bool Do();
private:
	VistaGraphicsManager *m_pGfxMgr;

};

class VistaToggleBBoxDrawingCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleBBoxDrawingCommand(VistaGraphicsManager *pGfxMgr);

	bool Do();
private:
	VistaGraphicsManager *m_pGfxMgr;
};

class VISTAKERNELAPI VistaChangeEyeDistanceCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaChangeEyeDistanceCommand( const float fChangeValue,
									VistaDisplayManager* pDisplayManager );
	virtual bool Do();
private:
	float					m_fChangeValue;
	VistaDisplayManager*	m_pDisplayManager;
};

class VISTAKERNELAPI VistaToggleVSyncCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleVSyncCommand( VistaDisplayManager* pDisplayManager );
	virtual bool Do();
private:
	VistaDisplayManager*	m_pDisplayManager;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEMCOMMANDS_H
