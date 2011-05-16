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

#ifndef _VISTACLUSTERAUX_H
#define _VISTACLUSTERAUX_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaClusterClient;
class VistaClusterServer;

class VistaClusterMaster;
class VistaClusterSlave;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterAux
{
public:
	VistaClusterAux();

	VistaClusterAux(VistaClusterServer *pServer,
					 VistaClusterClient *pClient);

	VistaClusterAux( VistaClusterMaster *pMaster,
			          VistaClusterSlave  *pSlave );

	~VistaClusterAux();

	VistaClusterClient *    GetVistaClusterClient () const;
	VistaClusterServer *    GetVistaClusterServer () const;

	VistaClusterMaster *    GetVistaClusterMaster() const;
	VistaClusterSlave  *    GetVistaClusterSlave() const;


	bool GetIsClient() const;
	bool GetIsServer() const;
	bool GetIsStandalone() const;

	bool GetIsMaster() const;
	bool GetIsSlave() const;


	double GetFrameClock() const;
	int    GenerateRandomNumber(int iLowerBound, int iMaxBound) const;
	void   SetRandomSeed(int iSeed);
	void   SetFrameClock(double dFrameClock);

	int GetFrameCount() const;

	std::string GetNodeName() const;

	microtime GetAvgUpdateTime() const;
	microtime GetAvgSwapTime() const;
protected:
private:
	VistaClusterServer *m_pServer;
	VistaClusterClient *m_pClient;

	VistaClusterMaster *m_pMaster;
	VistaClusterSlave  *m_pSlave;

	double m_nFrameClock;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERAUX_H

