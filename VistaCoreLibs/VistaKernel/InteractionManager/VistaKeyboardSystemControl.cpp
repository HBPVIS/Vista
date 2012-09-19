/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#include "VistaKeyboardSystemControl.h"

#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaConversion.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cassert>
#include <algorithm>
#include <sstream>
#include <iomanip>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VistaKeyboardSystemControl::SKeyboardAction::SKeyboardAction()
: m_pCallback(NULL)
{
}

VistaKeyboardSystemControl::SKeyboardAction::SKeyboardAction(
										IVistaExplicitCallbackInterface *pCommand,
										const std::string &strHelpText,
										const bool bCallOnKeyRepreat,
										const bool bManageDeletion )
: m_pCallback( pCommand )
, m_strHelpText(strHelpText)
, m_bManageDeletion( bManageDeletion )
, m_bCallOnKeyRepreat( bCallOnKeyRepreat )
{
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaKeyboardSystemControl::VistaKeyboardSystemControl( const bool bCheckForKeyRepeat )
: m_pKeySink(NULL)
, m_bCheckForKeyRepeat( bCheckForKeyRepeat )
, m_vecModdedCommandMaps( 8 )
{

}

VistaKeyboardSystemControl::~VistaKeyboardSystemControl()
{
	for( CommandMap::iterator it = m_mapCommandMap.begin();
			it != m_mapCommandMap.end(); ++it)
	{
		if( (*it).second.m_bManageDeletion )
			delete (*it).second.m_pCallback;
	}

	for( int i = 0; i < (int)m_vecModdedCommandMaps.size(); ++i )
	{
		for( CommandMap::iterator it = m_vecModdedCommandMaps[i].begin();
			it != m_vecModdedCommandMaps[i].end(); ++it)
		{
			if( (*it).second.m_bManageDeletion )
				delete (*it).second.m_pCallback;
		}
	}
}

bool VistaKeyboardSystemControl::GetCheckForKeyRepeat() const
{
	return m_bCheckForKeyRepeat;
}
void VistaKeyboardSystemControl::SetCheckForKeyRepeat( const bool bSet )
{
	m_bCheckForKeyRepeat = bSet;
}


bool VistaKeyboardSystemControl::InjectKeys( const std::vector<int> &vecKeyCodeList,
		                                      const std::vector<int> &vecModCodeList,
		                                      std::vector<int> &vecUnswallowedKeys,
		                                      std::vector<int> &vecUnswallowedMods)
{
	assert( vecKeyCodeList.size() == vecModCodeList.size());

	std::vector<int>::const_iterator itKey = vecKeyCodeList.begin();
	std::vector<int>::const_iterator itMods = vecModCodeList.begin();

	if( m_bCheckForKeyRepeat )
	{
#ifdef WIN32
		for(; itKey != vecKeyCodeList.end(); ++itKey, ++itMods )
		{
			// check for key repeat
			bool bWasPressed = false;
			if( m_vecLastFrameKeys.empty() == false )
			{
				std::vector<int>::const_iterator itLastPress 
						= std::find( m_vecLastFrameKeys.begin(), m_vecLastFrameKeys.end(), (*itKey) );
				if( itLastPress != m_vecLastFrameKeys.end() ) // was the key pressed last frame?
				{
					std::vector<int>::const_iterator itLastReleased 
							= std::find<std::vector<int>::const_iterator, int>( 
									itLastPress, m_vecLastFrameKeys.end(), -(*itKey) );
					if( itLastReleased == m_vecLastFrameKeys.end() ) // was not released again last frame
						bWasPressed = true;
				}
			}
			if(InjectKey( (*itKey), (*itMods), bWasPressed ) == false ) // unswallowed?
			{
				vecUnswallowedKeys.push_back(*itKey);
				vecUnswallowedMods.push_back(*itMods);
			}
		}
		m_vecLastFrameKeys = vecKeyCodeList;
#else // LINUX
		for(; itKey != vecKeyCodeList.end(); ++itKey, ++itMods )
		{
			// check for key repeat
			bool bWasPressed = false;
			if( (*itKey) < 0 )
			{
				std::vector<int>::const_iterator itNextKey = itKey;
				if( ++itNextKey != vecKeyCodeList.end() )
				{
					std::vector<int>::const_iterator itNextMods = itMods;
					++itNextMods;
					if( (*itKey) == -(*itNextKey) && (*itMods) == (*itNextMods) )
					{
						++itKey;
						++itMods;
						bWasPressed = true;
					}
				}
			}
			if(InjectKey( (*itKey), (*itMods), bWasPressed ) == false ) // unswallowed?
			{
				vecUnswallowedKeys.push_back(*itKey);
				vecUnswallowedMods.push_back(*itMods);
			}
		}
		m_vecLastFrameKeys = vecKeyCodeList;
#endif
	}
	else
	{
	
		for(; itKey != vecKeyCodeList.end(); ++itKey, ++itMods )
		{
			if( InjectKey( (*itKey), (*itMods), false ) == false ) // unswallowed?
			{
				vecUnswallowedKeys.push_back(*itKey);
				vecUnswallowedMods.push_back(*itMods);
			}
		}
	}

	// true: both are empty: all keys and mods were swallowed
	// false: at least on key/mod pair unswallowed
	return ( vecUnswallowedKeys.empty() && vecUnswallowedMods.empty() );
}

bool VistaKeyboardSystemControl::InjectKey( int nKeyCode, int nModifier, bool bIsKeyRepeat )
{
	if(m_pKeySink)
	{
		if( m_pKeySink->HandleKeyPress( nKeyCode, nModifier, bIsKeyRepeat ) == true )
			return true; // swallow
	}
	CommandMap::iterator itAction = m_vecModdedCommandMaps[nModifier].find( nKeyCode );

	if( itAction == m_vecModdedCommandMaps[nModifier].end() )
	{
		itAction = m_mapCommandMap.find( nKeyCode );
		if( itAction == m_mapCommandMap.end() )
			return false; // no entry
	}
	
	if( bIsKeyRepeat && (*itAction).second.m_bCallOnKeyRepreat == false )
		return false; // ignore key repeats

	(*itAction).second.m_pCallback->PrepareCallback();

	// swallow this key? use the return value of Do() as a trigger
	bool bRet = (*itAction).second.m_pCallback->Do();
	(*itAction).second.m_pCallback->PostCallback();

	return bRet;
}

bool VistaKeyboardSystemControl::BindAction(int nKeyCode, 
												IVistaExplicitCallbackInterface *pCallback,
												const std::string& sHelpText,
												bool bManageDeletion,
												bool bCallOnKeyRepeat,
												bool bForce )
{
	return BindAction( nKeyCode, VISTA_KEYMOD_ANY, pCallback, sHelpText, bManageDeletion, bCallOnKeyRepeat, bForce );
}

bool VistaKeyboardSystemControl::BindAction( int nKeyCode,
												int nModifiers,
												IVistaExplicitCallbackInterface* pCallback,
												const std::string& sHelpText,
												bool bManageDeletion,
												bool bCallOnKeyRepeat,
												bool bForce )

{
	CommandMap& mapCommands = ( nModifiers == VISTA_KEYMOD_ANY ) 
							? ( m_mapCommandMap )
							: ( m_vecModdedCommandMaps[nModifiers] );
	CommandMap::iterator itCurrent = mapCommands.find( nKeyCode );
	if( itCurrent != mapCommands.end() )
	{
		if( bForce && (*itCurrent).second.m_bManageDeletion )
		{
			delete (*itCurrent).second.m_pCallback;
		}
		else
		{
			vstr::warnp() << "[KeyboardSysControl::BindAction]: Cannot bind Callback - Key ["
					<< GetModifiersName(nModifiers) << GetKeyName(nKeyCode) 
					<< "] already in use by (" << (*itCurrent).second.m_strHelpText << ") !" << std::endl;
			return false;
		}

	}
	mapCommands[nKeyCode] = SKeyboardAction( pCallback, sHelpText, bCallOnKeyRepeat, bManageDeletion );
	return true;
}

IVistaExplicitCallbackInterface* VistaKeyboardSystemControl::GetActionForToken( 
													int nKeyCode, int nModifiers ) const
{
	const CommandMap& mapCommands = ( nModifiers == VISTA_KEYMOD_ANY ) 
							? ( m_mapCommandMap )
							: ( m_vecModdedCommandMaps[nModifiers] );
	CommandMap::const_iterator cit = mapCommands.find( nKeyCode );
	if(cit == mapCommands.end())
		return NULL;

	return (*cit).second.m_pCallback;
}

bool VistaKeyboardSystemControl::UnbindAction( int nKeyCode, int nModifiers )
{
	CommandMap& mapCommands = ( nModifiers == VISTA_KEYMOD_ANY ) 
							? ( m_mapCommandMap )
							: ( m_vecModdedCommandMaps[nModifiers] );

	CommandMap::iterator cit = mapCommands.find(nKeyCode);
	if(cit == mapCommands.end())
		return false;

	if( (*cit).second.m_bManageDeletion )
		delete (*cit).second.m_pCallback;

	mapCommands.erase(cit);
	return true;
}

bool VistaKeyboardSystemControl::UnbindAction( IVistaExplicitCallbackInterface *pAction )
{
	for( CommandMap::iterator itAction = m_mapCommandMap.begin();
				itAction != m_mapCommandMap.end(); ++itAction )
	{
		if( (*itAction).second.m_pCallback == pAction )
		{
			if( (*itAction).second.m_bManageDeletion )
				delete (*itAction).second.m_pCallback;
			m_mapCommandMap.erase( itAction );
			return true;
		}
	}
	for( std::vector<CommandMap>::iterator itMap = m_vecModdedCommandMaps.begin();
				itMap != m_vecModdedCommandMaps.end(); ++itMap )
	{
		for( CommandMap::iterator itAction = (*itMap).begin();
				itAction != (*itMap).end(); ++itAction )
		{
			if( (*itAction).second.m_pCallback == pAction )
			{
				if( (*itAction).second.m_bManageDeletion )
					delete (*itAction).second.m_pCallback;
				(*itMap).erase( itAction );
				return true;
			}
		}
	}
	return false;
}

int VistaKeyboardSystemControl::GetTokenList( std::list<int>& liTarget, int nModifiers ) const
{
	const CommandMap& mapCommands = ( nModifiers == VISTA_KEYMOD_ANY ) 
							? ( m_mapCommandMap )
							: ( m_vecModdedCommandMaps[nModifiers] );

	for( CommandMap::const_iterator cit = mapCommands.begin();
		cit != mapCommands.end(); ++cit)
	{
		liTarget.push_back((*cit).first);		
	}

	return (int)liTarget.size();
}

std::string VistaKeyboardSystemControl::GetHelpTextForToken( int nKeyCode, int nModifiers ) const
{
	const CommandMap& mapCommands = ( nModifiers == VISTA_KEYMOD_ANY ) 
							? ( m_mapCommandMap )
							: ( m_vecModdedCommandMaps[nModifiers] );

	CommandMap::const_iterator cit = mapCommands.find(nKeyCode);
	if(cit == mapCommands.end())
		return "";
	return (*cit).second.m_strHelpText;
}


VistaKeyboardSystemControl::IVistaDirectKeySink *VistaKeyboardSystemControl::GetDirectKeySink() const
{
	return m_pKeySink;
}

void VistaKeyboardSystemControl::SetDirectKeySink(IVistaDirectKeySink *pSink)
{
	m_pKeySink = pSink;
}

std::string VistaKeyboardSystemControl::GetKeyName( const int nKeyCode )
{
	int nActual = abs( nKeyCode );
	switch( nActual )
	{
		case VISTA_KEY_UPARROW:
			return "UP";
		case VISTA_KEY_DOWNARROW:
			return "DOWN";
		case VISTA_KEY_RIGHTARROW:
			return "RIGHT";
		case VISTA_KEY_LEFTARROW:
			return "LEFT";
		case VISTA_KEY_ESC:
			return "ESC";
		case VISTA_KEY_F1:
			return "F1";
		case VISTA_KEY_F2:
			return "F2";
		case VISTA_KEY_F3:
			return "F3";
		case VISTA_KEY_F4:
			return "F4";
		case VISTA_KEY_F5:
			return "F5";
		case VISTA_KEY_F6:
			return "F6";
		case VISTA_KEY_F7:
			return "F7";
		case VISTA_KEY_F8:
			return "F8";
		case VISTA_KEY_F9:
			return "F9";
		case VISTA_KEY_F10:
			return "F10";
		case VISTA_KEY_F11:
			return "F11";
		case VISTA_KEY_F12:
			return "F12";
		case VISTA_KEY_ENTER:
			return "ENTER";
		case VISTA_KEY_TAB:
			return "TAB";
		case VISTA_KEY_BACKSPACE:
			return "BACK";
		case VISTA_KEY_DELETE:
			return "DEL";
		case VISTA_KEY_HOME:
			return "HOME";
		case VISTA_KEY_END:
			return "END";
		case VISTA_KEY_PAGEUP:
			return "PG_UP";
		case VISTA_KEY_PAGEDOWN:
			return "PG_DN";
		case VISTA_KEY_MIDDLE:
			return "MID";
		default:
			return std::string( 1, (char)nActual );
	}
}

std::string VistaKeyboardSystemControl::GetModifiersName( const int nModifiers )
{
	if( nModifiers == VISTA_KEYMOD_ANY )
		return "";
	else if( nModifiers == VISTA_KEYMOD_NONE )
		return "NONE";
	std::string sReturn = "";
	bool bEmpty = true;
	if( nModifiers & VISTA_KEYMOD_SHIFT )
	{
		sReturn += "SHIFT";
		bEmpty = false;
	}
	if( nModifiers & VISTA_KEYMOD_CTRL )
	{
		if( bEmpty == false )
			sReturn += "+CTRL";
		else
			sReturn += "CTRL";
		bEmpty = false;
	}
	if( nModifiers & VISTA_KEYMOD_ALT )
	{
		if( bEmpty == false )
			sReturn += "+ALT";
		else
			sReturn += "ALT";
	}

	return sReturn;
}

int VistaKeyboardSystemControl::GetKeyValueFromString( const std::string& sKeyString )
{
	if( sKeyString.size() == 1 )
	{
		// just one entry - use this as ascii char
		return sKeyString[0];
	}

	VistaAspectsComparisonStuff::StringCompareObject oCompare( false );

	if( oCompare( sKeyString, "VISTA_KEY_UPARROW" ) || oCompare( sKeyString, "UP" ) )
		return VISTA_KEY_UPARROW;
	else if( oCompare( sKeyString, "VISTA_KEY_DOWNARROW" ) || oCompare( sKeyString, "DOWN" ) )
		return VISTA_KEY_DOWNARROW;
	else if( oCompare( sKeyString, "VISTA_KEY_RIGHTARROW" ) || oCompare( sKeyString, "RIGHT" ) )
		return VISTA_KEY_RIGHTARROW;
	else if( oCompare( sKeyString, "VISTA_KEY_LEFTARROW" ) || oCompare( sKeyString, "ESC" ) )
		return VISTA_KEY_LEFTARROW;
	else if( oCompare( sKeyString, "VISTA_KEY_ESC" ) || oCompare( sKeyString, "ESC" ) )
		return VISTA_KEY_ESC;
	else if( oCompare( sKeyString, "VISTA_KEY_F1" ) || oCompare( sKeyString, "F1" ) )
		return VISTA_KEY_F1;
	else if( oCompare( sKeyString, "VISTA_KEY_F2" ) || oCompare( sKeyString, "F2" ) )
		return VISTA_KEY_F2;
	else if( oCompare( sKeyString, "VISTA_KEY_F3" ) || oCompare( sKeyString, "F3" ) )
		return VISTA_KEY_F3;
	else if( oCompare( sKeyString, "VISTA_KEY_F4" ) || oCompare( sKeyString, "F4" ) )
		return VISTA_KEY_F4;
	else if( oCompare( sKeyString, "VISTA_KEY_F5" ) || oCompare( sKeyString, "F5" ) )
		return VISTA_KEY_F5;
	else if( oCompare( sKeyString, "VISTA_KEY_F6" ) || oCompare( sKeyString, "F6" ) )
		return VISTA_KEY_F6;
	else if( oCompare( sKeyString, "VISTA_KEY_F7" ) || oCompare( sKeyString, "F7" ) )
		return VISTA_KEY_F7;
	else if( oCompare( sKeyString, "VISTA_KEY_F8" ) || oCompare( sKeyString, "F8" ) )
		return VISTA_KEY_F8;
	else if( oCompare( sKeyString, "VISTA_KEY_F9" ) || oCompare( sKeyString, "F9" ) )
		return VISTA_KEY_F9;
	else if( oCompare( sKeyString, "VISTA_KEY_F10" ) || oCompare( sKeyString, "F10" ) )
		return VISTA_KEY_F10;
	else if( oCompare( sKeyString, "VISTA_KEY_F11" ) || oCompare( sKeyString, "F11" ) )
		return VISTA_KEY_F11;
	else if( oCompare( sKeyString, "VISTA_KEY_F12" ) || oCompare( sKeyString, "F12" ) )
		return VISTA_KEY_F12;
	else if( oCompare( sKeyString, "VISTA_KEY_ENTER" ) || oCompare( sKeyString, "ENTER" ) )
		return VISTA_KEY_ENTER;
	else if( oCompare( sKeyString, "VISTA_KEY_TAB" ) || oCompare( sKeyString, "TAB" ) || oCompare( sKeyString, "TABULATOR" ) )
		return VISTA_KEY_TAB;
	else if( oCompare( sKeyString, "VISTA_KEY_BACKSPACE" ) || oCompare( sKeyString, "BACK" ) || oCompare( sKeyString, "BACKSPACE" ) )
		return VISTA_KEY_BACKSPACE;
	else if( oCompare( sKeyString, "VISTA_KEY_DELETE" ) || oCompare( sKeyString, "DEL" ) || oCompare( sKeyString, "DELETE" ) )
		return VISTA_KEY_DELETE;
	else if( oCompare( sKeyString, "VISTA_KEY_HOME" ) || oCompare( sKeyString, "HOME" ) || oCompare( sKeyString, "POS1" ) )
		return VISTA_KEY_HOME;
	else if( oCompare( sKeyString, "VISTA_KEY_END" ) || oCompare( sKeyString, "END" ) )
		return VISTA_KEY_END;
	else if( oCompare( sKeyString, "VISTA_KEY_PAGEUP" ) || oCompare( sKeyString, "PG_UP" ) || oCompare( sKeyString, "PAGE_UP" ) || oCompare( sKeyString, "PAGEUP" ) )
		return VISTA_KEY_PAGEUP;
	else if( oCompare( sKeyString, "VISTA_KEY_PAGEDOWN" ) || oCompare( sKeyString, "PG_DN" ) || oCompare( sKeyString, "PAGE_DOWN" ) || oCompare( sKeyString, "PAGEUP" ) )
		return VISTA_KEY_PAGEDOWN;
	else if( oCompare( sKeyString, "VISTA_KEY_MIDDLE" ) || oCompare( sKeyString, "MID" ) || oCompare( sKeyString, "MIDDLE" ) )
		return VISTA_KEY_MIDDLE;
	
	if( sKeyString.size() > 1 )
	{
		int nKeyCode;
		if( VistaConversion::FromString<int>( sKeyString, nKeyCode )
			&& nKeyCode >= 0 )
			return nKeyCode;
	}

	return -1;
}

int VistaKeyboardSystemControl::GetModifiersValueFromString( const std::string& sModString )
{
	std::size_t nStart = 0;
	std::size_t nEnd;
	int nModifier = 0;
	std::string sSubString;

	if( sModString.empty() )
		return VISTA_KEYMOD_ANY;

	VistaAspectsComparisonStuff::StringCompareObject oCompare( false );

	if( oCompare( sModString, "NONE" ) )
		return VISTA_KEYMOD_NONE;

	for( ;; )
	{
		nEnd = sModString.find( '+', nStart );
		if( nEnd == std::string::npos )
			sSubString = sModString.substr( nStart );
		else
			sSubString = sModString.substr( nStart, nEnd - nStart );

		if( oCompare( sSubString, "CTRL" ) || oCompare( sSubString, "CONTROL" ) )
			nModifier |= VISTA_KEYMOD_CTRL;
		else if( oCompare( sSubString, "SHIFT" ) )
			nModifier |= VISTA_KEYMOD_SHIFT;
		else if( oCompare( sSubString, "ALT" ) )
			nModifier |= VISTA_KEYMOD_ALT;		
		else
			return -1; // invalid

		if( nEnd == std::string::npos )
			break;
		
		nStart = nEnd + 1;

	}
	return nModifier;
}

bool VistaKeyboardSystemControl::GetKeyAndModifiersValueFromString(
												const std::string& sKeyModString,
												int& nKey, int& nModifiers )
{
	std::string sKeyString;
	std::string sModString;

	// check for last plus, separating modifiers and keys
	// take care not to select last char, because it can be a plus key symbol
	std::size_t nPlusPos = sKeyModString.rfind( '+', sKeyModString.size() - 2 );
	if( nPlusPos == std::string::npos || nPlusPos == sKeyModString.size() - 1 )
	{
		// no +, so just the key value
		sKeyString = sKeyModString;
	}
	else
	{
		sModString = sKeyModString.substr( 0, nPlusPos );
		sKeyString = sKeyModString.substr( nPlusPos + 1 );
	}
	
	int nKeyValue = GetKeyValueFromString( sKeyString );
	if( nKeyValue == -1 )
		return false;

	int nModValue = GetModifiersValueFromString( sModString );
	if( nModValue == -1 )
		return false;
	
	nKey = nKeyValue;
	nModifiers = nModValue;
	return true;
}

std::string VistaKeyboardSystemControl::GetKeyBindingTableString() const
{
	std::stringstream oStream;
	oStream << "------------------------------------------------------------------------\n";
	oStream << std::right << std::setw( 15 ) << " "
			<< std::left << std::setw( 5 ) << " Key "
			<< " ) | " << "ASCII "
			<< " ) | " << "Description \n";
	oStream << "------------------------------------------------------------------------\n";
	for( CommandMap::const_iterator itAction = m_mapCommandMap.begin();
				itAction != m_mapCommandMap.end(); ++itAction )
	{
		oStream << std::right << std::setw( 15 ) << " "
				<< std::left << std::setw( 5 ) << GetKeyName( (*itAction).first )
				<< " ) | " << std::setw(5) << (*itAction).first
				<< " ) | " << (*itAction).second.m_strHelpText << "\n";
	}
	for( int i = 0; i <= 7; ++i )
	{
		for( CommandMap::const_iterator itAction = m_vecModdedCommandMaps[i].begin();
				itAction != m_vecModdedCommandMaps[i].end(); ++itAction )
		{
				oStream << std::right << std::setw( 14 ) << GetModifiersName( i ) << "+"
					<< std::left << std::setw( 5 ) << GetKeyName( (*itAction).first )
					<< " ) | " << std::setw(5) << (*itAction).first
					<< " ) | " << (*itAction).second.m_strHelpText << "\n";
		}
	}
	oStream << "------------------------------------------------------------------------\n";
	return oStream.str();
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


