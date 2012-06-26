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
// $Id: VistaTCPIPClusterDataSync.cpp 29861 2012-04-10 15:04:51Z dr165799 $

#include "VistaZeroMQCommon.h"

#include <zmq.hpp>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

int S_nUserCount = 0;
zmq::context_t* S_pContext = NULL;


void VistaZeroMQCommon::RegisterZeroMQUser()
{
	if( S_nUserCount == 0 )
	{
		// @todo: thread count?
		S_pContext = new zmq::context_t( 1 );
	}
	++S_nUserCount;
}

void VistaZeroMQCommon::UnregisterZeroMQUser()
{
	if( --S_nUserCount == 0 )
	{
		zmq_term( S_pContext );
	}
}

zmq::context_t& VistaZeroMQCommon::GetContext()
{
	assert( S_pContext );
	return *S_pContext;
}
