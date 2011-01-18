/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
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

#ifndef _VISTAQUATERNION_H
#define _VISTAQUATERNION_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaBaseConfig.h"

#include "VistaMathBasics.h"
#include "VistaVector3D.h"

#include <cmath>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaTransformMatrix;


/** @todo where to put these? */
struct VISTABASEAPI VistaEulerAngles
{
	VistaEulerAngles ( const float fRoll, const float fPitch, const float fYaw )
	: a( fRoll ), b( fPitch ), c( fYaw )
	{
	}
	float a, b, c;
};

class VISTABASEAPI VistaAxisAndAngle
{
public:
	VistaAxisAndAngle()
	: m_v3Axis( 0, 0, 1 ), m_fAngle(0.0f)
	{
	}

	explicit VistaAxisAndAngle ( const VistaVector3D& v3Axis, const float fAngle )
	: m_v3Axis ( v3Axis ), m_fAngle ( fAngle )
	{
		m_v3Axis.Normalize();
	}

	VistaVector3D m_v3Axis;
	float m_fAngle;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTABASEAPI VistaQuaternion
{	
public:
	VistaQuaternion();
	VistaQuaternion( float fX, float fY, float fZ, float fW );
	explicit VistaQuaternion( const float a4fValues[4] );
	explicit VistaQuaternion( const double a4dValues[4] );
	explicit VistaQuaternion( const VistaEulerAngles& eulRotation );
	explicit VistaQuaternion( const VistaAxisAndAngle& aaaRotation);
	explicit VistaQuaternion( const VistaVector3D& v3From,
							const VistaVector3D& v3To );
	explicit VistaQuaternion( const VistaTransformMatrix& matRotMatrix );

	void SetValues( const float a4fValues[4] );
	void SetValues( const double a4dValues[4] );
	void GetValues( float a4fValues[4] ) const;
	void GetValues( double a4dValues[4] ) const;

	VistaAxisAndAngle GetAxisAndAngle() const;
	VistaEulerAngles GetAngles() const;
	void GetAngles( float& fRoll, float& fPitch, float& fYaw ) const;
	VistaEulerAngles GetAnglesXYZ() const;
	VistaEulerAngles GetAnglesZXY() const;
	VistaEulerAngles GetAnglesZYX() const;

	float Dot( const VistaQuaternion& qRot2 ) const;
	VistaVector3D Rotate( const VistaVector3D& v3Vector ) const;
	VistaQuaternion Slerp( const VistaQuaternion& qEnd, float fFraction ) const;	
	int Compare( const VistaQuaternion& qCompare ) const;

	bool CheckForValidity() const;

	/** @todo: do we need those?, if so, rename */
	//VistaQuaternion Log() const;
	//void Exp( float fX, float fY, float fZ );
	//void Exp( const VistaVector3D& qQuat );

	void Invert();	
	void ComplexConjugate();
	void Normalize();
	void SetToNeutralQuaternion();

	VistaQuaternion GetInverted() const;
	VistaQuaternion GetComplexConjugated() const;
	VistaQuaternion GetNormalized() const;
	float GetLengthSquared() const;
	float GetLength() const;	
	
	const VistaQuaternion& operator+=( const VistaQuaternion& v3Other );
	const VistaQuaternion& operator-=( const VistaQuaternion& v3Other );
	const VistaQuaternion& operator*=( const VistaQuaternion& v3Other );
	const VistaQuaternion& operator*=( const float fScale );
	const VistaQuaternion& operator/=( const float fScale );

	VistaQuaternion operator+( const VistaQuaternion& v3Other ) const;
	VistaQuaternion operator-( const VistaQuaternion& v3Other ) const;
	VistaQuaternion operator*( const VistaQuaternion& v3Other ) const;

	VistaQuaternion operator-() const;
	bool operator==( const VistaQuaternion& v3Other ) const;
	bool operator!=( const VistaQuaternion& v3Other ) const;
	
	const float& operator[]( const int iAxis ) const;
	float& operator[]( const int iAxis );

private:
	float		m_a4fValues[4];
};

inline VistaQuaternion operator*( const float fScale, const VistaQuaternion& qQuat );
inline VistaQuaternion operator*( const VistaQuaternion& qQuat, const float fScale );
inline VistaQuaternion operator/( const VistaQuaternion& qQuat, const float fScale );
inline VistaQuaternion operator*( const VistaVector3D& v3Vec, const VistaQuaternion& qQuat );
inline VistaQuaternion operator*( const VistaQuaternion& qQuat, const VistaVector3D& v3Vec );

inline std::ostream& operator<<( std::ostream& oStream, const VistaQuaternion& qQuaternion );

/*============================================================================*/
/* INLINE IMPLEMENTATIONS                                                     */
/*============================================================================*/

inline VistaQuaternion::VistaQuaternion()
{
	m_a4fValues[0] = 0.0f;
	m_a4fValues[1] = 0.0f;
	m_a4fValues[2] = 0.0f;
	m_a4fValues[3] = 1.0f;
}
inline VistaQuaternion::VistaQuaternion( float fX, float fY, float fZ, float fW )
{
	m_a4fValues[0] = fX;
	m_a4fValues[1] = fY;
	m_a4fValues[2] = fZ;
	m_a4fValues[3] = fW;
}

inline VistaQuaternion::VistaQuaternion( const float a4fValues[4] )
{
	m_a4fValues[0] = a4fValues[0];
	m_a4fValues[1] = a4fValues[1];
	m_a4fValues[2] = a4fValues[2];
	m_a4fValues[3] = a4fValues[3];
}

inline VistaQuaternion::VistaQuaternion( const double a4dValues[4] )
{
	m_a4fValues[0] = (float)a4dValues[0];
	m_a4fValues[1] = (float)a4dValues[1];
	m_a4fValues[2] = (float)a4dValues[2];
	m_a4fValues[3] = (float)a4dValues[3];
}

inline VistaQuaternion::VistaQuaternion( const VistaAxisAndAngle& aaaRotation )
{
	const float fSin = sin( aaaRotation.m_fAngle / 2 );

	m_a4fValues[0] = fSin * aaaRotation.m_v3Axis[0];
	m_a4fValues[1] = fSin * aaaRotation.m_v3Axis[1];
	m_a4fValues[2] = fSin * aaaRotation.m_v3Axis[2];

	m_a4fValues[3] = cos( aaaRotation.m_fAngle / 2 );
}


inline bool VistaQuaternion::CheckForValidity() const
{
	return ( Vista::IsValidNumber( m_a4fValues[0] )
			&& Vista::IsValidNumber( m_a4fValues[1] )
			&& Vista::IsValidNumber( m_a4fValues[2] )
			&& Vista::IsValidNumber( m_a4fValues[3] ) );
}
inline void VistaQuaternion::SetValues( const float a4fValues[4] )
{
	m_a4fValues[0] = a4fValues[0];
	m_a4fValues[1] = a4fValues[1];
	m_a4fValues[2] = a4fValues[2];
	m_a4fValues[3] = a4fValues[3];
}
inline void VistaQuaternion::SetValues( const double a4dValues[4] )
{
	m_a4fValues[0] = (float)a4dValues[0];
	m_a4fValues[1] = (float)a4dValues[1];
	m_a4fValues[2] = (float)a4dValues[2];
	m_a4fValues[3] = (float)a4dValues[3];
}

inline void VistaQuaternion::GetValues( float a4fValues[4] ) const
{
	a4fValues[0] = m_a4fValues[0];
	a4fValues[1] = m_a4fValues[1];
	a4fValues[2] = m_a4fValues[2];
	a4fValues[3] = m_a4fValues[3];
}
inline void VistaQuaternion::GetValues( double a4dValues[4] ) const
{
	a4dValues[0] = (double)m_a4fValues[0];
	a4dValues[1] = (double)m_a4fValues[1];
	a4dValues[2] = (double)m_a4fValues[2];
	a4dValues[3] = (double)m_a4fValues[3];
}

inline VistaAxisAndAngle VistaQuaternion::GetAxisAndAngle() const
{
	const float fLength = sqrt( m_a4fValues[0] * m_a4fValues[0]
								+ m_a4fValues[1] * m_a4fValues[1]
								+ m_a4fValues[2] * m_a4fValues[2] );

	// is it a null vector?
	if ( fLength < Vista::Epsilon )
	{
		// yes -> assume Z axis (safety)
		return VistaAxisAndAngle( VistaVector3D ( 0, 0, 1 ), 0 );		
	}
	else
	{
		// no -> correct operation
		VistaVector3D v3Axis( m_a4fValues[0], m_a4fValues[1], m_a4fValues[2] );
		v3Axis /= fLength;
		return VistaAxisAndAngle ( v3Axis, 2.0f * acos( m_a4fValues[3] ) );		
	}
}

inline float VistaQuaternion::Dot( const VistaQuaternion& qRot2 ) const
{
	return ( m_a4fValues[0] * qRot2[0]
			+ m_a4fValues[1] * qRot2[1]
			+ m_a4fValues[2] * qRot2[2]
			+ m_a4fValues[3] * qRot2[3] );
}

inline VistaVector3D VistaQuaternion::Rotate( const VistaVector3D& v3Vector ) const
{
	VistaQuaternion qReturn = (*this) * v3Vector * GetComplexConjugated();
	return VistaVector3D( qReturn[0], qReturn[1], qReturn[2] );
}

inline int VistaQuaternion::Compare( const VistaQuaternion& qCompare ) const
{
	if( m_a4fValues[3] == 0 && qCompare.m_a4fValues[3] == 0 )
		return 1;
	if( m_a4fValues[0] == qCompare.m_a4fValues[0]
		&& m_a4fValues[1] == qCompare.m_a4fValues[1]
		&& m_a4fValues[2] == qCompare.m_a4fValues[2]
		&& m_a4fValues[3] == qCompare.m_a4fValues[3] )
		return 1;
	if( m_a4fValues[0] == -qCompare.m_a4fValues[0]
		&& m_a4fValues[1] == -qCompare.m_a4fValues[1]
		&& m_a4fValues[2] == -qCompare.m_a4fValues[2]
		&& m_a4fValues[3] == -qCompare.m_a4fValues[3] )
		return -1;
	return 0;
}

inline void VistaQuaternion::Invert()
{
	float fScale = -1.0f / GetLength();
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
}
inline void VistaQuaternion::ComplexConjugate()
{
	m_a4fValues[0] = -m_a4fValues[0];
	m_a4fValues[1] = -m_a4fValues[1];
	m_a4fValues[2] = -m_a4fValues[2];
}

inline void VistaQuaternion::Normalize()
{
	const float fLength = 1.0f / GetLength();
	m_a4fValues[0] *= fLength;
	m_a4fValues[1] *= fLength;
	m_a4fValues[2] *= fLength;
	m_a4fValues[3] *= fLength;
}

inline void VistaQuaternion::SetToNeutralQuaternion()
{
	m_a4fValues[0] = 0.0f;
	m_a4fValues[1] = 0.0f;
	m_a4fValues[2] = 0.0f;
	m_a4fValues[3] = 1.0f;
}


inline VistaQuaternion VistaQuaternion::GetInverted() const
{
	const float fLength = 1.0f / GetLength();
	return VistaQuaternion( -m_a4fValues[0] * fLength,
							-m_a4fValues[1] * fLength,
							-m_a4fValues[2] * fLength,
							m_a4fValues[3] * fLength );
}

inline VistaQuaternion VistaQuaternion::GetComplexConjugated() const
{
	return VistaQuaternion( -m_a4fValues[0],
							-m_a4fValues[1],
							-m_a4fValues[2],
							m_a4fValues[3] );
}

inline VistaQuaternion VistaQuaternion::GetNormalized() const
{
	const float fFactor = 1.0f / GetLength();
	return VistaQuaternion( m_a4fValues[0] * fFactor,
							m_a4fValues[1] * fFactor,
							m_a4fValues[2] * fFactor,
							m_a4fValues[3] * fFactor );
}

inline float VistaQuaternion::GetLengthSquared() const
{
	return ( m_a4fValues[0] * m_a4fValues[0]
			+ m_a4fValues[1] * m_a4fValues[1]
			+ m_a4fValues[2] * m_a4fValues[2]
			+ m_a4fValues[3] * m_a4fValues[3] );
}

inline float VistaQuaternion::GetLength() const
{
	return sqrt( m_a4fValues[0] * m_a4fValues[0]
				+ m_a4fValues[1] * m_a4fValues[1]
				+ m_a4fValues[2] * m_a4fValues[2]
				+ m_a4fValues[3] * m_a4fValues[3] );
}

///// OPERATORS /////

inline const VistaQuaternion& VistaQuaternion::operator+=( const VistaQuaternion& v3Other )
{
	m_a4fValues[0] += v3Other.m_a4fValues[0];
	m_a4fValues[1] += v3Other.m_a4fValues[1];
	m_a4fValues[2] += v3Other.m_a4fValues[2];
	m_a4fValues[3] += v3Other.m_a4fValues[3];
	return (*this);
}

inline const VistaQuaternion& VistaQuaternion::operator-=( const VistaQuaternion& v3Other )
{
	m_a4fValues[0] -= v3Other.m_a4fValues[0];
	m_a4fValues[1] -= v3Other.m_a4fValues[1];
	m_a4fValues[2] -= v3Other.m_a4fValues[2];
	m_a4fValues[3] -= v3Other.m_a4fValues[3];
	return (*this);
}

inline const VistaQuaternion& VistaQuaternion::operator*=( const VistaQuaternion& v3Other )
{
	m_a4fValues[0] = m_a4fValues[1] * v3Other[2] - m_a4fValues[2] * v3Other[1] 
					+ m_a4fValues[3] * v3Other[0] + m_a4fValues[0] * v3Other[3];
	m_a4fValues[1] = m_a4fValues[2] * v3Other[0] - m_a4fValues[0] * v3Other[2]
					+ m_a4fValues[3] * v3Other[1] + m_a4fValues[1] * v3Other[3];
	m_a4fValues[2] =  m_a4fValues[0] * v3Other[1] - m_a4fValues[1] * v3Other[0]
					+ m_a4fValues[3] * v3Other[2] + m_a4fValues[2] * v3Other[3];
	m_a4fValues[3] = - m_a4fValues[0]*v3Other[0] - m_a4fValues[1]*v3Other[1] 
					- m_a4fValues[2]*v3Other[2] + m_a4fValues[3]*v3Other[3];
	return (*this);
}

inline const VistaQuaternion& VistaQuaternion::operator*=( const float fScale )
{
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	m_a4fValues[3] *= fScale;
	return (*this);
}
inline const VistaQuaternion& VistaQuaternion::operator/=( const float fScale )
{
	const float fInvScale = 1.0f / fScale;
	m_a4fValues[0] /= fInvScale;
	m_a4fValues[1] /= fInvScale;
	m_a4fValues[2] /= fInvScale;
	m_a4fValues[3] /= fInvScale;
	return (*this);
}


inline VistaQuaternion VistaQuaternion::operator+( const VistaQuaternion& v3Other ) const
{
	return VistaQuaternion( m_a4fValues[0] + v3Other.m_a4fValues[0],
							m_a4fValues[1] + v3Other.m_a4fValues[1],
							m_a4fValues[2] + v3Other.m_a4fValues[2],
							m_a4fValues[3] + v3Other.m_a4fValues[3] );
}

inline VistaQuaternion VistaQuaternion::operator-( const VistaQuaternion& v3Other ) const
{
	return VistaQuaternion( m_a4fValues[0] - v3Other.m_a4fValues[0],
							m_a4fValues[1] - v3Other.m_a4fValues[1],
							m_a4fValues[2] - v3Other.m_a4fValues[2],
							m_a4fValues[3] - v3Other.m_a4fValues[3] );
}

inline VistaQuaternion VistaQuaternion::operator*( const VistaQuaternion& v3Other ) const
{
	return VistaQuaternion( m_a4fValues[1] * v3Other[2] - m_a4fValues[2] * v3Other[1] 
						+ m_a4fValues[3] * v3Other[0] + m_a4fValues[0] * v3Other[3],
					m_a4fValues[2] * v3Other[0] - m_a4fValues[0] * v3Other[2]
						+ m_a4fValues[3] * v3Other[1] + m_a4fValues[1] * v3Other[3],
					m_a4fValues[0] * v3Other[1] - m_a4fValues[1] * v3Other[0]
						+ m_a4fValues[3] * v3Other[2] + m_a4fValues[2] * v3Other[3],
					- m_a4fValues[0]*v3Other[0] - m_a4fValues[1]*v3Other[1] 
						- m_a4fValues[2]*v3Other[2] + m_a4fValues[3]*v3Other[3] );
}

inline VistaQuaternion VistaQuaternion::operator-() const
{
	return GetInverted();
}

inline bool VistaQuaternion::operator==( const VistaQuaternion& v3Other ) const
{
	//@todo: check for equal components or equal rotation?
	return ( m_a4fValues[0] == v3Other.m_a4fValues[0]
			&& m_a4fValues[1] == v3Other.m_a4fValues[1]
			&& m_a4fValues[2] == v3Other.m_a4fValues[2]
			&& m_a4fValues[3] == v3Other.m_a4fValues[3] );
}
inline bool VistaQuaternion::operator!=( const VistaQuaternion& v3Other ) const
{
	//@todo: check for equal components or equal rotation?
	return !operator==( v3Other );
}


inline const float& VistaQuaternion::operator[]( const int iAxis ) const
{
	return m_a4fValues[iAxis];
}
inline float& VistaQuaternion::operator[]( const int iAxis )
{
	return m_a4fValues[iAxis];
}

VistaQuaternion operator*( const float fScale, const VistaQuaternion& qQuat )
{
	return VistaQuaternion( qQuat[0] * fScale,
							qQuat[1] * fScale,
							qQuat[2] * fScale,
							qQuat[3] * fScale );
}
VistaQuaternion operator*( const VistaQuaternion& qQuat, const float fScale )
{
	return ( fScale * qQuat );
}
VistaQuaternion operator/( const VistaQuaternion& qQuat, const float fScale )
{
	return ( ( 1.0f / fScale ) * qQuat );
}
VistaQuaternion operator*( const VistaVector3D& v3Vec, const VistaQuaternion& qQuat )
{
	// like quaternion rotation, with q2 = ( v3, 0 )
	return VistaQuaternion( v3Vec[0] * qQuat[3] + v3Vec[1] * qQuat[2] - v3Vec[2] * qQuat[1],
							v3Vec[1] * qQuat[3] + v3Vec[2] * qQuat[0] - v3Vec[0] * qQuat[2],
							v3Vec[2] * qQuat[3]	+ v3Vec[0] * qQuat[1] - v3Vec[1] * qQuat[0],
							-v3Vec[0] * qQuat[0] - v3Vec[1] * qQuat[1] - v3Vec[2] * qQuat[2]); 
}
VistaQuaternion operator*( const VistaQuaternion& qQuat, const VistaVector3D& v3Vec )
{
	// like quaternion rotation, with q2 = ( v3, 0 )
	return VistaQuaternion( qQuat[3] * v3Vec[0] + qQuat[1] * v3Vec[2] - qQuat[2] * v3Vec[1],
							qQuat[3] * v3Vec[1] + qQuat[2] * v3Vec[0] - qQuat[0] * v3Vec[2],
							qQuat[3] * v3Vec[2] + qQuat[0] * v3Vec[1] - qQuat[1] * v3Vec[0],
							-qQuat[0] * v3Vec[0] - qQuat[1] * v3Vec[1] - qQuat[2] * v3Vec[2]); 
}


inline std::ostream& operator<<( std::ostream& oStream, 
										const VistaQuaternion& qQuaternion )
{
	const std::streamsize iOldPrecision( oStream.precision( 3 ) );
	const std::ios::fmtflags iOldflags( oStream.flags() );
	// set fix point notation
	oStream.setf( std::ios::fixed | std::ios::showpos );

	// write to the stream
	oStream << "( " << qQuaternion[0] 
			<< ", " << qQuaternion[1]
			<< ", " << qQuaternion[2]
			<< " | " << qQuaternion[3] 
			<< " ) - " 
			<< Vista::DegToRad( acos( qQuaternion[3] ) ) << "�";

	// restore old stream settings
	oStream.precision( iOldPrecision );
	oStream.flags( iOldflags );

	return oStream;
}

#endif //_VISTAQUATERNION_H
