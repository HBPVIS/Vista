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

#ifndef _VISTAGEOMFACTORY_H
#define _VISTAGEOMFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSceneGraph;
class VistaColor;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This factory needs to work properly an instance of VistaSceneGraph.
 * All methods can be called without any parameters.
 */
class VISTAKERNELAPI VistaGeometryFactory
{
public:

	VistaGeometryFactory(VistaSceneGraph *pSG);
	virtual ~VistaGeometryFactory();

	/**
	 * Simple plane
	 */
	VistaGeometry* CreatePlane(
		float sizeX = 1, float sizeZ = 1,
		int resolutionX = 1.0f, int resolutionZ = 1.0f,
		int facing = Vista::Y,
		VistaColor color = VistaColor::WHITE
	);

	/**
	 * The good old box
	 */
	VistaGeometry* CreateBox (
		float sizeX = 1.0f, float sizeY = 1.0f, float sizeZ = 1.0f,
		int resolutionX = 1, int resolutionY = 1, int resolutionZ = 1,
		VistaColor color = VistaColor::WHITE
	);

	/**
	 * A round plane...
	 */
	VistaGeometry* CreateDisk(
		float radius = 0.5f,
		int resolutionC = 16, int resolutionD = 1,
		int normal = Vista::Y,
		VistaColor color = VistaColor::WHITE
	);

	/**
	 * Creates a Cone.
	 * Note: A pyramid is a cone with bad resolution ;)
	 */
	VistaGeometry* CreateCone(
		float radiusBottom = 0.5f, float radiusTop = 0.5f, float height = 1.0f,
		int resolutionC = 16, int resolutionD = 1, int resolutionY = 1,
		VistaColor color = VistaColor::WHITE,
		bool bottom = true, bool top = true, bool sides = true
	);

	/**
	 * Create a Torus with given parameters.
	 */
	VistaGeometry* CreateTorus(
		float ringRadius = 0.1f, float outerRadius = 0.4f,
		int resolutionSides = 30, int resolutionRing = 30,
		VistaColor color = VistaColor::WHITE
	);

	/**
	 * Creates an Ellipsiod with the given radius
	 * and precision in each direction.
	 */
	VistaGeometry* CreateEllipsoid(
		float radius_a = 0.5f,
		float radius_b = 0.5f,
		float radius_c = 0.5f,
		int thetaPrecision = 32,
		int phiPrecision = 32,
		const VistaColor & color = VistaColor::WHITE
	);

	/**
	 * Simplified interface to provide a sphere as a perfect round
	 * ellipsoid.
	 */
	VistaGeometry* CreateSphere(
		float radius = 0.5f,
		int resolution = 32,
		VistaColor color = VistaColor::WHITE
	);
	
	/**
	 * Creates a triangle between the points a,b & c.
	 * The resolution gives the amount of trianlge rows.
	 * The amount of inner triangles is resolution^2
	 */
	VistaGeometry* CreateTriangle(
		const VistaVector3D & a = VistaVector3D(-1.0f/2,-1.0f/2,0),
		const VistaVector3D & b = VistaVector3D( 1.0f/2,-1.0f/2,0),
		const VistaVector3D & c = VistaVector3D(      0, 1.0f/2,0),
		int resolution = 3,
		const VistaColor & color = VistaColor::WHITE
	);
	
	/**
	 * Call this method to create geometries from a PropertyList.
	 * Please have a look at the code to get the supportet
	 * attributes.
	 */
	VistaGeometry* CreateFromPropertyList( const VistaPropertyList &props );

protected:
	VistaSceneGraph* GetSG() const;

private:
	VistaSceneGraph *m_pSG;
};

#endif
