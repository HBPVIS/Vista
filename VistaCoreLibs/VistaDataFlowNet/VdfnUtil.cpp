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
#include "VdfnQuaternionSlerpNode.h"

#include "VdfnNodeCreators.h"

#include <VistaBase/VistaVectorMath.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaConversion.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace
{
		template<class T>
	class PropertyGetAsString : public VdfnPortFactory::StringGet
	{
	public:
		typedef std::string (*Convert)( const T& );
		PropertyGetAsString( Convert pFConvert = NULL )
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

	template<typename T>
	void RegisterDefaultPortAndFunctorAccess( VdfnPortFactory* pFac )
	{
		pFac->AddPortAccess( typeid(T).name(),
					new VdfnPortFactory::CPortAccess(
							new VdfnTypedPortCreate<T>,
							new TVdfnTranscodePortSet<T>,
							new VdfnTypedPortStringGet<T> ) );

		pFac->AddFunctorAccess( typeid(T).name(),
					new VdfnPortFactory::CFunctorAccess(
							new VdfnTypedPortTypeCompareCreate<T>,
							new TActionSet<T>,
							new VdfnTypedPortCreate<T>,
							new PropertyGetAsString<T> ) );
	}
}

namespace VistaConversion
{
	template<>
	struct ConvertTypeObject<VistaVector3D, VistaTransformMatrix>
	{
		VistaVector3D operator()( const VistaTransformMatrix& oFrom )
		{
			return oFrom.GetTranslation();
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

	RegisterDefaultPortAndFunctorAccess<bool>( pFac );
	RegisterDefaultPortAndFunctorAccess<int>( pFac );
	RegisterDefaultPortAndFunctorAccess<unsigned int>( pFac );
	RegisterDefaultPortAndFunctorAccess<VistaType::sint64>( pFac );
	RegisterDefaultPortAndFunctorAccess<VistaType::microtime>( pFac );
	RegisterDefaultPortAndFunctorAccess<float>( pFac );
	RegisterDefaultPortAndFunctorAccess<double>( pFac );

	RegisterDefaultPortAndFunctorAccess<std::string>( pFac );

	RegisterDefaultPortAndFunctorAccess<VistaVector3D>( pFac );
	RegisterDefaultPortAndFunctorAccess<VistaQuaternion>( pFac );
	RegisterDefaultPortAndFunctorAccess<VistaTransformMatrix>( pFac );
	RegisterDefaultPortAndFunctorAccess<VistaAxisAndAngle>( pFac );

	//RegisterDefaultPortAndFunctorAccess<std::vector<bool> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<int> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<unsigned int> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<VistaType::sint64> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<VistaType::microtime> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<float> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<double> >( pFac );

	RegisterDefaultPortAndFunctorAccess<std::vector<std::string> >( pFac );

	RegisterDefaultPortAndFunctorAccess<std::vector<VistaVector3D> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<VistaQuaternion> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<VistaTransformMatrix> >( pFac );
	RegisterDefaultPortAndFunctorAccess<std::vector<VistaAxisAndAngle> >( pFac );
	RegisterDefaultPortAndFunctorAccess<VdfnHistoryPortData*>( pFac );

	pFac->AddPortAccess( typeid(VistaVector<float,4>).name(),
						new VdfnPortFactory::CPortAccess(
									new VdfnTypedPortCreate<VistaVector<float,4> >,
									new TVdfnTranscodePortSet<VistaVector<float,4> > ) );

	pFac->AddFunctorAccess( typeid(VistaVector<float,4>).name(),
						new VdfnPortFactory::CFunctorAccess(
									new VdfnTypedPortTypeCompareCreate<VistaVector<float,4> >,
									new TActionSet<VistaVector<float,4> >,
									new VdfnTypedPortCreate<VistaVector<float,4> >,
									new PropertyGetAsString<VistaVector<float,4> > ) );

	pFac->AddPortAccess( typeid(std::map<std::string, float>).name(),
						new VdfnPortFactory::CPortAccess(
									new VdfnTypedPortCreate<std::map<std::string, float> >,
									new TVdfnTranscodePortSet<std::map<std::string, float> > ) );

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
	pFac->SetNodeCreator( "TypeConvert[VistaTransformMatrix,string]", new VdfnTypeConvertNodeCreate<VistaTransformMatrix,std::string>() );

	pFac->SetNodeCreator( "TypeConvert[VistaVector3D,VistaTransformMatrix]",   new VdfnTypeConvertNodeCreate<VistaVector3D,VistaTransformMatrix>() );
 	pFac->SetNodeCreator( "TypeConvert[VistaQuaternion,VistaTransformMatrix]", new VdfnTypeConvertNodeCreate<VistaQuaternion,VistaTransformMatrix>() );
 	pFac->SetNodeCreator( "TypeConvert[VistaTransformMatrix,VistaQuaternion]", new VdfnTypeConvertNodeCreate<VistaTransformMatrix,VistaQuaternion>() );
	pFac->SetNodeCreator( "TypeConvert[VistaTransformMatrix,VistaVector3D]", new VdfnTypeConvertNodeCreate<VistaTransformMatrix,VistaVector3D>() );

 	pFac->SetNodeCreator( "TypeConvert[vector[double],string]", new VdfnTypeConvertNodeCreate<std::vector<double>,std::string>() );


	pFac->SetNodeCreator( "Add[string]", new TVdfnBinOpCreate<std::string,std::string,std::string>( new VdfnBinaryOps::AddOp<std::string,std::string,std::string>) );
	pFac->SetNodeCreator( "Add[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::AddOp<int,int,int> ) );
	pFac->SetNodeCreator( "Add[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::AddOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Add[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::AddOp<float,float,float> ) );
	pFac->SetNodeCreator( "Add[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::AddOp<double,double,double> ) );
	pFac->SetNodeCreator( "Add[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,VistaVector3D>( new VdfnBinaryOps::AddOp<VistaVector3D,VistaVector3D,VistaVector3D>) );

	pFac->SetNodeCreator( "Substract[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,VistaVector3D>( new VdfnBinaryOps::SubOp<VistaVector3D,VistaVector3D,VistaVector3D>) );
	pFac->SetNodeCreator( "Substract[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::SubOp<int,int,int>) );

	pFac->SetNodeCreator( "Multiply[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::MultOp<int,int,int> ) );
	pFac->SetNodeCreator( "Multiply[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::MultOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Multiply[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::MultOp<float,float,float> ) );
	pFac->SetNodeCreator( "Multiply[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::MultOp<double,double,double> ) );

	pFac->SetNodeCreator( "Divide[int]", new TVdfnBinOpCreate<int,int,int>( new VdfnBinaryOps::DivOp<int,int,int> ) );
	pFac->SetNodeCreator( "Divide[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,unsigned int>( new VdfnBinaryOps::DivOp<unsigned int,unsigned int,unsigned int> ) );
	pFac->SetNodeCreator( "Divide[float]", new TVdfnBinOpCreate<float,float,float>( new VdfnBinaryOps::DivOp<float,float,float> ) );
	pFac->SetNodeCreator( "Divide[double]", new TVdfnBinOpCreate<double,double,double>( new VdfnBinaryOps::DivOp<double,double,double> ) );

	pFac->SetNodeCreator( "Multiply[VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::MultOp<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Multiply[VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion, VistaQuaternion, VistaQuaternion>(
															  new VdfnBinaryOps::MultOp<VistaQuaternion, VistaQuaternion, VistaQuaternion> ) );

	pFac->SetNodeCreator( "Multiply[int,VistaVector3D]", new TVdfnBinOpCreate<int, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<int, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[unsigned int,VistaVector3D]", new TVdfnBinOpCreate<unsigned int, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<unsigned int, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[float,VistaVector3D]", new TVdfnBinOpCreate<float, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<float, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[double,VistaVector3D]", new TVdfnBinOpCreate<double, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<double, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Multiply[VistaTransformMatrix,VistaVector3D]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::MultOp<VistaTransformMatrix, VistaVector3D, VistaVector3D> ) );

	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,VistaVector3D]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,VistaQuaternion]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaQuaternion, VistaQuaternion>(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, VistaQuaternion, VistaQuaternion> ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,VistaTransformMatrix]", new TVdfnBinOpCreate<VistaVector3D, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, VistaTransformMatrix, VistaTransformMatrix> ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,VistaVector3D]", new TVdfnBinOpCreate<VistaQuaternion, VistaVector3D, VistaVector3D>(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, VistaVector3D, VistaVector3D> ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion, VistaQuaternion, VistaQuaternion>(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, VistaQuaternion, VistaQuaternion> ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,VistaTransformMatrix]", new TVdfnBinOpCreate<VistaQuaternion, VistaTransformMatrix, VistaTransformMatrix>(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, VistaTransformMatrix, VistaTransformMatrix> ) );

	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,vector[VistaVector3D]]", new TVdfnBinOpCreate<VistaTransformMatrix, std::vector<VistaVector3D>, std::vector<VistaVector3D> >(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, std::vector<VistaVector3D>, std::vector<VistaVector3D> > ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,vector[VistaQuaternion]]", new TVdfnBinOpCreate<VistaTransformMatrix, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> >(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> > ) );
	pFac->SetNodeCreator( "Transform[VistaTransformMatrix,vector[VistaTransformMatrix]]", new TVdfnBinOpCreate<VistaTransformMatrix, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> >(
															  new VdfnBinaryOps::TransformOp<VistaTransformMatrix, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> > ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,vector[VistaVector3D]]", new TVdfnBinOpCreate<VistaVector3D, std::vector<VistaVector3D>, std::vector<VistaVector3D> >(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, std::vector<VistaVector3D>, std::vector<VistaVector3D> > ) );
	pFac->SetNodeCreator( "Transform[VistaVector3D,vector[VistaTransformMatrix]]", new TVdfnBinOpCreate<VistaVector3D, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> >(
															  new VdfnBinaryOps::TransformOp<VistaVector3D, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> > ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,vector[VistaVector3D]]", new TVdfnBinOpCreate<VistaQuaternion, std::vector<VistaVector3D>, std::vector<VistaVector3D> >(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, std::vector<VistaVector3D>, std::vector<VistaVector3D> > ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,vector[VistaQuaternion]]", new TVdfnBinOpCreate<VistaQuaternion, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> >(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, std::vector<VistaQuaternion>, std::vector<VistaQuaternion> > ) );
	pFac->SetNodeCreator( "Transform[VistaQuaternion,vector[VistaTransformMatrix]]", new TVdfnBinOpCreate<VistaQuaternion, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> >(
															  new VdfnBinaryOps::TransformOp<VistaQuaternion, std::vector<VistaTransformMatrix>, std::vector<VistaTransformMatrix> > ) );

	pFac->SetNodeCreator( "Equals[string]", new TVdfnBinOpCreate<std::string,std::string,bool>( new VdfnBinaryOps::EqualsOp<std::string,std::string,bool> ) );
	pFac->SetNodeCreator( "Equals[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnBinaryOps::EqualsOp<bool,bool,bool> ) );
	pFac->SetNodeCreator( "Equals[int]", new TVdfnBinOpCreate<int,int,bool>( new VdfnBinaryOps::EqualsOp<int,int,bool> ) );
	pFac->SetNodeCreator( "Equals[unsigned int]", new TVdfnBinOpCreate<unsigned int,unsigned int,bool>( new VdfnBinaryOps::EqualsOp<unsigned int,unsigned int,bool> ) );
	pFac->SetNodeCreator( "Equals[float]", new TVdfnBinOpCreate<float,float,bool>( new VdfnBinaryOps::EqualsOp<float,float,bool> ) );
	pFac->SetNodeCreator( "Equals[double]", new TVdfnBinOpCreate<double,double,bool>( new VdfnBinaryOps::EqualsOp<double,double,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaVector3D]", new TVdfnBinOpCreate<VistaVector3D,VistaVector3D,bool>( new VdfnBinaryOps::EqualsOp<VistaVector3D,VistaVector3D,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaQuaternion]", new TVdfnBinOpCreate<VistaQuaternion,VistaQuaternion,bool>( new VdfnBinaryOps::EqualsOp<VistaQuaternion,VistaQuaternion,bool> ) );
	pFac->SetNodeCreator( "Equals[VistaTransformMatrix]", new TVdfnBinOpCreate<VistaTransformMatrix,VistaTransformMatrix,bool>( new VdfnBinaryOps::EqualsOp<VistaTransformMatrix,VistaTransformMatrix,bool> ) );

	pFac->SetNodeCreator( "Compose3DVector", new TVdfnDefaultNodeCreate<VdfnCompose3DVectorNode> );
	pFac->SetNodeCreator( "Decompose3DVector", new TVdfnDefaultNodeCreate<VdfnDecompose3DVectorNode> );

	pFac->SetNodeCreator( "QuaternionSlerp", new TVdfnDefaultNodeCreate<VdfnQuaternionSlerpNode> );

	pFac->SetNodeCreator( "And[bool]", new TVdfnBinOpCreate<bool,bool,bool>( new VdfnBinaryOps::AndOp<bool,bool,bool> ) );

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

