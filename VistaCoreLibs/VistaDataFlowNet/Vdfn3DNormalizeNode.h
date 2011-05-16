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

#ifndef _VDFN3DNORMALIZENODE_H
#define _VDFN3DNORMALIZENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include "VdfnNodeFactory.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Normalizes a 3D coordinate from [0;source_max] to [min_x;min_x+target_w]
 * in each dimension. This node is useful if, for example you need a matrix
 * that will compute a normalized coordinate, for example a pixel point relative
 * to a window. As the source width and height may change, they are in-ports.
 * To cut a long story short: the output is a matrix that you can use in the
 * usual way to 'normalize' a 3D vector v: transform * v = v'
 *
 * @inport{min_x,float,optional,the minimum x value to normalize to}
 * @inport{min_y,float,optional,the minimum y value to normalize to}
 * @inport{min_z,float,optional,the minimum z value to normalize to}
 * @inport{target_z,float}
 * @inport{target_y,float}
 * @inport{target_x,float}
 * @inport{source_w,float}
 * @inport{source_d,float}
 * @inport{source_h,float}
 * @outport{transform,VistaTransformMatrix}
 */
class VISTADFNAPI Vdfn3DNormalizeNode : public IVdfnNode
{
public:
	Vdfn3DNormalizeNode();

	bool GetIsValid() const;
	bool PrepareEvaluationRun();

	void SetMinX( float f ) { m_minx = f; };
	void SetMinY( float f ) { m_miny = f; };
	void SetMinZ( float f ) { m_minz = f; };

	void SetFlipX( bool b ) { m_flipx = b; };
	void SetFlipY( bool b ) { m_flipy = b; };
	void SetFlipZ( bool b ) { m_flipz = b; };

	void SetTargetW( float f ) { m_targetw = f; };
	void SetTargetH( float f ) { m_targeth = f; };
	void SetTargetD( float f ) { m_targetd = f; };

protected:
	bool DoEvalNode();

	TVdfnPort<float> *m_pMinX,
					 *m_pMinY,
					 *m_pMinZ,
					 *m_pTgW,
					 *m_pTgH,
					 *m_pTgD,
					 *m_pSrW,
					 *m_pSrH,
					 *m_pSrD;

	TVdfnPort<bool> *m_pFlipX,
					*m_pFlipY,
					*m_pFlipZ;

	float m_minx;
	float m_miny;
	float m_minz;

	float m_targetw;
	float m_targeth;
	float m_targetd;

	bool m_flipx;
	bool m_flipy;
	bool m_flipz;

	TVdfnPort<VistaTransformMatrix> *m_pOut;

};

class VISTADFNAPI Vista3DNormalizeNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	Vista3DNormalizeNodeCreate()
	{

	}

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		Vdfn3DNormalizeNode *pNode = new Vdfn3DNormalizeNode;
        try {
            const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
            if (prams.HasProperty("min_x")) {
                pNode->SetMinX((float)prams.GetDoubleValue(("min_x")));
            }
            if (prams.HasProperty("min_y")) {
                pNode->SetMinY((float)prams.GetDoubleValue(("min_y")));
            }
            if (prams.HasProperty("min_z")) {
                pNode->SetMinZ((float)prams.GetDoubleValue(("min_z")));
            }
            if (prams.HasProperty("target_w")) {
                pNode->SetTargetW((float)prams.GetDoubleValue(("target_w")));
            }
            if (prams.HasProperty("target_h")) {
                pNode->SetTargetH((float)prams.GetDoubleValue(("target_h")));
            }
            if (prams.HasProperty("target_d")) {
                pNode->SetTargetD((float)prams.GetDoubleValue(("target_d")));
            }
            if (prams.HasProperty("flip_x")) {
                pNode->SetFlipX(prams.GetBoolValue(("flip_x")));
            }
            if (prams.HasProperty("flip_y")) {
                pNode->SetFlipY(prams.GetBoolValue(("flip_y")));
            }
            if (prams.HasProperty("flip_z")) {
                pNode->SetFlipZ(prams.GetBoolValue(("flip_z")));
            }
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return pNode;
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFN3DNORMALIZENODE_H

