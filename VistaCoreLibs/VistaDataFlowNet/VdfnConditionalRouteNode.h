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
// $Id: VdfnConditionalRouteNode.h 20730 2011-03-30 15:56:24Z dr165799 $

#ifndef _VDFNCONDITIONALROUTENODE_H__
#define _VDFNCONDITIONALROUTENODE_H__


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDataFlowNet/VdfnConfig.h>

#include "VdfnSerializer.h"
#include "VdfnShallowNode.h"
#include "VdfnNodeFactory.h"
#include "VdfnPort.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#include <map>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTADFNAPI VdfnConditionalRouteNode : public VdfnShallowNode
{
public:
	VdfnConditionalRouteNode();
	virtual ~VdfnConditionalRouteNode();

	virtual bool SetInPort(const std::string &sName,
						   IVdfnPort *pPort );

	bool PrepareEvaluationRun();

protected:
	bool DoEvalNode();
	virtual unsigned int    CalcUpdateNeededScore() const;

private:
	typedef std::map<IVdfnPort*, unsigned int> REVMAP;

	mutable unsigned int m_nUpdateScore;
	REVMAP m_mpRevisions;
	TVdfnPort<bool> *m_pTrigger;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNCONDITIONALROUTENODE_H__

