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

#include "VistaKernelOut.h"

#include "VistaBase/VistaBaseOut.h"
#include "VistaAspects/VistaAspectsOut.h"
#include "VistaDataFlowNet/VdfnOut.h"
#include "VistaInterProcComm/VistaInterProcCommOut.h"
#include "VistaKernelOpenSGExt/VistaKernelOpenSGExtOut.h"
#include "VistaMath/VistaMathOut.h"
#include "VistaTools/VistaToolsOut.h"
#include "VistaKernelOpenSGExt/VistaKernelOpenSGExtOut.h"
#include "VistaDeviceDriversBase/VistaDeviceDriversOut.h"

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
std::ostream* VistaKernelOut::m_out = &std::cout;
std::ostream* VistaKernelOut::m_err = &std::cerr;
std::ostream* VistaKernelOut::m_log = &std::clog;

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
std::ostream& VistaKernelOut::out()
{	
	return *m_out;
}

std::ostream& VistaKernelOut::err()
{
	return *m_err;
}

std::ostream& VistaKernelOut::log()
{
	return *m_log;
}

void VistaKernelOut::SetOutStream(std::ostream * pStream)
{
	m_out = pStream;
}

void VistaKernelOut::SetErrorStream(std::ostream * pStream)
{
	m_err = pStream;
}

void VistaKernelOut::SetLogStream(std::ostream * pStream)
{
	m_log = pStream;
}

void VistaKernelOut::SetAllOutStreams(std::ostream* pStream)
{
	VistaBaseOut::SetOutStream( pStream );
	VistaAspectsOut::SetOutStream( pStream );
	VistaInterProcCommOut::SetOutStream( pStream );
	VistaMathOut::SetOutStream( pStream );
	VistaToolsOut::SetOutStream( pStream );
	VdfnOut::SetOutStream( pStream );
	VistaDeviceDriversOut::SetOutStream( pStream );
	VistaKernelOut::SetOutStream( pStream );
	//VistaKernelOpenSGExtOut::SetOutStream( pStream );
}

void VistaKernelOut::SetAllErrorStreams(std::ostream* pStream)
{
	VistaBaseOut::SetErrorStream( pStream );
	VistaAspectsOut::SetErrorStream( pStream );
	VistaInterProcCommOut::SetErrorStream( pStream );
	VistaMathOut::SetErrorStream( pStream );
	VistaToolsOut::SetErrorStream( pStream );
	VdfnOut::SetErrorStream( pStream );
	VistaDeviceDriversOut::SetErrorStream( pStream );
	VistaKernelOut::SetErrorStream( pStream );
	//VistaKernelOpenSGExtOut::SetErrorStream( pStream );
}

void VistaKernelOut::SetAllLogStreams(std::ostream * pStream)
{
	VistaBaseOut::SetLogStream( pStream );
	VistaAspectsOut::SetLogStream( pStream );
	VistaInterProcCommOut::SetLogStream( pStream );
	VistaMathOut::SetLogStream( pStream );
	VistaToolsOut::SetLogStream( pStream );
	VdfnOut::SetLogStream( pStream );
	VistaDeviceDriversOut::SetLogStream( pStream );
	VistaKernelOut::SetLogStream( pStream );
	//VistaKernelOpenSGExtOut::SetLogStream( pStream );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

