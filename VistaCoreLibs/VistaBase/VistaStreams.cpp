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
// $Id: VistaStreams.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#include "VistaStreams.h"

#include <algorithm>

#if defined(WIN32)
	#include <windows.h>
	#include <conio.h>
#else
	#include <unistd.h>
#endif



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/*  COLORSTREAM                                                               */
/*============================================================================*/

#ifdef WIN32
	// singletons for the Win console file handle, plus  areference count
	HANDLE		m_hConsole = NULL;
	int			m_ConsoleRefCount = 0;

	void ApplyTextColor( const vstr::CONSOLE_COLOR iColor, WORD& oConsoleAttributes )
	{
		oConsoleAttributes &= ~( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY );

		switch( iColor )
		{		
			case vstr::CC_BLACK:
				break;
			case vstr::CC_DARK_GRAY:
				oConsoleAttributes |= FOREGROUND_INTENSITY;
				break;
			default:
			case vstr::CC_DEFAULT:
			case vstr::CC_LIGHT_GRAY:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;
			case vstr::CC_WHITE:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_RED:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_RED:
				oConsoleAttributes |= FOREGROUND_RED;
				break;
			case vstr::CC_GREEN:
				oConsoleAttributes |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_GREEN:
				oConsoleAttributes |= FOREGROUND_GREEN;
				break;
			case vstr::CC_BLUE:	
				oConsoleAttributes |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_BLUE:
				oConsoleAttributes |= FOREGROUND_BLUE;
				break;
			case vstr::CC_YELLOW:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_YELLOW:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_GREEN;
				break;
			case vstr::CC_MAGENTA:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_MAGENTA:
				oConsoleAttributes |= FOREGROUND_RED | FOREGROUND_BLUE;
				break;
			case vstr::CC_CYAN:	
				oConsoleAttributes |= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_CYAN:
				oConsoleAttributes |= FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;		
		}
	}
	void ApplyBackgroundColor( const vstr::CONSOLE_COLOR iColor,	WORD& oConsoleAttributes )
	{
		oConsoleAttributes &= ~( BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY );

		switch( iColor )
		{
			default:
			case vstr::CC_DEFAULT:
			case vstr::CC_BLACK:
				break;
			case vstr::CC_DARK_GRAY:
				oConsoleAttributes |= BACKGROUND_INTENSITY;
				break;
			case vstr::CC_LIGHT_GRAY:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
				break;
			case vstr::CC_WHITE:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case vstr::CC_RED:
				oConsoleAttributes |= BACKGROUND_RED |BACKGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_RED:
				oConsoleAttributes |= BACKGROUND_RED;
				break;
			case vstr::CC_GREEN:
				oConsoleAttributes |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_GREEN:
				oConsoleAttributes |= BACKGROUND_GREEN;
				break;
			case vstr::CC_BLUE:	
				oConsoleAttributes |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_BLUE:
				oConsoleAttributes |= BACKGROUND_BLUE;
				break;
			case vstr::CC_YELLOW:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_YELLOW:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_GREEN;
				break;
			case vstr::CC_MAGENTA:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_MAGENTA:
				oConsoleAttributes |= BACKGROUND_RED | BACKGROUND_BLUE;
				break;
			case vstr::CC_CYAN:	
				oConsoleAttributes |= BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
				break;
			case vstr::CC_DARK_CYAN:
				oConsoleAttributes |= BACKGROUND_GREEN | BACKGROUND_BLUE;
				break;	
		}	
	}
#else
	vstr::CONSOLE_COLOR iStdTextColor = vstr::CC_DEFAULT;
	vstr::CONSOLE_COLOR iStdBackgroundColor = vstr::CC_DEFAULT;
	std::string sStdColorTag = "\033[0m";

	std::string aForegroundTags[] =
	{
		"\033[0;30",  //CC_BLACK
		"\033[0;1;30",  //CC_DARK_GRAY
		"\033[0;37",  //CC_LIGHT_GRAY
		"\033[0;1;37",  //CC_WHITE
		"\033[0;1;31",  //CC_RED
		"\033[0;31",  //CC_DARK_RED
		"\033[0;1;32",  //CC_GREEN,
		"\033[0;32",  //CC_DARK_GREEN
		"\033[0;1;34",  //CC_BLUE
		"\033[0;34",  //CC_DARK_BLUE
		"\033[0;1;33",  //CC_YELLOW
		"\033[0;33",  //CC_DARK_YELLOW
		"\033[0;1;35",  //CC_MAGENTA
		"\033[0;35",  //CC_DARK_MAGENTA
		"\033[0;1;36",  //CC_CYAN,
		"\033[0;36",  //CC_DARK_CYAN
		"\033[0",      //CC_DEFAULT
	};
	std::string aBackgroundTags[] =
	{
		";40m",  //CC_BLACK
		";40m",  //CC_DARK_GRAY
		";47m",  //CC_LIGHT_GRAY
		";m",	 //CC_WHITE
		";41m",  //CC_RED
		";41m",  //CC_DARK_RED
		";42m",  //CC_GREEN,
		";42m",  //CC_DARK_GREEN
		";44m",  //CC_BLUE
		";44m",  //CC_DARK_BLUE
		";43m",  //CC_YELLOW
		";43m",  //CC_DARK_YELLOW
		";45m",  //CC_MAGENTA
		";45m",  //CC_DARK_MAGENTA
		";46m",  //CC_CYAN,
		";46m",  //CC_DARK_CYAN
		"m",     //CC_DEFAULT
	};
#endif

const std::string aColorNames[] = 
{
	"BLACK",
	"DARK_GRAY",
	"LIGHT_GRAY",
	"WHITE",
	"RED",
	"DARK_RED",
	"GREEN",
	"DARK_GREEN",
	"BLUE",
	"DARK_BLUE",
	"YELLOW",
	"DARK_YELLOW",
	"MAGENTA",
	"DARK_MAGENTA",
	"CYAN",
	"DARK_CYAN",
	"DEFAULT",
};

const std::string& vstr::GetConsoleColorName( const CONSOLE_COLOR iColor )
{
	return aColorNames[iColor];
}

void vstr::SetStdConsoleTextColor( const CONSOLE_COLOR oColor )
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO oInfo;
	GetConsoleScreenBufferInfo( m_hConsole, &oInfo );
	ApplyTextColor( oColor, oInfo.wAttributes );
	SetConsoleTextAttribute( m_hConsole, oInfo.wAttributes );
#else
	iStdTextColor = oColor;
	sStdColorTag = aForegroundTags[iStdTextColor] + aBackgroundTags[iStdBackgroundColor];
	std::cout << sStdColorTag;
#endif
}
void vstr::SetStdConsoleBackgroundColor( const CONSOLE_COLOR oColor )
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO oInfo;
	GetConsoleScreenBufferInfo( m_hConsole, &oInfo );
	ApplyBackgroundColor( oColor, oInfo.wAttributes );
	SetConsoleTextAttribute( m_hConsole, oInfo.wAttributes );
#else
	iStdBackgroundColor = oColor;
	sStdColorTag = aForegroundTags[iStdTextColor] + aBackgroundTags[iStdBackgroundColor];
	std::cout << sStdColorTag;
#endif
}


/*============================================================================*/
/* VistaColorOutstream::VistaColorOutstreamBuffer                           */
/*============================================================================*/

class VistaColorOutstream::VistaColorOutstreamBuffer : public std::streambuf
{
public:
	VistaColorOutstreamBuffer( const size_t iBufferSize )
	: std::streambuf()
	, m_iTextColor( vstr::CC_DEFAULT )
	, m_iBackgroundColor( vstr::CC_DEFAULT )
	, m_vecBuffer( iBufferSize )
	, m_iReferenceCount( 1 )
	{
		char* pBase = &m_vecBuffer.front();
		setp( pBase, pBase + m_vecBuffer.size() - 1 );

#ifdef WIN32
		//m_hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		if( m_ConsoleRefCount == 0 )
		{
			m_hConsole = CreateFile( "CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,
									NULL, OPEN_EXISTING, 0, NULL );
		}
		++m_ConsoleRefCount;
		CONSOLE_SCREEN_BUFFER_INFO oInfo;
		GetConsoleScreenBufferInfo( m_hConsole, &oInfo );
		m_oAttributes = oInfo.wAttributes;
#else
		
#endif
	}

	~VistaColorOutstreamBuffer()
	{
#ifdef WIN32
		--m_ConsoleRefCount;
		if( m_ConsoleRefCount <= 0 )
			CloseHandle( m_hConsole );
#else
#endif
		sync();
	}

	void RefUp()
	{
		++m_iReferenceCount;
	}

	void RefDown()
	{
		--m_iReferenceCount;
		if( m_iReferenceCount < 0 )
			delete this;
	}

	void SetTextColor( const vstr::CONSOLE_COLOR iColor )
	{
		Flush();
		m_iTextColor = iColor;
#ifdef WIN32
		ApplyTextColor( m_iTextColor, m_oAttributes );
#else
		m_sTag = aForegroundTags[m_iTextColor] + aBackgroundTags[m_iBackgroundColor];
#endif
	}
	vstr::CONSOLE_COLOR GetTextColor() const
	{
		return m_iTextColor;
	}
	void SetBackgroundColor( const vstr::CONSOLE_COLOR iColor )
	{
		Flush();
		m_iBackgroundColor = iColor;
#ifdef WIN32
		ApplyBackgroundColor( m_iBackgroundColor, m_oAttributes );
#else
		m_sTag = aForegroundTags[m_iTextColor] + aBackgroundTags[m_iBackgroundColor];
#endif
	}
	vstr::CONSOLE_COLOR GetBackgroundColor() const
	{
		return m_iBackgroundColor;
	}

protected:
	virtual int_type overflow( int_type cChar )
	{
		if( cChar != traits_type::eof() )
		{
			*pptr() = cChar;
			pbump(1);
			if( Flush() )
				return cChar;
		}

		return traits_type::eof();

	}
	virtual int sync()
	{
		if( Flush() )
			return 0;
		else
			return -1;
	}

#ifdef WIN32
	bool Flush()
	{
		std::ptrdiff_t n = pptr() - pbase();
		if( n == 0 )
			return true;

		std::cout.flush();
		
		CONSOLE_SCREEN_BUFFER_INFO oInfo;
		GetConsoleScreenBufferInfo( m_hConsole, &oInfo );
		SetConsoleTextAttribute( m_hConsole, m_oAttributes );		

		LPDWORD oReturnChars = 0;
		WriteConsole( m_hConsole, &m_vecBuffer[0], (DWORD)n, oReturnChars, NULL );

		//std::cout.write( &m_vecBuffer[0], n );
		
		SetConsoleTextAttribute( m_hConsole, oInfo.wAttributes );

		pbump( (DWORD)-n );
		return true;
	}
#else
	bool Flush()
	{
		std::ptrdiff_t n = pptr() - pbase();
		if( n == 0 )
			return true;

		std::cout.flush();
		std::cout << m_sTag;;
		std::cout.write( &m_vecBuffer[0], n );
		std::cout << sStdColorTag;
		
		pbump( -n );
		return true;
	}
#endif
private:
#ifdef WIN32
	WORD					m_oAttributes;
#else
	std::string				m_sTag;
#endif
	vstr::CONSOLE_COLOR		m_iTextColor;
	vstr::CONSOLE_COLOR		m_iBackgroundColor;
	std::vector<char>		m_vecBuffer;
	int						m_iReferenceCount;
};	

/*============================================================================*/
/* VistaColorOutstream                                                       */
/*============================================================================*/
VistaColorOutstream::VistaColorOutstream( const vstr::CONSOLE_COLOR iTextColor,
											const vstr::CONSOLE_COLOR iBackgroundColor,
											const size_t iBufferSize)
: std::ostream( (std::streambuf*)( new VistaColorOutstreamBuffer( iBufferSize ) ) )

{
	m_pBuffer = static_cast<VistaColorOutstreamBuffer*>( rdbuf() );
	m_pBuffer->SetTextColor( iTextColor );
	m_pBuffer->SetBackgroundColor( iBackgroundColor );	
}
VistaColorOutstream::VistaColorOutstream(  const VistaColorOutstream& oCopy )
: std::ostream( (std::streambuf*)( oCopy.m_pBuffer ) )
, m_pBuffer( oCopy.m_pBuffer )
{
	m_pBuffer->RefUp();
}
VistaColorOutstream::~VistaColorOutstream()
{
	m_pBuffer->RefDown();
}

void VistaColorOutstream::SetTextColor( const vstr::CONSOLE_COLOR iColor )
{
	m_pBuffer->SetTextColor( iColor );
}
vstr::CONSOLE_COLOR VistaColorOutstream::GetTextColor() const
{
	return m_pBuffer->GetTextColor();
}
void VistaColorOutstream::SetBackgroundColor( const vstr::CONSOLE_COLOR iColor )
{
	m_pBuffer->SetBackgroundColor( iColor );
}
vstr::CONSOLE_COLOR VistaColorOutstream::GetBackgroundColor() const
{
	return m_pBuffer->GetBackgroundColor();
}


/*============================================================================*/
/*  SPLITOUTSTREAM                                                            */
/*============================================================================*/

/*============================================================================*/
/* VistaSplitOutstream::VistaSplitOutstreamBuffer                           */
/*============================================================================*/
class VistaSplitOutstream::VistaSplitOutstreamBuffer : public std::streambuf
{
public:
	VistaSplitOutstreamBuffer()
	: std::streambuf()
	{		
	}

	~VistaSplitOutstreamBuffer()
	{
	}

	bool AddStream( std::ostream* oStream )
	{
		m_vecStreams.push_back( oStream );
		return true;
	}
	bool GetHasStream( std::ostream* oStream ) const
	{
		std::vector<std::ostream*>::const_iterator itStream
			= std::find( m_vecStreams.begin(), m_vecStreams.end(), oStream );
		return ( itStream != m_vecStreams.end() );
	}
	bool RemoveStream( std::ostream* oStream )
	{
		std::vector<std::ostream*>::iterator itStream
			= std::find( m_vecStreams.begin(), m_vecStreams.end(), oStream );
		if( itStream == m_vecStreams.end() )
			return false;

		m_vecStreams.erase( itStream );
		return true;
	}
	const std::vector<std::ostream*>& GetStreams() const
	{
		return m_vecStreams;
	}
	void SetStreams( const std::vector<std::ostream*>& vecStreams )
	{
		m_vecStreams = vecStreams;
	}

protected:
	virtual int_type overflow( int_type cChar )
	{
		for( std::vector<std::ostream*>::const_iterator itStream = m_vecStreams.begin();
				itStream != m_vecStreams.end(); ++itStream )
		{
			(*itStream)->put( cChar );
		}
		return cChar;
	}
	virtual int sync()
	{
		for( std::vector<std::ostream*>::const_iterator itStream = m_vecStreams.begin();
			itStream != m_vecStreams.end(); ++itStream )
		{
			(*itStream)->flush();			
		}
		return 0;
	}


private:
	std::vector<std::ostream*>	m_vecStreams;
};	

/*============================================================================*/
/* VistaSplitOutstream                                                       */
/*============================================================================*/

VistaSplitOutstream::VistaSplitOutstream()
: std::ostream( (std::streambuf*)( new VistaSplitOutstreamBuffer() ) )

{	
	m_pBuffer = static_cast<VistaSplitOutstreamBuffer*>( rdbuf() );
}
VistaSplitOutstream::VistaSplitOutstream(  const VistaSplitOutstream& oCopy )
: std::ostream( (std::streambuf*)( new VistaSplitOutstreamBuffer() ) )
{
	m_pBuffer = static_cast<VistaSplitOutstreamBuffer*>( rdbuf() );
	m_pBuffer->SetStreams( static_cast<VistaSplitOutstreamBuffer*>(
												oCopy.rdbuf() )->GetStreams() );
}
VistaSplitOutstream::~VistaSplitOutstream()
{
}

bool VistaSplitOutstream::AddStream( std::ostream* oStream )
{
	return m_pBuffer->AddStream( oStream );
}
bool VistaSplitOutstream::GetHasStream( std::ostream* oStream ) const
{
	return m_pBuffer->GetHasStream( oStream );
}
bool VistaSplitOutstream::RemoveStream( std::ostream* oStream )
{
	return m_pBuffer->RemoveStream( oStream );
}
const std::vector<std::ostream*>& VistaSplitOutstream::GetStreams() const
{
	return m_pBuffer->GetStreams();
}
void VistaSplitOutstream::SetStreams( const std::vector<std::ostream*>& vecStreams )
{
	return m_pBuffer->SetStreams( vecStreams );
}
