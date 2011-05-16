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
/* $Id::                                                                    $ */

#ifndef _VISTAMATRIXCOMPOSER_H
#define _VISTAMATRIXCOMPOSER_H

/**
 * Composition and Decomposition of transformation matrices. The order of
 * transformations is done according to ISO/IEC 19775-1:2008 (VRML/X3D).
 * It features a householder decomposition as seen in Graphics Gems IV
 * (see cpp for details).
 * <em>3x faster then opensg decomposition (7x in debug mode)
 * [may vary with different matrices as this is no constant time operation!]
 * </em>
 */

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaMathConfig.h"
#include <VistaBase/VistaVectorMath.h>
//class VistaTransformMatrix;
//class VistaVector3D;
//class VistaQuaternion;

/**
 * @todo add interfaces with the "center" component?
 * attention: the center is an <em>input</em> parameter to <em>both</em>, composition and decomposition!!!
 */
/**
 * @todo change vistamatrix/vector/quaternion interfaces so that they can be
 * used where float[] is expected (add "float[] GetValue()" methods).
 */
class VISTAMATHAPI VistaMatrixDeComposer
{
public:


	/**
	 * @brief		Decompose the given transformation matrix.
	 * @param		inputMatrix					The input matrix in GL order.
	 * @param [out]	translation					The translation. 
	 * @param [out]	orientationQuatXYZW			The orientation quaternion.
	 * @param [out]	scale						The scale. 
	 * @param [out]	scaleOrientationQuatXYZW	The scale orientation quaternion.
	 */
	static void Decompose(const float inputMatrixGLOrdered[16], ///< GL order!
		float translation[3], float      orientationQuatXYZW[4],
		float       scale[3], float scaleOrientationQuatXYZW[4]);


	/**
	 * @brief		Decomposes the given transformation matrix. 
	 * @param		inputMatrix			The input matrix. 
	 * @param [out]	translation			the translation. 
	 * @param [out]	orientation			the orientation. 
	 * @param [out]	scale				the scale. 
	 * @param [out]	scaleOrientation	the scale orientation. 
	 */
	static void Decompose(const VistaTransformMatrix &inputMatrix,
		VistaVector3D &translation, VistaQuaternion &orientation,
		VistaVector3D &scale, VistaQuaternion &scaleOrientation);


	/**
	 * @brief		Composes a transformation matrix from the given components.
	 * @param [out]	outputMatrixGLOrdered	 The output matrix gl ordered.
	 * @param		translation				 The translation. 
	 * @param		orientationQuatXYZW		 The orientation quaternion. 
	 * @param		scaleOrientationQuatXYZW The scale orientation quaternion.
	 */
	static void Compose(float outputMatrixGLOrdered[16], ///< GL order!
						const float translation[3] = 0, const float      orientationQuatXYZW[4] = 0,
						const float scale      [3] = 0, const float scaleOrientationQuatXYZW[4] = 0);


	/**
	 * @brief	Composes a transformation matrix from the given components.
	 * @param	translation			The translation. 
	 * @param	orientation			The orientation. 
	 * @param	scale				The scale. 
	 * @param	scaleOrientation	The scale orientation. 
	 * @return	The composed matrix. 
	 */
	static VistaTransformMatrix Compose(
						const VistaVector3D &translation,
						const VistaQuaternion &orientation = VistaQuaternion(0, 0, 0, 1.0),
						const VistaVector3D &scale = VistaVector3D(1, 1, 1, 0),
						const VistaQuaternion &scaleOrientation = VistaQuaternion(0, 0, 0, 1.0));

	/**
	 * @brief	Composes a transformation matrix from the given components.
	 * @param [out]	outputMatrix	the output matrix. 
	 * @param	translation				The translation. 
	 * @param	orientation				The orientation. 
	 * @param	scale					The scale. 
	 * @param	scaleOrientation		The scale orientation. 
	 */
	static void Compose(VistaTransformMatrix &outputMatrix,
						const VistaVector3D &translation,
						const VistaQuaternion &orientation = VistaQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
						const VistaVector3D &scale = VistaVector3D(1, 1, 1, 0),
						const VistaQuaternion &scaleOrientation = VistaQuaternion(0, 0, 0, 1.0));
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTAMATRIXDECOMPOSITION_H
