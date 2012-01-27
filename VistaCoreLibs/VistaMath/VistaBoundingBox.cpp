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


//============================================================================
//  INCLUDES
//============================================================================

#include <cmath>          // sin, cos, sqrt, fabs
#include <iostream>
#include "VistaBoundingBox.h"

#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaStreamUtils.h>

#if !defined(WIN32) || defined(_MSC_VER) && _MSC_VER > 1300
using namespace std;
#endif

//============================================================================
//  MAKROS AND DEFINES
//============================================================================

#define min(a,b)	((a<b)?(a):(b))
#define max(a,b)	((a>b)?(a):(b))
#define intersect(a1,a2,b1,b2)	((a1 <= b1) ? (a2>=b1) : (b2>=a1))

//using namespace std;

bool VistaBoundingBox::ComputeAABB(const float p1[3],
						const float p2[3],
						const VistaTransformMatrix &m,
						VistaBoundingBox &bbOut)
{
	VistaVector3D v3Min(p1[0], p1[1], p1[2]);
	VistaVector3D v3Max(p2[0], p2[1], p2[2]);

	VistaVector3D v[8];
	v[0] = VistaVector3D(v3Min);
	v[1] = VistaVector3D(v3Max[Vista::X], v3Min[Vista::Y], v3Min[Vista::Z]);
	v[2] = VistaVector3D(v3Min[Vista::X], v3Max[Vista::Y], v3Min[Vista::Z]);
	v[3] = VistaVector3D(v3Max[Vista::X], v3Max[Vista::Y], v3Min[Vista::Z]);
	v[4] = VistaVector3D(v3Min[Vista::X], v3Min[Vista::Y], v3Max[Vista::Z]);
	v[5] = VistaVector3D(v3Max[Vista::X], v3Min[Vista::Y], v3Max[Vista::Z]);
	v[6] = VistaVector3D(v3Min[Vista::X], v3Max[Vista::Y], v3Max[Vista::Z]);
	v[7] = VistaVector3D(v3Max);

	v[0] = m * v[0];
	v[1] = m * v[1];
	v[2] = m * v[2];
	v[3] = m * v[3];
	v[4] = m * v[4];
	v[5] = m * v[5];
	v[6] = m * v[6];
	v[7] = m * v[7];

	v3Min = m*v3Min;
	v3Max = m*v3Max;

	for(unsigned i=0; i < 8; ++i)
	{
		if(v[i][Vista::X] < v3Min[Vista::X])
			v3Min[Vista::X] = v[i][Vista::X];
		if(v[i][Vista::X] > v3Max[Vista::X])
			v3Max[Vista::X] = v[i][Vista::X];	

		if(v[i][Vista::Y] < v3Min[Vista::Y])
			v3Min[Vista::Y] = v[i][Vista::Y];
		if(v[i][Vista::Y] > v3Max[Vista::Y])
			v3Max[Vista::Y] = v[i][Vista::Y];	

		if(v[i][Vista::Z] < v3Min[Vista::Z])
			v3Min[Vista::Z] = v[i][Vista::Z];
		if(v[i][Vista::Z] > v3Max[Vista::Z])
			v3Max[Vista::Z] = v[i][Vista::Z];	
	}

	bbOut = VistaBoundingBox(&v3Min[0], &v3Max[0]);
	return true;
}


//============================================================================
//  CONSTRUCTORS / DESTRUCTOR
//============================================================================

VistaBoundingBox::VistaBoundingBox()	
{
	m_min[0] = m_min[1] = m_min[2] = 0.0f;
	m_max[0] = m_max[1] = m_max[2] = 0.0f;
}

VistaBoundingBox::VistaBoundingBox (const float min[3], const float max[3])
{
	m_min[0] = min[0];
	m_min[1] = min[1];
	m_min[2] = min[2];

	m_max[0] = max[0];
	m_max[1] = max[1];
	m_max[2] = max[2];
}

VistaBoundingBox::VistaBoundingBox(const float a[3], const float b[3], const float c[3])
{
	m_min[0] = min(a[0], min(b[0], c[0]));
	m_max[0] = max(a[0], max(b[0], c[0]));
	m_min[1] = min(a[1], min(b[1], c[1]));
	m_max[1] = max(a[1], max(b[1], c[1]));
	m_min[2] = min(a[2], min(b[2], c[2]));
	m_max[2] = max(a[2], max(b[2], c[2]));
}

VistaBoundingBox::~VistaBoundingBox()
{
}

//============================================================================
//  IMPLEMENTATION
//============================================================================

void VistaBoundingBox::SetBounds(const float min[3], const float max[3])
{
	m_min[0] = min[0];
	m_min[1] = min[1];
	m_min[2] = min[2];

	m_max[0] = max[0];
	m_max[1] = max[1];
	m_max[2] = max[2];	
}

void VistaBoundingBox::GetBounds(float min[3], float max[3]) const
{
	min[0] = m_min[0];
	min[1] = m_min[1];
	min[2] = m_min[2];

	max[0] = m_max[0];
	max[1] = m_max[1];
	max[2] = m_max[2];	
}


bool VistaBoundingBox::IsEmpty() const
{
	float width, height, depth;

	GetSize(width, height, depth);

	if(width == 0.0f && height == 0.0f && depth == 0.0f)
		return true;
	else
		return false;
}

void VistaBoundingBox::GetSize(float& width, float& height, float& depth) const
{
	width = m_max[0] - m_min[0];
	height = m_max[1] - m_min[1];
	depth = m_max[2] - m_min[2];
}

float VistaBoundingBox::GetDiagonalLength() const
{
	float width, height, depth;
	
	GetSize(width, height, depth);

	return (float)sqrt(width*width + height*height + depth*depth);
}

float VistaBoundingBox::GetVolume() const
{
	float width, height, depth;
	
	GetSize(width, height, depth);

	return width*height*depth;
}

void VistaBoundingBox::GetCenter(float center[3]) const
{
	center[0] = (m_max[0] + m_min[0])/2.0f;
	center[1] = (m_max[1] + m_min[1])/2.0f;
	center[2] = (m_max[2] + m_min[2])/2.0f;
}

VistaBoundingBox VistaBoundingBox::Expand(const float &size)
{
	m_min[0] -= size;
	m_min[1] -= size;
	m_min[2] -= size;
	m_max[0] += size;
	m_max[1] += size;
	m_max[2] += size;
	return *this;
}

bool VistaBoundingBox::Intersects (const float point[3]) const
{ 
	if (m_min[0] <= point[0] && point[0] <= m_max[0]
	 && m_min[1] <= point[1] && point[1] <= m_max[1]
	 && m_min[2] <= point[2] && point[2] <= m_max[2])
		return true;
	else
		return false;
}

bool VistaBoundingBox::Contains (const VistaBoundingBox& BBox) const
{
	if (m_min[0] <= BBox.m_min[0] && BBox.m_max[0] <= m_max[0]
	 && m_min[1] <= BBox.m_min[1] && BBox.m_max[1] <= m_max[1]
	 && m_min[2] <= BBox.m_min[2] && BBox.m_max[2] <= m_max[2])
		return true;
	else
		return false;
}

bool VistaBoundingBox::Intersects (const VistaBoundingBox& BBox) const
{
	if(intersect(m_min[0], m_max[0], BBox.m_min[0], BBox.m_max[0])
	&& intersect(m_min[1], m_max[1], BBox.m_min[1], BBox.m_max[1])
	&& intersect(m_min[2], m_max[2], BBox.m_min[2], BBox.m_max[2]))
		return true;
	else
		return false;
}

VistaBoundingBox VistaBoundingBox::Include(const VistaBoundingBox& BBox)
{
	if (BBox.m_min[0] < m_min[0])
	{
		m_min[0] = BBox.m_min[0];
	}
	if (BBox.m_max[0] > m_max[0])
	{
		m_max[0] = BBox.m_max[0];
	}
	if (BBox.m_min[1] < m_min[1])
	{
		m_min[1] = BBox.m_min[1];
	}
	if (BBox.m_max[1] > m_max[1])
	{
		m_max[1] = BBox.m_max[1];
	}
	if (BBox.m_min[2] < m_min[2])
	{
		m_min[2] = BBox.m_min[2];
	}
	if (BBox.m_max[2] > m_max[2])
	{
		m_max[2] = BBox.m_max[2];
	}
	return(*this);
}

VistaBoundingBox VistaBoundingBox::Include(const float Pnt[3])
{
	if (Pnt[0] < m_min[0])
	{
		m_min[0] = Pnt[0];
	}
	if (Pnt[0] > m_max[0])
	{
		m_max[0] = Pnt[0];
	}
	if (Pnt[1] < m_min[1])
	{
		m_min[1] = Pnt[1];
	}
	if (Pnt[1] > m_max[1])
	{
		m_max[1] = Pnt[1];
	}
	if (Pnt[2] < m_min[2])
	{
		m_min[2] = Pnt[2];
	}
	if (Pnt[2] > m_max[2])
	{
		m_max[2] = Pnt[2];
	}
	return(*this);
}

bool VistaBoundingBox::Intersects(const float origin[3], 
									const float direction[3], 
									const bool isRay,
									const float epsilon) const
{
	float directionNorm[3];
	directionNorm[0] = direction[0];
	directionNorm[1] = direction[1];
	directionNorm[2] = direction[2];

	// If the computation is done for a ray, normalize the
	// direction vector first.
	if (isRay == true)
	{
		const float sqrlen = (direction[0]*direction[0]
							+ direction[1]*direction[1]
							+ direction[2]*direction[2]);
		if (sqrlen>0.0)
		{
			const float inverse_length = 1.0f/(float)sqrt(sqrlen);
			directionNorm[0] *= inverse_length;
			directionNorm[1] *= inverse_length;
			directionNorm[2] *= inverse_length;
		}
	}

	float tmin = 0;
	float tmax = 0;
	bool init = true;
	// Check all three axis one by one.
	for(int i=0; i<3; ++i)
	{
		if(fabs(directionNorm[i]) > epsilon)
		{
			// Compute the parametric values for the intersection
			// points of the line and the bounding box according
			// to the current axis only.
			float tmpmin = (m_min[i] - origin[i]) / directionNorm[i];
			float tmpmax = (m_max[i] - origin[i]) / directionNorm[i];

			if (tmpmin > tmpmax)
			{
				// Switch tmpmin and tmpmax.
				const float tmp = tmpmin;
				tmpmin = tmpmax;
				tmpmax = tmp;
			}
			if (init)
			{
				tmin = tmpmin;
				tmax = tmpmax;

				if (tmax < -epsilon)
					return 0;
				if (tmin < 0.0)
					tmin = 0.0;

				if(!isRay) // is a line segment
				{
					// First intersection is outside the scope of
					// the line segment.
					if(tmin > 1.0 + epsilon)
						return 0;
					if(tmax > 1.0)
						tmax = 1.0;
				}

				init = false;
			}
			else
			{
				// This is the regular check if the direction
				// vector is non-zero along the current axis.
				if(tmpmin > tmax + epsilon)
					return 0;
				if(tmpmax < tmin - epsilon)
					return 0;
				if(tmpmin > tmin)
					tmin = tmpmin;
				if(tmpmax < tmax)
					tmax = tmpmax;
			}
		}
		else // line parallel to box
		{
			// If the ray or line segment is parallel to an axis
			// and has its origin outside the box's min and max
			// coordinate for that axis, the ray/line cannot hit
			// the box.
			if ((origin[i] < m_min[i] - epsilon)
				||	(origin[i] > m_max[i] + epsilon))
			{
				return 0;
			}
		}
	}
	return(true);
}

/**
 * Intersection of the bounding-box with the sphere given by position and radius.
 * \param fPosition The position of the center of the sphere
 * \param fRadius The radius of the sphere
 * \return bool Do the bounding box and the sphere intersect?
 * */
bool VistaBoundingBox::IntersectsSphere
		(const float fPosition[3], const float fRadius) const
{
	int iTypeOfIntersection[3] = { 0, 0, 0 };
	// detection by exclusion
	for(int iCtr = 0; iCtr < 3; ++iCtr)
	{
		if(fPosition[iCtr] < m_min[iCtr] - fRadius || m_max[iCtr] + fRadius < fPosition[iCtr])
			return false;
		else
			if(m_min[iCtr] <= fPosition[iCtr] && fPosition[iCtr] <= m_max[iCtr])
				iTypeOfIntersection[iCtr] = 1;
	}
	// okay, if we reached this point, we have determined that the center of
	// the sphere is not outside the cube given by the boundingbox extended by
	// the radius of the sphere. There's no trouble if the center of the sphere
	// is inside the boundingbox (case 3 of the following switch statement),
	// but if not one has to take into account a sphere is tested against the
	// box and at the edges of the boundingbox there are special cases.

	switch(iTypeOfIntersection[0] + iTypeOfIntersection[1] + iTypeOfIntersection[2])
	{
		case 3: // center of sphere inside the bounding box
		case 2: // center of sphere inside the extensions of the side of the bounding box
			return true;
			break;
		case 1: // center of sphere inside the extension along the edge of the bounding box
			// determine the edge which is a candidate for intersection
		{
			float fPoint[3];
			float fDirection[3] = { 0.0f, 0.0f, 0.0f };
			int iEdge[3];
			iEdge[0] = (iTypeOfIntersection[0] == 1) ? 0 : ((iTypeOfIntersection[1] == 1) ? 1 : 2);
			iEdge[1] = (iEdge[0] + 1)%3;
			iEdge[2] = (iEdge[0] + 2)%3;
			fDirection[iEdge[0]] = 1.0f;
			fPoint[iEdge[0]] = m_min[iEdge[0]];
			if(fPosition[iEdge[1]] < m_min[iEdge[1]])	fPoint[iEdge[1]] = m_min[iEdge[1]];
			else										fPoint[iEdge[1]] = m_max[iEdge[1]];
			if(fPosition[iEdge[2]] < m_min[iEdge[2]])	fPoint[iEdge[2]] = m_min[iEdge[2]];
			else										fPoint[iEdge[2]] = m_max[iEdge[2]];
			// a sphere is given by (p-c)(p-c)-r^2 = 0 and a ray by (o+td) = p
			// => (o+td-c)(o+td-c) - r^2 = 0
			// => At^2+Bt+C = 0 with A = dd, B = 2(o-c)d, C = (o-c)(o-c)-r^2
			// => if B^2 - 4AC >= 0 then there's an intersection
			// idea from http://www.geometrictools.com
			float fDiff[3] = { fPoint[0]-fPosition[0], fPoint[1]-fPosition[1], fPoint[2]-fPosition[2] };
			float fA0 = fDiff[0]*fDiff[0] + fDiff[1]*fDiff[1] + fDiff[2]*fDiff[2] - fRadius*fRadius;
			float fA1 = fDirection[0]*fDiff[0] + fDirection[1]*fDiff[1] + fDirection[2]*fDiff[2];
			if(fA1*fA1 - fA0 >= 0.0f)
				return true;
			break;
		}
		case 0: // center of sphere inside the extension of a corner of the bounding box
			// determine corner next to the position of the sphere
		{
			float fCorner[3] = { m_min[0], m_min[1], m_min[2] };
			for(int iCtr = 0; iCtr < 3; ++iCtr)
				if(fPosition[iCtr] > m_min[iCtr]) fCorner[iCtr] = m_max[iCtr];
			// compute the distance from the corner to the center of the sphere
			float fSqrDistance =
					(fCorner[0] - fPosition[0]) * (fCorner[0] - fPosition[0]) +
					(fCorner[1] - fPosition[1]) * (fCorner[1] - fPosition[1]) +
					(fCorner[2] - fPosition[2]) * (fCorner[2] - fPosition[2]);
			// if the distance (attention there's no square root therefore
			// we've to check against the squared radius) is smaller than the
			// squared radius there is an intersection
			if(fSqrDistance <= fRadius * fRadius)
				return true;
			break;
		}
	}
	return false;
}

bool VistaBoundingBox::IntersectsBox
	(const float fTransformation[16],
	 const float fWidth, const float fHeight, const float fDepth) const
{
	// some variables needed later on
	const float fMyExtend[3] = {	(m_max[0] - m_min[0])/2.0f,
									(m_max[1] - m_min[1])/2.0f,
									(m_max[2] - m_min[2])/2.0f };
	const  float fHisExtend[3] = {	fWidth /2.0f,
									fHeight/2.0f,
									fDepth /2.0f };
	const float fMyCenter[3]={	m_min[0] + fMyExtend[0],
								m_min[1] + fMyExtend[1],
								m_min[2] + fMyExtend[2] };
	const float fHisCenter[3]={	fTransformation[3],
								fTransformation[7],
								fTransformation[11] };

	// compute the axis of the rotated box
	float fHisAxis[3][3] = {{ fTransformation[ 0], fTransformation[ 4], fTransformation[ 8] },
							{ fTransformation[ 1], fTransformation[ 5], fTransformation[ 9] },
							{ fTransformation[ 2], fTransformation[ 6], fTransformation[10] } };
	float fMyAxis[3][3] = {	{ 1.0f, 0.0f, 0.0f },
							{ 0.0f, 1.0f, 0.0f },
							{ 0.0f, 0.0f, 1.0f } };
	// determine the bounding box of the rotated box
	float fMin[3] = { fHisCenter[0], fHisCenter[1], fHisCenter[2] },
		  fMax[3] = { fHisCenter[0], fHisCenter[1], fHisCenter[2] };

	int iCtrAxis = 0;
	for(iCtrAxis = 0; iCtrAxis < 3; ++iCtrAxis)
		for(int iCtr = 0; iCtr < 3; ++iCtr)
		{
			if(fHisAxis[iCtrAxis][iCtr] > 0.0f)
			{
				fMin[iCtr] -= fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
				fMax[iCtr] += fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
			}
			else
			{
				fMin[iCtr] += fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
				fMax[iCtr] -= fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
			}
		}
	if(!(m_min[0] <= fMax[0] && fMin[0] <= m_max[0])) return false;
	if(!(m_min[1] <= fMax[1] && fMin[1] <= m_max[1])) return false;
	if(!(m_min[2] <= fMax[2] && fMin[2] <= m_max[2])) return false;

	const float fMyPoints[8][3] = { { m_min[0], m_min[1], m_min[2] },
									{ m_min[0], m_min[1], m_max[2] },
									{ m_min[0], m_max[1], m_min[2] },
									{ m_min[0], m_max[1], m_max[2] },
									{ m_max[0], m_min[1], m_min[2] },
									{ m_max[0], m_min[1], m_max[2] },
									{ m_max[0], m_max[1], m_min[2] },
									{ m_max[0], m_max[1], m_max[2] } };

	for(iCtrAxis = 0; iCtrAxis < 3; ++iCtrAxis)
	{
		// compute the scalar product of all relevant points
		ProjectBoxToAxis(fHisCenter, fHisAxis[iCtrAxis], fMyPoints, fMin[iCtrAxis], fMax[iCtrAxis]);
		if(!(fMin[iCtrAxis] <= fHisExtend[iCtrAxis] && -fHisExtend[iCtrAxis] <= fMax[iCtrAxis]))
			return false;
	}

	float fExtendedAxis[3][3] = { {	fTransformation[ 0]*fHisExtend[0],
									fTransformation[ 4]*fHisExtend[0],
									fTransformation[ 8]*fHisExtend[0] },
								  { fTransformation[ 1]*fHisExtend[1],
									fTransformation[ 5]*fHisExtend[1],
									fTransformation[ 9]*fHisExtend[1] },
								  { fTransformation[ 2]*fHisExtend[2],
									fTransformation[ 6]*fHisExtend[2],
									fTransformation[10]*fHisExtend[2] } };

	const float fHisPoints[8][3] = {
		{
			fHisCenter[0]+fExtendedAxis[0][0]+fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]+fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]+fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]+fExtendedAxis[0][0]+fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]+fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]+fExtendedAxis[1][2]-fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]+fExtendedAxis[0][0]-fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]-fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]-fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]+fExtendedAxis[0][0]-fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]-fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]-fExtendedAxis[1][2]-fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]+fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]+fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]+fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]+fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]+fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]+fExtendedAxis[1][2]-fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]-fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]-fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]-fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]-fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]-fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]-fExtendedAxis[1][2]-fExtendedAxis[2][2] 
		} };

	for(int iCtrAxisA = 0; iCtrAxisA < 3; ++iCtrAxisA)
		for(int iCtrAxisB = 0; iCtrAxisB < 3; ++iCtrAxisB)
		{
			const float fAxis[3] = {	fMyAxis[iCtrAxisA][1]*fHisAxis[iCtrAxisB][2] +
										fMyAxis[iCtrAxisA][2]*fHisAxis[iCtrAxisB][1],
										fMyAxis[iCtrAxisA][0]*fHisAxis[iCtrAxisB][2] +
										fMyAxis[iCtrAxisA][2]*fHisAxis[iCtrAxisB][0],
										fMyAxis[iCtrAxisA][0]*fHisAxis[iCtrAxisB][1] +
										fMyAxis[iCtrAxisA][1]*fHisAxis[iCtrAxisB][0] };
			// cross product equals zero-vector?? If true the axis are parallel ...
			bool bAxisParallel = false;
			float epsilon = 1e-5f;
			if(-epsilon < fAxis[0] && fAxis[0] < epsilon)
				if(-epsilon < fAxis[0] && fAxis[0] < epsilon)
					if(-epsilon < fAxis[0] && fAxis[0] < epsilon)
						bAxisParallel = true;

			if(! bAxisParallel)
			{
				ProjectBoxToAxis(fMyCenter, fAxis, fMyPoints,  fMin[0], fMax[0]);
				ProjectBoxToAxis(fMyCenter, fAxis, fHisPoints, fMin[1], fMax[1]);
				if(!(fMin[0] <= fMax[1] && fMin[1] <= fMax[0]))
				{
					vstr::outi() << fMin[0] << " " << fMax[0] << "|" << fMin[1] << " " << fMax[1] << std::endl;
					return false;
				}
			}
		}
	return true;
}

/** \brief A helper function to project a box to an axis
 * The box is project to the given axis and the minimum and maximum values are returned.
 * \param float[3] The origin of the axis
 * \param float[3] The direction of the axis
 * \param float[8][3] The points of the box
 * \param float The minimum (call by reference!)
 * \param float The maximum (call by reference!)
 **/
void VistaBoundingBox::ProjectBoxToAxis
	(const float fOrigin[3], const float fAxis[3],
	 const float fBoxPoints[8][3],
	 float& fMin, float& fMax) const
{
	float fScalar[8];
	float fTBoxPoints[8][3];

	int iCtr = 0;
	for(iCtr = 0; iCtr < 8; ++iCtr)
	{
		fTBoxPoints[iCtr][0] = fBoxPoints[iCtr][0] - fOrigin[0];
		fTBoxPoints[iCtr][1] = fBoxPoints[iCtr][1] - fOrigin[1];
		fTBoxPoints[iCtr][2] = fBoxPoints[iCtr][2] - fOrigin[2];
	}

	float fNAxis[3]; // the normalized Axis
	float fLength = fAxis[0]*fAxis[0] + fAxis[1]*fAxis[1] + fAxis[2]*fAxis[2];
	fLength = sqrt(fLength);
	fNAxis[0] = fAxis[0] / fLength;
	fNAxis[1] = fAxis[1] / fLength;
	fNAxis[2] = fAxis[2] / fLength;

	for(iCtr = 0; iCtr < 8; ++iCtr)
		fScalar[iCtr] =
			fTBoxPoints[iCtr][0]*fNAxis[0] +
			fTBoxPoints[iCtr][1]*fNAxis[1] +
			fTBoxPoints[iCtr][2]*fNAxis[2];
	fMin = fScalar[0];
	fMax = fScalar[0];
	for(iCtr = 1; iCtr < 8; ++iCtr)
	{
		if(fScalar[iCtr] < fMin)
			fMin = fScalar[iCtr];
		else if(fMax < fScalar[iCtr])
			fMax = fScalar[iCtr];
	}
}

int VistaBoundingBox::Intersection(const float origin[3], 
									const float direction[3], 
									const bool isRay,
									float isect1[3], float& delta1, 
									float isect2[3], float& delta2,
									const float epsilon) const
{
	float directionNorm[3];
	directionNorm[0] = direction[0];
	directionNorm[1] = direction[1];
	directionNorm[2] = direction[2];

	// If the computation is done for a ray, normalize the
	// direction vector first.
	if (isRay == true)
	{
		const float sqrlen = (direction[0]*direction[0]
							+ direction[1]*direction[1]
							+ direction[2]*direction[2]);
		if (sqrlen>0.0)
		{
			const float inverse_length = 1.0f/(float)sqrt(sqrlen);
			directionNorm[0] *= inverse_length;
			directionNorm[1] *= inverse_length;
			directionNorm[2] *= inverse_length;
		}
	}

	float tmin = 0;
	float tmax = 0;
	bool init = true;
	// Check all three axis one by one.
	for(int i=0; i<3; ++i)
	{
		if(fabs(directionNorm[i]) > epsilon)
		{
			// Compute the parametric values for the intersection
			// points of the line and the bounding box according
			// to the current axis only.
			float tmpmin = (m_min[i] - origin[i]) / directionNorm[i];
			float tmpmax = (m_max[i] - origin[i]) / directionNorm[i];

			if (tmpmin > tmpmax)
			{
				// Switch tmpmin and tmpmax.
				const float tmp = tmpmin;
				tmpmin = tmpmax;
				tmpmax = tmp;
			}
			if (init)
			{
				tmin = tmpmin;
				tmax = tmpmax;

				if (tmax < -epsilon)
					return 0;
				if (tmin < 0.0)
					tmin = 0.0;

				if(!isRay) // is a line segment
				{
					// First intersection is outside the scope of
					// the line segment.
					if(tmin > 1.0 + epsilon)
						return 0;
					if(tmax > 1.0)
						tmax = 1.0;
				}

				init = false;
			}
			else
			{
				// This is the regular check if the direction
				// vector is non-zero along the current axis.
				if(tmpmin > tmax + epsilon)
					return 0;
				if(tmpmax < tmin - epsilon)
					return 0;
				if(tmpmin > tmin)
					tmin = tmpmin;
				if(tmpmax < tmax)
					tmax = tmpmax;
			}
		}
		else // line parallel to box
		{
			// If the ray or line segment is parallel to an axis
			// and has its origin outside the box's min and max
			// coordinate for that axis, the ray/line cannot hit
			// the box.
			if ((origin[i] < m_min[i] - epsilon)
				||	(origin[i] > m_max[i] + epsilon))
			{
				return 0;
			}
		}
	}

	// If init is still true, the vector must be of zero length.
	if (init == true)
	{
		// If the direction vector is of zero length (the line segment/ray is
		// just a single point) and it lies inside the box, make sure that the
		// method indicates this as one intersection point.
		tmin = tmax = 0.0f;
	}

	// Calculate both intersection points according to the
	// computed parameteric values.
	isect1[0] = origin[0] + tmin*directionNorm[0];
	isect1[1] = origin[1] + tmin*directionNorm[1];
	isect1[2] = origin[2] + tmin*directionNorm[2];

	isect2[0] = origin[0] + tmax*directionNorm[0];
	isect2[1] = origin[1] + tmax*directionNorm[1];
	isect2[2] = origin[2] + tmax*directionNorm[2];

	delta1 = tmin;
	delta2 = tmax;

	if( fabs(tmin-tmax) < epsilon )
		return 1;
	else
		return 2;
}

//============================================================================
//  end OF FILE VistaBoundingBox.cpp
//============================================================================
