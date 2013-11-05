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

#ifndef _VISTATESTINGUTILS_H
#define _VISTATESTINGUTILS_H

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

namespace VistaTestingRandom
{
	static const float S_nRandRange = 100.0;

	static inline int InitializeRandomSeed()
	{
		unsigned int nSeed = (unsigned int)( 100000.0 * time(NULL) );
		srand( nSeed );
		return nSeed;
	}

	inline double RandomDouble()
	{
		double dRand = (double)rand() / (double)RAND_MAX; // [0,1]
		dRand -= 0.5;
		dRand *= S_nRandRange;
		return dRand;
	}
	inline float RandomFloat()
	{
		return (float)RandomDouble();
	}

	inline int RandomInt( int nMax )
	{
		return( rand() % nMax );
	}

	template<typename T>
	inline T GenerateRandom()
	{
		// should handle all base numeric types
		return T( RandomDouble() );
	}

	template<>
	inline bool GenerateRandom<bool>()
	{
		if( RandomDouble() > 0 )
			return true;
		else
			return false;
	}

	template<>
	inline char GenerateRandom<char>()
	{
		return char( 45 + RandomInt( 81 ) );
	}

	template<>
	inline std::string GenerateRandom<std::string>()
	{
		std::string oVal( (std::size_t)RandomInt( 1024 ), ' ' );
		for( std::size_t i = 0; i < oVal.size(); ++i )
			oVal[i] = GenerateRandom<char>();
		return oVal;
	}

	template<>
	inline VistaVector3D GenerateRandom<VistaVector3D>()
	{
		return VistaVector3D( RandomFloat(), RandomFloat(), RandomFloat() );
	}

	template<>
	inline VistaQuaternion GenerateRandom<VistaQuaternion>()
	{
		VistaQuaternion qRand( RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat() );
		qRand.Normalize();
		return qRand;
	}

	template<>
	inline VistaTransformMatrix GenerateRandom<VistaTransformMatrix>()
	{
		return VistaTransformMatrix( RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat(),
							RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat(),
							RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat(),
							RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat() );
	}

	inline VistaPropertyList GenerateRandomProplist( unsigned int nRecursionDepth )
	{
		VistaPropertyList oPropList;
		int nNumEntries = RandomInt( 20 );
		for( int i = 0; i < nNumEntries; ++i )
		{
			std::string sName = GenerateRandom<std::string>();
			int nType = RandomInt( 7 );
			switch( nType )
			{
			case 0:
				oPropList.SetValue( sName, GenerateRandom<std::string>() );
				break;
			case 1:
				oPropList.SetValue( sName, GenerateRandom<int>() );
				break;
			case 2:
				oPropList.SetValue( sName, GenerateRandom<bool>() );
				break;
			case 3:
				oPropList.SetValue( sName, GenerateRandom<float>() );
				break;
			case 4:
				oPropList.SetValue( sName, GenerateRandom<VistaVector3D>() );
				break;
			case 5:
				oPropList.SetValue( sName, GenerateRandom<VistaQuaternion>() );
				break;
			case 6:
				if( RandomInt( 3 ) + 1 > (int)nRecursionDepth )
					oPropList.SetValue( sName, GenerateRandomProplist( nRecursionDepth + 1 ) );
				break;
			}
		}
		return oPropList;
	}

	template<>
	inline VistaPropertyList GenerateRandom<VistaPropertyList>()
	{
		return GenerateRandomProplist( 0 );
	}
}

namespace VistaTestingCompare
{
	template<typename T>
	inline ::testing::AssertionResult Compare( const T& oLeft, const T& oRight, const double nMaxRelDeviation )
	{
		if( oLeft == oRight )
			return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}

	template<>
	inline ::testing::AssertionResult Compare<float>( const float& oLeft, const float& oRight, const double nMaxRelDeviation )
	{
		if( nMaxRelDeviation == 0 )
		{
			if( oLeft == oRight )
				return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
			return ::testing::AssertionFailure() << oLeft << " != " << oRight;
		}
		float nDelta = std::abs( oLeft - oRight );
		float nMaxDev = std::max( std::numeric_limits<float>::min(), (float)nMaxRelDeviation * std::abs( oLeft ) );
		if( nDelta < nMaxDev )
			return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<double>( const double& oLeft, const double& oRight, const double nMaxRelDeviation )
	{
		if( nMaxRelDeviation == 0 )
		{
			if( oLeft == oRight )
				return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
			return ::testing::AssertionFailure() << oLeft << " != " << oRight;
		}
		double nDelta = std::abs( oLeft - oRight );
		double nMaxDev = std::max( std::numeric_limits<double>::min(), nMaxRelDeviation * std::abs( oLeft ) );
		if( nDelta < nMaxDev )
			return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<long double>( const long double& oLeft, const long double& oRight, const double nMaxRelDeviation )
	{
		if( nMaxRelDeviation == 0 )
		{
			if( oLeft == oRight )
				return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
			return ::testing::AssertionFailure() << oLeft << " != " << oRight;
		}
		long double nDelta = std::abs( oLeft - oRight );
		long double nMaxDev = std::max( std::numeric_limits<long double>::min(), (long double)nMaxRelDeviation * std::abs( oLeft ) );
		if( nDelta < nMaxDev )
			return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
		return ::testing::AssertionFailure() << oLeft << " != " << oRight;
	}

	template<>
	inline ::testing::AssertionResult Compare<VistaVector3D>( const VistaVector3D& oLeft, const VistaVector3D& oRight, const double nMaxRelDeviation )
	{
		int nCheckComponents = 4;
		if( oRight[3] == 0.0f || oRight[3] == 1.0f )
			nCheckComponents = 3;
		for( int i = 0; i < nCheckComponents; ++i )
		{
			::testing::AssertionResult oRes = Compare( oLeft[i], oRight[i], nMaxRelDeviation );
			if( oRes == false )
				return oRes << " ( Vec3D entry " << i << ")";
		}
		return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<VistaQuaternion>( const VistaQuaternion& oLeft, const VistaQuaternion& oRight, const double nMaxRelDeviation )
	{
		for( int i = 0; i < 4; ++i )
		{
			::testing::AssertionResult oRes = Compare( oLeft[i], oRight[i], nMaxRelDeviation );
			if( oRes == false )
				return oRes << " ( Quaternion entry " << i << ")";
		}
		return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
	}
	template<>
	inline ::testing::AssertionResult Compare<VistaTransformMatrix>( const VistaTransformMatrix& oLeft,
																const VistaTransformMatrix& oRight, const double nMaxRelDeviation )
	{
		for( int i = 0; i < 4; ++i )
		{
			for( int j = 0; j < 4; ++j )
			{
				::testing::AssertionResult oRes = Compare( oLeft[i][j], oRight[i][j], nMaxRelDeviation );
				if( oRes == false )
					return oRes << " ( TransMat entry " << i << ", " << j << ")";
			}
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult Compare( const T& oLeft, const T& oRight )
	{
		return( Compare( oLeft, oRight, 0 ) );
	}	
	
	template<typename T>
	inline ::testing::AssertionResult CompareVec( const std::vector<T>& oLeft, 
											const std::vector<T>& oRight, const double nMaxRelDeviation = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
								<< oLeft.size() << " != " << oRight.size();
		}
		typename std::vector<T>::const_iterator itLeft = oLeft.begin();
		typename std::vector<T>::const_iterator itRight = oRight.begin();
		for( std::size_t i = 0; i < oLeft.size(); ++i, ++itLeft, ++itRight )
		{
			::testing::AssertionResult oRes = Compare<T>( (*itLeft), (*itRight), nMaxRelDeviation );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();
	}
	template<typename T>
	inline ::testing::AssertionResult CompareList( const std::list<T>& oLeft,
											const std::list<T>& oRight, const double nMaxRelDeviation = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
				<< oLeft.size() << " != " << oRight.size();
		}
		typename std::list<T>::const_iterator itLeft = oLeft.begin();
		typename std::list<T>::const_iterator itRight = oRight.begin();
		for( std::size_t i = 0; i < oLeft.size(); ++i, ++itLeft, ++itRight )
		{
			::testing::AssertionResult oRes = Compare<T>( (*itLeft), (*itRight), nMaxRelDeviation );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();
	}
	template<typename T>
	inline ::testing::AssertionResult CompareDeque( const std::deque<T>& oLeft,
											const std::deque<T>& oRight, const double nMaxRelDeviation = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
				<< oLeft.size() << " != " << oRight.size();
		}
		typename std::deque<T>::const_iterator itLeft = oLeft.begin();
		typename std::deque<T>::const_iterator itRight = oRight.begin();
		for( std::size_t i = 0; i < oLeft.size(); ++i, ++itLeft, ++itRight )
		{
			::testing::AssertionResult oRes = Compare<T>( (*itLeft), (*itRight), nMaxRelDeviation );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult CompareStack( std::stack<T> oLeft,
											std::stack<T> oRight, const double nMaxRelDeviation = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
				<< oLeft.size() << " != " << oRight.size();
		}
		int i = 0;
		while( oLeft.empty() == false )
		{
			::testing::AssertionResult oRes = Compare<T>( oLeft.top(), oRight.top(), nMaxRelDeviation );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
			oLeft.pop();
			oRight.pop();
			++i;
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult CompareQueue( std::queue<T> oLeft,
											std::queue<T> oRight, const double nMaxRelDeviation = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
				<< oLeft.size() << " != " << oRight.size();
		}
		int i = 0;
		while( oLeft.empty() == false )
		{
			::testing::AssertionResult oRes = Compare<T>( oLeft.front(), oRight.front(), nMaxRelDeviation );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
			oLeft.pop();
			oRight.pop();
			++i;
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult CompareArray( const T* oLeft, const T* oRight,
											const std::size_t nSize, const double nMaxRelDeviation = 0 )
	{
		for( std::size_t i = 0; i < nSize; ++i )
		{
			::testing::AssertionResult oRes = Compare<T>( oLeft[i], oRight[i], nMaxRelDeviation );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();	
	}
}


struct DummyStruct
{
	DummyStruct() : m_fData1( 42.0f ), m_bData2( false ) {};
	
	bool operator() ( const DummyStruct& oOther ) const
	{
		return ( m_fData1 == oOther.m_fData1
				&& m_bData2 == oOther.m_bData2 );
	}

	static std::string ConvertToString( const DummyStruct& oObject )
	{
		return ( VistaConversion::ToString( oObject.m_fData1 )
			+ " | "
			+ VistaConversion::ToString( oObject.m_bData2 ) );
	}
	static DummyStruct ConvertFromString( const std::string& sString )
	{
		DummyStruct oNew;
		// can throw exceptions!
		std::vector<std::string> vecEntries;
		if( VistaConversion::FromString( sString, vecEntries, '|' ) == false
			|| vecEntries.size() != 2)
		{
			VISTA_THROW( "String-To_DummyObject conversion failed", -1 );
		}
		oNew.m_fData1 = VistaConversion::FromString<float>( vecEntries[0] );
		oNew.m_bData2 = VistaConversion::FromString<bool>( vecEntries[1] );
		return oNew;
	}

	float	m_fData1;
	bool	m_bData2;
};

namespace VistaTestingRandom
{
	template<>
	inline DummyStruct GenerateRandom<DummyStruct>()
	{
		DummyStruct oDummy;
		oDummy.m_bData2 = GenerateRandom<bool>();
		oDummy.m_fData1 = GenerateRandom<float>();
		return oDummy;
	}
}

namespace VistaTestingCompare
{
	template<>
	inline ::testing::AssertionResult Compare<DummyStruct>( const DummyStruct& oLeft,
														const DummyStruct& oRight, const double nMaxRelDeviation )
	{
		::testing::AssertionResult oRes = Compare( oLeft.m_fData1, oRight.m_fData1, nMaxRelDeviation );
		if( oRes == false )
		{
			return oRes << "(floatdata)";
		}
		::testing::AssertionResult oRes2 = Compare( oLeft.m_bData2, oRight.m_bData2, nMaxRelDeviation );
		if( oRes2 == false )
		{
			return oRes2 << "(booldata)";
		}
		return ::testing::AssertionSuccess();
	}
}

namespace VistaConversion
{ 
	template<>
	struct StringConvertObject<DummyStruct>
	{
		static void ToString( const DummyStruct& oSource, std::string& sTarget,
									char cSeparator = S_cDefaultSeparator )
		{		
			sTarget = DummyStruct::ConvertToString( oSource );
		}
		static bool FromString( const std::string& sSource, DummyStruct& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			try
			{
				oTarget = DummyStruct::ConvertFromString( sSource );
			}
			catch( VistaExceptionBase& )
			{
				return false;
			}
			return true;
		}
	};
}



/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

#endif //_VISTATESTINGUTILS_H

