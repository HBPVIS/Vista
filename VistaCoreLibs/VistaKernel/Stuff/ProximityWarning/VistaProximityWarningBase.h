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

#ifndef _VISTAPROXIMITYWARNINGBASE_H
#define _VISTAPROXIMITYWARNINGBASE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaVectorMath.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaBoundingBox;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaProximityWarningBase
{
public:
	IVistaProximityWarningBase( const float nBeginWarnignDistance,
								const float nMaxWarningDistance );
	virtual ~IVistaProximityWarningBase();

	virtual void AddHalfPlane( const VistaVector3D& v3Center, 
						const VistaVector3D& v3Normal );
	virtual void SetUseExtents( const VistaBoundingBox& bbExtents,
						const VistaQuaternion& qBoxRotation = VistaQuaternion() );

	bool Update( const VistaVector3D& v3ViewerPosition,
				const VistaQuaternion& qViewerOrientation );
	bool Update( const VistaVector3D& v3ViewerPosition,
				const VistaQuaternion& qViewerOrientation,
				const std::vector<VistaVector3D>& vecAdditionalPoints );

	float GetSafeDistance() const;
	void SetSafeDistance( const float nDistance );
	float GetDangerDistance() const;
	void SetDangerDistance( const float nDistance );

protected:
	virtual bool DoUpdate( const float nMinDistance,
							const float nWarningLevel, // [0,1] - 0 is all okay, 1 max warning
							const VistaVector3D& v3PointOnBounds,
							const VistaVector3D& v3UserPosition,
							const VistaQuaternion& qUserOrientation ) = 0;
private:
	struct HalfPlane
	{		
		VistaVector3D m_v3Normal;
		VistaVector3D m_v3Center;
	};	

	void UpdateFromExtents( const VistaVector3D& v3UserPosition,
							float& fDistance,
							VistaVector3D& v3ClostestPoint );
	void UpdateFromHalfplane( const HalfPlane& oPlane,
							const VistaVector3D& v3UserPosition,
							float& fDistance,
							VistaVector3D& v3ClostestPoint );

private:
	float m_nSafeDistance;
	float m_nDangerDistance;

	std::vector<HalfPlane> m_vecHalfPlanes;
};

#endif //_VISTAPROXIMITYWARNINGBASE_H

