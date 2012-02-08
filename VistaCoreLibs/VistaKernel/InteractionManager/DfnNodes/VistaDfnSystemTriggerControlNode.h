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

#ifndef _VISTADFNSYSTEMTRIGGERNODE_H
#define _VISTADFNSYSTEMTRIGGERNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <vector>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


class VistaKeyboardSystemControl;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDfnSystemTriggerControlNode : public IVdfnNode
{
public:
	VistaDfnSystemTriggerControlNode( VistaKeyboardSystemControl *pCtrl );
	~VistaDfnSystemTriggerControlNode();

	bool GetIsValid() const;
	bool PrepareEvaluationRun();
protected:
	bool DoEvalNode();
private:
	VistaKeyboardSystemControl         *m_pCtrl;
	TVdfnPort< std::vector<int> >       *m_pTriggers, *m_pModders,
										*m_pNonSwallowTriggers,
										*m_pNonSwallowMods;
	TVdfnPort<int>                      *m_pTrigger,
	                                    *m_pModder,
	                                    *m_pNonSwallowTrigger,
	                                    *m_pNonSwallowMod;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNSYSTEMTRIGGERNODE_H

