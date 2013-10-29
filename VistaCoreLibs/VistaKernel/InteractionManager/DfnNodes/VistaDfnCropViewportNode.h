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

#ifndef _VISTADFNCROPVIEWPORTNODE_H
#define _VISTADFNCROPVIEWPORTNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <vector>
#include <VistaBase/VistaColor.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class VistaViewport;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VISTAKERNELAPI VistaDfnCropViewportNode : public IVdfnNode
{
public:
	VistaDfnCropViewportNode( float nLeft, float nRight, float nBottom, float nTop,
							bool bUseProjExtents, const VistaColor& oCropColor );
	~VistaDfnCropViewportNode();

	void AddViewport( VistaViewport* pViewports );

	bool PrepareEvaluationRun();
	bool GetIsValid() const;

protected:
	bool DoEvalNode();

private:
	float m_nLeft;
	float m_nRight;
	float m_nBottom;
	float m_nTop;
	TVdfnPort<float>* m_pRightPort;
	TVdfnPort<float>* m_pLeftPort;
	TVdfnPort<float>* m_pTopPort;
	TVdfnPort<float>* m_pBottomPort;

	bool m_bUseProjExtents;
	VistaColor m_oCropColor;

	class CropViewportOverlay;
	std::vector<CropViewportOverlay*> m_vecOverlays;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNCROPVIEWPORTNODE_H

