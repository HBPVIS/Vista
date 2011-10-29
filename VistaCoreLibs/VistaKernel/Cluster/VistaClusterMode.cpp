
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

#include "VistaClusterMode.h"

#include <VistaInterProcComm/Connections/VistaNetworkSync.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterMode::VistaClusterMode()
: m_iFrameCount( -1 )
, m_dFrameClock( 0 )
, m_pDefaultNetSync( NULL )
{
}

VistaClusterMode::~VistaClusterMode()
{
	delete m_pDefaultNetSync;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaType::systemtime VistaClusterMode::GetFrameClock() const
{
	return m_dFrameClock;
}

int VistaClusterMode::GetFrameCount() const
{
	return m_iFrameCount;
}

IVistaNetworkSync* VistaClusterMode::GetDefaultNetworkSync()
{
	if( m_pDefaultNetSync == NULL )
		m_pDefaultNetSync = CreateNetworkSync( true );
	return m_pDefaultNetSync;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


