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

#ifndef _VISTASTREAMUTILS_H
#define _VISTASTREAMUTILS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaBaseConfig.h"

#include "VistaStreamManager.h"

#include <ostream>
#include <iostream>
#include <iomanip>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


namespace vstr
{
	//////////////////////////////////////////////
	////  COLORS
	//////////////////////////////////////////////
	enum VISTABASEAPI CONSOLE_COLOR
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

	VISTABASEAPI const std::string& GetConsoleColorName( const CONSOLE_COLOR oColor );
	VISTABASEAPI void SetStdConsoleTextColor( const CONSOLE_COLOR oColor );
	VISTABASEAPI void SetStdConsoleBackgroundColor( const CONSOLE_COLOR oColor );

	//////////////////////////////////////////////
	////  STREAM MANAGER
	//////////////////////////////////////////////
	/**
	 *  returns the StreamManager Singleton (and creates one if necessary)
	 */
	VISTABASEAPI VistaStreamManager* GetStreamManager();
	VISTABASEAPI void SetStreamManager( VistaStreamManager* pStreamManager,
											const bool bDeleteOldManager = true );
	VISTABASEAPI void DestroyStreamManager();
	/**
	 * Returns the Stream with the specified name from the StreamManager Singleton;
	 */
	VISTABASEAPI std::ostream& Stream( const std::string& sName = "" );

	//////////////////////////////////////////////
	////  STREAM MANIPULATOR
	//////////////////////////////////////////////	
	/**
	 * stream manipulator that prints the current system time
	 */
	VISTABASEAPI std::ostream& systime( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current mcirotime
	 */
	VISTABASEAPI std::ostream& relativetime( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current frameclock
	 * only works if the StreamManagers InfoInterface was set (prints 0 otherwise)
	 */
	VISTABASEAPI std::ostream& frameclock( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current date, formatted as
	 * YYMMDD_HHMMSS
	 * only works if the StreamManagers InfoInterface was set (prints "" otherwise)
	 */
	VISTABASEAPI std::ostream& date( std::ostream& oStream );
	/**
	 * stream manipulator that displays the number of frames since the
	 * creation of the Logger singleton
	 * only works if the StreamManagers InfoInterface was set (prints 0 otherwise)
	 */
	VISTABASEAPI std::ostream& framecount( std::ostream& oStream );
	/**
	 * stream manipulator that prints general information as defined by
	 * the InfoLayout functionality
	 */
	VISTABASEAPI std::ostream& info( std::ostream& oStream );
	/**
	 * stream manipulator that prints the name of the current cluster node
	 * only works if the StreamManagers InfoInterface was set (returns "" otherwise)
	 */
	VISTABASEAPI std::ostream& nodename( std::ostream& oStream );
	/**
	 * stream manipulator that prints the current framerate
	 * only works if the StreamManagers InfoInterface was set (returns 0 otherwise)
	 */
	VISTABASEAPI std::ostream& framerate( std::ostream& oStream );


	/**
	 * stream manipulator that formats a time appropriately, i.e. by default with 6
	 * decimal places. The stream's formatting is not altered after a call.
	 */
	class VISTABASEAPI formattime
	{
	public:
		formattime( const double dTime, const int fPrecision = 6 );
		std::ostream& operator()( std::ostream& oStream ) const;
	private:
		double m_dTime;
		int	m_nPrecision;
	};
	/**
	 * stream coloring, only works with VistaColorStream or cout/cerr
	 */
	class VISTABASEAPI color
	{
	public:
		color( const CONSOLE_COLOR iTextColor,
				const CONSOLE_COLOR iBackgroundColor );		
		std::ostream& operator()( std::ostream& oStream ) const;
	private:
		vstr::CONSOLE_COLOR m_iTextColor, m_iBackgroundColor;
	};
	class VISTABASEAPI textcolor
	{
	public:
		textcolor( const CONSOLE_COLOR iTextColor );		
		std::ostream& operator()( std::ostream& oStream ) const;
	private:
		vstr::CONSOLE_COLOR m_iTextColor;
	};
	class VISTABASEAPI bgcolor
	{
	public:
		bgcolor( const CONSOLE_COLOR iBackgroundColor );		
		std::ostream& operator()( std::ostream& oStream ) const;
	private:
		vstr::CONSOLE_COLOR m_iBackgroundColor;
	};
};

inline VISTABASEAPI std::ostream& operator<< ( std::ostream& oStream, const vstr::formattime& oObj )
{
	return oObj( oStream );
}

inline VISTABASEAPI std::ostream& operator<< ( std::ostream& oStream, const vstr::color& oObj )
{
	return oObj( oStream );
}
inline VISTABASEAPI std::ostream& operator<< ( std::ostream& oStream, const vstr::textcolor& oObj )
{
	return oObj( oStream );
}
inline VISTABASEAPI std::ostream& operator<< ( std::ostream& oStream, const vstr::bgcolor& oObj )
{
	return oObj( oStream );
}


#endif // _VISTASTREAMUTILS_H

