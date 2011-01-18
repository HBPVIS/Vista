/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
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

#ifndef _VISTADFNDEVICEDEBUGNODE_H
#define _VISTADFNDEVICEDEBUGNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeCreators.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnHistoryPort.h>

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class IVistaSimpleTextOverlay;
class IVistaWindowingToolkit;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VdfnDeviceDebugNode : public IVdfnNode
{
public:
	VdfnDeviceDebugNode( IVistaWindowingToolkit *wta,
			              const std::string &strDriverName,
			              const std::list<std::string> &strShowList );

	~VdfnDeviceDebugNode();

	virtual void OnActivation( double dTs );
	virtual void OnDeactivation( double dTs );

	bool PrepareEvaluationRun();

	bool GetShowType() const;
	void SetShowType( bool bShowType );

	void SetColor( float r, float g, float b);
	void GetColor( float &r, float &g, float &b) const;
protected:
	bool DoEvalNode();
private:

	std::string FormatLabel(const std::string &strLabel,
			                const std::string &strType);
	void UpdateStaticLabels();
	void UpdateDynamicLabels();

	IVistaTextEntity *CreateText(int nY, const std::string &str) const;

	HistoryPort           *m_pHistory;
	IVistaWindowingToolkit *m_pWta;
	IVistaSimpleTextOverlay *m_pOverlay;
	std::string            m_strDriverName;
	bool                   m_bShowType;

	IVistaTextEntity *m_pNewMeasures,
	                                     *m_pUpdateTime,
	                                     *m_pFreq;

	typedef std::map<std::string, IVistaTextEntity*> LABMAP;
	LABMAP m_mpDynamicLabels;
	std::list<std::string> m_liShowList;
	float m_r, m_g, m_b;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNDEVICEDEBUGNODE_H

