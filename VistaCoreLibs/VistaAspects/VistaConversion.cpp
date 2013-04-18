/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#include "VistaConversion.h"

#include <algorithm>

#if defined(LINUX)
#include <ctype.h>

#if defined(__GNUC__)
#include <cxxabi.h>
#endif

#endif // for tolower, toupper


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace
{
	bool CheckForBraces( char c )
	{
		return ( c == '(' || c == ')' 
			|| c == '[' || c == ']'
			|| c == '{' || c == '}' );
	}
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

std::string VistaConversion::StringToUpper( const std::string& oSource )
{
	std::string sTemp( oSource.size(), (char)0 );
	std::string::iterator itTarget = sTemp.begin();
	for( std::string::const_iterator itChar = oSource.begin();
			itChar != oSource.end(); ++itChar, ++itTarget )
	{
		(*itTarget) = toupper( (*itChar) );
	}
	return sTemp;

}
std::string VistaConversion::StringToLower( const std::string& oSource )
{
	std::string sTemp( oSource.size(), (char)0 );
	std::string::iterator itTarget = sTemp.begin();
	for( std::string::const_iterator itChar = oSource.begin();
			itChar != oSource.end(); ++itChar, ++itTarget )
	{
		(*itTarget) = toupper( (*itChar) );
	}
	return sTemp;
}

std::string VistaConversion::CleanOSTypeName( const std::string& sName )
{
#if defined(__GNUC__)
	int nState;
	char* acCleaned = abi::__cxa_demangle( sName.c_str(), NULL, NULL, &nState );
	if( nState == 0 && acCleaned )
	{
		std::string sCleaned = acCleaned;
		free( acCleaned );
		return sCleaned;
	}
	else
		return sName;
#else
	return sName;
#endif
}

void VistaConversion::RemoveBraces( std::string& sString )
{
	sString.resize( std::remove_if( sString.begin(), sString.end(), CheckForBraces ) - sString.begin() );
}

bool VistaConversion::CheckRemainingStringEmpty( const std::string& sString, 
								   std::size_t nFrom, char cSeparator )
{
	std::string::const_iterator itEntry = sString.begin() + nFrom;
	for( ; itEntry != sString.end(); ++itEntry )
	{
		if( (*itEntry) != ' ' && (*itEntry) != '\t'
			&& (*itEntry) != '\n' && (*itEntry) != cSeparator )
			return false;
	}
	return true;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
