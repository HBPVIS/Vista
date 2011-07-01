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

#ifndef _VISTACLUSTERMASTER_H
#define _VISTACLUSTERMASTER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <list>
#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaClusterAux;
class VistaWeightedAverageTimer;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterMaster
{
public:
	VistaClusterMaster(VistaEventManager *pEvMgr,
			            const std::string &strMasterName );
	~VistaClusterMaster();

	// ##################################################################
	// stats
	// ##################################################################
	std::string     GetMasterSectionName() const;
	std::string     GetMasterName() const;
	VistaType::microtime       GetAvgUpdateTime() const;
	VistaType::microtime       GetAvgSwapTime() const;

	// #####################################################################
	// interaction
	// #####################################################################
	int    GetCurrentFrameIndex() const;

	// #####################################################################
	// CLUSTER MANAGEMENT
	// #####################################################################
	bool AddSlave(const std::string &strName,
			      const std::string &sIpString,
			      int iPort, bool bDoSwap, bool bDoSync);
	bool RemSlave( const std::string &strName );

	int  GetNumberOfSlaves() const;
	void GetSlaveNames(std::list<std::string> &liStorage) const;

	// ####################################################################
	// SYSTEM API
	void   BeginClusterProcessing();
	void   WaitForSlaveInit();
	void   SetClusterAux(VistaClusterAux *);
	bool   Sync();
	bool   Init( const std::string &strIniFile,
				 const std::string &strSectionName);
	// ####################################################################


	// #####################################################################
	// print debug info into stream
	// #####################################################################
	virtual void Debug ( std::ostream & out ) const;
protected:
private:
	VistaEventManager *m_pEvMgr;
	VistaClusterAux   *m_pAux;
	std::string         m_strMasterName,        /*<< symbolic name for the master node */
	                    m_strMasterSectionName; /*<< the setup db section key */
	bool                m_bWakeupDone;
	VistaWeightedAverageTimer *m_pAvgBc;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// internal API
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	class EventObserver;
	EventObserver     *m_pEventObserver;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERMASTER_H

