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

#ifndef _VISTACLUSTERSLAVE_H
#define _VISTACLUSTERSLAVE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <list>
#include <vector>
#include <ostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaClusterAux;

class VistaConnectionIP;
class IVistaSocket;
class VistaInteractionEvent;
class VistaSystemEvent;
class VistaExternalMsgEvent;
class VistaInteractionManager;
class VistaMsg;
class VistaWeightedAverageTimer;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterSlave
{
public:
	VistaClusterSlave(VistaEventManager *pEvMgr,
			           const std::string &strSlaveName );
	~VistaClusterSlave();


	unsigned int GetSlaveIndex() const;
	std::string  GetSlaveName() const;
	std::string  GetSlaveSectionName() const;
	unsigned int GetCurrentFrameToken() const;


	void SetClusterAux(VistaClusterAux *pAux);

	void SetInteractionManager( VistaInteractionManager *pMgr );
	bool WaitForClusterMaster( const std::string &strIp, int nPort, int nAckPort );
	bool PostSystemInitEvent();
	/**
	 * @return true iff loop exit is signalled
	 */
	bool ProcessCurrentFrame();
	bool Init( const std::string &strIniFile,
			   const std::string &strSectionName);

	void SwapSync();

	virtual void Debug ( std::ostream & out ) const;

	VistaType::microtime GetAvgUpdateTime() const;
	VistaType::microtime GetAvgSwapTime() const;
protected:
private:
	std::string         m_strSlaveName,
	                    m_strSlaveSectionName;
	unsigned int        m_nSlaveIndex;
	unsigned int        m_nBeforeAckDelay,
	                    m_nBeforeSwapDelay,
	                    m_nAfterSwapSyncDelay;

	int                 m_nFrameToken;
	bool                m_bDoSwap,
	                    m_bDoOglFinish;
	VistaEventManager *m_pEvMgr;
	VistaClusterAux   *m_pAux;

	VistaConnectionIP *m_pConnection,
	                   *m_pAckConnection;
	IVistaSocket       *m_pSyncSocket;
	VistaWeightedAverageTimer   *m_pAvgUpd, *m_pAvgSwap;

	std::vector<unsigned char> m_vecMessage;

	// the events to be thrown
	VistaSystemEvent        *m_pSystemEvent;
	VistaExternalMsgEvent   *m_pExternalMsgEvent;
	VistaMsg                *m_pMsg;
	VistaInteractionEvent   *m_pInteractionEvent;
	VistaInteractionManager *m_pInMgr;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERSLAVE_H
