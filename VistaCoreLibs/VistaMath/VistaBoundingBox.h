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

#ifndef VISTABOUNDINGBOX_H
#define VISTABOUNDINGBOX_H

//============================================================================
//  INCLUDES
//============================================================================

#include "VistaMathConfig.h"

//============================================================================
//  FORWARD DECLARATIONS
//============================================================================

class VistaTransformMatrix;

//============================================================================
//  MAKROS AND DEFINES
//============================================================================


//============================================================================
//  CLASS DEFINITIONS
//============================================================================

/**
 * \brief Representation of an axis-aligned bounding box.
 *
 * This class can be used to represent axis-aligned bounding boxes and
 * perform computations like intersection with a ray or line segment on it.
 */
class VISTAMATHAPI VistaBoundingBox
{
public:
	/// Default constructor.
	VistaBoundingBox();
	/// Constructor overload.
	VistaBoundingBox (const float min[3], const float max[3]);
	/// Constructor for a triangle.
	VistaBoundingBox(const float a[3], const float b[3], const float c[3]);
	/// Destructor.
	~VistaBoundingBox();

	/// Test if the bounding box is of zero volume.
	bool IsEmpty() const;
	/// Set boundaries of the bounding box.
	void SetBounds(const float min[3], const float max[3]);
	/// Get bounds
	void GetBounds(float min[3], float max[3]) const;
	/// Increase the size of the bounding box by adding the specified value to all sides.
	VistaBoundingBox Expand(const float &size);

	/// Get the dimensions of the bounding box.
	void GetSize(float& width, float& height, float& depth) const;
	/// Get the length of the diagonal of the bounding box.
	float GetDiagonalLength() const;
	/// Get the volume of the bounding box.
	float GetVolume() const;
	/// Get the center of the bounding box.
	void GetCenter(float center[3]) const;

	/// Test if the specified point lies inside the bounding box.
	bool Intersects (const float point[3]) const;
	/// Test intersection with a second AABB.
	bool Intersects (const VistaBoundingBox& BBox) const;
	/// Test intersection with a ray or line segment.
	bool Intersects(const float origin[3],
					 const float direction[3],
					 const bool isRay,
					 const float epsilon=0.00001f) const;
	/// Does the sphere (given by position and radius) intersect the boundingbox?
	bool IntersectsSphere(const float[3], const float) const;
	/// Does the box (given by transformation and width, height and depth) intersect the boundingbox?

	bool IntersectsBox(const float[16], const float, const float, const float) const;

	/** @todo those tests are missing */
	// bool IntersectsCylinder(const float[16], const float, const float) const
	// bool IntersectsCone(const float[16], const float, const float) const

	/// Test if this AABB encloses the specified AABB.
	bool Contains (const VistaBoundingBox& BBox) const;

	/// Resize the bounding box such that the specified point is included.
	VistaBoundingBox Include(const float Pnt[3]);

	/// Resize the bounding box such that the specified box is included.
	VistaBoundingBox Include(const VistaBoundingBox& BBox);

	/**
	 * \brief Intersection of a ray or line segment with the bounding box.
	 *
	 * A ray or line segment is specified by its origin and its direction
	 * vector. The return value indicates the number of intersection points
	 * with the bounding box.
	 *
	 * \param origin Point of origin of the ray or line segment.
	 * \param direction Direction vector of the ray or line segment. In case
	 *                  isRay is true, the vector will be normalized.
	 *                  Otherwise, the direction vector also indicates the
	 *                  length of the line segment.
	 * \param isRay If true, the computation is done for a ray, otherwise a
	 *              line segment.
	 * \param isect1 This parameter will be overwritten with the
	 *               coordinates of the first intersection point.
	 * \param isect2 This parameter will be overwritten with the
	 *               coordinates of the second intersection point.
	 * \param delta1 This parameter will be overwritten with the
	 *               parameteric value of the first intersection point.
	 * \param delta2 This parameter will be overwritten with the
	 *               parameteric value of the second intersection point.
	 * \param epsilon Epsilon used to compensate floating precision errors.
	 *
	 * \note In case the line segment lies or ends inside the box, still
	 *       two intersection points are indicated and the end of the line
	 *       is returned as second intersection. The only case where one
	 *       intersection point is indicated is if the ray/line is of zero
	 *       length.
	 * \note In case the ray/line does not intersect the box, all parameters
	 *       passed by reference will not be changed.
	 */
	int Intersection(const float origin[3],
					 const float direction[3],
					 const bool isRay,
					 float isect1[3], float& delta1,
					 float isect2[3], float& delta2,
					 const float epsilon=0.00001f) const;

	static bool ComputeAABB(const float p1[3],
						const float p2[3],
						const VistaTransformMatrix &mtWorldTransform,
						VistaBoundingBox &bbOut);


private:
	/// A helper function to deceide if there is an intersection?!
	void ProjectBoxToAxis
		(const float[3], const float[3], const float[8][3],
		 float&, float&) const;

public:
	/// Minimum coordinates x/y/z of the bounding box.
	float	m_min[3];
	/// Maximum coordinates x/y/z of the bounding box.
	float	m_max[3];
};

//============================================================================
//  LOCAL VARS AND FUNCS
//============================================================================

//============================================================================
//  END OF FILE VistaCollisionOctree.h
//============================================================================

#endif // VISTABOUNDINGBOX_H
