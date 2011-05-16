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

#ifndef _VISTADATATUNNELFACTORY_H
#define _VISTADATATUNNELFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <string>
#include <list>

#include <VistaKernel/VistaKernelConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IDLVistaDataPacket;
class IVistaDataTunnel;
class VistaNetworkSync;
class VistaClusterAux;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
* VistaDataTunnelFactory creates VistaDataTunnels custom tailored to the current configuration ViSTA
* is running in (refer to VistaDataTunnel.h for details). Basically there are three types of tunnels
* which can be generated:
* -	VistaStandaloneDataTunnel		in case the application is running in STANDALONE mode
* - VistaMasterDataTunnel			in case the application is running in CLUSTER-MASTER mode
* - VistaClientDataTunnel			in case the application is running in CLUSTER-CLIENT mode
* Due to the transparent creation of the data tunnel, the application won't need to bother with implementation
* details about standalone/cluster usage. Thus, the very same application can be run in standalone and cluster
* mode without any additional changes.
*
*/

class VISTAKERNELAPI VistaDataTunnelFactory
{
public:
	static VistaDataTunnelFactory* GetFactory(VistaClusterAux *pAux);
	virtual ~VistaDataTunnelFactory();

	/**
	*
	*/
	IVistaDataTunnel* CreateDataTunnel(IDLVistaDataPacket* pPacketPrototype,
		const std::string &strClusterIniFile) const;

	/**
	* Create a new network sync object and connect it. After creation the
	* user will be able to use the sync object right away.
	* The sync object data (ports and addresses) will be read from the
	* cluster.ini information.
	* NULL is returned if creation fails for some reason.
	*/
	VistaNetworkSync *CreateSync(const std::string &strClusterIniFile);

	/**
	* Destroy factory singleton
	*/
	static void DestroyFactory();
protected:
	/**
	*
	*/
	VistaDataTunnelFactory( VistaClusterAux *pClusterAux );
private:
	/**
	* static singleton pointer
	*/
	static VistaDataTunnelFactory* m_pSingleton;
	/**
	* id of this node:
	* - -1 == STANDALONE
	* -  0 == MASTER
	* - >0 == CLIENT No i
	*/
	int m_iNodeId;
	VistaClusterAux *m_pClusterAux;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADATATUNNELFACTORY_H

