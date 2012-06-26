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

#include "VistaProximityWarningBase.h"

#include <VistaMath/VistaBoundingBox.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


IVistaProximityWarningBase::IVistaProximityWarningBase( const float nBeginWarnignDistance,
														const float nMaxWarningDistance )
: m_nSafeDistance( nBeginWarnignDistance )
, m_nDangerDistance( nMaxWarningDistance )
{
}

IVistaProximityWarningBase::~IVistaProximityWarningBase()
{
}

void IVistaProximityWarningBase::AddHalfPlane( const VistaVector3D& v3Center, const VistaVector3D& v3Normal )
{
	m_vecHalfPlanes.push_back( HalfPlane() );
	m_vecHalfPlanes.back().m_v3Center = v3Center;
	m_vecHalfPlanes.back().m_v3Normal = v3Normal;
}

void IVistaProximityWarningBase::SetUseExtents( const VistaBoundingBox& bbExtents, const VistaQuaternion& qBoxRotation )
{
	VistaVector3D v3Center;
	VistaVector3D v3Normal;

	VistaVector3D v3BoxCenter = bbExtents.GetCenter();
	VistaVector3D v3Extents = 0.5f * bbExtents.GetSize();

	v3Normal = qBoxRotation.Rotate( Vista::XAxis );
	v3Center = v3BoxCenter - v3Extents[Vista::X] * v3Normal;
	AddHalfPlane( v3Center, v3Normal );
	v3Normal = -v3Normal;
	v3Center = v3BoxCenter - v3Extents[Vista::X] * v3Normal;
	AddHalfPlane( v3Center, v3Normal );

	v3Normal = qBoxRotation.Rotate( Vista::YAxis );
	v3Center = v3BoxCenter - v3Extents[Vista::Y] * v3Normal;
	AddHalfPlane( v3Center, v3Normal );
	v3Normal = -v3Normal;
	v3Center = v3BoxCenter - v3Extents[Vista::Y] * v3Normal;
	AddHalfPlane( v3Center, v3Normal );

	v3Normal = qBoxRotation.Rotate( Vista::ZAxis );
	v3Center = v3BoxCenter - v3Extents[Vista::Z] * v3Normal;
	AddHalfPlane( v3Center, v3Normal );
	v3Normal = -v3Normal;
	v3Center = v3BoxCenter - v3Extents[Vista::Z] * v3Normal;
	AddHalfPlane( v3Center, v3Normal );
}

bool IVistaProximityWarningBase::Update( const VistaVector3D& v3ViewerPosition,
										const VistaQuaternion& qViewerOrientation )
{
	return Update( v3ViewerPosition, qViewerOrientation, std::vector<VistaVector3D>() );
}

bool IVistaProximityWarningBase::Update( const VistaVector3D& v3ViewerPosition,
										const VistaQuaternion& qViewerOrientation,
										const std::vector<VistaVector3D>& vecAdditionalPoints )
{
	float nDistance = std::numeric_limits<float>::max();
	VistaVector3D v3NearestPosition;

	for( std::vector<HalfPlane>::const_iterator itPlane = m_vecHalfPlanes.begin();
			itPlane != m_vecHalfPlanes.end(); ++itPlane )
	{
		UpdateFromHalfplane( (*itPlane), v3ViewerPosition, nDistance, v3NearestPosition );
		for( std::vector<VistaVector3D>::const_iterator itPoint = vecAdditionalPoints.begin();
				itPoint != vecAdditionalPoints.end(); ++itPoint )
		{
			UpdateFromHalfplane( (*itPlane), v3ViewerPosition, nDistance, v3NearestPosition );
		}
	}

	float nWarningLevel = nDistance - m_nDangerDistance;
	nWarningLevel /= ( m_nSafeDistance - m_nDangerDistance );
	nWarningLevel = Vista::Clamp<float>( nWarningLevel, 0, 1 );
	nWarningLevel = 1 - nWarningLevel;

	//std::cout << "Dist: " << nDistance << "               warn: " << nWarningLevel <<std::endl;
	return DoUpdate( nDistance, nWarningLevel, v3NearestPosition,
						v3ViewerPosition, qViewerOrientation );
}

void IVistaProximityWarningBase::UpdateFromHalfplane( const HalfPlane& oPlane,
													 const VistaVector3D& v3UserPosition,
													 float& nDistance,
													 VistaVector3D& v3ClostestPoint )
{
	VistaVector3D v3Delta = v3UserPosition - oPlane.m_v3Center;
	float nOwnDistance = v3Delta.Dot( oPlane.m_v3Normal );
	if( nOwnDistance < nDistance )
	{
		v3ClostestPoint = v3UserPosition - nOwnDistance * oPlane.m_v3Normal;
		nDistance = nOwnDistance;
	}
}

float IVistaProximityWarningBase::GetSafeDistance() const
{
	return m_nSafeDistance;
}

void IVistaProximityWarningBase::SetSafeDistance( const float nDistance )
{
	m_nSafeDistance = nDistance;
}

float IVistaProximityWarningBase::GetDangerDistance() const
{
	return m_nDangerDistance;
}

void IVistaProximityWarningBase::SetDangerDistance( const float nDistance )
{
	m_nDangerDistance = nDistance;
}
