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

#ifndef _VISTASTREAMS_H
#define _VISTASTREAMS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaBaseConfig.h"

#include "VistaStreamUtils.h"

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

/**
 * class VistaColorOutstream: allows to color console streams
 */
class VISTABASEAPI VistaColorOutstream : public std::ostream
{			
public:
	VistaColorOutstream( const vstr::CONSOLE_COLOR iTextColor = vstr::CC_DEFAULT,
						const vstr::CONSOLE_COLOR iBackgroundColor = vstr::CC_DEFAULT,
						const size_t iBufferSize = 50 );
	VistaColorOutstream( const VistaColorOutstream& oCopy );
	virtual ~VistaColorOutstream();

	void SetTextColor( const vstr::CONSOLE_COLOR iColor );
	vstr::CONSOLE_COLOR GetTextColor() const;
	void SetBackgroundColor( const vstr::CONSOLE_COLOR iColor );
	vstr::CONSOLE_COLOR GetBackgroundColor() const;
private:
	class VistaColorOutstreamBuffer;
	VistaColorOutstreamBuffer* m_pBuffer;
};

/**
 * class VistaSplitStream: allows to simultaneously write to multiple streams
 * Attention: When writing to multiple streams with the same target (e.g. multiple
 * console streams), the output may be scrambled due to different buffering schemes.
 */
class VISTABASEAPI VistaSplitOutstream : public std::ostream
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

#endif // _VISTASTREAMS_H

