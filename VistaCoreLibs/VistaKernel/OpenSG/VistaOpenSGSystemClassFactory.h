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

#ifndef _VISTAOPENSGSGSYSTEMCLASSFACTORY_H
#define _VISTAOPENSGSGSYSTEMCLASSFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <time.h>
#include <string>

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/VistaSystemClassFactory.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaGraphicsEvent.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGWebInterface.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystemEvent;
class VistaGraphicsEvent;
class VistaOpenSGDisplayBridge;
class VistaSystem;
class VistaEventManager;
class VistaDriverMap;
class VistaWeightedAverageTimer;
class IVistaWindowingToolkit;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenSGSystemClassFactory : public IVistaSystemClassFactory
{
public:
	VistaOpenSGSystemClassFactory(VistaSystem * pViSy, int argc, char **argv);

	~VistaOpenSGSystemClassFactory();

	virtual std::vector<IVistaSystemClassFactory::Manager> GetInitOrder() const;

	virtual VistaGraphicsManager    * CreateGraphicsManager();

	virtual IVistaGraphicsBridge     * CreateGraphicsBridge();

	virtual IVistaNodeBridge         * CreateNodeBridge();

	virtual VistaDisplayManager     * CreateDisplayManager();

	virtual IVistaWindowingToolkit  * CreateWindowingToolkit( std::string );

	//virtual VistaOldInteractionManager * CreateOldInteractionManager();
	virtual VistaInteractionManager *CreateInteractionManager(VistaDriverMap *pMap);

	bool Run();

	void Update(void);

	float GetLastFPS() const { return m_fLastFPS; }

	bool GetWebInterfaceEnabled() const;
	void SetWebInterfaceEnabled( bool bState, const unsigned int &port = 8888);

	void Debug(std::ostream &out, bool bVerbose = true) const;

	virtual microtime GetAvgEventLoopTime() const;
	virtual unsigned int GetFrameCount() const;

	/**
	 * Enum to define which windowing toolkit is used
	 */
	enum WINDOWING_TOOLKIT
	{
		UNKNOWN = -1,
		NONE	= 0,
		GLUT
	};

private:
	VistaSystemEvent m_oSystemEvent;
	VistaGraphicsEvent m_oGraphicsEvent;
	VistaSystem       *m_pVistaSystem;
	VistaWeightedAverageTimer   *m_pAvgLoopTime, *m_pFrameRate;
	OSG::RenderAction *m_pRenderAction;

	clock_t      m_LastTime;
	float        m_fLastFPS;
	unsigned int m_iNumFrames;
	long         m_lFCStoreOffset;

	int			 m_iArgc;
	char**		 m_pArgv;

	osg::WebInterface *m_pWeb;

	// store the used windowing toolkit as enum
	WINDOWING_TOOLKIT  m_iWindowingToolkit;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGSYSTEMCLASSFACTORY_H
