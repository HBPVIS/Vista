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

#include "VistaDfnDeviceDebugNode.h"

#include <VistaKernel/WindowingToolkit/VistaSimpleTextOverlay.h>
#include <VistaKernel/WindowingToolkit/VistaTextEntity.h>
#include <VistaKernel/WindowingToolkit/VistaWindowingToolkit.h>

#include <VistaBase/VistaTimerImp.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

#include <algorithm>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnDeviceDebugNode::VdfnDeviceDebugNode(
		IVistaWindowingToolkit *wta,
		const std::string &strDriverName,
		const std::list<std::string> &showList)
: IVdfnNode(),
m_pHistory(NULL),
m_pWta(wta),
m_strDriverName(strDriverName),
m_liShowList(showList),
m_bShowType(false),
m_r(1.0), m_g(0.0), m_b(0.0),
m_pNewMeasures(NULL) // deleted by the overlay, iff created
 , m_pUpdateTime( NULL )
 , m_pFreq(NULL)
, m_pOverlay(m_pWta->CreateSimpleTextOverlay())
{
	this->SetEvaluationFlag(true);
	RegisterInPortPrototype("history", new HistoryPortCompare);
}

VdfnDeviceDebugNode::~VdfnDeviceDebugNode()
{
	delete m_pOverlay;
}

IVistaTextEntity *VdfnDeviceDebugNode::CreateText( int nY, const std::string &sText ) const
{
	IVistaTextEntity *pRet = m_pWta->CreateTextEntity();
	pRet->SetYPos( (float)nY );
	pRet->SetText( sText );
	pRet->SetColor(VistaColorRGB(m_r, m_g, m_b));

	return pRet;
}

bool VdfnDeviceDebugNode::PrepareEvaluationRun()
{
	m_pHistory = dynamic_cast<HistoryPort*>(GetInPort("history"));
	if(m_pHistory)
	{
		int nLine = 1;

		IVistaTextEntity *pName = CreateText(nLine++, "DRIVER: " + m_strDriverName );
		m_pOverlay->AddText(pName);

		m_pUpdateTime = CreateText(nLine++, "avg upd time: ");
		m_pOverlay->AddText(m_pUpdateTime);

		m_pFreq = CreateText(nLine++, "upd freq:");
		m_pOverlay->AddText(m_pFreq);

		// cache values
		VistaDeviceSensor *pSensor         = m_pHistory->GetValue()->m_pSensor;
		IVistaMeasureTranscode  *pTranscode = m_pHistory->GetValue()->m_pTranscode;

		pName = CreateText(nLine++, "Sensor: " + pSensor->GetSensorName());
		m_pOverlay->AddText(pName);

		m_pNewMeasures = CreateText(nLine++, "New: "
				   + VistaAspectsConversionStuff::ConvertToString(
						   m_pHistory->GetValue()->m_nNewMeasures));
		m_pOverlay->AddText(m_pNewMeasures);

		// dynamic labels
		std::list<std::string> liProps;
		pTranscode->GetPropertySymbolList(liProps);
		for(std::list<std::string>::const_iterator it = liProps.begin();
			it != liProps.end(); ++it)
		{
			if(!m_liShowList.empty())
			{
				if(std::find( m_liShowList.begin(), m_liShowList.end(), *it) == m_liShowList.end())
					continue;
			}
			IVistaTextEntity *pE = CreateText(nLine++, (*it) + ": " + pTranscode->GetPropertyByName(*it).GetValue());
			m_pOverlay->AddText(pE);
			m_mpDynamicLabels[(*it)] = pE;
		}
	}
	else
	{
		IVistaTextEntity *pError = CreateText(2, "NOT CONNECTED");
		m_pOverlay->AddText(pError);
	}

	return GetIsValid();
}

bool VdfnDeviceDebugNode::DoEvalNode()
{
	UpdateStaticLabels();
	UpdateDynamicLabels();
	return true;
}


std::string VdfnDeviceDebugNode::FormatLabel(const std::string &strLabel,
		                                      const std::string &strType )
{
	if(m_bShowType)
		return strLabel + "[" + strType + "]: ";
	else
		return strLabel + ": ";
}

void VdfnDeviceDebugNode::UpdateStaticLabels()
{
	m_pNewMeasures->SetText("New: "
							+ VistaAspectsConversionStuff::ConvertToString(
							  m_pHistory->GetValue()->m_nNewMeasures));

	// update value from driver
	m_pUpdateTime->SetText("avg upd time: "
						   + VistaAspectsConversionStuff::ConvertToString( double(m_pHistory->GetValue()->m_nAvgDriverUpdTime)));
	m_pFreq->SetText("avg upd freq: "
					 + VistaAspectsConversionStuff::ConvertToString( double( m_pHistory->GetValue()->m_nAvgUpdFreq )));
}

void VdfnDeviceDebugNode::UpdateDynamicLabels()
{
	VdfnPortFactory *pFac = VdfnPortFactory::GetSingleton();


	for(LABMAP::const_iterator it = m_mpDynamicLabels.begin();
		it != m_mpDynamicLabels.end(); ++it)
	{
		IVistaTextEntity *pE = (*it).second;

		IVistaMeasureTranscode::ITranscodeGet *pTr
				= m_pHistory->GetValue()->m_pTranscode->GetMeasureProperty( (*it).first );
		if(pTr)
		{
			// ok, at least it is a transcode.
			VdfnPortFactory::CFunctorAccess *pFcA = pFac->GetFunctorAccess( pTr->GetReturnType().name() );
			if(pFcA)
			{
				// ok, we have an access
				if(pFcA->m_pGet)
				{
					std::string strOut;
					const VistaSensorMeasure *pLatest = m_pHistory->GetValue()->m_oHistory.GetCurrentRead();
					if(pLatest == NULL)
						return;

					if(pFcA->m_pGet->GetStringValue(pTr, pLatest, strOut))
					{
						pE->SetText(FormatLabel((*it).first, pTr->GetReturnType().name()) + strOut);
					}
					else
					{
						pE->SetText(FormatLabel((*it).first, pTr->GetReturnType().name()) + "<fail>");
					}
				}
				else
				{
					pE->SetText(FormatLabel((*it).first, pTr->GetReturnType().name()) + "<no-string-get>");
				}
			}
			else
			{
				pE->SetText(FormatLabel((*it).first, pTr->GetReturnType().name()) + "<no-access>");
			}
		}
		else
		{
			// no transcode, try normal property get
			VistaProperty prop((*it).first);
			if(m_pHistory->GetValue()->m_pTranscode->GetProperty(prop))
			{
				pE->SetText(FormatLabel((*it).first,"<none-set>") + prop.GetValue());
			}
			else
				pE->SetText(FormatLabel((*it).first, "<none-set>") + "<no-transcode / prop-get>");
		}

	}
}

bool VdfnDeviceDebugNode::GetShowType() const
{
	return m_bShowType;
}

void VdfnDeviceDebugNode::SetShowType( bool bShowType )
{
	m_bShowType = bShowType;
}

void VdfnDeviceDebugNode::SetColor( float r, float g, float b)
{
	m_r = r;
	m_g = g;
	m_b = b;

	for(LABMAP::iterator it = m_mpDynamicLabels.begin(); it != m_mpDynamicLabels.end(); ++it)
	{
		(*it).second->SetColor(VistaColorRGB(m_r, m_g, m_b));
	}
}

void VdfnDeviceDebugNode::GetColor( float &r, float &g, float &b) const
{
	r = m_r;
	g = m_g;
	b = m_b;
}

void VdfnDeviceDebugNode::OnActivation( double dTs )
{
    IVdfnNode::OnActivation(dTs);
    m_pNewMeasures->SetEnabled(true);
    for(LABMAP::iterator it = m_mpDynamicLabels.begin(); it != m_mpDynamicLabels.end(); ++it)
        (*it).second->SetEnabled(true);
}

void VdfnDeviceDebugNode::OnDeactivation( double dTs )
{
    IVdfnNode::OnDeactivation(dTs);
    m_pNewMeasures->SetEnabled(false);
    for(LABMAP::iterator it = m_mpDynamicLabels.begin(); it != m_mpDynamicLabels.end(); ++it)
        (*it).second->SetEnabled(false);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


