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

#include <list>
using namespace std;

#include "VdfnSerializer.h"

#include "VdfnUtil.h"

#include "VdfnNode.h"
#include "VdfnGraph.h"

#include "VdfnDriverSensorNode.h"
#include "VdfnHistoryProjectNode.h"
#include "VdfnConstantValueNode.h"
#include "VdfnTypeConvertNode.h"

#include "VdfnPortFactory.h"
#include "VdfnNodeFactory.h"
#include "VdfnHistoryPort.h"
#include "VdfnActionObject.h"
#include "VdfnActionNode.h"
#include "VdfnSamplerNode.h"
#include "VdfnCompositeNode.h"
#include "VdfnBinaryOpNode.h"
#include "VdfnAxisRotateNode.h"
#include "VdfnProjectVectorNode.h"
#include "VdfnForceFeedbackNode.h"
#include "Vdfn3DNormalizeNode.h"
#include "VdfnReadWorkspaceNode.h"
#include "VdfnInvertNode.h"
#include "VdfnValueToTriggerNode.h"
#include "VdfnConditionalRouteNode.h"
#include "VdfnDumpHistoryNode.h"
#include "VdfnDelayNode.h"
#include "VdfnMatrixComposeNode.h"
#include "VdfnLatestUpdateNode.h"
#include "VdfnCompose3DVectorNode.h"
#include "VdfnDecompose3DVectorNode.h"

#include "VdfnNodeCreators.h"

#include <VistaBase/VistaVectorMath.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace
{
//	VistaVector3D ConvertToVistaVector3D(const std::string &sValue)
//	{
//		list<float> liTemp;
//		int iSize = VistaAspectsConversionStuff::ConvertToList(sValue, liTemp);
//		if (iSize > 4)
//			iSize = 4;	// allow setting of the homogeneous coordinate
//
//		VistaVector3D v3Temp;
//		list<float>::iterator it=liTemp.begin();
//		for (int i=0; i<iSize; ++i, ++it)
//			v3Temp[i] = *it;
//
//		return v3Temp;
//	}
//
//	VistaQuaternion ConvertToVistaQuaternion(const std::string &sValue)
//	{
//		list<float> liTemp;
//		int iSize = VistaAspectsConversionStuff::ConvertToList(sValue, liTemp);
//		if( iSize == 3 ) // interpret as Euler Angles
//		{
//			float euAngles[3];
//			list<float>::iterator it=liTemp.begin();
//			for (int i=0; i<3; ++i, ++it)
//				euAngles[i] = Vista::DegToRad(*it);
//			VistaQuaternion qTemp( VistaEulerAngles(euAngles[0], euAngles[1], euAngles[2] ) );
//			return qTemp;
//		}
//		else if (iSize > 4)
//			iSize = 4;
//
//		if( iSize != 4)
//			return VistaQuaternion(); // < 3?
//
//		VistaQuaternion qTemp;
//		list<float>::iterator it=liTemp.begin();
//		for (int i=0; i<iSize; ++i, ++it)
//			qTemp[i] = *it;
//
//		return qTemp;
//	}
//
//	VistaTransformMatrix ConvertToVistaMatrix( const std::string &sValue )
//	{
//		list<float> liTemp;
//		int iSize = VistaAspectsConversionStuff::ConvertToList(sValue, liTemp);
//		if(iSize != 16)
//		{
//			vstr::warnp() << "non-matchung number of arguments (" << iSize << " != 16 )\n";
//			vstr::warni() << vstr::singleindent 
//					<< "original string: " << sValue << std::endl;
//			return VistaTransformMatrix();
//		}
//
//
//		VistaTransformMatrix m;
//		list<float>::iterator it=liTemp.begin();
//		for(int r=0; r < 4; ++r)
//			for(int c=0; c < 4; ++c)
//				m[r][c] = *it++;
//
//		return m;
//	}
//
//
//	bool ConvertDoubleToBool( double dVal )
//	{
//		return (dVal != 0 ? true : false);
//	}
//
//	bool ConvertFloatToBool( float fVal )
//	{
//		return (fVal != 0 ? true : false);
//	}
//
//#ifdef WIN32
//#pragma warning(push)
//#pragma warning(disable: 4800)
//#endif
//	// Convertion from bool to int triggers a warning. WE know of the
//	// problem, and disable the waring
//	template<class tFrom, class tTo>
//	tTo ConvertFrom( tFrom from )
//	{
//		return tTo(from);
//	}
//#ifdef WIN32
//#pragma warning(pop)
//#endif
//
//	template<class T>
//	std::string ConvertToString( T val )
//	{
//		return VistaAspectsConversionStuff::ConvertToString(val);
//	}
//
//	template<class T>
//	std::string ConvertArrayToString( T aArray[], int iSize )
//	{
//		std::string sRetVal = VistaAspectsConversionStuff::ConvertToString( aArray[0] );
//		
//		for( int i = 1; i < iSize; ++i )
//		{
//			sRetVal += " " + VistaAspectsConversionStuff::ConvertToString( aArray[i] );
//		}
//
//		return sRetVal;
//	}
//
//	std::string ConvertVecToString( VistaVector<float,4> v4 )
//	{
//		//return (VistaAspectsConversionStuff::ConvertToString(v4[0])
//		//		+ std::string(" ")
//		//		+ VistaAspectsConversionStuff::ConvertToString(v4[1])
//		//		+ std::string(" ")
//		//		+ VistaAspectsConversionStuff::ConvertToString(v4[2])
//		//		+ std::string(" ")
//		//		+ VistaAspectsConversionStuff::ConvertToString(v4[3]));
//		return ConvertArrayToString<float>( &v4[0], 4 );
//	}
//
//	template<class T> std::string ConvertVectorToString( std::vector<T> vec )
//	{
//	//	std::ostringstream ret;
//	//	ret.setf(std::ios::fixed, std::ios::floatfield);
//
//	//	for( typename std::vector<T>::const_iterator cit = vec.begin();
//	//			 cit != vec.end(); ++cit )
//	//	{
//	//		ret << VistaAspectsConversionStuff::ConvertToString( *cit )
//	//		    << std::string( ";" );
//	//	}
//
//	//	return ret.str();
//		return ConvertArrayToString<T>( &vec[0], (int)vec.size() );
//	}
//
//
//	std::string ConvertVec3DToString( VistaVector3D v3 )
//	{
//		//return ConvertVecToString(v3);
//		return ConvertArrayToString<float>( &v3[0], 4 );
//	}
//
//
//	std::string ConvertQuatToString( VistaQuaternion q )
//	{
//		//return ConvertVecToString(q);
//		return ConvertArrayToString<float>( &q[0], 4 );
//	}
//
//	VistaTransformMatrix ConvertVec3DToMatrix( VistaVector3D v3 )
//	{
//		VistaTransformMatrix mat;
//		mat.SetTranslation(v3);
//		return mat;
//	}
//
//	VistaTransformMatrix ConvertQuatToMatrix( VistaQuaternion q )
//	{
//		VistaTransformMatrix mat(q);
//		return mat;
//	}
//
//    VistaQuaternion ConvertMatrixToQuat( VistaTransformMatrix m )
//	{
//		VistaQuaternion qat(m);
//		return qat;
//	}
//
//	std::string ConvertInt64ToString( DLV_INT64 v )
//	{
//		char buffer[4096];
//	#ifdef WIN32
//		_i64toa_s((_int64) v, buffer, 4096, 10);
//	#else
//		sprintf(buffer, "%lld", (unsigned long long) v);
//	#endif
//		return std::string(buffer);
//	}
//
//	std::string ConvertUInt64ToString( VistaType::uint64 v )
//	{
//		char buffer[4096];
//	#ifdef WIN32
//		_i64toa_s((_int64) v, buffer, 4096, 10);
//	#else
//		sprintf(buffer, "%lld", (unsigned long long) v);
//	#endif
//		return std::string(buffer);
//	}
//
//	std::string ConvertMicrotimeToString( VistaType::microtime ts )
//	{
//		return VistaAspectsConversionStuff::ConvertToString( double(ts) );
//	}
//
//
//	std::string ConvertStringToString( std::string str)
//	{
//		return str;
//	}
//
//	std::string ConvertFloatToString( float f )
//	{
//		return VistaAspectsConversionStuff::ConvertToString( (double(f)) );
//	}
//
//	std::string ConvertAxisToString( VistaAxisAndAngle aaa )
//	{
//		std::string sRet = ConvertVec3DToString(aaa.m_v3Axis)
//						 + " ["
//						 + VistaAspectsConversionStuff::ConvertToString(aaa.m_fAngle)
//						 + "]";
//		return sRet;
//	}

	template<class T>
	class CGet : public VdfnPortFactory::StringGet
	{
	public:
		typedef std::string (*Convert)( const T& );
		CGet( Convert pFConvert = NULL )
		: m_pFC( pFConvert )
		{
			if( m_pFC == NULL )
				m_pFC = &VistaConversion::ToString<T>;
		}

		virtual bool GetStringValue(IVistaPropertyGetFunctor *pF,
										const VistaSensorMeasure *pMeasure,
										std::string &strOut)
		{
			IVistaMeasureTranscode::TTranscodeValueGet<T> *pGet
				= dynamic_cast<IVistaMeasureTranscode::TTranscodeValueGet<T>*>(pF);
			if(pGet != NULL)
			{
				T value;
				if(pGet->GetValue(pMeasure, value))
				{
					strOut = m_pFC(value);
					return true;
				}
				return false;
			}

			IVistaMeasureTranscode::TTranscodeIndexedGet<T> *pGetIdx
				= dynamic_cast<IVistaMeasureTranscode::TTranscodeIndexedGet<T>*>(pF);
			if(pGetIdx == NULL)
				return false;

			unsigned int n=0;
			while(n<256)
			{
				T value;
				if( pGetIdx->GetValueIndexed(pMeasure, value, n) == true )
				{
					strOut += std::string(" ") + m_pFC(value);
				}
				else
					return true;
				++n;
			}

			return false;
		}

	private:
		Convert m_pFC;
	};


	// ########################################################
	// Some basic binary ops, +,*, ==
	// ########################################################

	template<class T, class T2, class K>
	class VdfnAddOp : public TVdfnBinaryOpNode<T,T2,K>::BinOp
	{
	public:
		virtual K operator()(const T&left, const T2&right) const
		{
			return left+right;
		}
	};

	template<class T, class T2, class K>
	class VdfnSubOp : public TVdfnBinaryOpNode<T,T2,K>::BinOp
	{
	public:
		virtual K operator()(const T&left, const T2&right) const
		{
			return left-right;
		}
	};

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4244)
#endif
	// the MultOp can contain some type conversions that may result in warnings
	// we know and accept that, so we disable the warning locally
	template<class T, class T2, class K>
	class VdfnMultOp : public TVdfnBinaryOpNode<T,T2,K>::BinOp
	{
		virtual K operator()(const T&left, const T2&right) const
		{
			return left * right;
		}
	};
#ifdef WIN32
#pragma warning(pop)
#endif

	template<class T, class T2, class K>
	class VdfnDivOp : public TVdfnBinaryOpNode<T,T2,K>::BinOp
	{
		virtual K operator()(const T&left, const T2&right) const
		{
			return left / right;
		}
	};

	template<class T, class T2, class K>
	class VdfnEqualsOp : public TVdfnBinaryOpNode<T,T2,K>::BinOp
	{
		virtual K operator()(const T&left, const T2&right) const
		{
			return (left == right);
		}
	};

	template<class T, class T2, class K>
	class VdfnAndOp : public TVdfnBinaryOpNode<T,T2,K>::BinOp
	{
		virtual K operator()(const T&left, const T2&right) const
		{
			return (left && right);
		}
	};
}


namespace VdfnUtil
{
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool RegisterBasicPortSetters()
{
	VdfnPortFactory *pFac = VdfnPortFactory::GetSingleton();
	pFac->AddPortAccess( typeid(int).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<int>,
															new TVdfnTranscodePortSet<int> ) );
	pFac->AddPortAccess( typeid(unsigned int).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<unsigned int>,
															new TVdfnTranscodePortSet<unsigned int> ) );
	pFac->AddPortAccess( typeid(VistaType::sint64).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaType::sint64>,
															new TVdfnTranscodePortSet<VistaType::sint64> ) );

	pFac->AddPortAccess( typeid(VistaType::microtime).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaType::microtime>,
															new TVdfnTranscodePortSet<VistaType::microtime> ) );

	pFac->AddPortAccess( typeid(bool).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<bool>,
															new TVdfnTranscodePortSet<bool> ) );

	pFac->AddPortAccess( typeid(double).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<double>,
															new TVdfnTranscodePortSet<double> ) );


	pFac->AddPortAccess( typeid(float).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<float>,
															new TVdfnTranscodePortSet<float> ) );

	pFac->AddPortAccess( typeid(VistaVector3D).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaVector3D>,
															new TVdfnTranscodePortSet<VistaVector3D> ) );
	pFac->AddPortAccess( typeid(VistaQuaternion).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaQuaternion>,
															new TVdfnTranscodePortSet<VistaQuaternion> ) );

	pFac->AddPortAccess( typeid(VistaVector<float,4>).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaVector<float,4> >,
															new TVdfnTranscodePortSet<VistaVector<float,4> > ) );

	pFac->AddPortAccess( typeid(VistaTransformMatrix).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaTransformMatrix>,
															new TVdfnTranscodePortSet<VistaTransformMatrix> ) );

	pFac->AddPortAccess( typeid(VistaAxisAndAngle).name(),
						 new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<VistaAxisAndAngle>,
															new TVdfnTranscodePortSet<VistaAxisAndAngle> ) );

	pFac->AddPortAccess( typeid(std::vector<float>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<float> >,
										   new TVdfnTranscodePortSet<std::vector<float> > ) );

	pFac->AddPortAccess( typeid(std::vector<double>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<double> >,
										   new TVdfnTranscodePortSet<std::vector<double> > ) );

	pFac->AddPortAccess( typeid(std::vector<unsigned int>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<unsigned int> >,
										   new TVdfnTranscodePortSet<std::vector<unsigned int> > ) );

	pFac->AddPortAccess( typeid(std::vector<int>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<int> >,
										   new TVdfnTranscodePortSet<std::vector<int> > ) );

	pFac->AddPortAccess( typeid(std::vector<VistaVector3D>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<VistaVector3D> >,
										   new TVdfnTranscodePortSet<std::vector<VistaVector3D> > ) );

	pFac->AddPortAccess( typeid(std::vector<VistaTransformMatrix>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<VistaTransformMatrix> >,
										   new TVdfnTranscodePortSet<std::vector<VistaTransformMatrix> > ) );

	pFac->AddPortAccess( typeid(std::vector<VistaQuaternion>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::vector<VistaQuaternion> >,
										   new TVdfnTranscodePortSet<std::vector<VistaQuaternion> > ) );

	pFac->AddPortAccess( typeid(std::map<std::string, float>).name(),
		new VdfnPortFactory::CPortAccess( new VdfnTypedPortCreate<std::map<std::string, float> >,
										   new TVdfnTranscodePortSet<std::map<std::string, float> > ) );

	pFac->AddFunctorAccess( typeid(VistaVector3D).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaVector3D>,
																	  new TActionSet<VistaVector3D>,
																	  new VdfnTypedPortCreate<VistaVector3D>,
																	  new CGet<VistaVector3D> ) );
	pFac->AddFunctorAccess( typeid(VistaVector<float,4>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaVector<float,4> >,
																	  new TActionSet<VistaVector<float,4> >,
																	  new VdfnTypedPortCreate<VistaVector<float,4> >,
																	  new CGet<VistaVector<float,4> > ) );
	pFac->AddFunctorAccess( typeid(VistaQuaternion).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaQuaternion>,
																	  new TActionSet<VistaQuaternion>,
																	  new VdfnTypedPortCreate<VistaQuaternion>,
																	  new CGet<VistaQuaternion> ) );
	pFac->AddFunctorAccess( typeid(VistaTransformMatrix).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaTransformMatrix>,
																	  new TActionSet<VistaTransformMatrix>,
																	  new VdfnTypedPortCreate<VistaTransformMatrix>) );
	pFac->AddFunctorAccess( typeid(VistaAxisAndAngle).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaAxisAndAngle>,
																	  new TActionSet<VistaAxisAndAngle>,
																	  new VdfnTypedPortCreate<VistaAxisAndAngle>,
																	  new CGet<VistaAxisAndAngle> ) );


	pFac->AddFunctorAccess( typeid(DLV_INT64).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<DLV_INT64>,
																	  new TActionSet<DLV_INT64>,
																	  new VdfnTypedPortCreate<DLV_INT64>,
																	  new CGet<DLV_INT64> ) );
	pFac->AddFunctorAccess( typeid(VistaType::microtime).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaType::microtime>,
																	  new TActionSet<VistaType::microtime>,
																	  new VdfnTypedPortCreate<VistaType::microtime>,
																	  new CGet<VistaType::microtime> ) );

	pFac->AddFunctorAccess( typeid(VistaType::uint64).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<VistaType::uint64>,
																	  new TActionSet<VistaType::uint64>,
																	  new VdfnTypedPortCreate<VistaType::uint64>,
																	  new CGet<VistaType::uint64> ) );

	pFac->AddFunctorAccess( typeid(bool).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<bool>,
																	  new TActionSet<bool>,
																	  new VdfnTypedPortCreate<bool>,
																	  new CGet<bool> ) );
	pFac->AddFunctorAccess( typeid(double).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<double>,
																	  new TActionSet<double>,
																	  new VdfnTypedPortCreate<double>,
																	  new CGet<double> ) );
	pFac->AddFunctorAccess( typeid(float).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<float>,
																	  new TActionSet<float>,
																	  new VdfnTypedPortCreate<float>,
																	  new CGet<float> ) );
	pFac->AddFunctorAccess( typeid(std::string).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::string>,
																	  new TActionSet<std::string>,
																	  new VdfnTypedPortCreate<std::string>,
																	  new CGet<std::string> ) );
	pFac->AddFunctorAccess( typeid(int).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<int>,
																	  new TActionSet<int>,
																	  new VdfnTypedPortCreate<int>,
																	  new CGet<int> ) );
	pFac->AddFunctorAccess( typeid(unsigned int).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<unsigned int>,
																	  new TActionSet<unsigned int>,
																	  new VdfnTypedPortCreate<unsigned int>,
																	  new CGet<unsigned int> ) );

	pFac->AddFunctorAccess( typeid(std::vector<int>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<int> >,
																	  new TActionSet<std::vector<int> >,
																	  new VdfnTypedPortCreate<std::vector<int> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<unsigned int>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<unsigned int> >,
																	  new TActionSet<std::vector<unsigned int> >,
																	  new VdfnTypedPortCreate<std::vector<unsigned int> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<float>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<float> >,
																	  new TActionSet<std::vector<float> >,
																	  new VdfnTypedPortCreate<std::vector<float> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<double>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<double> >,
																	  new TActionSet<std::vector<double> >,
																	  new VdfnTypedPortCreate<std::vector<double> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<VistaVector3D>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<VistaVector3D> >,
																	  new TActionSet<std::vector<VistaVector3D> >,
																	  new VdfnTypedPortCreate<std::vector<VistaVector3D> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<VistaQuaternion>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<VistaQuaternion> >,
																	  new TActionSet<std::vector<VistaQuaternion> >,
																	  new VdfnTypedPortCreate<std::vector<VistaQuaternion> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<VistaAxisAndAngle>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<VistaAxisAndAngle> >,
																	  new TActionSet<std::vector<VistaAxisAndAngle> >,
																	  new VdfnTypedPortCreate<std::vector<VistaAxisAndAngle> > ) );
	pFac->AddFunctorAccess( typeid(std::vector<VistaTransformMatrix>).name(), new VdfnPortFactory::CFunctorAccess(
																	  new VdfnTypedPortTypeCompareCreate<std::vector<VistaTransformMatrix> >,
																	  new TActionSet<std::vector<VistaTransformMatrix> >,
																	  new VdfnTypedPortCreate<std::vector<VistaTransformMatrix> > ) );
	return true;
}


bool RegisterBasicNodeCreators(VistaDriverMap *pDrivers,
							   VdfnObjectRegistry *pObjRegistry)
{
	VdfnNodeFactory *pFac = VdfnNodeFactory::GetSingleton();

	pFac->SetNodeCreator( "Shallow", new VdfnShallowNodeCreate );
	pFac->SetNodeCreator( "Timer", new VdfnTimerNodeCreate );
	pFac->SetNodeCreator( "TickTimer", new VdfnTickTimerNodeCreate );

	pFac->SetNodeCreator( "DriverSensor", new VdfnDriverSensorNodeCreate( pDrivers ) );
	pFac->SetNodeCreator( "HistoryProject", new VdfnHistoryProjectNodeCreate );

	pFac->SetNodeCreator( "Composite", new VdfnCompositeNodeCreate );

	pFac->SetNodeCreator( "Action", new VdfnActionNodeCreate(pObjRegistry) );
	pFac->SetNodeCreator( "Sampler", new VdfnSamplerNodeCreate );

	pFac->SetNodeCreator( "UpdateThreshold", new VdfnUpdateThresholdNodeCreate );
	pFac->SetNodeCreator( "Threshold[int]", new VdfnThresholdNodeCreate<int> );
	pFac->SetNodeCreator( "Threshold[unsigned int]", new VdfnThresholdNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "Threshold[float]", new VdfnThresholdNodeCreate<float> );
	pFac->SetNodeCreator( "Threshold[double]", new VdfnThresholdNodeCreate<double> );

	pFac->SetNodeCreator( "Logger", new VdfnLoggerNodeCreate );
	pFac->SetNodeCreator( "DumpHistory", new VdfnDumpHistoryDefaultCreate );

	pFac->SetNodeCreator( "ConstantValue[unsigned int]", new VdfnConstantValueNodeCreate<unsigned int>() );
	pFac->SetNodeCreator( "ConstantValue[int]", new VdfnConstantValueNodeCreate<int>() );
	pFac->SetNodeCreator( "ConstantValue[float]", new VdfnConstantValueNodeCreate<float>() );
	pFac->SetNodeCreator( "ConstantValue[double]", new VdfnConstantValueNodeCreate<double>() );
	pFac->SetNodeCreator( "ConstantValue[bool]", new VdfnConstantValueNodeCreate<bool>() );
	pFac->SetNodeCreator( "ConstantValue[string]", new VdfnConstantValueNodeCreate<std::string>() );
	pFac->SetNodeCreator( "ConstantValue[VistaVector3D]", new VdfnConstantValueNodeCreate<VistaVector3D>() );
	pFac->SetNodeCreator( "ConstantValue[VistaQuaternion]", new VdfnConstantValueNodeCreate<VistaQuaternion>() );
	pFac->SetNodeCreator( "ConstantValue[VistaTransformMatrix]", new VdfnConstantValueNodeCreate<VistaTransformMatrix>() );
	pFac->SetNodeCreator( "EnvValue", new VdfnEnvStringValueNodeCreate );

	pFac->SetNodeCreator( "ChangeDetect[int]", new TVdfnChangeDetectNodeCreate<int> );
	pFac->SetNodeCreator( "ChangeDetect[unsigned int]", new TVdfnChangeDetectNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "ChangeDetect[int64]", new TVdfnChangeDetectNodeCreate<DLV_INT64> );
	pFac->SetNodeCreator( "ChangeDetect[VistaType::uint64]", new TVdfnChangeDetectNodeCreate<VistaType::uint64> );
	pFac->SetNodeCreator( "ChangeDetect[float]", new TVdfnChangeDetectNodeCreate<float> );
	pFac->SetNodeCreator( "ChangeDetect[double]", new TVdfnChangeDetectNodeCreate<double> );
	pFac->SetNodeCreator( "ChangeDetect[bool]", new TVdfnChangeDetectNodeCreate<bool> );
	pFac->SetNodeCreator( "ChangeDetect[VistaVector3D]", new TVdfnChangeDetectNodeCreate<VistaVector3D> );
	pFac->SetNodeCreator( "ChangeDetect[VistaQuaternion]", new TVdfnChangeDetectNodeCreate<VistaQuaternion> );

	//pFac->SetNodeCreator( "Aggregate[bool]", new VdfnAggregateNodeCreate<bool> );
	pFac->SetNodeCreator( "Aggregate[int]", new VdfnAggregateNodeCreate<int> );
	pFac->SetNodeCreator( "Aggregate[unsigned int]", new VdfnAggregateNodeCreate<int> );
	pFac->SetNodeCreator( "Aggregate[float]", new VdfnAggregateNodeCreate<float> );
	pFac->SetNodeCreator( "Aggregate[double]", new VdfnAggregateNodeCreate<double> );
	pFac->SetNodeCreator( "Aggregate[string]", new VdfnAggregateNodeCreate<std::string> );
	pFac->SetNodeCreator( "Aggregate[VistaVector3D]", new VdfnAggregateNodeCreate<VistaVector3D> );
	pFac->SetNodeCreator( "Aggregate[VistaQuaternion]", new VdfnAggregateNodeCreate<VistaQuaternion> );
	pFac->SetNodeCreator( "Aggregate[VistaTransformMatrix]", new VdfnAggregateNodeCreate<VistaTransformMatrix> );

	pFac->SetNodeCreator( "TypeConvert[double,bool]", new VdfnTypeConvertNodeCreate<double,bool> );
	pFac->SetNodeCreator( "TypeConvert[double,int]", new VdfnTypeConvertNodeCreate<double,int> );
	pFac->SetNodeCreator( "TypeConvert[double,unsigned int]", new VdfnTypeConvertNodeCreate<double,unsigned int> );
	pFac->SetNodeCreator( "TypeConvert[double,float]", new VdfnTypeConvertNodeCreate<double,float> );
	pFac->SetNodeCreator( "TypeConvert[double,string]", new VdfnTypeConvertNodeCreate<double,std::string> );
	pFac->SetNodeCreator( "TypeConvert[VistaType::microtime,string]", new VdfnTypeConvertNodeCreate<VistaType::microtime,std::string> );

	pFac->SetNodeCreator( "TypeConvert[int,bool]", new VdfnTypeConvertNodeCreate<int,bool>() );
	pFac->SetNodeCreator( "TypeConvert[int,unsigned int]", new VdfnTypeConvertNodeCreate<int,unsigned int>() );
	pFac->SetNodeCreator( "TypeConvert[int,float]", new VdfnTypeConvertNodeCreate<int,float>() );
	pFac->SetNodeCreator( "TypeConvert[int,double]", new VdfnTypeConvertNodeCreate<int,double>() );
	pFac->SetNodeCreator( "TypeConvert[int,string]", new VdfnTypeConvertNodeCreate<int,std::string>() );

	pFac->SetNodeCreator( "TypeConvert[bool,int]", new VdfnTypeConvertNodeCreate<bool,int>() );
	pFac->SetNodeCreator( "TypeConvert[bool,unsigned int]", new VdfnTypeConvertNodeCreate<bool,unsigned int>() );
	pFac->SetNodeCreator( "TypeConvert[bool,float]", new VdfnTypeConvertNodeCreate<bool,float>() );
	pFac->SetNodeCreator( "TypeConvert[bool,double]", new VdfnTypeConvertNodeCreate<bool,double>() );
	pFac->SetNodeCreator( "TypeConvert[bool,string]", new VdfnTypeConvertNodeCreate<bool,std::string>() );

	pFac->SetNodeCreator( "TypeConvert[float,bool]", new VdfnTypeConvertNodeCreate<float,bool>() );
	pFac->SetNodeCreator( "TypeConvert[float,int]", new VdfnTypeConvertNodeCreate<float,int>() );
	pFac->SetNodeCreator( "TypeConvert[float,unsigned int]", new VdfnTypeConvertNodeCreate<float,unsigned int>() );
	pFac->SetNodeCreator( "TypeConvert[float,double]", new VdfnTypeConvertNodeCreate<float,double>() );
	pFac->SetNodeCreator( "TypeConvert[float,string]", new VdfnTypeConvertNodeCreate<float,std::string>() );

	pFac->SetNodeCreator( "TypeConvert[unsigned int,bool]", new VdfnTypeConvertNodeCreate<unsigned int,bool>() );
	pFac->SetNodeCreator( "TypeConvert[unsigned int,int]", new VdfnTypeConvertNodeCreate<unsigned int,int>() );
	pFac->SetNodeCreator( "TypeConvert[unsigned int,float]", new VdfnTypeConvertNodeCreate<unsigned int,float>() );
	pFac->SetNodeCreator( "TypeConvert[unsigned int,double]", new VdfnTypeConvertNodeCreate<unsigned int,double>() );
	pFac->SetNodeCreator( "TypeConvert[unsigned int,string]", new VdfnTypeConvertNodeCreate<unsigned int,std::string>() );

	pFac->SetNodeCreator( "TypeConvert[VistaVector3D,string]", new VdfnTypeConvertNodeCreate<VistaVector3D,std::string>() );
 	pFac->SetNodeCreator( "TypeConvert[VistaQuaternion,string]", new VdfnTypeConvertNodeCreate<VistaQuaternion,std::string>() );

	pFac->SetNodeCreator( "TypeConvert[VistaVector3D,VistaTransformMatrix]",   new VdfnTypeConvertNodeCreate<VistaVector3D,VistaTransformMatrix>() );
 	pFac->SetNodeCreator( "TypeConvert[VistaQuaternion,VistaTransformMatrix]", new VdfnTypeConvertNodeCreate<VistaQuaternion,VistaTransformMatrix>() );
 	pFac->SetNodeCreator( "TypeConvert[VistaTransformMatrix,VistaQuaternion]", new VdfnTypeConvertNodeCreate<VistaTransformMatrix,VistaQuaternion>() );

 	pFac->SetNodeCreator( "TypeConvert[vector[double],string]", new VdfnTypeConvertNodeCreate<std::vector<double>,std::string>() );


	pFac->SetNodeCreator( "Add[string]", new TVdfnBinOpCreate<std::string,std::string,std::string>( new VdfnAddOp<std::string,std::string,std::string>) );
	pFac->SetNodeCreator( "Add[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnAddOp<int,int,int> ) );
	pFac->SetNodeCreator( "Add[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnAddOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Add[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnAddOp<float,float,float> ) );
	pFac->SetNodeCreator( "Add[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnAddOp<double,double,double> ) );
	pFac->SetNodeCreator( "Add[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,VistaVector3D>( new VdfnAddOp<VistaVector3D,VistaVector3D,VistaVector3D>) );

	pFac->SetNodeCreator( "Substract[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,VistaVector3D>( new VdfnSubOp<VistaVector3D,VistaVector3D,VistaVector3D>) );
	pFac->SetNodeCreator( "Substract[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnSubOp<int,int,int>) );

	pFac->SetNodeCreator( "Multiply[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnMultOp<int,int,int> ) );
	pFac->SetNodeCreator( "Multiply[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnMultOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Multiply[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnMultOp<float,float,float> ) );
	pFac->SetNodeCreator( "Multiply[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnMultOp<double,double,double> ) );

	pFac->SetNodeCreator( "Divide[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnDivOp<int,int,int> ) );
	pFac->SetNodeCreator( "Divide[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnDivOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Divide[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnDivOp<float,float,float> ) );
	pFac->SetNodeCreator( "Divide[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnDivOp<double,double,double> ) );

	pFac->SetNodeCreator( "Multiply[VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnMultOp<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Multiply[VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion, VistaQuaternion, VistaQuaternion>(
															  new VdfnMultOp<VistaQuaternion, VistaQuaternion, VistaQuaternion> ) );

	pFac->SetNodeCreator( "Multiply[int,VistaVector3D]", new TVdfnBinOpCreate<int, VistaVector3D, VistaVector3D>(
															  new VdfnMultOp<int, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[unsigned int,VistaVector3D]", new TVdfnBinOpCreate<unsigned int, VistaVector3D, VistaVector3D>(
															  new VdfnMultOp<unsigned int, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[float,VistaVector3D]", new TVdfnBinOpCreate<float, VistaVector3D, VistaVector3D>(
															  new VdfnMultOp<float, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[double,VistaVector3D]", new TVdfnBinOpCreate<double, VistaVector3D, VistaVector3D>(
															  new VdfnMultOp<double, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[VistaTransformMatrix,VistaVector3D]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaVector3D, VistaVector3D>(
															  new VdfnMultOp<VistaTransformMatrix, VistaVector3D, VistaVector3D> ) );
	//pFac->SetNodeCreator( "Multiply[VistaQuaternion,VistaVector3D]", new TVdfnBinOpCreate<VistaQuaternion, VistaVector3D, VistaVector3D>(
	//														  new VdfnMultOp<VistaQuaternion, VistaVector3D, VistaVector3D> ) );

	/**
         * @todo test this! -> should scale the angle of this quaternion...most probably the double*qaut implementation does something different
	pFac->SetNodeCreator( "Multiply[double,VistaQuaternion]",
			      new TVdfnBinOpCreate<double, VistaQuaternion, VistaQuaternion>(
				  new VdfnMultOp<double, VistaQuaternion, VistaQuaternion> ) );
	*/
	pFac->SetNodeCreator( "Equals[string]", new TVdfnBinOpCreate<std::string,std::string,bool>( new VdfnEqualsOp<std::string,std::string,bool> ) );
	pFac->SetNodeCreator( "Equals[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnEqualsOp<bool,bool,bool> ) );
	pFac->SetNodeCreator( "Equals[int]", new TVdfnBinOpCreate<int,int,bool>( new VdfnEqualsOp<int,int,bool> ) );
	pFac->SetNodeCreator( "Equals[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,bool>( new VdfnEqualsOp<unsigned int,unsigned int,bool> ) );
	pFac->SetNodeCreator( "Equals[float]", new TVdfnBinOpCreate<float,float,bool>( new VdfnEqualsOp<float,float,bool> ) );
	pFac->SetNodeCreator( "Equals[double]", new TVdfnBinOpCreate<double,double,bool>( new VdfnEqualsOp<double,double,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,bool>( new VdfnEqualsOp<VistaVector3D,VistaVector3D,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion,VistaQuaternion,bool>( new VdfnEqualsOp<VistaQuaternion,VistaQuaternion,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix,VistaTransformMatrix,bool>( new VdfnEqualsOp<VistaTransformMatrix,VistaTransformMatrix,bool> ) );

	pFac->SetNodeCreator( "Compose3DVector", new TVdfnDefaultNodeCreate<VdfnCompose3DVectorNode> );
	pFac->SetNodeCreator( "Decompose3DVector", new TVdfnDefaultNodeCreate<VdfnDecompose3DVectorNode> );

	pFac->SetNodeCreator( "And[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnAndOp<bool,bool,bool> ) );

	pFac->SetNodeCreator( "ValueToTrigger[int]", new TVdfnValueToTriggerNodeCreate<int> );
	pFac->SetNodeCreator( "ValueToTrigger[unsigned int]", new TVdfnValueToTriggerNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "ConditionalRoute", new TVdfnDefaultNodeCreate<VdfnConditionalRouteNode> );

	pFac->SetNodeCreator( "Counter[int]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "Counter[unsigned int]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "Counter[float]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "Counter[double]", new TVdfnCounterNodeCreate<int> );
	pFac->SetNodeCreator( "ModuloCounter[int]", new TVdfnModuloCounterNodeCreate<int> );
	pFac->SetNodeCreator( "ModuloCounter[unsigned int]", new TVdfnModuloCounterNodeCreate<unsigned int> );
	pFac->SetNodeCreator( "ModuloCounter[float]", new TVdfnModuloCounterNodeCreate<float> );
	pFac->SetNodeCreator( "ModuloCounter[double]", new TVdfnModuloCounterNodeCreate<double> );

	pFac->SetNodeCreator( "GetTransform", new VdfnGetTransformNodeCreate(pObjRegistry) );
	pFac->SetNodeCreator( "SetTransform", new VdfnSetTransformNodeCreate(pObjRegistry) );
	pFac->SetNodeCreator( "ApplyTransform", new VdfnApplyTransformNodeCreate(pObjRegistry) );

	pFac->SetNodeCreator( "3DNormalize", new Vista3DNormalizeNodeCreate );

	pFac->SetNodeCreator( "LatestUpdate[VistaVector3D]", new TVdfnDefaultNodeCreate<TVdfnLatestUpdateNode<VistaVector3D> > );
	pFac->SetNodeCreator( "LatestUpdate[bool]", new TVdfnDefaultNodeCreate<TVdfnLatestUpdateNode<bool> > );

	pFac->SetNodeCreator( "AxisRotate", new TVdfnDefaultNodeCreate<VdfnAxisRotateNode> );
	pFac->SetNodeCreator( "MatrixCompose", new TVdfnDefaultNodeCreate<VdfnMatrixComposeNode> );
	pFac->SetNodeCreator( "ProjectVector", new VdfnProjectVectorNodeCreate );
	pFac->SetNodeCreator( "ForceFeedback", new VdfnForceFeedbackNodeCreate( pDrivers ) );
	pFac->SetNodeCreator( "ReadWorkspace", new VdfnReadWorkspaceNodeCreate(pDrivers) );

	pFac->SetNodeCreator( "GetElement[vector<int>]", new TVdfnGetElementNodeCreate<std::vector<int>, int> );
	pFac->SetNodeCreator( "GetElement[vector<usnigned int>]", new TVdfnGetElementNodeCreate<std::vector<unsigned int>, unsigned int> );
	pFac->SetNodeCreator( "GetElement[vector<float>]", new TVdfnGetElementNodeCreate<std::vector<float>, float> );
	pFac->SetNodeCreator( "GetElement[vector<double>]", new TVdfnGetElementNodeCreate<std::vector<double>, double> );
	pFac->SetNodeCreator( "GetElement[vector<VistaVector3D>]", new TVdfnGetElementNodeCreate<std::vector<VistaVector3D>, VistaVector3D> );
	pFac->SetNodeCreator( "GetElement[vector<VistaQuaternion>]", new TVdfnGetElementNodeCreate<std::vector<VistaQuaternion>, VistaQuaternion> );
	pFac->SetNodeCreator( "GetElement[vector<VistaTransformMatrix>]", new TVdfnGetElementNodeCreate<std::vector<VistaTransformMatrix>, VistaTransformMatrix> );
	pFac->SetNodeCreator( "GetElement[VistaVector3D]", new TVdfnGetElementFromArrayNodeCreate<VistaVector3D, float, 4> );
	pFac->SetNodeCreator( "GetElement[VistaQuaternion]", new TVdfnGetElementFromArrayNodeCreate<VistaQuaternion, float, 4> );
	

	pFac->SetNodeCreator( "Invert[VistaVector3D]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<VistaVector3D> >);
	pFac->SetNodeCreator( "Invert[VistaQuaternion]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<VistaQuaternion> >);
	pFac->SetNodeCreator( "Invert[VistaTransformMatrix]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<VistaTransformMatrix> >);
	pFac->SetNodeCreator( "Invert[bool]", new TVdfnDefaultNodeCreate<TVdfnInvertNode<bool> >);

	pFac->SetNodeCreator( "Delay[VistaVector3D]", new VdfnDelayNodeCreate<VistaVector3D> );

	return true;
}


bool InitVdfn(VistaDriverMap *pDrivers,
			  VdfnObjectRegistry *pObjRegistry)
{
	bool bRet = RegisterBasicPortSetters();
	bRet = bRet && RegisterBasicNodeCreators(pDrivers,
											 pObjRegistry);
	return bRet;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


} // namespace VdfnUtil

