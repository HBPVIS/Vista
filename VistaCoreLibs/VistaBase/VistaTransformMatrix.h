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

#ifndef _VISTATRANSFORMMATRIX_H
#define _VISTATRANSFORMMATRIX_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaBaseConfig.h"

#include "VistaMathBasics.h"
#include "VistaVector3D.h"
#include "VistaQuaternion.h"

#include <cmath>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTABASEAPI VistaTransformMatrix
{
public:
	VistaTransformMatrix();
	VistaTransformMatrix(
		const float fVal00, const float fVal01, const float fVal02, const float fVal03,
		const float fVal10, const float fVal11, const float fVal12, const float fVal13,
		const float fVal20, const float fVal21, const float fVal22, const float fVal23,
		const float fVal30, const float fVal31, const float fVal32, const float fVal33 );
	explicit VistaTransformMatrix( const float a4fRow1[4],
									const float a4fRow2[4],
									const float a4fRow3[4],
									const float a4fRow4[4] );
	explicit VistaTransformMatrix( const float a4x4fMatrix[4][4] );
	explicit VistaTransformMatrix( const float a16fValues[16],
									const bool bColumnMajor = false );
	explicit VistaTransformMatrix( const double a4x4dMatrix[4][4] );
	explicit VistaTransformMatrix( const double a16dValues[16],
									const bool bColumnMajor = false );
	explicit VistaTransformMatrix( const VistaQuaternion& qRotation,
									const VistaVector3D& v3Translation = VistaVector3D( 0,0,0 ) );
	explicit VistaTransformMatrix( const VistaAxisAndAngle& aaaRotation );
	explicit VistaTransformMatrix( const VistaVector3D& v3Translation );
	explicit VistaTransformMatrix( const VistaVector3D& v3XAxis,
									const VistaVector3D& v3YAxis,
									const VistaVector3D& v3ZAxis );
	VistaTransformMatrix( const float fScale );

	const float* operator[]( const int iRowIndex ) const;
	float* operator[]( const int iRowIndex );

	void GetRow( const int iRowIndex, float a4fRow[4] ) const;
	void GetRow( const int iRowIndex, double a4dRow[4] ) const;
	VistaVector3D GetRow( const int iColumnIndex ) const;
	void SetRow( const int iRowIndex, const float a4fRow[4] );
	void SetRow( const int iRowIndex, const double a4dRow[4] );
	void SetRow( const int iRowIndex, const VistaVector3D& v3Row );

	void GetColumn( const int iColumnIndex, float a4fColumn[4] ) const;
	void GetColumn( const int iColumnIndex, double a4dColumn[4] ) const;
	VistaVector3D GetColumn( const int iColumnIndex ) const;
	void SetColumn( const int iColumnIndex, const float a4fColumn[4] );
	void SetColumn( const int iColumnIndex, const double a4dColumn[4] );
	void SetColumn( const int iColumnIndex, const VistaVector3D& v3Column );

	float GetValue( const int iRowIndex, const int iColumnIndex ) const;
	void GetValues( float a4x4fMatrix[4][4] ) const;
	void GetValues( float a16fMatrix[16] ) const;
	void GetTransposedValues( float a4x4fMatrix[4][4] ) const;
	void GetTransposedValues( float a16fMatrix[16] ) const;
	void GetValues( double a4x4dMatrix[4][4] ) const;
	void GetValues( double a16dMatrix[16] ) const;
	void GetTransposedValues( double a4x4dMatrix[4][4] ) const;
	void GetTransposedValues( double a16dMatrix[16] ) const;

	void SetValue( const int iRowIndex, const int iColumnIndex, const float fValue );
	void SetValues( const float a4x4fMatrix[4][4] );
	void SetValues( const float a16fMatrix[16] );
	void SetTransposedValues( const float a4x4fMatrix[4][4] );
	void SetTransposedValues( const float a16fMatrix[16] );
	void SetValues( const double a4x4dMatrix[4][4] );
	void SetValues( const double a16dMatrix[16] );
	void SetTransposedValues( const double a4x4dMatrix[4][4] );
	void SetTransposedValues( const double a16dMatrix[16] );

	VistaVector3D GetTranslation() const;
	void GetTranslation( VistaVector3D &v3Translation ) const;
	void GetBasisMatrix( float a3x3Matrix[3][3] ) const;
	void GetBasisMatrix( float a9fMatrix[9], const bool bTransposed = false ) const;
	VistaQuaternion GetRotationAsQuaternion() const;

	void SetTranslation( const float a3fTranslation[3] );
	void SetTranslation( const VistaVector3D& v3Translation );
	void SetBasisMatrix( const float a3x3Matrix[3][3] );
	void SetBasisMatrix( const float a9fMatrix[9] );
	void SetBasisMatrix( const VistaQuaternion& qRotation );

	void SetToIdentity();
	void SetToTranslationMatrix( const VistaVector3D& v3Translation );
	void SetTransform( const VistaQuaternion& qRotation,
		const VistaVector3D& v3Translation = VistaVector3D( 0,0,0,1 ) );
	void SetToRotationMatrix( const VistaQuaternion& qRotation );
	void SetToScaleMatrix( const float fUniformScale );
	void SetToScaleMatrix( const float fXScale,
							const float fYScale,
							const float fZScale );


	/**
	 * Decomposes the matrix into four components: the translation, rotation,
	 * as well as the scale in a local scale rotation
	 */
	bool Decompose( VistaVector3D& v3Translation,
					VistaQuaternion& qRotation,
					VistaVector3D& v3Scale,
					VistaQuaternion& qScaleRotation ) const;
	/**
	 * Composes a matrix from the four components: the translation, rotation,
	 * as well as the scale in a local scale rotation
	 */
	bool Compose( const VistaVector3D& v3Translation,
					const VistaQuaternion& qRotation,
					const VistaVector3D& v3Scale,
					const VistaQuaternion& qScaleRotation );
	/**
	 * Decomposes the matrix into three components: the translation, rotation,
	 * as well as the scale in a local scale rotation.
	 * Decomposition will fail if the matrix contains a rotated scaling component,
	 * i.e. if it is sheared!
	 */
	bool Decompose( VistaVector3D& v3Translation,
					VistaQuaternion& qRotation,
					VistaVector3D& v3Scale ) const;
	/**
	 * Composes a matrix from the three components: the translation, rotation,
	 * as well as the scale 
	 */
	bool Compose( const VistaVector3D& v3Translation,
					const VistaQuaternion& qRotation,
					const VistaVector3D& v3Scale  );

	float GetAdjunct( const int iRow, const int iColumn ) const;
	float GetDeterminant() const;

	VistaTransformMatrix GetInverted() const;
	bool GetInverted( VistaTransformMatrix& matTarget ) const;

	VistaTransformMatrix GetTranspose() const;
	void GetTranspose( VistaTransformMatrix &target ) const;

	void Transpose();
	void Invert();

	bool CheckForValidity() const;

	VistaVector3D Transform( const VistaVector3D& v3Vector ) const;
	VistaVector3D TransformPoint( const VistaVector3D& v3Point ) const;
	VistaVector3D TransformVector( const VistaVector3D& v3Vector ) const;
	VistaQuaternion Transform( const VistaQuaternion& qRotation ) const;
	VistaTransformMatrix Transform( const VistaTransformMatrix& matTransform ) const;

	VistaTransformMatrix& operator+=( const VistaTransformMatrix& matOther );
	VistaTransformMatrix& operator-=( const VistaTransformMatrix& matOther );
	VistaTransformMatrix& operator*=( const VistaTransformMatrix& matOther );

	bool operator==( const VistaTransformMatrix& matOther ) const;
	bool operator!=( const VistaTransformMatrix& matOther ) const;

private:
	float m_a4x4fMatrix[4][4];
};

// global operators
inline VistaVector3D operator*( const VistaTransformMatrix& matTransform,
										const VistaVector3D& v3Point );
inline VistaQuaternion operator*( const VistaTransformMatrix& matTransform,
										const VistaQuaternion& qRotation );
inline VistaTransformMatrix operator*( const VistaTransformMatrix& matTransform1,
										  const VistaTransformMatrix& matTransform2 );
inline VistaTransformMatrix operator+( const VistaTransformMatrix& matTransform1,
										  const VistaTransformMatrix& matTransform2 );
inline VistaTransformMatrix operator-( const VistaTransformMatrix& matTransform1,
										  const VistaTransformMatrix& matTransform2 );

inline VistaTransformMatrix operator*( const float fScale,
										  const VistaTransformMatrix& matTransform );
inline VistaTransformMatrix operator*( const VistaTransformMatrix& matTransform,
												const float fScale );
inline VistaTransformMatrix operator/( const VistaTransformMatrix& matTransform,
												const float fScale );

// outstream operator
inline std::ostream& operator<<( std::ostream& oStream, const VistaTransformMatrix& matTransform );

// constant VistaTransformMatrices
namespace Vista
{
	const VistaTransformMatrix UnitMatrix;
}

/*============================================================================*/
/* INLINE IMPLEMENTATIONS */
/*============================================================================*/

inline VistaTransformMatrix::VistaTransformMatrix()
{
	SetToIdentity();
}

inline VistaTransformMatrix::VistaTransformMatrix(
	const float fVal00, const float fVal01, const float fVal02, const float fVal03,
	const float fVal10, const float fVal11, const float fVal12, const float fVal13,
	const float fVal20, const float fVal21, const float fVal22, const float fVal23,
	const float fVal30, const float fVal31, const float fVal32, const float fVal33 )
{
	m_a4x4fMatrix[0][0] = fVal00;
	m_a4x4fMatrix[0][1] = fVal01;
	m_a4x4fMatrix[0][2] = fVal02;
	m_a4x4fMatrix[0][3] = fVal03;
	m_a4x4fMatrix[1][0] = fVal10;
	m_a4x4fMatrix[1][1] = fVal11;
	m_a4x4fMatrix[1][2] = fVal12;
	m_a4x4fMatrix[1][3] = fVal13;
	m_a4x4fMatrix[2][0] = fVal20;
	m_a4x4fMatrix[2][1] = fVal21;
	m_a4x4fMatrix[2][2] = fVal22;
	m_a4x4fMatrix[2][3] = fVal23;
	m_a4x4fMatrix[3][0] = fVal30;
	m_a4x4fMatrix[3][1] = fVal31;
	m_a4x4fMatrix[3][2] = fVal32;
	m_a4x4fMatrix[3][3] = fVal33;
}

inline VistaTransformMatrix::VistaTransformMatrix( const float a4fRow0[4],
												  const float a4fRow1[4],
												  const float a4fRow2[4],
												  const float a4fRow3[4] )
{
	SetRow( 0, a4fRow0 );
	SetRow( 1, a4fRow1 );
	SetRow( 2, a4fRow2 );
	SetRow( 3, a4fRow3 );
}

inline VistaTransformMatrix::VistaTransformMatrix ( const float a4x4fMatrix[4][4] )
{
	SetValues( a4x4fMatrix );
}

inline VistaTransformMatrix::VistaTransformMatrix ( const float a16fValues[16],
												   const bool bColumnMajor )
{
	if( !bColumnMajor )
		SetValues( a16fValues );
	else
		SetTransposedValues( a16fValues );
}

inline VistaTransformMatrix::VistaTransformMatrix ( const double a4x4dMatrix[4][4] )
{
	SetValues( a4x4dMatrix );
}

inline VistaTransformMatrix::VistaTransformMatrix ( const double a16dValues[16],
												   const bool bColumnMajor )
{
	if( !bColumnMajor )
		SetValues( a16dValues );
	else
		SetTransposedValues( a16dValues );
}


inline VistaTransformMatrix::VistaTransformMatrix ( const VistaQuaternion& qRotation,
												   const VistaVector3D& v3Translation )
{
	SetBasisMatrix( qRotation );
	SetTranslation( v3Translation );
	m_a4x4fMatrix[3][0] = 0.0f;
	m_a4x4fMatrix[3][1] = 0.0f;
	m_a4x4fMatrix[3][2] = 0.0f;
	m_a4x4fMatrix[3][3] = 1.0f;
}

inline VistaTransformMatrix::VistaTransformMatrix( const VistaAxisAndAngle& aaaRotation )
{
	SetToRotationMatrix( VistaQuaternion( aaaRotation ) );
}

inline VistaTransformMatrix::VistaTransformMatrix( const VistaVector3D& v3Translation )
{
	SetToTranslationMatrix( v3Translation );
}

inline VistaTransformMatrix::VistaTransformMatrix( const float fScale )
{
	SetToScaleMatrix( fScale );
}


inline const float* VistaTransformMatrix::operator[]( const int iRowIndex ) const
{
	return m_a4x4fMatrix[iRowIndex];
}

inline float* VistaTransformMatrix::operator[]( const int iRowIndex )
{
	return m_a4x4fMatrix[iRowIndex];
}

inline bool VistaTransformMatrix::CheckForValidity() const
{
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			if( Vista::IsValidNumber( m_a4x4fMatrix[i][j] ) == false )
				return false;
		}
	}
	return true;
}
inline void VistaTransformMatrix::GetRow( const int iRowIndex, float a4fRow[4] ) const
{
	a4fRow[0] = m_a4x4fMatrix[iRowIndex][0];
	a4fRow[1] = m_a4x4fMatrix[iRowIndex][1];
	a4fRow[2] = m_a4x4fMatrix[iRowIndex][2];
	a4fRow[3] = m_a4x4fMatrix[iRowIndex][3];
}
inline void VistaTransformMatrix::GetRow( const int iRowIndex, double a4dRow[4] ) const
{
	a4dRow[0] = (double)m_a4x4fMatrix[iRowIndex][0];
	a4dRow[1] = (double)m_a4x4fMatrix[iRowIndex][1];
	a4dRow[2] = (double)m_a4x4fMatrix[iRowIndex][2];
	a4dRow[3] = (double)m_a4x4fMatrix[iRowIndex][3];
}

inline VistaVector3D VistaTransformMatrix::GetRow( const int iRowIndex ) const
{
	return VistaVector3D( m_a4x4fMatrix[iRowIndex][0],
							m_a4x4fMatrix[iRowIndex][1],
							m_a4x4fMatrix[iRowIndex][2],
							m_a4x4fMatrix[iRowIndex][3] );
}


inline void VistaTransformMatrix::SetRow( const int iRowIndex, const VistaVector3D& v3Row )
{
	m_a4x4fMatrix[iRowIndex][0] = v3Row[0];
	m_a4x4fMatrix[iRowIndex][1] = v3Row[1];
	m_a4x4fMatrix[iRowIndex][2] = v3Row[2];
	m_a4x4fMatrix[iRowIndex][3] = v3Row[3];
}

inline void VistaTransformMatrix::SetRow( const int iRowIndex, const float a4fRow[4] )
{
	m_a4x4fMatrix[iRowIndex][0] = a4fRow[0];
	m_a4x4fMatrix[iRowIndex][1] = a4fRow[1];
	m_a4x4fMatrix[iRowIndex][2] = a4fRow[2];
	m_a4x4fMatrix[iRowIndex][3] = a4fRow[3];
}
inline void VistaTransformMatrix::SetRow( const int iRowIndex, const double a4dRow[4] )
{
	m_a4x4fMatrix[iRowIndex][0] = (float)a4dRow[0];
	m_a4x4fMatrix[iRowIndex][1] = (float)a4dRow[1];
	m_a4x4fMatrix[iRowIndex][2] = (float)a4dRow[2];
	m_a4x4fMatrix[iRowIndex][3] =(float) a4dRow[3];
}

inline void VistaTransformMatrix::GetColumn( const int iColumnIndex, float a4fColumn[4] ) const
{
	a4fColumn[0] = m_a4x4fMatrix[0][iColumnIndex];
	a4fColumn[1] = m_a4x4fMatrix[1][iColumnIndex];
	a4fColumn[2] = m_a4x4fMatrix[2][iColumnIndex];
	a4fColumn[3] = m_a4x4fMatrix[3][iColumnIndex];
}
inline void VistaTransformMatrix::GetColumn( const int iColumnIndex, double a4dColumn[4] ) const
{
	a4dColumn[0] = (double)m_a4x4fMatrix[0][iColumnIndex];
	a4dColumn[1] = (double)m_a4x4fMatrix[1][iColumnIndex];
	a4dColumn[2] = (double)m_a4x4fMatrix[2][iColumnIndex];
	a4dColumn[3] = (double)m_a4x4fMatrix[3][iColumnIndex];
}

inline VistaVector3D VistaTransformMatrix::GetColumn( const int iColumnIndex ) const
{
	return VistaVector3D( m_a4x4fMatrix[0][iColumnIndex],
							m_a4x4fMatrix[1][iColumnIndex],
							m_a4x4fMatrix[2][iColumnIndex],
							m_a4x4fMatrix[3][iColumnIndex] );
}

inline void VistaTransformMatrix::SetColumn( const int iColumnIndex, const float a4fColumn[4] )
{
	m_a4x4fMatrix[0][iColumnIndex] = a4fColumn[0];
	m_a4x4fMatrix[1][iColumnIndex] = a4fColumn[1];
	m_a4x4fMatrix[2][iColumnIndex] = a4fColumn[2];
	m_a4x4fMatrix[3][iColumnIndex] = a4fColumn[3];
}
inline void VistaTransformMatrix::SetColumn( const int iColumnIndex, const double a4dColumn[4] )
{
	m_a4x4fMatrix[0][iColumnIndex] = (float)a4dColumn[0];
	m_a4x4fMatrix[1][iColumnIndex] = (float)a4dColumn[1];
	m_a4x4fMatrix[2][iColumnIndex] = (float)a4dColumn[2];
	m_a4x4fMatrix[3][iColumnIndex] = (float)a4dColumn[3];
}

inline void VistaTransformMatrix::SetColumn( const int iColumnIndex, const VistaVector3D& v3Column )
{
	m_a4x4fMatrix[0][iColumnIndex] = v3Column[0];
	m_a4x4fMatrix[1][iColumnIndex] = v3Column[1];
	m_a4x4fMatrix[2][iColumnIndex] = v3Column[2];
	m_a4x4fMatrix[3][iColumnIndex] = v3Column[3];
}


inline float VistaTransformMatrix::GetValue( const int iRowIndex, const int iColumnIndex ) const
{
	return m_a4x4fMatrix[iRowIndex][iColumnIndex];
}

inline void VistaTransformMatrix::SetValue( const int iRowIndex, const int iColumnIndex, const float fValue )
{
	m_a4x4fMatrix[iRowIndex][iColumnIndex] = fValue;
}

inline void VistaTransformMatrix::SetValues( const float a4x4fMatrix[4][4] )
{
	m_a4x4fMatrix[0][0] = a4x4fMatrix[0][0];
	m_a4x4fMatrix[0][1] = a4x4fMatrix[0][1];
	m_a4x4fMatrix[0][2] = a4x4fMatrix[0][2];
	m_a4x4fMatrix[0][3] = a4x4fMatrix[0][3];
	m_a4x4fMatrix[1][0] = a4x4fMatrix[1][0];
	m_a4x4fMatrix[1][1] = a4x4fMatrix[1][1];
	m_a4x4fMatrix[1][2] = a4x4fMatrix[1][2];
	m_a4x4fMatrix[1][3] = a4x4fMatrix[1][3];
	m_a4x4fMatrix[2][0] = a4x4fMatrix[2][0];
	m_a4x4fMatrix[2][1] = a4x4fMatrix[2][1];
	m_a4x4fMatrix[2][2] = a4x4fMatrix[2][2];
	m_a4x4fMatrix[2][3] = a4x4fMatrix[2][3];
	m_a4x4fMatrix[3][0] = a4x4fMatrix[3][0];
	m_a4x4fMatrix[3][1] = a4x4fMatrix[3][1];
	m_a4x4fMatrix[3][2] = a4x4fMatrix[3][2];
	m_a4x4fMatrix[3][3] = a4x4fMatrix[3][3];
}
inline void VistaTransformMatrix::SetValues( const float a16fMatrix[16] )
{
	m_a4x4fMatrix[0][0] = a16fMatrix[0];
	m_a4x4fMatrix[0][1] = a16fMatrix[1];
	m_a4x4fMatrix[0][2] = a16fMatrix[2];
	m_a4x4fMatrix[0][3] = a16fMatrix[3];
	m_a4x4fMatrix[1][0] = a16fMatrix[4];
	m_a4x4fMatrix[1][1] = a16fMatrix[5];
	m_a4x4fMatrix[1][2] = a16fMatrix[6];
	m_a4x4fMatrix[1][3] = a16fMatrix[7];
	m_a4x4fMatrix[2][0] = a16fMatrix[8];
	m_a4x4fMatrix[2][1] = a16fMatrix[9];
	m_a4x4fMatrix[2][2] = a16fMatrix[10];
	m_a4x4fMatrix[2][3] = a16fMatrix[11];
	m_a4x4fMatrix[3][0] = a16fMatrix[12];
	m_a4x4fMatrix[3][1] = a16fMatrix[13];
	m_a4x4fMatrix[3][2] = a16fMatrix[14];
	m_a4x4fMatrix[3][3] = a16fMatrix[15];
}
inline void VistaTransformMatrix::SetTransposedValues( const float a4x4fMatrix[4][4] )
{	
	m_a4x4fMatrix[0][0] = a4x4fMatrix[0][0];
	m_a4x4fMatrix[0][1] = a4x4fMatrix[1][0];
	m_a4x4fMatrix[0][2] = a4x4fMatrix[2][0];
	m_a4x4fMatrix[0][3] = a4x4fMatrix[3][0];
	m_a4x4fMatrix[1][0] = a4x4fMatrix[0][1];
	m_a4x4fMatrix[1][1] = a4x4fMatrix[1][1];
	m_a4x4fMatrix[1][2] = a4x4fMatrix[2][1];
	m_a4x4fMatrix[1][3] = a4x4fMatrix[3][1];
	m_a4x4fMatrix[2][0] = a4x4fMatrix[0][2];
	m_a4x4fMatrix[2][1] = a4x4fMatrix[1][2];
	m_a4x4fMatrix[2][2] = a4x4fMatrix[2][2];
	m_a4x4fMatrix[2][3] = a4x4fMatrix[3][2];
	m_a4x4fMatrix[3][0] = a4x4fMatrix[0][3];
	m_a4x4fMatrix[3][1] = a4x4fMatrix[1][3];
	m_a4x4fMatrix[3][2] = a4x4fMatrix[2][3];
	m_a4x4fMatrix[3][3] = a4x4fMatrix[3][3];	
}
inline void VistaTransformMatrix::SetTransposedValues( const float a16fMatrix[16] )
{
	m_a4x4fMatrix[0][0] = a16fMatrix[0];
	m_a4x4fMatrix[0][1] = a16fMatrix[4];
	m_a4x4fMatrix[0][2] = a16fMatrix[8];
	m_a4x4fMatrix[0][3] = a16fMatrix[12];
	m_a4x4fMatrix[1][0] = a16fMatrix[1];
	m_a4x4fMatrix[1][1] = a16fMatrix[5];
	m_a4x4fMatrix[1][2] = a16fMatrix[9];
	m_a4x4fMatrix[1][3] = a16fMatrix[13];
	m_a4x4fMatrix[2][0] = a16fMatrix[2];
	m_a4x4fMatrix[2][1] = a16fMatrix[6];
	m_a4x4fMatrix[2][2] = a16fMatrix[10];
	m_a4x4fMatrix[2][3] = a16fMatrix[14];
	m_a4x4fMatrix[3][0] = a16fMatrix[3];
	m_a4x4fMatrix[3][1] = a16fMatrix[7];
	m_a4x4fMatrix[3][2] = a16fMatrix[11];
	m_a4x4fMatrix[3][3] = a16fMatrix[15];
}


inline void VistaTransformMatrix::SetValues( const double a4x4dMatrix[4][4] )
{
	m_a4x4fMatrix[0][0] = (float)a4x4dMatrix[0][0];
	m_a4x4fMatrix[0][1] = (float)a4x4dMatrix[0][1];
	m_a4x4fMatrix[0][2] = (float)a4x4dMatrix[0][2];
	m_a4x4fMatrix[0][3] = (float)a4x4dMatrix[0][3];
	m_a4x4fMatrix[1][0] = (float)a4x4dMatrix[1][0];
	m_a4x4fMatrix[1][1] = (float)a4x4dMatrix[1][1];
	m_a4x4fMatrix[1][2] = (float)a4x4dMatrix[1][2];
	m_a4x4fMatrix[1][3] = (float)a4x4dMatrix[1][3];
	m_a4x4fMatrix[2][0] = (float)a4x4dMatrix[2][0];
	m_a4x4fMatrix[2][1] = (float)a4x4dMatrix[2][1];
	m_a4x4fMatrix[2][2] = (float)a4x4dMatrix[2][2];
	m_a4x4fMatrix[2][3] = (float)a4x4dMatrix[2][3];
	m_a4x4fMatrix[3][0] = (float)a4x4dMatrix[3][0];
	m_a4x4fMatrix[3][1] = (float)a4x4dMatrix[3][1];
	m_a4x4fMatrix[3][2] = (float)a4x4dMatrix[3][2];
	m_a4x4fMatrix[3][3] = (float)a4x4dMatrix[3][3];
}
inline void VistaTransformMatrix::SetValues( const double a16dMatrix[16] )
{
	m_a4x4fMatrix[0][0] = (float)a16dMatrix[0];
	m_a4x4fMatrix[0][1] = (float)a16dMatrix[1];
	m_a4x4fMatrix[0][2] = (float)a16dMatrix[2];
	m_a4x4fMatrix[0][3] = (float)a16dMatrix[3];
	m_a4x4fMatrix[1][0] = (float)a16dMatrix[4];
	m_a4x4fMatrix[1][1] = (float)a16dMatrix[5];
	m_a4x4fMatrix[1][2] = (float)a16dMatrix[6];
	m_a4x4fMatrix[1][3] = (float)a16dMatrix[7];
	m_a4x4fMatrix[2][0] = (float)a16dMatrix[8];
	m_a4x4fMatrix[2][1] = (float)a16dMatrix[9];
	m_a4x4fMatrix[2][2] = (float)a16dMatrix[10];
	m_a4x4fMatrix[2][3] = (float)a16dMatrix[11];
	m_a4x4fMatrix[3][0] = (float)a16dMatrix[12];
	m_a4x4fMatrix[3][1] = (float)a16dMatrix[13];
	m_a4x4fMatrix[3][2] = (float)a16dMatrix[14];
	m_a4x4fMatrix[3][3] = (float)a16dMatrix[15];
}
inline void VistaTransformMatrix::SetTransposedValues( const double a4x4dMatrix[4][4] )
{	
	m_a4x4fMatrix[0][0] = (float)a4x4dMatrix[0][0];
	m_a4x4fMatrix[0][1] = (float)a4x4dMatrix[1][0];
	m_a4x4fMatrix[0][2] = (float)a4x4dMatrix[2][0];
	m_a4x4fMatrix[0][3] = (float)a4x4dMatrix[3][0];
	m_a4x4fMatrix[1][0] = (float)a4x4dMatrix[0][1];
	m_a4x4fMatrix[1][1] = (float)a4x4dMatrix[1][1];
	m_a4x4fMatrix[1][2] = (float)a4x4dMatrix[2][1];
	m_a4x4fMatrix[1][3] = (float)a4x4dMatrix[3][1];
	m_a4x4fMatrix[2][0] = (float)a4x4dMatrix[0][2];
	m_a4x4fMatrix[2][1] = (float)a4x4dMatrix[1][2];
	m_a4x4fMatrix[2][2] = (float)a4x4dMatrix[2][2];
	m_a4x4fMatrix[2][3] = (float)a4x4dMatrix[3][2];
	m_a4x4fMatrix[3][0] = (float)a4x4dMatrix[0][3];
	m_a4x4fMatrix[3][1] = (float)a4x4dMatrix[1][3];
	m_a4x4fMatrix[3][2] = (float)a4x4dMatrix[2][3];
	m_a4x4fMatrix[3][3] = (float)a4x4dMatrix[3][3];	
}
inline void VistaTransformMatrix::SetTransposedValues( const double a16dMatrix[16] )
{
	m_a4x4fMatrix[0][0] = (float)a16dMatrix[0];
	m_a4x4fMatrix[0][1] = (float)a16dMatrix[4];
	m_a4x4fMatrix[0][2] = (float)a16dMatrix[8];
	m_a4x4fMatrix[0][3] = (float)a16dMatrix[12];
	m_a4x4fMatrix[1][0] = (float)a16dMatrix[1];
	m_a4x4fMatrix[1][1] = (float)a16dMatrix[5];
	m_a4x4fMatrix[1][2] = (float)a16dMatrix[9];
	m_a4x4fMatrix[1][3] = (float)a16dMatrix[13];
	m_a4x4fMatrix[2][0] = (float)a16dMatrix[2];
	m_a4x4fMatrix[2][1] = (float)a16dMatrix[6];
	m_a4x4fMatrix[2][2] = (float)a16dMatrix[10];
	m_a4x4fMatrix[2][3] = (float)a16dMatrix[14];
	m_a4x4fMatrix[3][0] = (float)a16dMatrix[3];
	m_a4x4fMatrix[3][1] = (float)a16dMatrix[7];
	m_a4x4fMatrix[3][2] = (float)a16dMatrix[11];
	m_a4x4fMatrix[3][3] = (float)a16dMatrix[15];
}


inline void VistaTransformMatrix::GetValues( float a4x4fMatrix[4][4] ) const
{
	GetRow( 0, a4x4fMatrix[0] );
	GetRow( 1, a4x4fMatrix[1] );
	GetRow( 2, a4x4fMatrix[2] );
	GetRow( 3, a4x4fMatrix[3] );
}
inline void VistaTransformMatrix::GetValues( float a16fMatrix[16] ) const
{
	GetRow( 0, &a16fMatrix[0] );
	GetRow( 1, &a16fMatrix[4] );
	GetRow( 2, &a16fMatrix[8] );
	GetRow( 3, &a16fMatrix[12] );
}
inline void VistaTransformMatrix::GetTransposedValues( float a4x4fMatrix[4][4] ) const
{
	GetColumn( 0, a4x4fMatrix[0] );
	GetColumn( 1, a4x4fMatrix[1] );
	GetColumn( 2, a4x4fMatrix[2] );
	GetColumn( 3, a4x4fMatrix[3] );
}
inline void VistaTransformMatrix::GetTransposedValues( float a16fMatrix[16] ) const
{
	GetColumn( 0, &a16fMatrix[0] );
	GetColumn( 1, &a16fMatrix[4] );
	GetColumn( 2, &a16fMatrix[8] );
	GetColumn( 3, &a16fMatrix[12] );
}

inline void VistaTransformMatrix::GetValues( double a4x4dMatrix[4][4] ) const
{
	GetRow( 0, a4x4dMatrix[0] );
	GetRow( 1, a4x4dMatrix[1] );
	GetRow( 2, a4x4dMatrix[2] );
	GetRow( 3, a4x4dMatrix[3] );
}
inline void VistaTransformMatrix::GetValues( double a16dMatrix[16] ) const
{
	GetRow( 0, &a16dMatrix[0] );
	GetRow( 1, &a16dMatrix[4] );
	GetRow( 2, &a16dMatrix[8] );
	GetRow( 3, &a16dMatrix[12] );
}
inline void VistaTransformMatrix::GetTransposedValues( double a4x4dMatrix[4][4] ) const
{
	GetColumn( 0, a4x4dMatrix[0] );
	GetColumn( 1, a4x4dMatrix[1] );
	GetColumn( 2, a4x4dMatrix[2] );
	GetColumn( 3, a4x4dMatrix[3] );
}
inline void VistaTransformMatrix::GetTransposedValues( double a16dMatrix[16] ) const
{
	GetColumn( 0, &a16dMatrix[0] );
	GetColumn( 1, &a16dMatrix[4] );
	GetColumn( 2, &a16dMatrix[8] );
	GetColumn( 3, &a16dMatrix[12] );
}

inline VistaVector3D VistaTransformMatrix::GetTranslation() const
{
	return VistaVector3D( m_a4x4fMatrix[0][3], m_a4x4fMatrix[1][3], m_a4x4fMatrix[2][3] );
}

inline void VistaTransformMatrix::GetTranslation( VistaVector3D &v3Translation ) const
{
	v3Translation[0] = m_a4x4fMatrix[0][3];
	v3Translation[1] = m_a4x4fMatrix[1][3];
	v3Translation[2] = m_a4x4fMatrix[2][3];
}

inline void VistaTransformMatrix::GetBasisMatrix( float a3x3Matrix[3][3] ) const
{
	a3x3Matrix[0][0] = m_a4x4fMatrix[0][0];
	a3x3Matrix[0][1] = m_a4x4fMatrix[0][1];
	a3x3Matrix[0][2] = m_a4x4fMatrix[0][2];
	a3x3Matrix[1][0] = m_a4x4fMatrix[1][0];
	a3x3Matrix[1][1] = m_a4x4fMatrix[1][1];
	a3x3Matrix[1][2] = m_a4x4fMatrix[1][2];
	a3x3Matrix[2][0] = m_a4x4fMatrix[2][0];
	a3x3Matrix[2][1] = m_a4x4fMatrix[2][1];
	a3x3Matrix[2][2] = m_a4x4fMatrix[2][2];
}
inline void VistaTransformMatrix::GetBasisMatrix( float a9fMatrix[9],
												const bool bTransposed ) const
{
	if( bTransposed )
	{
		a9fMatrix[0] = m_a4x4fMatrix[0][0];
		a9fMatrix[3] = m_a4x4fMatrix[0][1];
		a9fMatrix[6] = m_a4x4fMatrix[0][2];
		a9fMatrix[1] = m_a4x4fMatrix[1][0];
		a9fMatrix[4] = m_a4x4fMatrix[1][1];
		a9fMatrix[7] = m_a4x4fMatrix[1][2];
		a9fMatrix[2] = m_a4x4fMatrix[2][0];
		a9fMatrix[5] = m_a4x4fMatrix[2][1];
		a9fMatrix[8] = m_a4x4fMatrix[2][2];
	}
	else
	{
		a9fMatrix[0] = m_a4x4fMatrix[0][0];
		a9fMatrix[1] = m_a4x4fMatrix[0][1];
		a9fMatrix[2] = m_a4x4fMatrix[0][2];
		a9fMatrix[3] = m_a4x4fMatrix[1][0];
		a9fMatrix[4] = m_a4x4fMatrix[1][1];
		a9fMatrix[5] = m_a4x4fMatrix[1][2];
		a9fMatrix[6] = m_a4x4fMatrix[2][0];
		a9fMatrix[7] = m_a4x4fMatrix[2][1];
		a9fMatrix[8] = m_a4x4fMatrix[2][2];
	}
}

inline void VistaTransformMatrix::SetTranslation( const float a3fTranslation[3] )
{
	m_a4x4fMatrix[0][3] = a3fTranslation[0];
	m_a4x4fMatrix[1][3] = a3fTranslation[1];
	m_a4x4fMatrix[2][3] = a3fTranslation[2];
}
inline void VistaTransformMatrix::SetTranslation( const VistaVector3D& v3Translation )
{
	m_a4x4fMatrix[0][3] = v3Translation[0];
	m_a4x4fMatrix[1][3] = v3Translation[1];
	m_a4x4fMatrix[2][3] = v3Translation[2];
}
inline void VistaTransformMatrix::SetBasisMatrix( const float a3x3Matrix[3][3] )
{
	m_a4x4fMatrix[0][0] = a3x3Matrix[0][0];
	m_a4x4fMatrix[0][1] = a3x3Matrix[1][0];
	m_a4x4fMatrix[0][2] = a3x3Matrix[2][0];
	m_a4x4fMatrix[1][0] = a3x3Matrix[0][1];
	m_a4x4fMatrix[1][1] = a3x3Matrix[1][1];
	m_a4x4fMatrix[1][2] = a3x3Matrix[2][1];
	m_a4x4fMatrix[2][0] = a3x3Matrix[0][2];
	m_a4x4fMatrix[2][1] = a3x3Matrix[1][2];
	m_a4x4fMatrix[2][2] = a3x3Matrix[2][2];
}
inline void VistaTransformMatrix::SetBasisMatrix( const float a9fMatrix[9] )
{
	m_a4x4fMatrix[0][0] = a9fMatrix[0];
	m_a4x4fMatrix[0][1] = a9fMatrix[1];
	m_a4x4fMatrix[0][2] = a9fMatrix[2];
	m_a4x4fMatrix[1][0] = a9fMatrix[3];
	m_a4x4fMatrix[1][1] = a9fMatrix[4];
	m_a4x4fMatrix[1][2] = a9fMatrix[5];
	m_a4x4fMatrix[2][0] = a9fMatrix[6];
	m_a4x4fMatrix[2][1] = a9fMatrix[7];
	m_a4x4fMatrix[2][2] = a9fMatrix[8];
}
inline void VistaTransformMatrix::SetBasisMatrix( const VistaQuaternion& qRotation )
{
	const float fNorm = qRotation.GetLengthSquared();

	if( fNorm < Vista::Epsilon )
	{		
		m_a4x4fMatrix[0][0] = 1.0f;
		m_a4x4fMatrix[0][1] = 0.0f;
		m_a4x4fMatrix[0][2] = 0.0f;
		m_a4x4fMatrix[1][0] = 1.0f;
		m_a4x4fMatrix[1][1] = 0.0f;
		m_a4x4fMatrix[1][2] = 0.0f;
		m_a4x4fMatrix[2][0] = 0.0f;
		m_a4x4fMatrix[2][1] = 0.0f;
		m_a4x4fMatrix[2][2] = 1.0f;
		return;
	}

	const float s = 2.0f / fNorm;

	const float xs = qRotation[0] * s,   ys = qRotation[1] * s,   zs = qRotation[2] * s;
	const float wx = qRotation[3] * xs,  wy = qRotation[3] * ys,  wz = qRotation[3] * zs;
	const float xx = qRotation[0] * xs,  xy = qRotation[0] * ys,  xz = qRotation[0] * zs;
	const float yy = qRotation[1] * ys,  yz = qRotation[1] * zs,  zz = qRotation[2] * zs;

	m_a4x4fMatrix[0][0] = 1.0f - (yy + zz);  
	m_a4x4fMatrix[0][1] = xy - wz;          
	m_a4x4fMatrix[0][2] = xz + wy;         
	
	m_a4x4fMatrix[1][0] = xy + wz;          
	m_a4x4fMatrix[1][1] = 1.0f - (xx + zz);  
	m_a4x4fMatrix[1][2] = yz - wx;          

	m_a4x4fMatrix[2][0] = xz - wy;	        
	m_a4x4fMatrix[2][1] = yz + wx;          
	m_a4x4fMatrix[2][2] = 1.0f - (xx + yy);  
}



inline void VistaTransformMatrix::SetToIdentity()
{
	m_a4x4fMatrix[0][0] = 1.0f;
	m_a4x4fMatrix[0][1] = 0.0f;
	m_a4x4fMatrix[0][2] = 0.0f;
	m_a4x4fMatrix[0][3] = 0.0f;
	m_a4x4fMatrix[1][0] = 0.0f;
	m_a4x4fMatrix[1][1] = 1.0f;
	m_a4x4fMatrix[1][2] = 0.0f;
	m_a4x4fMatrix[1][3] = 0.0f;
	m_a4x4fMatrix[2][0] = 0.0f;
	m_a4x4fMatrix[2][1] = 0.0f;
	m_a4x4fMatrix[2][2] = 1.0f;
	m_a4x4fMatrix[2][3] = 0.0f;
	m_a4x4fMatrix[3][0] = 0.0f;
	m_a4x4fMatrix[3][1] = 0.0f;
	m_a4x4fMatrix[3][2] = 0.0f;
	m_a4x4fMatrix[3][3] = 1.0f;
}

inline void VistaTransformMatrix::SetToTranslationMatrix( const VistaVector3D& v3Translation )
{
	m_a4x4fMatrix[0][0] = 1.0f;
	m_a4x4fMatrix[0][1] = 0.0f;
	m_a4x4fMatrix[0][2] = 0.0f;
	m_a4x4fMatrix[0][3] = v3Translation[0];
	m_a4x4fMatrix[1][0] = 0.0f;
	m_a4x4fMatrix[1][1] = 1.0f;
	m_a4x4fMatrix[1][2] = 0.0f;
	m_a4x4fMatrix[1][3] = v3Translation[1];
	m_a4x4fMatrix[2][0] = 0.0f;
	m_a4x4fMatrix[2][1] = 0.0f;
	m_a4x4fMatrix[2][2] = 1.0f;
	m_a4x4fMatrix[2][3] = v3Translation[2];
	m_a4x4fMatrix[3][0] = 0.0;
	m_a4x4fMatrix[3][1] = 0.0;
	m_a4x4fMatrix[3][2] = 0.0f;
	m_a4x4fMatrix[3][3] = 1.0f;
}

inline void VistaTransformMatrix::SetTransform( const VistaQuaternion& qRotation,
											const VistaVector3D& v3Translation )
{
	SetBasisMatrix( qRotation );
	SetTranslation( v3Translation );
	m_a4x4fMatrix[3][0] = 0.0f;
	m_a4x4fMatrix[3][1] = 0.0f;
	m_a4x4fMatrix[3][2] = 0.0f;
	m_a4x4fMatrix[3][3] = 1.0f;
}

inline void VistaTransformMatrix::SetToScaleMatrix( const float fUniformScale )
{
	m_a4x4fMatrix[0][0] = fUniformScale;
	m_a4x4fMatrix[0][1] = 0.0f;
	m_a4x4fMatrix[0][2] = 0.0f;
	m_a4x4fMatrix[0][3] = 0.0f;
	m_a4x4fMatrix[1][0] = 0.0f;
	m_a4x4fMatrix[1][1] = fUniformScale;
	m_a4x4fMatrix[1][2] = 0.0f;
	m_a4x4fMatrix[1][3] = 0.0f;
	m_a4x4fMatrix[2][0] = 0.0f;
	m_a4x4fMatrix[2][1] = 0.0f;
	m_a4x4fMatrix[2][2] = fUniformScale;
	m_a4x4fMatrix[2][3] = 0.0f;
	m_a4x4fMatrix[3][0] = 0.0f;
	m_a4x4fMatrix[3][1] = 0.0f;
	m_a4x4fMatrix[3][2] = 0.0f;
	m_a4x4fMatrix[3][3] = 1.0f;
}

inline void VistaTransformMatrix::SetToScaleMatrix( const float fXScale,
												   const float fYScale,
												   const float fZScale )
{
	m_a4x4fMatrix[0][0] = fXScale;
	m_a4x4fMatrix[0][1] = 0.0f;
	m_a4x4fMatrix[0][2] = 0.0f;
	m_a4x4fMatrix[0][3] = 0.0f;
	m_a4x4fMatrix[1][0] = 0.0f;
	m_a4x4fMatrix[1][1] = fYScale;
	m_a4x4fMatrix[1][2] = 0.0f;
	m_a4x4fMatrix[1][3] = 0.0f;
	m_a4x4fMatrix[2][0] = 0.0f;
	m_a4x4fMatrix[2][1] = 0.0f;
	m_a4x4fMatrix[2][2] = fZScale;
	m_a4x4fMatrix[2][3] = 0.0f;
	m_a4x4fMatrix[3][0] = 0.0f;
	m_a4x4fMatrix[3][1] = 0.0f;
	m_a4x4fMatrix[3][2] = 0.0f;
	m_a4x4fMatrix[3][3] = 1.0f;
}

inline void VistaTransformMatrix::SetToRotationMatrix( const VistaQuaternion& qRotation )
{
	SetTransform( qRotation );
}



inline float VistaTransformMatrix::GetDeterminant() const
{
	return ( m_a4x4fMatrix[0][0] * GetAdjunct ( 0, 0 )
		+ m_a4x4fMatrix[0][1] * GetAdjunct ( 0, 1 )
		+ m_a4x4fMatrix[0][2] * GetAdjunct ( 0, 2 )
		+ m_a4x4fMatrix[0][3] * GetAdjunct ( 0, 3 ) );
}


inline VistaTransformMatrix VistaTransformMatrix::GetInverted() const
{
	VistaTransformMatrix matReturn;
	if( GetInverted( matReturn ) )
		return matReturn;
	else
		return VistaTransformMatrix();
}

inline bool VistaTransformMatrix::GetInverted( VistaTransformMatrix& matTarget ) const
{
	int a4fIndexC[4], a4fIndexR[4];
	int a4fPivot[4] = { 0, 0, 0, 0 };
	int iCol,iRow;

	float fBig, fDum, fInvPivot, fTmp;

	matTarget.SetValues( m_a4x4fMatrix );

	for( register int  i = 0; i < 4; ++i )
	{
		fBig = 0.0f;
		for( register int  j = 0; j < 4; ++j )
		{
			if( a4fPivot[j] != 1 )
			{
				for( register int  k = 0 ; k < 4 ; ++k )
				{
					if( a4fPivot[k] == 0 )
					{
						if( fabs( matTarget[k][j] ) >= fBig )
						{
							fBig = fabs( matTarget[k][j] );
							iRow = j;
							iCol = k;
						}
					}
					else
						if( a4fPivot[k] > 1.0f )
							return false; // singular matrix
				}
			}
		}

		++a4fPivot[iCol];

		if( iRow != iCol )
		{
			for ( register int l = 0 ; l<4 ; l++)
			{
				fTmp = matTarget[l][iCol];
				matTarget[l][iCol] = matTarget[l][iRow];
				matTarget[l][iRow] = fTmp;
			}
		}

		a4fIndexR[i] = iRow;
		a4fIndexC[i] = iCol;

		if( matTarget[iCol][iCol] == 0.0 )
			return false; // singular matrix

		fInvPivot = 1.0f / matTarget[iCol][iCol];
		matTarget[iCol][iCol] = 1.0f ;

		for ( register int l = 0; l < 4; ++l )
		{
			matTarget[l][iCol] = matTarget[l][iCol] * fInvPivot ;
		}

		for( register int ll = 0; ll < 4; ++ll )
		{
			if( ll != iCol )
			{
				fDum = matTarget[iCol][ll];
				matTarget[iCol][ll] = 0.0;
				for( register int l = 0 ; l<4 ; l++)
					matTarget[l][ll] = matTarget[l][ll] - matTarget[l][iCol] * fDum ;
			}
		}
	}

	for( register int l = 4; l--; )
	{
		if( a4fIndexR[l] != a4fIndexC[l] )
			for( register int k = 0; k < 4; ++k )
			{
				fTmp = matTarget[a4fIndexR[l]][k];
				matTarget[a4fIndexR[l]][k] = matTarget[a4fIndexC[l]][k];
				matTarget[a4fIndexC[l]][k] = fTmp;
			}
	}

	return true;
}

inline VistaTransformMatrix VistaTransformMatrix::GetTranspose() const
{
	return VistaTransformMatrix(
		m_a4x4fMatrix[0][0], m_a4x4fMatrix[1][0], m_a4x4fMatrix[2][0], m_a4x4fMatrix[3][0],
		m_a4x4fMatrix[0][1], m_a4x4fMatrix[1][1], m_a4x4fMatrix[2][1], m_a4x4fMatrix[3][1],
		m_a4x4fMatrix[0][2], m_a4x4fMatrix[1][2], m_a4x4fMatrix[2][2], m_a4x4fMatrix[3][2],
		m_a4x4fMatrix[0][3], m_a4x4fMatrix[1][3], m_a4x4fMatrix[2][3], m_a4x4fMatrix[3][3] );
}

inline void VistaTransformMatrix::GetTranspose( VistaTransformMatrix& matTraget ) const
{
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
			matTraget[i][j] = m_a4x4fMatrix[j][i];
	}
}


inline void VistaTransformMatrix::Transpose()
{
	/** @todo: in-place trasnpose */
	(*this) = GetTranspose();
}

inline void VistaTransformMatrix::Invert()
{
	(*this) = GetInverted();
}

inline VistaVector3D VistaTransformMatrix::Transform( const VistaVector3D& v3Vector ) const
{
	return VistaVector3D( m_a4x4fMatrix[0][0] * v3Vector[0] + m_a4x4fMatrix[0][1] * v3Vector[1]
						+ m_a4x4fMatrix[0][2] * v3Vector[2] + m_a4x4fMatrix[0][3] * v3Vector[3],
						m_a4x4fMatrix[1][0] * v3Vector[0] + m_a4x4fMatrix[1][1] * v3Vector[1]
						+ m_a4x4fMatrix[1][2] * v3Vector[2] + m_a4x4fMatrix[1][3] * v3Vector[3],
						m_a4x4fMatrix[2][0] * v3Vector[0] + m_a4x4fMatrix[2][1] * v3Vector[1]
						+ m_a4x4fMatrix[2][2] * v3Vector[2] + m_a4x4fMatrix[2][3] * v3Vector[3],
						m_a4x4fMatrix[3][0] * v3Vector[0] + m_a4x4fMatrix[3][1] * v3Vector[1]
						+ m_a4x4fMatrix[3][2] * v3Vector[2] + m_a4x4fMatrix[3][3] * v3Vector[3] );
}


inline VistaVector3D VistaTransformMatrix::TransformPoint( const VistaVector3D& v3Point ) const
{
	return VistaVector3D( m_a4x4fMatrix[0][0] * v3Point[0] + m_a4x4fMatrix[0][1] * v3Point[1]
						+ m_a4x4fMatrix[0][2] * v3Point[2] + m_a4x4fMatrix[0][3],
						m_a4x4fMatrix[1][0] * v3Point[0] + m_a4x4fMatrix[1][1] * v3Point[1]
						+ m_a4x4fMatrix[1][2] * v3Point[2] + m_a4x4fMatrix[1][3],
						m_a4x4fMatrix[2][0] * v3Point[0] + m_a4x4fMatrix[2][1] * v3Point[1]
						+ m_a4x4fMatrix[2][2] * v3Point[2] + m_a4x4fMatrix[2][3],
						m_a4x4fMatrix[3][0] * v3Point[0] + m_a4x4fMatrix[3][1] * v3Point[1]
						+ m_a4x4fMatrix[3][2] * v3Point[2] + m_a4x4fMatrix[3][3] );
}

inline VistaVector3D VistaTransformMatrix::TransformVector( const VistaVector3D& v3Vector ) const
{
	return VistaVector3D( m_a4x4fMatrix[0][0] * v3Vector[0] + m_a4x4fMatrix[0][1] * v3Vector[1]
						+ m_a4x4fMatrix[0][2] * v3Vector[2],
						m_a4x4fMatrix[1][0] * v3Vector[0] + m_a4x4fMatrix[1][1] * v3Vector[1]
						+ m_a4x4fMatrix[1][2] * v3Vector[2],
						m_a4x4fMatrix[2][0] * v3Vector[0] + m_a4x4fMatrix[2][1] * v3Vector[1]
						+ m_a4x4fMatrix[2][2] * v3Vector[2],
						m_a4x4fMatrix[3][0] * v3Vector[0] + m_a4x4fMatrix[3][1] * v3Vector[1]
						+ m_a4x4fMatrix[3][2] * v3Vector[2] + m_a4x4fMatrix[3][3] * v3Vector[3] );
}

inline VistaQuaternion VistaTransformMatrix::Transform( const VistaQuaternion& qRotation ) const
{
	return ( VistaQuaternion( (*this) ) * qRotation );
}
inline VistaTransformMatrix VistaTransformMatrix::Transform( const VistaTransformMatrix& matTransform ) const
{
	VistaTransformMatrix matNew;
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			matNew[i][j] = m_a4x4fMatrix[i][0] * matTransform[0][j]
						+ m_a4x4fMatrix[i][1] * matTransform[1][j]
						+ m_a4x4fMatrix[i][2] * matTransform[2][j]
						+ m_a4x4fMatrix[i][3] * matTransform[3][j];
			//matNew[i][j] = m_a4x4fMatrix[0][i] * matTransform[j][0]
			//			+ m_a4x4fMatrix[1][i] * matTransform[j][1]
			//			+ m_a4x4fMatrix[2][i] * matTransform[j][2]
			//			+ m_a4x4fMatrix[3][i] * matTransform[j][3];

		}
	}
	return matNew;
}


inline VistaTransformMatrix& VistaTransformMatrix::operator+=( const VistaTransformMatrix& matOther )
{
	m_a4x4fMatrix[0][0] += matOther[0][0];
	m_a4x4fMatrix[0][1] += matOther[0][1];
	m_a4x4fMatrix[0][2] += matOther[0][2];
	m_a4x4fMatrix[0][3] += matOther[0][3];
	m_a4x4fMatrix[1][0] += matOther[1][0];
	m_a4x4fMatrix[1][1] += matOther[1][1];
	m_a4x4fMatrix[1][2] += matOther[1][2];
	m_a4x4fMatrix[1][3] += matOther[1][3];
	m_a4x4fMatrix[2][0] += matOther[2][0];
	m_a4x4fMatrix[2][1] += matOther[2][1];
	m_a4x4fMatrix[2][2] += matOther[2][2];
	m_a4x4fMatrix[2][3] += matOther[2][3];
	m_a4x4fMatrix[3][0] += matOther[3][0];
	m_a4x4fMatrix[3][1] += matOther[3][1];
	m_a4x4fMatrix[3][2] += matOther[3][2];
	m_a4x4fMatrix[3][3] += matOther[3][3];
	return (*this);
}

inline VistaTransformMatrix& VistaTransformMatrix::operator-=( const VistaTransformMatrix& matOther )
{
	m_a4x4fMatrix[0][0] -= matOther[0][0];
	m_a4x4fMatrix[0][1] -= matOther[0][1];
	m_a4x4fMatrix[0][2] -= matOther[0][2];
	m_a4x4fMatrix[0][3] -= matOther[0][3];
	m_a4x4fMatrix[1][0] -= matOther[1][0];
	m_a4x4fMatrix[1][1] -= matOther[1][1];
	m_a4x4fMatrix[1][2] -= matOther[1][2];
	m_a4x4fMatrix[1][3] -= matOther[1][3];
	m_a4x4fMatrix[2][0] -= matOther[2][0];
	m_a4x4fMatrix[2][1] -= matOther[2][1];
	m_a4x4fMatrix[2][2] -= matOther[2][2];
	m_a4x4fMatrix[2][3] -= matOther[2][3];
	m_a4x4fMatrix[3][0] -= matOther[3][0];
	m_a4x4fMatrix[3][1] -= matOther[3][1];
	m_a4x4fMatrix[3][2] -= matOther[3][2];
	m_a4x4fMatrix[3][3] -= matOther[3][3];
	return (*this);
}

inline VistaTransformMatrix& VistaTransformMatrix::operator*=( const VistaTransformMatrix& matOther )
{
	(*this) = (*this) * matOther;
	return (*this);
}


inline bool VistaTransformMatrix::operator==( const VistaTransformMatrix& matOther ) const
{
	return( m_a4x4fMatrix[0][0] == matOther[0][0]
			&& m_a4x4fMatrix[0][1] == matOther[0][1]
			&& m_a4x4fMatrix[0][2] == matOther[0][2]
			&& m_a4x4fMatrix[0][3] == matOther[0][3]
			&& m_a4x4fMatrix[1][0] == matOther[1][0]
			&& m_a4x4fMatrix[1][1] == matOther[1][1]
			&& m_a4x4fMatrix[1][2] == matOther[1][2]
			&& m_a4x4fMatrix[1][3] == matOther[1][3]
			&& m_a4x4fMatrix[2][0] == matOther[2][0]
			&& m_a4x4fMatrix[2][1] == matOther[2][1]
			&& m_a4x4fMatrix[2][2] == matOther[2][2]
			&& m_a4x4fMatrix[2][3] == matOther[2][3]
			&& m_a4x4fMatrix[3][0] == matOther[3][0]
			&& m_a4x4fMatrix[3][1] == matOther[3][1]
			&& m_a4x4fMatrix[3][2] == matOther[3][2]
			&& m_a4x4fMatrix[3][3] == matOther[3][3] );

}

inline bool VistaTransformMatrix::operator!=( const VistaTransformMatrix& matOther ) const
{
	return !operator==( matOther );

}

VistaVector3D operator*( const VistaTransformMatrix& matTransform,
										const VistaVector3D& v3Point )
{
	return matTransform.Transform( v3Point );
}
VistaQuaternion operator*( const VistaTransformMatrix& matTransform,
										const VistaQuaternion& qRotation )
{
	return matTransform.Transform( qRotation );
}
VistaTransformMatrix operator*( const VistaTransformMatrix& matTransform1,
										  const VistaTransformMatrix& matTransform2 )
{
	return matTransform1.Transform( matTransform2 );
}
VistaTransformMatrix operator+( const VistaTransformMatrix& matTransform1,
										  const VistaTransformMatrix& matTransform2 )
{
	return VistaTransformMatrix( matTransform1[0][0] + matTransform2[0][0],
								matTransform1[0][1] + matTransform2[0][1],
								matTransform1[0][2] + matTransform2[0][2],
								matTransform1[0][3] + matTransform2[0][3],
								matTransform1[1][0] + matTransform2[1][0],
								matTransform1[1][1] + matTransform2[1][1],
								matTransform1[1][2] + matTransform2[1][2],
								matTransform1[1][3] + matTransform2[1][3],
								matTransform1[2][0] + matTransform2[2][0],
								matTransform1[2][1] + matTransform2[2][1],
								matTransform1[2][2] + matTransform2[2][2],
								matTransform1[2][3] + matTransform2[2][3],
								matTransform1[3][0] + matTransform2[3][0],
								matTransform1[3][1] + matTransform2[3][1],
								matTransform1[3][2] + matTransform2[3][2],
								matTransform1[3][3] + matTransform2[3][3] );
}
VistaTransformMatrix operator-( const VistaTransformMatrix& matTransform1,
										  const VistaTransformMatrix& matTransform2 )
{
	return VistaTransformMatrix( matTransform1[0][0] - matTransform2[0][0],
								matTransform1[0][1] - matTransform2[0][1],
								matTransform1[0][2] - matTransform2[0][2],
								matTransform1[0][3] - matTransform2[0][3],
								matTransform1[1][0] - matTransform2[1][0],
								matTransform1[1][1] - matTransform2[1][1],
								matTransform1[1][2] - matTransform2[1][2],
								matTransform1[1][3] - matTransform2[1][3],
								matTransform1[2][0] - matTransform2[2][0],
								matTransform1[2][1] - matTransform2[2][1],
								matTransform1[2][2] - matTransform2[2][2],
								matTransform1[2][3] - matTransform2[2][3],
								matTransform1[3][0] - matTransform2[3][0],
								matTransform1[3][1] - matTransform2[3][1],
								matTransform1[3][2] - matTransform2[3][2],
								matTransform1[3][3] - matTransform2[3][3] );
}

VistaTransformMatrix operator*( const float fScale,
										  const VistaTransformMatrix& matTransform )
{
	return VistaTransformMatrix( fScale * matTransform[0][0],
								fScale * matTransform[0][1],
								fScale * matTransform[0][2],
								fScale * matTransform[0][3],
								fScale * matTransform[1][0],
								fScale * matTransform[1][1],
								fScale * matTransform[1][2],
								fScale * matTransform[1][3],
								fScale * matTransform[2][0],
								fScale * matTransform[2][1],
								fScale * matTransform[2][2],
								fScale * matTransform[2][3],
								fScale * matTransform[3][0],
								fScale * matTransform[3][1],
								fScale * matTransform[3][2],
								fScale * matTransform[3][3] );
}
VistaTransformMatrix operator*( const VistaTransformMatrix& matTransform,
												const float fScale )
{
	return ( fScale * matTransform );
}
VistaTransformMatrix operator/( const VistaTransformMatrix& matTransform,
												const float fScale )
{
	return ( ( 1.0f / fScale ) * matTransform );
}

std::ostream& operator<<( std::ostream& oStream, const VistaTransformMatrix& matTransform )
{
	const std::streamsize iOldPrecision( oStream.precision( 3 ) );
	const std::ios::fmtflags iOldflags( oStream.flags() );

	// set fix point notation
	oStream.setf( std::ios::fixed | std::ios::showpos );

	// write to the stream
	oStream << "( " << matTransform[0][0] << ", " << matTransform[0][1] 
			<< ", " << matTransform[0][2] << ", " << matTransform[0][3] << ", "
			<< std::endl
			<< "  " << matTransform[1][0] << ", " << matTransform[1][1] 
			<< ", " << matTransform[1][2] << ", " << matTransform[1][3] << ", "
			<< std::endl
			<< "  " << matTransform[2][0] << ", " << matTransform[2][1] 
			<< ", " << matTransform[2][2] << ", " << matTransform[2][3] << ", "
			<< std::endl
			<< "  " << matTransform[3][0] << ", " << matTransform[3][1] 
			<< ", " << matTransform[3][2] << ", " << matTransform[3][3] << " )";

	// restore old stream settings
	oStream.precision( iOldPrecision );
	oStream.flags( iOldflags );

	return oStream;
}


#endif //_VISTATRANSFORMMATRIX_H
