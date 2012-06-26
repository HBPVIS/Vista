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


#ifndef _VISTANETSYNCEDTIMERIMP_H
#define _VISTANETSYNCEDTIMERIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaDefaultTimerImp.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaClusterMode;
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Default implementation of IVistaTimerImp
 * Should not be used directly - use VistaTimer instead, which automatically
 * retrieves the TimerImpl singleton
 */
class VISTAKERNELAPI VistaNetSyncedTimerImp : public VistaDefaultTimerImp
{
public:
	VistaNetSyncedTimerImp();	
	virtual ~VistaNetSyncedTimerImp();

	void Sync( VistaClusterMode* pCluster, int nIterations = 1000 );
	void Sync( VistaConnectionIP* pConn, bool bIsSlave, int nIterations = 1000 );
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif /* _VISTANETSYNCEDTIMERIMP_H */
