/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#ifndef _VISTACLUSTERMODE_H
#define _VISTACLUSTERMODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
class VistaInteractionContext;
class IVistaDataTunnel;
class VistaConnectionIP;
class IVistaNetworkSync;
class IDLVistaDataPacket;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterMode
{
public:
	virtual ~VistaClusterMode();

	virtual bool Init( const std::string& sClusterSection,
						const VistaPropertyList& oConfig ) = 0;
	virtual bool PostInit() = 0;

	enum CLUSTER_MODE
	{
		CM_INVALID = -1,
		CM_UNKNOWN = 0,
		CM_STANDALONE,
		CM_MASTER_SLAVE,
	};
	virtual int GetClusterMode() const = 0;
	virtual std::string GetClusterModeName() const = 0;
	
	enum CLUSTER_NODE_TYPE
	{
		NT_INVALID = -1,
		NT_UNKNOWN = 0,
		NT_STANDALONE,
		NT_MASTER,
		NT_SLAVE,
	};
	virtual int GetNodeType() const = 0;
	virtual std::string GetNodeTypeName() const = 0;
	virtual std::string GetNodeName() const = 0;
	virtual std::string GetConfigSectionName() const = 0;
	virtual int GetNodeID() const = 0;

	virtual bool GetIsLeader() const = 0;
	virtual bool GetIsFollower() const = 0;	

	virtual int GetNumberOfNodes() const = 0;	
	virtual std::string GetNodeName( const int iNodeID ) const = 0;
	struct NodeInfo
	{
		std::string			m_sNodeName;
		CLUSTER_NODE_TYPE	m_eNodeType;
		int					m_iNodeID;
		bool				m_bIsActive;
	};
	virtual bool GetNodeInfo( const int iNodeID, NodeInfo& oInfo ) const = 0;

	virtual bool StartFrame() = 0;
	virtual bool ProcessFrame() = 0;
	virtual bool EndFrame() = 0;

	virtual void SwapSync() = 0;
	
	virtual bool CreateConnections( std::vector<VistaConnectionIP*>& vecConnections ) = 0;
	virtual bool CreateNamedConnections( std::vector<std::pair<VistaConnectionIP*, std::string> >&
															vecConnections ) = 0;
	virtual IVistaDataTunnel* CreateDataTunnel( IDLVistaDataPacket* pPacketProto ) = 0;
	virtual IVistaNetworkSync* CreateNetworkSync( bool bUseExistingConnections = false ) = 0;
	virtual IVistaNetworkSync* GetDefaultNetworkSync();

	virtual void Debug( std::ostream& oStream ) const = 0;
	
	virtual VistaType::systemtime GetFrameClock() const;
	virtual int GetFrameCount() const;

	
protected:
	VistaClusterMode();

protected:
	VistaType::systemtime			m_dFrameClock;
	int					m_iFrameCount;
	IVistaNetworkSync*	m_pDefaultNetSync;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERMODE_H
