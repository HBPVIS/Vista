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

#include "VistaDfnKeyCallbackNode.h" 

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaDfnKeyCallbackNode::CounterCallback : public IVistaExplicitCallbackInterface
{
public:
	CounterCallback()
	: m_nCounter( 0 )
	{
	}

	virtual bool Do() 
	{
		++m_nCounter;
		return true;
	}

	int GetCounterValue() const
	{
		return m_nCounter;
	}
private:
	unsigned int m_nCounter;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaDfnKeyCallbackNode::VistaDfnKeyCallbackNode( VistaKeyboardSystemControl* pKeyboard,
																const int nKeyCode, const int nModCode,
																const std::string& sDescription,
																const bool bForce )
: IVdfnNode()
, m_pCallback( new CounterCallback )
, m_pCallbackCountPort( new TVdfnPort<int> )
, m_pKeyboard( pKeyboard )
{
	RegisterOutPort( "value", m_pCallbackCountPort );

	if( m_pKeyboard->BindAction( nKeyCode, nModCode, m_pCallback, sDescription, false, true, bForce ) == false )
	{
		delete m_pCallback;
		m_pCallback = NULL;
	}
}

VistaDfnKeyCallbackNode::~VistaDfnKeyCallbackNode()
{
	if( m_pCallback )
		m_pKeyboard->UnbindAction( m_pCallback );
	delete m_pCallback;
}
 
bool VistaDfnKeyCallbackNode::PrepareEvaluationRun()
{
	return GetIsValid();
}

bool VistaDfnKeyCallbackNode::GetIsValid() const
{
	return ( m_pCallback != NULL );
}

bool VistaDfnKeyCallbackNode::DoEvalNode()
{
	m_pCallbackCountPort->SetValue( m_pCallback->GetCounterValue(), GetUpdateTimeStamp() );

	return true;
}

unsigned int VistaDfnKeyCallbackNode::CalcUpdateNeededScore() const
{
	// we return counter value plus 1, because else with 0 no-count gets lost
	return (unsigned int)m_pCallback->GetCounterValue() + 1;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


