/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
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

#include "VistaStreamUtils.h"

#include "VistaStreamManager.h"
#include "VistaStreams.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* StreamManager Singleton                                                    */
/*============================================================================*/

VistaStreamManager* s_pManager = NULL;

VistaStreamManager* vstr::GetStreamManager()
{
	if( s_pManager == NULL )
		s_pManager = new VistaStreamManager;
	return s_pManager;
}
void vstr::SetStreamManager( VistaStreamManager* pStreamManager,
										const bool bDeleteOldManager )
{
	if( s_pManager != NULL && bDeleteOldManager )
		delete s_pManager;
	s_pManager = pStreamManager;
}
void vstr::DestroyStreamManager()
{
	delete s_pManager;
	s_pManager = NULL;
}
std::ostream& vstr::Stream( const std::string& sName )
{
	return GetStreamManager()->GetStream( sName );
}

/*============================================================================*/
/* StreamManipulators                                                         */
/*============================================================================*/

vstr::formattime::formattime( const double dTime )
: m_dTime( dTime )
{
}

std::ostream& vstr::formattime::operator()( std::ostream& oStream ) const
{
	std::ios_base::fmtflags oFlags = oStream.flags();
	std::streamsize nCurrentPrecision = oStream.precision( 6 );		
	oStream.setf( std::ios_base::fixed );
	//oStream.setf( std::ios_base::floatfield );
	oStream << m_dTime;
	oStream.precision( nCurrentPrecision );	
	oStream.flags( oFlags );
	return oStream;
}

vstr::color::color( const vstr::CONSOLE_COLOR iTextColor, 
				   const vstr::CONSOLE_COLOR iBackgroundColor )
: m_iTextColor( iTextColor )
, m_iBackgroundColor( iBackgroundColor )
{
}

std::ostream& vstr::color::operator()( std::ostream& oStream ) const
{
	if( oStream == std::cout || oStream == std::cerr )
	{
		vstr::SetStdConsoleTextColor( m_iTextColor );
		vstr::SetStdConsoleBackgroundColor( m_iBackgroundColor );
		return oStream;
	}

	VistaColorOutstream* pColorStream 
				= dynamic_cast<VistaColorOutstream*>( &oStream );
	if( pColorStream != NULL )
	{
		pColorStream->SetTextColor( m_iTextColor );
		pColorStream->SetBackgroundColor( m_iBackgroundColor );
		return oStream;
	}

	VistaSplitOutstream* pSplitStream
				= dynamic_cast<VistaSplitOutstream*>( &oStream );
	if( pSplitStream != NULL )
	{
		vstr::color oMani( m_iTextColor, m_iBackgroundColor );
		for( std::vector<std::ostream*>::const_iterator 
				itStream = pSplitStream->GetStreams().begin();
				itStream != pSplitStream->GetStreams().end();
				++itStream )
		{
			(*(*itStream)) << oMani;
		}		
	}

	return oStream;
}

vstr::textcolor::textcolor( const vstr::CONSOLE_COLOR iTextColor )
: m_iTextColor( iTextColor )
{
}

std::ostream& vstr::textcolor::operator()( std::ostream& oStream ) const
{
	if( oStream == std::cout || oStream == std::cerr )
	{
		vstr::SetStdConsoleTextColor( m_iTextColor );
		return oStream;
	}

	VistaColorOutstream* pColorStream 
		= dynamic_cast<VistaColorOutstream*>( &oStream );
	if( pColorStream != NULL )
	{
		pColorStream->SetTextColor( m_iTextColor );
		return oStream;
	}

	VistaSplitOutstream* pSplitStream
		= dynamic_cast<VistaSplitOutstream*>( &oStream );
	if( pSplitStream != NULL )
	{
		vstr::textcolor oMani( m_iTextColor );
		for( std::vector<std::ostream*>::const_iterator 
			itStream = pSplitStream->GetStreams().begin();
			itStream != pSplitStream->GetStreams().end();
		++itStream )
		{
			(*(*itStream)) << oMani;
		}		
	}

	return oStream;
}

vstr::bgcolor::bgcolor( const vstr::CONSOLE_COLOR iBackgroundColor )
: m_iBackgroundColor( iBackgroundColor )
{
}

std::ostream& vstr::bgcolor::operator()( std::ostream& oStream ) const
{
	if( oStream == std::cout || oStream == std::cerr )
	{
		vstr::SetStdConsoleBackgroundColor( m_iBackgroundColor );
		return oStream;
	}

	VistaColorOutstream* pColorStream 
		= dynamic_cast<VistaColorOutstream*>( &oStream );
	if( pColorStream != NULL )
	{
		pColorStream->SetBackgroundColor( m_iBackgroundColor );
		return oStream;
	}

	VistaSplitOutstream* pSplitStream
		= dynamic_cast<VistaSplitOutstream*>( &oStream );
	if( pSplitStream != NULL )
	{
		vstr::bgcolor oMani( m_iBackgroundColor );
		for( std::vector<std::ostream*>::const_iterator 
			itStream = pSplitStream->GetStreams().begin();
			itStream != pSplitStream->GetStreams().end();
		++itStream )
		{
			(*(*itStream)) << oMani;
		}		
	}

	return oStream;
}

std::ostream& vstr::systime( std::ostream& oStream )
{
	oStream << std::setw( 11 )
		<< vstr::formattime( vstr::GetStreamManager()->GetSystemTime() );
	return oStream;
}
std::ostream& vstr::frameclock( std::ostream& oStream )
{
	oStream << vstr::formattime( vstr::GetStreamManager()->GetFrameClock() );	
	return oStream;
}
std::ostream& vstr::date( std::ostream& oStream )
{
	oStream << std::setw(16) 
		<< vstr::GetStreamManager()->GetDateString();
	return oStream;
}
std::ostream& vstr::framecount( std::ostream& oStream )
{
	oStream << std::setw(8) 
		<< vstr::GetStreamManager()->GetFrameCount();
	return oStream;
}
std::ostream& vstr::info( std::ostream& oStream )
{
	GetStreamManager()->PrintInfo( oStream );
	return oStream;
}
std::ostream& vstr::nodename( std::ostream& oStream )
{
	oStream << vstr::GetStreamManager()->GetNodeName();
	return oStream;
}
std::ostream& vstr::framerate( std::ostream& oStream )
{
	oStream << vstr::GetStreamManager()->GetFrameRate();
	return oStream;
}	



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
