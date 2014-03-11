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

#ifndef _VISTAMATHBASICS_H
#define _VISTAMATHBASICS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <cmath>
#include <limits>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

// windows.h may define min and max macros that break the std::min/max and
// numeric_limints::Min/max, so we undefine them
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace Vista
{
	const float Pi      = 3.14159265358979323846f;
	const float Epsilon = std::numeric_limits<float>::epsilon();

	enum AXIS
	{
		X	= 0,
		Y	= 1,
		Z	= 2,
		W	= 3,
	};	

	float DegToRad( const float fDegrees );
	float RadToDeg( const float fRadians );
	template< typename TFloat >
	bool IsValidNumber( const TFloat fValue );
	template< typename T > 
	T Clamp( const T val, const T minVal, const T maxVal );
}

/**
 * Convert angles given in radians to degrees and vice versa.
 */
inline float Vista::DegToRad( const float fDegrees )
{
	return fDegrees / 180.0f * Vista::Pi;
};

inline float Vista::RadToDeg( const float fRadians )
{
	return fRadians * 180.0f / Vista::Pi;
};

template< typename TFloat >
inline bool Vista::IsValidNumber( const TFloat nNumber )
{
	if( nNumber != nNumber )
		return false;
	if( nNumber == 0 )
		return true;
	TFloat nAbs = std::abs( nNumber );
	if( std::numeric_limits<TFloat>::has_infinity && nAbs == std::numeric_limits<TFloat>::infinity() )
		return false;
	if( std::numeric_limits<TFloat>::has_denorm && nAbs < std::numeric_limits<TFloat>::min() )
		return false;
	return true;
}

template< typename T >
T Vista::Clamp( const T nVal, const T nMinVal, const T nMaxVal )
{
	// @todo Interfers with min/max macros on windows.
	//return std::min( std::max( nVal, nMinVal ), nMaxVal );

	//     ------- min check -------|------------ max check -------------
	return nVal < nMinVal ? nMinVal : ( nVal > nMaxVal ? nMaxVal : nVal );
}


#endif //_VISTAMATHBASICS_H
