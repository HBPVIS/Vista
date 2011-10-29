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


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>
#include <iomanip>
#include <cmath>

#include "VistaDfnNavigationNode.h"

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace
{

// not tested, the naming is probably wrong!
/*	VistaQuaternion EulerToQuat(float pitch, float yaw, float roll)
	{
		VistaQuaternion q;
		q[0] = cosf(pitch/2.0f)*cosf(yaw/2.0f)*cosf(roll/2.0f) +
               sinf(pitch/2.0f)*sinf(yaw/2.0f)*sinf(roll/2.0f);
		q[1] = sinf(pitch/2.0f)*cosf(yaw/2.0f)*cosf(roll/2.0f) -
               cosf(pitch/2.0f)*sinf(yaw/2.0f)*sinf(roll/2.0f);
		q[2] = cosf(pitch/2.0f)*sinf(yaw/2.0f)*cosf(roll/2.0f) +
               sinf(pitch/2.0f)*cosf(yaw/2.0f)*sinf(roll/2.0f);
		q[3] = cosf(pitch/2.0f)*cosf(yaw/2.0f)*sinf(roll/2.0f) -
               sinf(pitch/2.0f)*sinf(yaw/2.0f)*cosf(roll/2.0f);
	}
*/
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnNavigationNode::VistaDfnNavigationNode( 
							const int iDefaultNavigationMode,
							const float fDefaultLinearVelocity,
							const float fDefaultAngularVelocity ) :
	m_pTransform(NULL),
	m_pOut(new TVdfnPort<VistaTransformMatrix>),
	m_pTranslation(NULL),
	m_pRotation(NULL),
	m_pDeltaTime(NULL),
	m_pNavigationMode(NULL),
	m_iDefaultNavigationMode( iDefaultNavigationMode ),
	m_fDefaultLinearVelocity( fDefaultLinearVelocity ),
	m_fDefaultAngularVelocity( fDefaultAngularVelocity ),
	m_nLastDTUpateCount(0)
{
	// in-ports:
	// - navigation mode (heli, object in hand, ...) : int
	// - translation : VistaVector3D
	// - rotation : VistaQuaternion
	// - linear_velocity: float
	// - angular_velocity: float
	RegisterInPortPrototype( "dt", new TVdfnPortTypeCompare<TVdfnPort<double> > );
	RegisterInPortPrototype( "navigation_mode", new TVdfnPortTypeCompare<TVdfnPort<int> > );
	RegisterInPortPrototype( "translation", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
	RegisterInPortPrototype( "rotation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );
	RegisterInPortPrototype( "transform", new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix> > );
	RegisterInPortPrototype( "linear_velocity", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "angular_velocity", new TVdfnPortTypeCompare<TVdfnPort<float> > );

	// out-ports:
	// - transformation : VistaTransformationMatrix
	RegisterOutPort( "transform", m_pOut );
}

VistaDfnNavigationNode::~VistaDfnNavigationNode()
{

}
 
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaDfnNavigationNode::PrepareEvaluationRun()
{
	m_pTransform = dynamic_cast<TVdfnPort<VistaTransformMatrix>*>(GetInPort("transform"));
	m_pTranslation = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("translation"));
	m_pRotation = dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort("rotation"));
	m_pDeltaTime = dynamic_cast<TVdfnPort<double>*>(GetInPort("dt"));
	m_pNavigationMode = dynamic_cast<TVdfnPort<int>*>(GetInPort("navigation_mode"));
	m_pLinearVelocity = dynamic_cast<TVdfnPort<float>*>(GetInPort("linear_velocity"));
	m_pAngularVelocity = dynamic_cast<TVdfnPort<float>*>(GetInPort("angular_velocity"));
	return GetIsValid();
}

bool VistaDfnNavigationNode::GetIsValid() const
{
	return ( ( m_pTranslation || m_pRotation ) && m_pDeltaTime );
}

bool VistaDfnNavigationNode::DoEvalNode()
{
	int iNavigationMode = ( m_pNavigationMode != NULL ) 
							? m_pNavigationMode->GetValueConstRef()
							: m_iDefaultNavigationMode;
 
	if( m_pDeltaTime->GetUpdateCounter() == m_nLastDTUpateCount )
		return false;

	m_nLastDTUpateCount = m_pDeltaTime->GetUpdateCounter();

	float fDeltaTime = float(m_pDeltaTime->GetValueConstRef());
	if( fDeltaTime < Vista::Epsilon )
		return true;

	float fTranslationFraction = ( m_pLinearVelocity != NULL )
		? m_pLinearVelocity->GetValueConstRef()*fDeltaTime
		: m_fDefaultLinearVelocity*fDeltaTime;

	float fRotationFraction = ( m_pAngularVelocity != NULL )
		? m_pAngularVelocity->GetValueConstRef()*fDeltaTime
		: m_fDefaultAngularVelocity*fDeltaTime;

	VistaTransformMatrix matTransform;
	if( m_pTransform )
		matTransform = m_pTransform->GetValueConstRef();
	
	switch( iNavigationMode )
	{
		case 1:
		{
			// Helicopter Mode: No roll, no pitch, just yaw
			ApplyRotationYawOnly( fRotationFraction, matTransform );
			ApplyTranslation( fTranslationFraction, matTransform );
			break;
		}
		case 2:
		{
			// Camera Mode: No roll, just yaw/pitch
			ApplyRotationNoRoll( fRotationFraction, matTransform );
			ApplyTranslation( fTranslationFraction, matTransform );
			break;
		}
		case 0:
		default:
		{
			// Free Mode: All rotations allowed
			ApplyRotationFull( fRotationFraction, matTransform );
			ApplyTranslation( fTranslationFraction, matTransform );
			break;
		}
	}

	m_pOut->GetValueRef() = matTransform;
	m_pOut->IncUpdateCounter();

	return true;
}

void VistaDfnNavigationNode::ApplyTranslation( 
							const float fTranslationFraction,
							VistaTransformMatrix& matTransform )
{
	if( m_pTranslation == NULL )
		return;

	VistaVector3D v3Change( fTranslationFraction * m_pTranslation->GetValueConstRef() );

	v3Change[3] = 0.0f;
	v3Change = matTransform.Transform( v3Change );

	VistaVector3D v3Trans;
	matTransform.GetTranslation(v3Trans);
	matTransform.SetTranslation(v3Trans+v3Change);
}

void VistaDfnNavigationNode::ApplyRotationFull( 
							const float fRotationFraction,
							VistaTransformMatrix& matTransform )
{
	if( m_pRotation == NULL )
		return;

	const VistaQuaternion& qRotation( m_pRotation->GetValueConstRef() );
		
	// calculate rotation slerp-interpolated by the fraction
	float fDtFrac = fRotationFraction / ( 2 * Vista::Pi );
	fDtFrac = fDtFrac - int(fDtFrac);
	VistaQuaternion qRotDt = VistaQuaternion().Slerp( qRotation, fDtFrac );
	
	VistaVector3D v3Trans;
	matTransform.GetTranslation(v3Trans);
	matTransform.SetTranslation(VistaVector3D(0,0,0));
	matTransform = matTransform * VistaTransformMatrix( qRotDt );
	matTransform.SetTranslation(v3Trans);
}

void VistaDfnNavigationNode::ApplyRotationYawOnly( 
							const float fRotationFraction,
							VistaTransformMatrix& matTransform )
{
	if( m_pRotation == NULL )
		return;

	VistaQuaternion qRot( m_pRotation->GetValue() );
	qRot = VistaQuaternion().Slerp( qRot, fRotationFraction/(Vista::Pi*2.0f) );

	VistaVector3D vY = qRot.Rotate(VistaVector3D(0,0,1));
	vY[Vista::Y] = 0.0f;
	vY.Normalize();
	VistaQuaternion qY = VistaQuaternion( VistaVector3D(0,0,1), vY );

	matTransform = matTransform * VistaTransformMatrix( qY );
}

void VistaDfnNavigationNode::ApplyRotationNoRoll(
							const float fRotationFraction,
							VistaTransformMatrix& matTransform )
{
	if( m_pRotation == NULL )
		return;

	VistaQuaternion qRot( m_pRotation->GetValueConstRef() );
	qRot = VistaQuaternion().Slerp( qRot, fRotationFraction/(Vista::Pi*2.0f) );
	
	VistaVector3D v3OrigTranslation;
	matTransform.GetTranslation( v3OrigTranslation );
	VistaQuaternion qFullRot( matTransform );
	qFullRot = qFullRot * qRot;

	//determine rotation around global y
	VistaVector3D vY = qFullRot.Rotate(VistaVector3D(0,0,1));
	vY[Vista::Y] = 0.0f;
	vY.Normalize();
	VistaQuaternion qY = VistaQuaternion( VistaVector3D(0,0,1), vY );

	//Calculate reduced Rotation
	qFullRot = qY.GetInverted() * qFullRot;

	//determine rotation around global X	
	VistaVector3D vX = qFullRot.Rotate(VistaVector3D(0,0,1));
	vX[Vista::X] = 0.0f;
	vX.Normalize();
	VistaQuaternion qX = VistaQuaternion( VistaVector3D(0,0,1), vX );

	//Prevent Gimbal Lock
	VistaAxisAndAngle aaaX = qX.GetAxisAndAngle();
	if( aaaX.m_fAngle > 0.45f*Vista::Pi )
	{
		aaaX.m_fAngle = 0.45f*Vista::Pi;
		qX = VistaQuaternion( aaaX );
	}
	else if( aaaX.m_fAngle < -0.45f*Vista::Pi )
	{
		aaaX.m_fAngle = -0.45f*Vista::Pi;
		qX = VistaQuaternion( aaaX );
	}

	// Determine Final rotation
	qFullRot = qY * qX;
	matTransform = VistaTransformMatrix( qFullRot, v3OrigTranslation );

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/



