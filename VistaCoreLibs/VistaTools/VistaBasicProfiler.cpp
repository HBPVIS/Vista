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

#include "VistaBasicProfiler.h"

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

#include <map>
#include <list>
#include <iomanip>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaBasicProfiler* pSingleton = NULL;


VistaBasicProfiler::ProfileScopeObject::ProfileScopeObject( const std::string& sName )
: m_sName( sName )
{
	VistaBasicProfiler::GetSingleton()->StartSection( sName );
}

VistaBasicProfiler::ProfileScopeObject::~ProfileScopeObject()
{
	VistaBasicProfiler::GetSingleton()->StopSection( m_sName );
}



/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaBasicProfiler::VistaBasicProfiler()
: m_pProfileRoot( new ProfileTreeNode( "ViSTA", NULL ) )
, m_iMaxNameLength( 5 )
{
	m_pProfileCurrent = m_pProfileRoot;
}

VistaBasicProfiler::VistaBasicProfiler( const std::string& sRootName )
: m_pProfileRoot( new ProfileTreeNode( sRootName, NULL ) )
, m_iMaxNameLength( (int)sRootName.size() )
{
	m_pProfileCurrent = m_pProfileRoot;
}

VistaBasicProfiler::~VistaBasicProfiler()
{
	delete m_pProfileRoot;
}

void VistaBasicProfiler::StartSection( const std::string& sName )
{
	m_pProfileCurrent = m_pProfileCurrent->Sub( sName );
	m_iMaxNameLength = std::max<int>( m_iMaxNameLength, (int)sName.size() );	
}

void VistaBasicProfiler::StopSection()
{
	m_pProfileCurrent = m_pProfileCurrent->Leave();
}

bool VistaBasicProfiler::StopSection( const std::string& sName )
{
	if( m_pProfileCurrent->m_sName == sName )
	{
		m_pProfileCurrent = m_pProfileCurrent->Leave();
		return true;
	}
	else
	{
		std::cerr << "[VistaBasicProfiler]: trying to end section ["
				<< sName << "], but section [" 
				<< m_pProfileCurrent->m_sName
				<< "] is active!" << std::endl;
		return false;
	}
}

VistaBasicProfiler* VistaBasicProfiler::GetSingleton()
{
	return pSingleton;
}

void VistaBasicProfiler::SetSingleton( VistaBasicProfiler* pProfiler )
{
	pSingleton = pProfiler;
}

void VistaBasicProfiler::PrintProfile( std::ostream& oStream, int iMaxDepth )
{	
	std::ios_base::fmtflags oFlags = oStream.flags();
	std::streamsize nCurrentPrecision = oStream.precision( 3 );		
	oStream.setf( std::ios_base::fixed );
	
	oStream << std::string( iMaxDepth + m_iMaxNameLength - 8, ' ' )
			<< "SectionName"
			<< " || last frame| fast avg  | slow avg  | max \n";
	oStream << std::string( iMaxDepth+ m_iMaxNameLength + 52, '-' ) << "\n";
	m_pProfileRoot->Print( oStream, 0, iMaxDepth, m_iMaxNameLength );

	oStream.precision( nCurrentPrecision );	
	oStream.flags( oFlags );
	oStream.flush();
}

void VistaBasicProfiler::NewFrame()
{
	if( m_pProfileRoot->m_dEntryTime != 0.0 ) // not the first frame
	{
		m_pProfileRoot->Leave();
		m_pProfileRoot->NewFrame();
	}
	m_pProfileRoot->Enter();
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::GetRoot() const
{
	return m_pProfileRoot;
}

/*============================================================================*/
/* PROFILETREENODE                                                            */
/*============================================================================*/


VistaBasicProfiler::ProfileTreeNode::ProfileTreeNode( const std::string& sName, ProfileTreeNode* pParent ) : m_pParent( pParent )
, m_sName( sName )
, m_dCurrentFrame( 0 )
, m_dLastFrame( 0 )
, m_dSlowAvg( 0 )
, m_dFastAvg( 0 )
, m_dEntryTime( 0 )
, m_dMax( 0 )
{

}

VistaBasicProfiler::ProfileTreeNode::~ProfileTreeNode()
{
	for( std::map<std::string, ProfileTreeNode*>::iterator itChild = m_mapChildren.begin();
		itChild != m_mapChildren.end(); ++itChild )
		delete (*itChild).second;
}

void VistaBasicProfiler::ProfileTreeNode::Enter()
{
	m_dEntryTime = VistaTimeUtils::GetStandardTimer().GetMicroTime();
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::Sub( const std::string& sName )
{
	std::map<std::string, ProfileTreeNode*>::iterator it = m_mapChildren.find( sName );
	if( it == m_mapChildren.end() )
	{
		m_pCurrentChild = new ProfileTreeNode( sName, this );
		m_mapChildren[sName] = m_pCurrentChild;
		m_liChildren.push_back( m_pCurrentChild );
	}
	else
		m_pCurrentChild = (*it).second;

	m_pCurrentChild->Enter();
	return m_pCurrentChild;
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::Leave()
{
	m_dCurrentFrame += VistaTimeUtils::GetStandardTimer().GetMicroTime() - m_dEntryTime;
	return m_pParent;
}

void VistaBasicProfiler::ProfileTreeNode::Print( std::ostream& oStream, int iDepth, int iMaxDepth, int iMaxNameLength )
{
	if( m_mapChildren.empty() )
	{
		oStream << std::string( iDepth, ' ' ) << char(200)
			<< char(205) << char(205)
			<< std::left << std::setw( iMaxNameLength + iMaxDepth - iDepth ) 
			<< m_sName << std::right
			<< " || " << std::setw(7) << 1000.0*m_dLastFrame << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dFastAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dSlowAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dMax << "ms\n";
	}
	else
	{
		oStream << std::string( iDepth, ' ' ) << char(200)
			<< char(203) << char(205)
			<< std::left << std::setw( iMaxNameLength + iMaxDepth - iDepth ) 
			<< m_sName << std::right
			<< " || " << std::setw(7) << 1000.0*m_dLastFrame << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dFastAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dSlowAvg << "ms"
			<< " | " << std::setw(7) << 1000.0*m_dMax << "ms\n";

		if( iDepth > iMaxDepth )
			return;

		++iDepth;
		for( std::list<ProfileTreeNode*>::iterator itChild = m_liChildren.begin();
			itChild != m_liChildren.end(); ++itChild )
			(*itChild)->Print( oStream, iDepth, iMaxDepth, iMaxNameLength );
	}
}

void VistaBasicProfiler::ProfileTreeNode::NewFrame()
{
	m_dLastFrame = m_dCurrentFrame;
	m_dMax = std::max<VistaType::microtime>( m_dMax, m_dLastFrame );
	//@todo: inaccurate at the beginning
	if( m_dSlowAvg > 0.0 )
	{
		m_dSlowAvg = 0.005 * m_dLastFrame + 0.995 * m_dSlowAvg;
		m_dFastAvg = 0.025 * m_dLastFrame + 0.975 * m_dFastAvg;
	}
	else
	{
		m_dSlowAvg = m_dLastFrame;
		m_dFastAvg = m_dLastFrame;
	}
	for( std::map<std::string, ProfileTreeNode*>::iterator itChild = m_mapChildren.begin();
		itChild != m_mapChildren.end(); ++itChild )
		(*itChild).second->NewFrame();

	m_dCurrentFrame = 0;
}

int VistaBasicProfiler::ProfileTreeNode::GetNumChildren() const
{
	return (int)m_liChildren.size();
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::GetChild( int iIndex ) const
{
	std::list<ProfileTreeNode*>::const_iterator itChild = m_liChildren.begin();
	int iCurrentIndex = 0;
	for( std::list<ProfileTreeNode*>::const_iterator itChild = m_liChildren.begin();
				itChild != m_liChildren.end(); ++itChild, ++iCurrentIndex )
	{
		if( iCurrentIndex == iIndex ) 
			return (*itChild);
	}
return NULL;
}

VistaBasicProfiler::ProfileTreeNode* VistaBasicProfiler::ProfileTreeNode::GetChild( 
													const std::string& sName,
													bool bSearchWholeSubtree ) const
{
	std::map<std::string, ProfileTreeNode*>::const_iterator itChild 
											= m_mapChildren.find( sName );
	if( itChild != m_mapChildren.end() )
		return (*itChild).second;

	if( bSearchWholeSubtree )
	{	
		for( std::list<ProfileTreeNode*>::const_iterator itChild = m_liChildren.begin();
				itChild != m_liChildren.end(); ++itChild )
		{
			ProfileTreeNode* pSubtreeResult = (*itChild)->GetChild( sName, true );
			if( pSubtreeResult != NULL ) 
				return pSubtreeResult;
		}
	}

	return NULL;
}

VistaType::microtime VistaBasicProfiler::ProfileTreeNode::GetLastFrameTime() const
{
	return m_dLastFrame;
}

VistaType::microtime VistaBasicProfiler::ProfileTreeNode::GetFastAverage() const
{
	return m_dFastAvg;
}

VistaType::microtime VistaBasicProfiler::ProfileTreeNode::GetSlowAverage() const
{
	return m_dSlowAvg;
}

bool VistaBasicProfiler::ProfileTreeNode::operator<( const VistaBasicProfiler::ProfileTreeNode& oCompare )
{
	return m_dEntryTime < oCompare.m_dEntryTime;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

