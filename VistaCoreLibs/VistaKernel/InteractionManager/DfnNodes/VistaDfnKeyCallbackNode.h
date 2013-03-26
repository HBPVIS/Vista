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

#ifndef _VISTADFNCALLBACKCOUNTERNODE_H
#define _VISTADFNCALLBACKCOUNTERNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaExplicitCallbackInterface;
class VistaKeyboardSystemControl;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VISTAKERNELAPI VistaDfnKeyCallbackNode : public IVdfnNode
{
public:
	VistaDfnKeyCallbackNode( VistaKeyboardSystemControl* pKeyboard,
									const int nKeyCode, const int nModCode,
									const std::string& sDescription,
									const bool bForce );
	~VistaDfnKeyCallbackNode();

	bool PrepareEvaluationRun();
	bool GetIsValid() const;

	IVistaExplicitCallbackInterface* GetCallback() const;

protected:
	bool DoEvalNode();

	virtual unsigned int CalcUpdateNeededScore() const;

private:
	class CounterCallback;
	CounterCallback* m_pCallback;
	TVdfnPort<int>* m_pCallbackCountPort;	
	VistaKeyboardSystemControl* m_pKeyboard;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNCALLBACKCOUNTERNODE_H

