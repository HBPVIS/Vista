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
// $Id: VistaDfnMouseWheelChangeDetectNode.h 20730 2011-03-30 15:56:24Z dr165799 $

#ifndef _VISTADFNMOUSEWHEELCONVERERNODE_H
#define _VISTADFNMOUSEWHEELCONVERERNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDfnMouseWheelChangeDetectNode : public IVdfnNode
{
public:
	VistaDfnMouseWheelChangeDetectNode();
	~VistaDfnMouseWheelChangeDetectNode();

	bool PrepareEvaluationRun();
protected:
	virtual bool DoEvalNode();
private:
	TVdfnPort<int>*				m_pWheelState;
	TVdfnPort<int>*				m_pWheelChange;

	int							m_iLastState;
};

class VISTAKERNELAPI VistaDfnMouseWheelChangeDetectNodeCreate
								: public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnMouseWheelChangeDetectNodeCreate();

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNMOUSEWHEELCONVERERNODE_H

