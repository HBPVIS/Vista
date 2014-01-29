/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#include "VistaDriverAbstractWindowAspect.h" 
#include "VistaDeviceDriverAspectRegistry.h"

#include <cassert>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverAbstractWindowAspect::m_nAspectId  = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverAbstractWindowAspect::VistaDriverAbstractWindowAspect()
: IVistaDeviceDriver::IVistaDeviceDriverAspect(),
  m_pTouchSeq(NULL)
{
	if(VistaDriverAbstractWindowAspect::GetAspectId() == -1) // unregistered
		VistaDriverAbstractWindowAspect::SetAspectId( 
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("WINDOW"));

	SetId(VistaDriverAbstractWindowAspect::GetAspectId());
}

VistaDriverAbstractWindowAspect::~VistaDriverAbstractWindowAspect()
{
	delete m_pTouchSeq;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaDriverAbstractWindowAspect::AttachToWindow( const WindowHandle& oWindow )
{
	if(!m_pTouchSeq)
		return false;

	return (*m_pTouchSeq).AttachSequence( oWindow );
}

bool VistaDriverAbstractWindowAspect::DetachFromWindow( const WindowHandle& oWindow )
{
	if(!m_pTouchSeq)
		return false;
	return (*m_pTouchSeq).DetachSequence( oWindow );
}


// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverAbstractWindowAspect::GetAspectId()
{
	return VistaDriverAbstractWindowAspect::m_nAspectId;
}

void VistaDriverAbstractWindowAspect::SetAspectId(int nId)
{
	assert(VistaDriverAbstractWindowAspect::m_nAspectId == -1);
	VistaDriverAbstractWindowAspect::m_nAspectId = nId;
}


bool VistaDriverAbstractWindowAspect::SetTouchSequence(IVistaDriverAbstractWindowTouchSequence *pAtSeq)
{
	m_pTouchSeq = pAtSeq;
	return true;
}

VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence *VistaDriverAbstractWindowAspect::GetTouchSequence() const
{
	return m_pTouchSeq;
}

std::list<VistaDriverAbstractWindowAspect::WindowHandle> VistaDriverAbstractWindowAspect::GetWindowList() const
{
	if(m_pTouchSeq)
		return (*m_pTouchSeq).GetWindowList();

	return std::list<WindowHandle>();
}

///////////////////////////////////////////////////////////////////////////

VistaDriverAbstractWindowAspect::WindowHandle::WindowHandle( int iWindowID,
															OSHANDLE pHandle,
															VistaWindow* pWindow )
: m_iID( iWindowID )
, m_pHandle( pHandle )
, m_pWindow( pWindow )
{
}

VistaDriverAbstractWindowAspect::WindowHandle::OSHANDLE VistaDriverAbstractWindowAspect::WindowHandle::GetOSHandle() const
{
	return m_pHandle;
}

int VistaDriverAbstractWindowAspect::WindowHandle::GetID() const
{
	return m_iID;
}

VistaWindow* VistaDriverAbstractWindowAspect::WindowHandle::GetWindow() const
{
	return m_pWindow;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


