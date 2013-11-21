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

#ifndef _VISTATESTINGCOMPARE_H
#define _VISTATESTINGCOMPARE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaConversion.h>
#include <VistaAspects/VistaPropertyList.h>
#include <time.h>

#include <gtest/gtest.h>

/*============================================================================*/
/* HELPERS                                                                    */
/*============================================================================*/

template< typename S, typename T>
std::ostream& operator<< ( std::ostream& oStream, const std::pair<S,T>& oVal )
{
	oStream << "( " << oVal.first << " | " << oVal.second << " )";
	return oStream;
}
namespace VistaTestingCompare
{
	template<typename T>
	inline ::testing::AssertionResult Compare( const T& oLeft, const T& oRight, const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
	{
		if( oLeft == oRight )
			return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}

	template<>
	inline ::testing::AssertionResult Compare<float>( const float& oLeft, const float& oRight, const double nMaxRelDeviation, const double nMinRelevantSize )
	{
		if( nMinRelevantSize != 0 )
		{
			if( std::abs( oLeft ) < (float)nMinRelevantSize && std::abs( oRight ) < (float)nMinRelevantSize )
				return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
		}
		if( nMaxRelDeviation == 0 )
		{
			if( oLeft == oRight )
				return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
			return ::testing::AssertionFailure() << oLeft << " != " << oRight;
		}
		float nDelta = std::abs( oLeft - oRight );
		float nMaxDev = std::max( std::numeric_limits<float>::min(), (float)nMaxRelDeviation * std::abs( oLeft ) );
		if( nDelta < nMaxDev )
			return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<double>( const double& oLeft, const double& oRight, const double nMaxRelDeviation, const double nMinRelevantSize )
	{
		if( nMinRelevantSize != 0 )
		{
			if( std::abs( oLeft ) < nMinRelevantSize && std::abs( oRight ) < nMinRelevantSize )
				return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
		}
		if( nMaxRelDeviation == 0 )
		{
			if( oLeft == oRight )
				return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
			return ::testing::AssertionFailure() << oLeft << " != " << oRight;
		}
		double nDelta = std::abs( oLeft - oRight );
		double nMaxDev = std::max( std::numeric_limits<double>::min(), nMaxRelDeviation * std::abs( oLeft ) );
		if( nDelta < nMaxDev )
			return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<long double>( const long double& oLeft, const long double& oRight, const double nMaxRelDeviation, const double nMinRelevantSize )
	{
		if( nMinRelevantSize != 0 )
		{
			if( std::abs( oLeft ) < nMinRelevantSize && std::abs( oRight ) < nMinRelevantSize )
				return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
		}
		if( nMaxRelDeviation == 0 )
		{
			if( oLeft == oRight )
				return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
			return ::testing::AssertionFailure() << oLeft << " != " << oRight;
		}
		long double nDelta = std::abs( oLeft - oRight );
		long double nMaxDev = std::max( std::numeric_limits<long double>::min(), (long double)nMaxRelDeviation * std::abs( oLeft ) );
		if( nDelta < nMaxDev )
			return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}

	template<>
	inline ::testing::AssertionResult Compare<VistaVector3D>( const VistaVector3D& oLeft, const VistaVector3D& oRight, const double nMaxRelDeviation, const double nMinRelevantSize )
	{
		int nCheckComponents = 4;
		if( oRight[3] == 0.0f || oRight[3] == 1.0f )
			nCheckComponents = 3;
		for( int i = 0; i < nCheckComponents; ++i )
		{
			::testing::AssertionResult oRes = Compare( oLeft[i], oRight[i], nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " ( Vec3D entry " << i << ")";
		}
		return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<VistaQuaternion>( const VistaQuaternion& oLeft, const VistaQuaternion& oRight, const double nMaxRelDeviation, const double nMinRelevantSize )
	{
		// check: to we need to invert?
		float nFactor = 1.0f;
		for( int i = 3; i >= 0; --i )
		{
			if( Compare( oLeft[i], 0.0f, nMaxRelDeviation, nMinRelevantSize ) )
				continue;
			if( ::testing::AssertionResult oRes = Compare( oLeft[i], -oRight[i], nMaxRelDeviation, nMinRelevantSize ) )
				nFactor = -1.0f;
			break;
		}
		for( int i = 0; i < 4; ++i )
		{
			::testing::AssertionResult oRes = Compare( oLeft[i], nFactor * oRight[i], nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " ( Quaternion entry " << i << ")";
		}
		return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<VistaTransformMatrix>( const VistaTransformMatrix& oLeft,	const VistaTransformMatrix& oRight,
													const double nMaxRelDeviation, const double nMinRelevantSize )
	{
		for( int i = 0; i < 4; ++i )
		{
			for( int j = 0; j < 4; ++j )
			{
				::testing::AssertionResult oRes = Compare( oLeft[i][j], oRight[i][j], nMaxRelDeviation, nMinRelevantSize );
				if( oRes == false )
					return oRes << " ( TransMat entry " << i << ", " << j << ")";
			}
		}
		return ::testing::AssertionSuccess();
	}	
}

#endif //_VISTATESTINGCOMPARE_H

