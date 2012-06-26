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

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnNavigationNode::VistaDfnNavigationNode( 
							const int iDefaultNavigationMode,
							const float fDefaultLinearVelocity,
							const float fDefaultAngularVelocity,
							const float fDefaultLinearAcceleration,
							const float fDefaultLinearDecceleration,
							const float fDefaultAngularAcceleration,
							const float fDefaultAngularDecceleration )
: IVdfnNode()
, m_pTransform(NULL)
, m_pOut( new TVdfnPort<VistaTransformMatrix> )
, m_pTranslation( NULL )
, m_pRotation( NULL )
, m_pRotationPivot( NULL )
, m_pDeltaTime( NULL )
, m_pNavigationMode( NULL )
, m_iNavigationMode( iDefaultNavigationMode )
, m_fTargetLinearVelocity( fDefaultLinearVelocity )
, m_fCurrentLinearVelocity( 0 )
, m_fLinearAcceleration( fDefaultLinearDecceleration )
, m_fLinearDecceleration( fDefaultLinearAcceleration )
, m_fTargetAngularVelocity( fDefaultAngularVelocity )
, m_fCurrentAngularVelocity( 0 )
, m_fAngularAcceleration( fDefaultAngularDecceleration )
, m_fAngularDecceleration( fDefaultAngularAcceleration )
, m_nLastDTUpateCount(0)
, m_fDeltaTime( 0 )
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
	RegisterInPortPrototype( "pivot", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
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
	m_pRotationPivot = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("pivot"));
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
	if( m_pDeltaTime->GetUpdateCounter() == m_nLastDTUpateCount )
		return false;
	
	if( m_pNavigationMode )
		m_iNavigationMode = m_pNavigationMode->GetValue();
	if( m_pLinearVelocity )
		m_fTargetLinearVelocity = m_pLinearVelocity->GetValue();
	if( m_pAngularVelocity )
		m_fTargetAngularVelocity = m_pAngularVelocity->GetValue();

	m_nLastDTUpateCount = m_pDeltaTime->GetUpdateCounter();

	// += ensures that we don't loose time on many very small updates
	m_fDeltaTime += float(m_pDeltaTime->GetValueConstRef());
	if( m_fDeltaTime < Vista::Epsilon )
		return true;

	UpdateVelocities();

	VistaTransformMatrix matTransform;
	if( m_pTransform )
		matTransform = m_pTransform->GetValueConstRef();
	
	switch( m_iNavigationMode )
	{
		case 1:
		{
			// Helicopter Mode: No roll, no pitch, just yaw
			ApplyRotationYawOnly( matTransform );
			ApplyTranslation( matTransform );
			break;
		}
		case 2:
		{
			// Camera Mode: No roll, just yaw/pitch
			ApplyRotationNoRoll( matTransform );
			ApplyTranslation( matTransform );
			break;
		}
		case 0:
		default:
		{
			// Free Mode: All rotations allowed
			ApplyRotationFull( matTransform );
			ApplyTranslation( matTransform );
			break;
		}
	}

	m_pOut->GetValueRef() = matTransform;
	m_pOut->IncUpdateCounter();

	m_fDeltaTime = 0;

	return true;
}

void VistaDfnNavigationNode::ApplyTranslation( 
							VistaTransformMatrix& matTransform )
{
	if( m_pTranslation == NULL || m_fCurrentLinearVelocity == 0 )
		return;

	VistaVector3D v3Change = m_fDeltaTime * m_fCurrentLinearVelocity
							* m_pTranslation->GetValueConstRef();

	v3Change = matTransform.TransformVector( v3Change );

	VistaVector3D v3Trans;
	matTransform.GetTranslation( v3Trans );
	matTransform.SetTranslation( v3Trans + v3Change );
}

void VistaDfnNavigationNode::ApplyRotationFull( 
							VistaTransformMatrix& matTransform )
{
	if( m_pRotation == NULL || m_fCurrentAngularVelocity == 0 )
		return;

	const VistaQuaternion& qRotation = m_pRotation->GetValueConstRef();

	VistaVector3D v3Pivot;
	if( m_pRotationPivot )
		v3Pivot = m_pRotationPivot->GetValue();
		
	// calculate rotation slerp-interpolated by the fraction
	float fDtFrac = m_fDeltaTime * m_fCurrentAngularVelocity / ( 2 * Vista::Pi );
	fDtFrac = fDtFrac - int(fDtFrac);
	VistaQuaternion qRotDt = VistaQuaternion().Slerp( qRotation, fDtFrac );
	
	// apply rotation relative to pivot point
	matTransform = matTransform
					* VistaTransformMatrix( v3Pivot )
					* VistaTransformMatrix( qRotDt )
					* VistaTransformMatrix( -v3Pivot );
}

void VistaDfnNavigationNode::ApplyRotationYawOnly( 
							VistaTransformMatrix& matTransform )
{
	if( m_pRotation == NULL )
		return;

	const VistaQuaternion& qRotation = m_pRotation->GetValueConstRef();

	VistaVector3D v3Pivot;
	if( m_pRotationPivot )
		v3Pivot = m_pRotationPivot->GetValue();
		
	// calculate rotation slerp-interpolated by the fraction
	float fDtFrac = m_fDeltaTime * m_fCurrentAngularVelocity / ( 2 * Vista::Pi );
	fDtFrac = fDtFrac - int(fDtFrac);
	VistaQuaternion qRotDt = VistaQuaternion().Slerp( qRotation, fDtFrac );

	VistaVector3D vY = qRotDt.Rotate( Vista::ViewVector );
	vY[Vista::Y] = 0.0f;
	vY.Normalize();
	VistaQuaternion qY = VistaQuaternion( Vista::ViewVector, vY );

	// apply rotation relative to pivot point
	matTransform = matTransform
					* VistaTransformMatrix( v3Pivot )
					* VistaTransformMatrix( qY )
					* VistaTransformMatrix( -v3Pivot );
}

void VistaDfnNavigationNode::ApplyRotationNoRoll(
							VistaTransformMatrix& matTransform )
{
	if( m_pRotation == NULL )
		return;

	const VistaQuaternion& qRotation = m_pRotation->GetValueConstRef();

	VistaVector3D v3Pivot;
	if( m_pRotationPivot )
		v3Pivot = m_pRotationPivot->GetValue();

	// calculate rotation slerp-interpolated by the fraction
	float fDtFrac = m_fDeltaTime * m_fCurrentAngularVelocity / ( 2 * Vista::Pi );
	fDtFrac = fDtFrac - int(fDtFrac);
	VistaQuaternion qRotDt = VistaQuaternion().Slerp( qRotation, fDtFrac );
	
	VistaVector3D v3OrigTranslation;
	matTransform.GetTranslation( v3OrigTranslation );
	VistaQuaternion qFullRot( matTransform );
	qFullRot = qFullRot * qRotDt;

	//determine rotation around global y
	VistaVector3D vY = qFullRot.Rotate( Vista::ViewVector );
	vY[Vista::Y] = 0.0f;
	vY.Normalize();
	VistaQuaternion qY = VistaQuaternion( Vista::ViewVector, vY );

	//Calculate reduced Rotation
	qFullRot = qY.GetInverted() * qFullRot;

	//determine rotation around global X	
	VistaVector3D vX = qFullRot.Rotate( Vista::ViewVector );
	vX[Vista::X] = 0.0f;
	vX.Normalize();
	VistaQuaternion qX = VistaQuaternion( Vista::ViewVector, vX );

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

	// @todo: use pivot in this mode

	// Determine Final rotation
	qFullRot = qY * qX;
	matTransform.SetBasisMatrix( qFullRot );

}

void VistaDfnNavigationNode::UpdateVelocities()
{
	if( m_fCurrentLinearVelocity > m_fTargetLinearVelocity )
	{
		if( m_fLinearDecceleration <= 0 )
			m_fCurrentLinearVelocity = m_fTargetLinearVelocity;
		else
		{
			m_fCurrentLinearVelocity -= m_fDeltaTime * m_fLinearDecceleration;
			if( m_fCurrentLinearVelocity < m_fTargetLinearVelocity )
				m_fCurrentLinearVelocity = m_fTargetLinearVelocity;
		}
	}
	else if( m_fCurrentLinearVelocity < m_fTargetLinearVelocity )
	{
		if( m_fLinearAcceleration <= 0 )
			m_fCurrentLinearVelocity = m_fTargetLinearVelocity;
		else
		{
			m_fCurrentLinearVelocity += m_fDeltaTime * m_fLinearAcceleration;
			if( m_fCurrentLinearVelocity > m_fTargetLinearVelocity )
				m_fCurrentLinearVelocity = m_fTargetLinearVelocity;
		}
	}

	if( m_fCurrentAngularVelocity > m_fTargetAngularVelocity )
	{
		if( m_fAngularDecceleration <= 0 )
			m_fCurrentAngularVelocity = m_fTargetAngularVelocity;
		else
		{
			m_fCurrentAngularVelocity -= m_fDeltaTime * m_fAngularDecceleration;
			if( m_fCurrentAngularVelocity < m_fTargetAngularVelocity )
				m_fCurrentAngularVelocity = m_fTargetAngularVelocity;
		}
	}
	else if( m_fCurrentAngularVelocity < m_fTargetAngularVelocity )
	{
		if( m_fAngularAcceleration <= 0 )
			m_fCurrentAngularVelocity = m_fTargetAngularVelocity;
		else
		{
			m_fCurrentAngularVelocity += m_fDeltaTime * m_fAngularAcceleration;
			if( m_fCurrentAngularVelocity > m_fTargetAngularVelocity )
				m_fCurrentAngularVelocity = m_fTargetAngularVelocity;
		}
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/



