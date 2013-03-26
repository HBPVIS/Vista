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

#include "VistaQuaternion.h" 

#include "VistaTransformMatrix.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaQuaternion::VistaQuaternion( const VistaEulerAngles& eulRotation )
{
	float croll  = cos( 0.5f * eulRotation.a );
	float cpitch = cos( 0.5f * eulRotation.b );
	float cyaw   = cos( 0.5f * eulRotation.c );

	float sroll  = sin( 0.5f * eulRotation.a );
	float spitch = sin( 0.5f * eulRotation.b );
	float syaw   = sin( 0.5f * eulRotation.c );

	float cyaw_cpitch = cyaw * cpitch;
	float syaw_spitch = syaw * spitch;
	float cyaw_spitch = cyaw * spitch;
	float syaw_cpitch = syaw * cpitch;

	m_a4fValues[0] = cyaw_cpitch * sroll - syaw_spitch * croll;
	m_a4fValues[1] = cyaw_spitch * croll + syaw_cpitch * sroll;
	m_a4fValues[2] = syaw_cpitch * croll - cyaw_spitch * sroll;
	m_a4fValues[3] = cyaw_cpitch * croll + syaw_spitch * sroll;
}

VistaQuaternion::VistaQuaternion( const VistaVector3D& v3From,
										const VistaVector3D& v3To )
{
	// one possible rotational axis is perpendicular to [start] and [end] -> axis = start x end
	VistaVector3D v3Axis = v3From.Cross( v3To );
	// compute the scalar product; we need this for angle computation or to check the orientation
	float fDot = v3From * v3To;

	// now check for collinearity or whether start or end equal to zero, ie, axis = 0
	if( v3Axis.GetLengthSquared() < Vista::Epsilon )
	{
		// start and end vectors are collinear or equal 0; no need to rotate or just rotate by PI

		if( fabs( fDot ) < Vista::Epsilon )
		{
			// either start or stop equal 0, they cannot be perpendicular because the cross product = 0
			// Set to unit quaternion
			m_a4fValues[0] = 0.0f;
			m_a4fValues[1] = 0.0f;
			m_a4fValues[2] = 0.0f;
			m_a4fValues[3] = 1.0f;
			return;
		}

		// check if they are pointing in the same direction
		if( fDot > 0 )
		{
			SetToNeutralQuaternion();
			return;
		} 
		else 
		{
			// they are pointing in opposing directions 
			// construct an axis for the rotation by PI
			if( ( fabs( v3From[0] ) < Vista::Epsilon ) && ( fabs( v3From[1] ) < Vista::Epsilon ) )
			{
				// start = [0,0,z], it is obvious that [0,0,z] * [0,1,0] = 0
				// i.e. they are orthogonal
				// furthermore z neq 0, because start neq 0
				v3Axis[0] = 0.0f;
				v3Axis[1] = 1.0f;
				v3Axis[2] = 0.0f;
			}
			else
			{
				// [x,y,z] * [-y,x,0] = -xy + xy + 0 = 0
				v3Axis[0] = -v3From[1];
				v3Axis[1] = v3From[0];
				v3Axis[2] = 0.0f;

				v3Axis.Normalize();
			}
			*this = VistaQuaternion( VistaAxisAndAngle ( v3Axis, Vista::Pi ) );
			return;
		}
	}

	// now we need the angle between [start] and [end]

	float fStart = v3From.GetLength();
	float fEnd = v3To.GetLength();

	// catch values problematic for acos comp.
	float fDenominator = fStart * fEnd; // definitely >= 0
	if( fDenominator < Vista::Epsilon )
	{
		// quite unlikely... should've been caught before
		// This means that start or end equal 0
		SetToNeutralQuaternion();
		return;
	} 

	float fArg = fDot / fDenominator;
	if( fArg > ( 1.0 - Vista::Epsilon ) ) // ie, arg > 1
	{
		// should've been caught before...
		// once again, this means, that no rotation has to be done, 
		// because start and stop are collinear
		SetToNeutralQuaternion();
		return;
	}
	if ( fArg < ( Vista::Epsilon - 1.0 ) ) // ie arg < -1
	{
		// should've been caught before...
		// start and stop are collinear but pointing in opposing directions 
		// however, the axis should be fine (somewhat > 0) therefore just use the constructor
		v3Axis.Normalize();
		*this = VistaQuaternion ( VistaAxisAndAngle ( v3Axis, Vista::Pi ) );
		return;
	}

	// that's it! a really simple computation with 1000 exceptions..
	v3Axis.Normalize();
	*this = VistaQuaternion ( VistaAxisAndAngle ( v3Axis, acos( fArg ) ) );
}

VistaQuaternion::VistaQuaternion( const VistaTransformMatrix& matRotMatrix )
{
	// This algorithm avoids near-zero divides by looking for a large component,
	// first w, then x, y, or z. When the trace is greater than zero,
	// |w| is greater than 1/2, which is as small as a largest component can be.
	// Otherwise, the largest diagonal entry corresponds to the largest of |x|,
	// |y|, or |z|, one of which must be larger than |w|, and at least 1/2. 

	const float tr = matRotMatrix[0][0] + matRotMatrix[1][1] + matRotMatrix[2][2];

	if (tr >= 0.0f) 
	{
		float s = sqrt ( tr + matRotMatrix[3][3]);

		m_a4fValues[3] = s * 0.5f;
		s = 0.5f / s;
		
		m_a4fValues[0] = (matRotMatrix[2][1] - matRotMatrix[1][2]) * s;
		m_a4fValues[1] = (matRotMatrix[0][2] - matRotMatrix[2][0]) * s;
		m_a4fValues[2] = (matRotMatrix[1][0] - matRotMatrix[0][1]) * s;
	} 
	else 
	{
		float  s;
		int    h = 0;
		
		if ( matRotMatrix[1][1] > matRotMatrix[0][0] ) 
			h = 1;
		
		if ( matRotMatrix[2][2] > matRotMatrix[h][h] ) 
			h = 2;

		switch ( h ) 
		{
#ifdef HPUX
			//caseMacro(0,1,2);
			case 0:
			{
				float s = sqrt ( (matRotMatrix[0][0] - (matRotMatrix[1][1]+matRotMatrix[0][0])) + matRotMatrix[3][3] );
				m_a4fValues[0] = s*0.5f;
				s = 0.5f / s;
				m_a4fValues[1] = (matRotMatrix[0][1] + matRotMatrix[1][0]) * s;
				m_a4fValues[2] = (matRotMatrix[2][0] + matRotMatrix[0][2]) * s;
				m_a4fValues[3] = (matRotMatrix[2][1] - matRotMatrix[1][2]) * s;
			}
			break;
			//caseMacro(1,2,0);
			case 1:
			{
				float s = sqrt ( (matRotMatrix[1][1] - (matRotMatrix[2][2]+matRotMatrix[0][0])) + matRotMatrix[3][3] );
				m_a4fValues[1] = s*0.5;
				s = 0.5 / s;
				m_a4fValues[2] = (matRotMatrix[1][2] + matRotMatrix[2][1]) * s;
				m_a4fValues[0] = (matRotMatrix[0][1] + matRotMatrix[1][0]) * s;
				m_a4fValues[3] = (matRotMatrix[0][2] - matRotMatrix[2][0]) * s;
			}
			break;
			//caseMacro(2,0,1);
			case 2:
			{
				float s = sqrt ( (matRotMatrix[2][2] - (matRotMatrix[0][0]+matRotMatrix[1][1])) + matRotMatrix[3][3] );
				m_a4fValues[2] = s*0.5;
				s = 0.5 / s;
				m_a4fValues[0] = (matRotMatrix[2][0] + matRotMatrix[0][2]) * s;
				m_a4fValues[1] = (matRotMatrix[1][2] + matRotMatrix[2][1]) * s;
				m_a4fValues[3] = (matRotMatrix[1][0] - matRotMatrix[0][1]) * s;
			}
			break;
#else
		#define caseMacro(I,J,K) \
				case I:\
				s = sqrt ( (matRotMatrix[I][I] - (matRotMatrix[J][J]+matRotMatrix[K][K])) + matRotMatrix[3][3] );\
				m_a4fValues[I] = s*0.5f;\
				s = 0.5f / s;\
				m_a4fValues[J] = (matRotMatrix[I][J] + matRotMatrix[J][I]) * s;\
				m_a4fValues[K] = (matRotMatrix[K][I] + matRotMatrix[I][K]) * s;\
				m_a4fValues[3] = (matRotMatrix[K][J] - matRotMatrix[J][K]) * s;\
				break

			caseMacro(0,1,2);
			caseMacro(1,2,0);
			caseMacro(2,0,1);
		#undef caseMacro
#endif
		}
	}

	// build a UNIT quaternion
	if ( matRotMatrix[3][3] != 1.0 )
	{
		float n =  sqrt ( matRotMatrix[3][3] );
		
		m_a4fValues[0] /= n;
		m_a4fValues[1] /= n;
		m_a4fValues[2] /= n;
	}
}



VistaEulerAngles VistaQuaternion::GetAngles() const
{
	// this method is based on the approach described in
	// Bourg: Physics for Game Developers, O'Reilly, 2002, pp. 311f
	// the quaternion is converted into parts of a rotation matRotMatrix

	float q00 = m_a4fValues[3] * m_a4fValues[3];
	float q11 = m_a4fValues[0] * m_a4fValues[0];
	float q22 = m_a4fValues[1] * m_a4fValues[1];
	float q33 = m_a4fValues[2] * m_a4fValues[2];

	float r11 = q00 + q11 - q22 - q33;
	float r21 = 2 * ( m_a4fValues[0] * m_a4fValues[1] + m_a4fValues[3] * m_a4fValues[2] );
	float r31 = 2 * ( m_a4fValues[0] * m_a4fValues[2] - m_a4fValues[3] * m_a4fValues[1] );
	float r32 = 2 * ( m_a4fValues[1] * m_a4fValues[2] + m_a4fValues[3] * m_a4fValues[0] );
	float r33 = q00 - q11 - q22 + q33;

	float pitch, roll, yaw;

	float tmp = fabs( r31 );
	if( tmp > 1.0 - Vista::Epsilon )
	{
		float r12 = 2 * ( m_a4fValues[0] * m_a4fValues[1] - m_a4fValues[3] * m_a4fValues[2] );
		float r13 = 2 * ( m_a4fValues[0] * m_a4fValues[2] + m_a4fValues[3] * m_a4fValues[1] );

		roll = 0.0f;
		if( r31 > 0.0 )
			pitch = (float)(- Vista::Pi * 0.5f);
		else
			pitch = (float)(Vista::Pi * 0.5f);
		yaw = (float)atan2( -r12, -r31 * r13 );
	} else {
		roll = (float)atan2( r32, r33 );
		pitch = (float)asin( -r31 );
		yaw = (float)atan2( r21, r11 );
	}

	return VistaEulerAngles( roll, pitch, yaw );
}

void VistaQuaternion::GetAngles( float& fRoll, float& fPitch, float& fYaw ) const
{
	VistaEulerAngles eulAngles = GetAngles();
	fRoll = eulAngles.a;
	fPitch = eulAngles.b;
	fYaw = eulAngles.c;
}


VistaEulerAngles VistaQuaternion::GetAnglesXYZ() const
{
	//// this method is based on the approach described in
	//// David Eberly: Euler Angle Formulas, 1999 http://www.geometrictools.com
	//// the quaternion is converted into parts of a rotation matRotMatrix

	//VistaTransformMatrix matRot(*this);

	//float thetaX, thetaY, thetaZ;

	//thetaZ = (float)asin(-matRot[0][1]);
	//if ( thetaZ <  0.5f * Vista::Pi )
	//{
	//	if ( thetaZ > -0.5f * Vista::Pi )
	//	{
	//		thetaX = (float)atan2( matRot[2][1], matRot[1][1] );
	//		thetaY = (float)atan2( matRot[0][2], matRot[0][0] );
	//	}
	//	else
	//	{
	//		thetaX = -(float)atan2( -matRot[2][0], matRot[2][2] );
	//		thetaY = 0.0f;
	//	}
	//}
	//else
	//{
	//		thetaX = (float)atan2( -matRot[2][0], matRot[2][2] );
	//		thetaY = 0.0f;
	//}

	//return VistaEulerAngles( thetaX, thetaY, thetaZ );

	return VistaEulerAngles( 0, 0, 0 );
}

VistaEulerAngles VistaQuaternion::GetAnglesZXY() const
{
	//// this method is based on the approach described in
	//// David Eberly: Euler Angle Formulas, 1999 http://www.geometrictools.com
	//// the quaternion is converted into parts of a rotation matRotMatrix

	//VistaTransformMatrix matRot(*this);

	//float thetaX, thetaY, thetaZ;

	//thetaX = (float)asin(matRot[2][1]);
	//if ( thetaX <  0.5f * Vista::Pi )
	//{
	//	if ( thetaX > -0.5f * Vista::Pi )
	//	{
	//		thetaZ = (float)atan2( -matRot[0][1], matRot[1][1] );
	//		thetaY = (float)atan2( -matRot[2][0], matRot[2][2] );
	//	}
	//	else
	//	{
	//		thetaZ = -(float)atan2( matRot[0][2], matRot[0][0] );
	//		thetaY = 0.0f;
	//	}
	//}
	//else
	//{
	//		thetaZ = (float)atan2( matRot[0][2], matRot[0][0] );
	//		thetaY = 0.0f;
	//}

	//return VistaEulerAngles( thetaX, thetaY, thetaZ );

	return VistaEulerAngles( 0, 0, 0 );
}

VistaEulerAngles VistaQuaternion::GetAnglesZYX() const
{
	//// this method is based on the approach described in
	//// David Eberly: Euler Angle Formulas, 1999 http://www.geometrictools.com
	//// the quaternion is converted into parts of a rotation matRotMatrix

	//VistaTransformMatrix matRot(*this);

	//float thetaX, thetaY, thetaZ;

	//thetaY = (float)asin(-matRot[2][0]);
	//if ( thetaY <  0.5f * Vista::Pi )
	//{
	//	if ( thetaY > -0.5f * Vista::Pi )
	//	{
	//		thetaZ = (float)atan2( matRot[1][0], matRot[0][0] );
	//		thetaX = (float)atan2( matRot[2][1], matRot[2][2] );
	//	}
	//	else
	//	{
	//		thetaZ = -(float)atan2( -matRot[0][1], matRot[0][2] );
	//		thetaX = 0.0f;
	//	}
	//}
	//else
	//{
	//		thetaZ = (float)atan2( -matRot[0][1], matRot[0][2] );
	//		thetaX = 0.0f;
	//}

	//return VistaEulerAngles( thetaX, thetaY, thetaZ );
	return VistaEulerAngles( 0, 0, 0 );
}


VistaQuaternion VistaQuaternion::Slerp( const VistaQuaternion& qEnd, float fFraction ) const
{
	// REQUIREMENTS
	// |this| == 1.0 and |qEnd| == 1.0 and 0.0 <= fParam <= 1.0

	float fDot = Dot( qEnd );

	if( fabs( fDot ) > (1.0f - Vista::Epsilon) )
		return *this;

	double theta, stheta;
	float p1, p2;

	if( fDot > -Vista::Epsilon ) // >= 0.0
	{
		theta = acos( fDot );
		stheta = sin( theta );
		p1 = (float)(sin( (1.0 - fFraction) * theta ) / stheta);
		p2 = (float)(sin( fFraction         * theta ) / stheta);
		
		// this may look weird, but it's twice faster than: (*this * p1) + (qEnd * p2)
		return VistaQuaternion( p1*m_a4fValues[0]+p2*qEnd[0], p1*m_a4fValues[1]+p2*qEnd[1], p1*m_a4fValues[2]+p2*qEnd[2], p1*m_a4fValues[3]+p2*qEnd[3] );
	} 

	// qEnd is more proximate to -this; therefore invert qEnd
	// this can be done because qEnd and -qEnd represent the same rotation..
	theta = acos( -fDot );
	stheta = sin( theta );
	p1 = (float)(sin( (1.0 - fFraction) * theta ) / stheta);
	p2 = (float)(sin( fFraction         * theta ) / stheta);

	return VistaQuaternion( p1*m_a4fValues[0]-p2*qEnd[0], p1*m_a4fValues[1]-p2*qEnd[1], p1*m_a4fValues[2]-p2*qEnd[2], p1*m_a4fValues[3]-p2*qEnd[3] );
}

VistaVector3D VistaQuaternion::GetViewDir() const
{
	return Rotate( Vista::ViewVector );
}

VistaVector3D VistaQuaternion::GetUpDir() const
{
	return Rotate( Vista::UpVector );
}

void VistaQuaternion::SetFromViewAndUpDir( const VistaVector3D& v3View, const VistaVector3D& v3Up )
{
	VistaVector3D v3ZAxis = -v3View;
	VistaVector3D v3XAxis = v3Up.Cross( v3ZAxis );
	VistaTransformMatrix matTransform( v3XAxis, v3Up, v3ZAxis );
	(*this) = matTransform.GetRotationAsQuaternion();
}

