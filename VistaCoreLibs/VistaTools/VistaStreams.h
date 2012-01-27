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
// $Id: VistaStreams.h 21315 2011-05-16 13:47:39Z dr165799 $

#ifndef _VISTASTREAMS_H
#define _VISTASTREAMS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaToolsConfig.h"

#include <VistaBase/VistaStreamUtils.h>

#include <string>
#include <ostream>
#include <iostream>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VistaStreams
{
	/**
	 * Makes a passed stream thread-safe by changing its streambuffer.
	 * This allows concurrent access to the stream, but ONLY for logging, i.e.
	 * one can only write to its back, but neither read nor position the stream.
	 * If bBufferInternally is true, the stream also buffers all input internally
	 * until the stream is flushed. This ensures that messages are printed as
	 * a whole (not interrupted by output from concurrent writes), but only prints
	 * if the stream is flushed - if no flush is called, the input will never
	 * be printed!
	 */
	VISTATOOLSAPI bool MakeStreamThreadSafe( std::ostream& oStream,
										const bool bBufferInternally = true );
	VISTATOOLSAPI bool MakeStreamThreadSafe( std::ostream* pStream,
										const bool bBufferInternally = true );
}

/**
 * class VistaColorOutstream: allows to color console streams
 */
class VISTATOOLSAPI VistaColorOutstream : public std::ostream
{
public:
	enum CONSOLE_COLOR
	{
		CC_FIRST_COLOR = 0,
		CC_BLACK = 0,
		CC_DARK_GRAY,
		CC_LIGHT_GRAY,
		CC_WHITE,
		CC_RED,
		CC_DARK_RED,
		CC_GREEN,
		CC_DARK_GREEN,
		CC_BLUE,	
		CC_DARK_BLUE,
		CC_YELLOW,
		CC_DARK_YELLOW,
		CC_MAGENTA,
		CC_DARK_MAGENTA,
		CC_CYAN,	
		CC_DARK_CYAN,
		CC_DEFAULT,
		CC_NUM_COLORS,
	};
	static std::string GetConsoleColorName( const CONSOLE_COLOR oColor );

public:
	VistaColorOutstream( const CONSOLE_COLOR iTextColor = CC_DEFAULT,
						const CONSOLE_COLOR iBackgroundColor = CC_DEFAULT,
						const size_t iBufferSize = 50 );
	VistaColorOutstream( const VistaColorOutstream& oCopy );
	virtual ~VistaColorOutstream();

	void SetTextColor( const CONSOLE_COLOR iColor );
	CONSOLE_COLOR GetTextColor() const;
	void SetBackgroundColor( const CONSOLE_COLOR iColor );
	CONSOLE_COLOR GetBackgroundColor() const;
private:
	class VistaColorOutstreamBuffer;
	VistaColorOutstreamBuffer* m_pBuffer;
};

/**
 * class VistaSplitStream: allows to simultaneously write to multiple streams
 * Attention: When writing to multiple streams with the same target (e.g. multiple
 * console streams), the output may be scrambled due to different buffering schemes.
 */
class VISTATOOLSAPI VistaSplitOutstream : public std::ostream
{		
public:
	VistaSplitOutstream();
	VistaSplitOutstream( const VistaSplitOutstream& oCopy );
	virtual ~VistaSplitOutstream();

	/**
	 * Adds a stream, but does NOT test if the stream is already appended
	 * Thus, it is possible to write to the same stream twice 
	 * (might be somehow useful for whatever reason...)
	 */
	bool AddStream( std::ostream* oStream );
	bool RemoveStream( std::ostream* oStream );
	bool GetHasStream( std::ostream* oStream ) const;
	const std::vector<std::ostream*>& GetStreams() const;
	void SetStreams( const std::vector<std::ostream*>& vecStreams );
private:
	class VistaSplitOutstreamBuffer;
	VistaSplitOutstreamBuffer* m_pBuffer;
};

/**
 * class VistaNullOutstream: stream that outputs nothing
 * can be used to swallow unwanted output. Note, however, that usually
 * this is not significantly faster than, e.g., writing to a file, since
 * all formating and conversion is still performed.
 */
class VISTATOOLSAPI VistaNullOutstream : public std::ostream
{			
public:
	VistaNullOutstream();
	virtual ~VistaNullOutstream();

	// to have a more performant null stream, we overload all input operators
	// note: these, of course, only work when writing directly into a Nullstream,
	// (which is pretty senseless). If it is casted to an std::ostream, all
	// types will still be converted and only discarded upon flushing them
	template<typename T>
	inline VistaNullOutstream& operator<<( const T& ) { return *this; }
	inline VistaNullOutstream& operator<<( std::ostream& ( *pf )( std::ostream& ) ) { return *this; }
	inline VistaNullOutstream& operator<<( std::ios& ( *pf )( std::ios&)  ) { return *this; }
	inline VistaNullOutstream& operator<<( std::ios_base& ( *pf )( std::ios_base& )) { return *this; }

private:
	class VistaNullOutstreamBuffer;
};

#endif // _VISTASTREAMS_H

