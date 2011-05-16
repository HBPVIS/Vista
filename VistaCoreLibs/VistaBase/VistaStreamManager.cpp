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

#include "VistaStreamManager.h"

#include "VistaTimer.h"
#include "VistaTimeUtils.h"
#include "VistaStreamUtils.h"

#include <fstream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaStreamManager::VistaStreamManager()
: m_pTimer( new VistaTimer )
, m_pInfo( NULL )
{	
	m_vecInfoLayout.push_back( LE_FRAMECOUNT );
	m_vecInfoLayout.push_back( LE_COLON );
	m_vecInfoLayout.push_back( LE_FRAMECLOCK );
	m_vecInfoLayout.push_back( LE_PIPE );
}

VistaStreamManager::~VistaStreamManager()
{
	for( std::vector<std::ostream*>::iterator itStream = m_vecOwnStreams.begin();
			itStream != m_vecOwnStreams.end(); ++itStream )
	{
		delete (*itStream);
	}
	delete m_pInfo;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

bool VistaStreamManager::AddNewLogFileStream( const std::string& sStreamName,
											const std::string& sFileName,
											const std::string& sFileExtension,
											const bool bAddNodeName,
											const bool bAddTimeStamp )
{
	std::map<std::string, std::ostream*>::iterator itStream
							= m_mpStreams.find(sStreamName);
	if( itStream != m_mpStreams.end() )
		return false; // name already exists

	std::string sFullName;
	if( sFileName.empty() )
	{
		if( sStreamName.empty() )
			sFullName = "logfile";
		else
			sFullName = sStreamName;
	}
	else
		sFullName = sFileName;

	if( bAddTimeStamp )
		sFullName += "." + GetDateString();

	if( bAddNodeName )
		sFullName += "." + GetNodeName();

	if( sFileExtension.empty() == false )
		sFullName += "." + sFileExtension;

	std::ofstream* oStream = new std::ofstream( sFullName.c_str() );

	
	if( oStream->bad() )
	{		
		delete oStream;
		return false;
	}

	m_mpStreams[sStreamName] = oStream;
	m_vecOwnStreams.push_back( oStream );
	return true;
}

bool VistaStreamManager::AddStream( const std::string& sName,
										std::ostream& oStream,
										bool bManageDeletion,
										bool bReplaceExistingStream  )
{
	std::map<std::string, std::ostream*>::iterator itStream = m_mpStreams.find( sName );
	if( itStream != m_mpStreams.end() )
	{
		return false;
	}
	m_mpStreams[sName] = &oStream;
	return true;
}
bool VistaStreamManager::RemoveStream( const std::string& sName,
										const bool bDelete )
{
	std::map<std::string, std::ostream*>::iterator itStream = m_mpStreams.find( sName );
	if( itStream != m_mpStreams.end() )
	{
		m_mpStreams.erase( itStream );		
		return true;
	}
	return false;
}
std::ostream& VistaStreamManager::GetStream( const std::string& sName )
{
	std::map<std::string, std::ostream*>::iterator itStream = m_mpStreams.find( sName );
	if( itStream != m_mpStreams.end() )
		return *((*itStream).second);
	else
		return GetDefaultStream( sName );
}

bool VistaStreamManager::SetInfoLayout( const std::vector<INFO_LAYOUT_ELEMENT>& vecLayout )
{
	vstr::GetStreamManager()->m_vecInfoLayout = vecLayout;
	return true;
}
bool VistaStreamManager::GetInfoLayout( std::vector<INFO_LAYOUT_ELEMENT>& vecLayout )
{
	vecLayout = vstr::GetStreamManager()->m_vecInfoLayout;
	return true;
}

std::vector<VistaStreamManager::INFO_LAYOUT_ELEMENT>& VistaStreamManager::GetInfoLayoutRef()
{
	return m_vecInfoLayout;
}

systemtime VistaStreamManager::GetSystemTime() const
{
	return m_pTimer->GetSystemTime(); 
}
microtime VistaStreamManager::GetFrameClock() const
{
	if( m_pInfo == NULL )
		return 0;
	return m_pInfo->GetFrameClock(); 
}
float VistaStreamManager::GetFrameRate() const
{
	if( m_pInfo == NULL )
		return 0;
	return m_pInfo->GetFrameRate();
}
unsigned int VistaStreamManager::GetFrameCount() const
{
	if( m_pInfo == NULL )
		return 0;
	return m_pInfo->GetFrameCount();
}

std::string VistaStreamManager::GetNodeName() const
{
	if( m_pInfo == NULL )
		return "";
	return m_pInfo->GetNodeName();
}

std::string VistaStreamManager::GetDateString() const
{
	if( m_pInfo != NULL )
		return VistaTimeUtils::ConvertToLexicographicDateString( m_pInfo->GetFrameClock() );
	else
		return VistaTimeUtils::ConvertToLexicographicDateString( m_pTimer->GetSystemTime() );
}

void VistaStreamManager::PrintInfo( std::ostream& oStream ) const
{
	for( std::vector<INFO_LAYOUT_ELEMENT>::const_iterator 
				itElement = m_vecInfoLayout.begin(); 
				itElement != m_vecInfoLayout.end();
				++itElement )
	{
		switch( (*itElement) )
		{
			case LE_FRAMECLOCK:
			{
				oStream << vstr::frameclock;
				break;
			}
			case LE_SYSTEMTIME:
			{
				oStream << vstr::systime;
				break;
			}
			case LE_DATE:
			{
				oStream << vstr::date;
				break;
			}
			case LE_NODENAME:
			{
				oStream << vstr::nodename;
				break;
			}
			case LE_FRAMECOUNT:
			{
				oStream << vstr::framecount;
				break;
			}
			case LE_FRAMERATE:
			{
				oStream << vstr::framerate;
				break;
			}
			case LE_COLON:
			{
				oStream << ": ";
				break;
			}
			case LE_DASH:
			{
				oStream << " - ";
				break;
			}
			case LE_PIPE:
			{
				oStream << " | ";
				break;
			}
			case LE_SPACE:
			{
				oStream << " ";
				break;
			}
			case LE_TAB:
			{
				oStream << "\t";
				break;
			}
			case LE_LINEBREAK:
			{
				oStream << "\n";
				break;
			}
			default:
				break;
		}
	}
}

void VistaStreamManager::SetInfoInterface( VistaStreamManager::IInfoInterface* pInfoInterface )
{
	m_pInfo = pInfoInterface;
}

VistaStreamManager::IInfoInterface* VistaStreamManager::GetInfoInterface() const
{
	return m_pInfo;
}

std::ostream& VistaStreamManager::GetDefaultStream( const std::string& sName )
{
	return std::cout;
}

