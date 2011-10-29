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
// $Id: VistaHalfFunction.h 20730 2011-03-30 15:56:24Z dr165799 $

///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// Primary authors:
//     Florian Kainz <kainz@ilm.com>
//     Rod Bogart <rgb@ilm.com>


//---------------------------------------------------------------------------
//
//	halfFunction<T> -- a class for fast evaluation
//			   of half --> T functions
//
//	The constructor for a halfFunction object,
//
//	    halfFunction (function,
//			  domainMin, domainMax,
//			  defaultValue,
//			  posInfValue, negInfValue,
//			  nanValue);
//
//	evaluates the function for all finite half values in the interval
//	[domainMin, domainMax], and stores the results in a lookup table.
//	For finite half values that are not in [domainMin, domainMax], the
//	constructor stores defaultValue in the table.  For positive infinity,
//	negative infinity and NANs, posInfValue, negInfValue and nanValue
//	are stored in the table.
//
//	The tabulated function can then be evaluated quickly for arbitrary
//	half values by calling the the halfFunction object's operator()
//	method.
//
//	Example:
//
//	    #include <math.h>
//	    #include <halfFunction.h>
//
//	    halfFunction<half> hsin (sin);
//
//	    halfFunction<half> hsqrt (sqrt,		// function
//				      0, HALF_MAX,	// domain
//				      half::qNan(),	// sqrt(x) for x < 0
//				      half::posInf(),	// sqrt(+inf)
//				      half::qNan(),	// sqrt(-inf)
//				      half::qNan());	// sqrt(nan)
//
//	    half x = hsin (1);
//	    half y = hsqrt (3.5);
//
//---------------------------------------------------------------------------

#ifndef _VISTAHALF_FUNCTION_H_
#define _VISTAHALF_FUNCTION_H_

#include "VistaHalf.h"


template <class T>
class VistaHalfFunction
{
public:

	//------------
	// Constructor
	//------------

	template <class Function>
	VistaHalfFunction (
		Function f,
		VistaHalf domainMin =  std::limits<VistaHalf>::min(),
		VistaHalf domainMax =  std::limits<VistaHalf>::max(),
		T defaultValue = 0,
		T posInfValue  = 0,
		T negInfValue  = 0,
		T nanValue     = 0);

	//-----------
	// Evaluation
	//-----------

	T		operator () (VistaHalf x) const;

private:

	T		_lut[1 << 16];
};


//---------------
// Implementation
//---------------

template <class T>
template <class Function>
VistaHalfFunction<T>::VistaHalfFunction (Function f,
				   VistaHalf domainMin,
				   VistaHalf domainMax,
				   T defaultValue,
				   T posInfValue,
				   T negInfValue,
				   T nanValue)
{
	for (int i = 0; i < (1 << 16); i++)
	{
	VistaHalf x;
	x.setBits (i);

	if (x.isNan())
		_lut[i] = nanValue;
	else if (x.isInfinity())
		_lut[i] = x.isNegative()? negInfValue: posInfValue;
	else if (x < domainMin || x > domainMax)
		_lut[i] = defaultValue;
	else
		_lut[i] = f (x);
	}
}


template <class T>
inline T
VistaHalfFunction<T>::operator () (VistaHalf x) const
{
	return _lut[x.bits()];
}


#endif
