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

#ifndef _VISTADFNPROXIMITYWARNINGNODE_H
#define _VISTADFNPROXIMITYWARNINGNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class IVistaProximityWarningBase;

/**
 * Node to show different types of proximity warnings
 *
 * @inport{user_pos ,VistaVector3D, mandatory, position of the user}
 * @inport{user_ori ,VistaQuaternion, optional, orientation of the user, helpful for some types}
 * @inport{-any-, VistaVector3D, optional ,time delta to last evaluation}
 */
class VISTAKERNELAPI VistaDfnProximityWarningNode : public IVdfnNode
{
public:
	VistaDfnProximityWarningNode();
	virtual ~VistaDfnProximityWarningNode();

	void AddWarning( IVistaProximityWarningBase* pWarn );

	virtual bool PrepareEvaluationRun();
	virtual bool GetIsValid() const;
protected:
	virtual bool DoEvalNode();

	virtual bool SetInPort( const std::string &sName, IVdfnPort *pPort );

private:
	TVdfnPort<VistaVector3D>* m_pUserPositionPort;
	TVdfnPort<VistaQuaternion>* m_pUserOrientationPort;

	std::vector<TVdfnPort<VistaVector3D>*> m_vecAdditionalPorts;

	std::vector<IVistaProximityWarningBase*> m_vecWarnings;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif // _VISTADFNPROXIMITYWARNINGNODE_H
