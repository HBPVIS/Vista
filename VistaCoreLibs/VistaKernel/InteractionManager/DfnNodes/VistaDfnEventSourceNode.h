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

#ifndef _VISTADFNEVENTSOURCENODE_H
#define _VISTADFNEVENTSOURCENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnShallowNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaInteractionManager;
class VistaInteractionEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaDfnEventSourceNode : public VdfnShallowNode
{
public:
	VistaDfnEventSourceNode( VistaEventManager *pEvMgr,
					  VistaInteractionManager *pInMa,
					  const std::string &strSourceTag);

	~VistaDfnEventSourceNode();

	bool PrepareEvaluationRun();
protected:
	bool DoEvalNode();


private:
	VistaEventManager *m_pEvMgr;
	VistaInteractionEvent *m_pEvent;
	VistaInteractionManager *m_pInMa;

	typedef std::map<IVdfnPort *, unsigned int> RevisionMap;

	RevisionMap m_mpRevision;

	std::string m_strSourceTag;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNEVENTSOURCE_H

