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
// $Id: VistaMathBasics.h 20730 2011-03-30 15:56:24Z dr165799 $

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
	bool IsValidNumber( const float fValue );
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

inline bool Vista::IsValidNumber( const float fValue )
{
	// own implementation of both isnan and isinf, since not all compilers know
	// these functions (yes, msvc, I'm looking at you)
	return ( fValue == fValue
			&& fValue != std::numeric_limits<float>::infinity() );
};


#endif //_VISTAMATHBASICS_H
