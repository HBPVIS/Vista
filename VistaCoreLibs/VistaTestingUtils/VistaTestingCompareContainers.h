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
// $Id: VistaTestingUtils.h 39125 2013-11-05 20:24:43Z dr165799 $

#ifndef _VISTATESTINGCOMPARECONTAINERS_H
#define _VISTATESTINGCOMPARECONTAINERS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaTestingCompare.h"

#include <gtest/gtest.h>

#include <vector>
#include <list>
#include <queue>
#include <deque>

/*============================================================================*/
/* HELPERS                                                                    */
/*============================================================================*/

namespace VistaTestingCompare
{	
	template<typename T>
	inline ::testing::AssertionResult CompareVec( const std::vector<T>& oLeft, const std::vector<T>& oRight, 
													const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
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
			::testing::AssertionResult oRes = Compare<T>( (*itLeft), (*itRight), nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();
	}
	template<typename T>
	inline ::testing::AssertionResult CompareList( const std::list<T>& oLeft, const std::list<T>& oRight,
										const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
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
			::testing::AssertionResult oRes = Compare<T>( (*itLeft), (*itRight), nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();
	}
	template<typename T>
	inline ::testing::AssertionResult CompareDeque( const std::deque<T>& oLeft, const std::deque<T>& oRight,
										const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
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
			::testing::AssertionResult oRes = Compare<T>( (*itLeft), (*itRight), nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult CompareStack( std::stack<T> oLeft, std::stack<T> oRight,
										const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
				<< oLeft.size() << " != " << oRight.size();
		}
		int i = 0;
		while( oLeft.empty() == false )
		{
			::testing::AssertionResult oRes = Compare<T>( oLeft.top(), oRight.top(), nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
			oLeft.pop();
			oRight.pop();
			++i;
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult CompareQueue( std::queue<T> oLeft, std::queue<T> oRight,
								const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
	{
		if( oLeft.size() != oRight.size() )
		{
			return ::testing::AssertionFailure() << "different sizes: " 
				<< oLeft.size() << " != " << oRight.size();
		}
		int i = 0;
		while( oLeft.empty() == false )
		{
			::testing::AssertionResult oRes = Compare<T>( oLeft.front(), oRight.front(), nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
			oLeft.pop();
			oRight.pop();
			++i;
		}
		return ::testing::AssertionSuccess();
	}

	template<typename T>
	inline ::testing::AssertionResult CompareArray( const T* oLeft, const T* oRight, const std::size_t nSize,
											const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0 )
	{
		for( std::size_t i = 0; i < nSize; ++i )
		{
			::testing::AssertionResult oRes = Compare<T>( oLeft[i], oRight[i], nMaxRelDeviation, nMinRelevantSize );
			if( oRes == false )
				return oRes << " (entry " << i << ")";
		}
		return ::testing::AssertionSuccess();	
	}
}

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

#endif //_VISTATESTINGCOMPARECONTAINERS_H

