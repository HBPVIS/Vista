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

#ifndef _VDFNMODULOCOUNTERNODE_H
#define _VDFNMODULOCOUNTERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnCounterNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
#include "VdfnUtil.h"

#include "VdfnOut.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * class template for a counter node
 *
 * @inport{delta,T,mandatory,inport for the value to be added to the current count}
 * @inport{reset_to_value,T,optional,resets the counter to the value at the inport}
 * @inport{modulo,T,optional,modulo value of the function
 * @outport{out,T,current value of the counter}
 *
 * The ModuloCounterNode extents the functionality of the CounterNode by applying
 * a modulo-m operation, i.e. clamping the accumulated value to the range [0,m].
 * The modulo value can either be passed as ctor parameter, or adjusted dynamically
 * by the port "modulo".
 * @see TVdfnCounterNode
 */
template<class T>
class TVdfnModuloCounterNode : public TVdfnCounterNode<T>
{
public:
	TVdfnModuloCounterNode( T oInitialValue,
							T oModulo )
		: TVdfnCounterNode<T>( oInitialValue ),
		  m_pModulo( NULL ),
		  m_oModuloValue( oModulo )
	{
		RegisterInPortPrototype( "modulo", new TVdfnPortTypeCompare<TVdfnPort<T> > );
	}

	bool PrepareEvaluationRun()
	{
		// call super-class port-connect
		TVdfnCounterNode<T>::PrepareEvaluationRun();
		m_pModulo = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "modulo", this );
		return TVdfnCounterNode<T>::GetIsValid();
	}


protected:
	bool DoEvalNode()
	{
		// Check if modulo is set, and if so, use the inport value
		if( m_pModulo )
		{
			m_oModuloValue = m_pModulo->GetValue();
			if( m_oModuloValue == 0 )
				vdfnerr << "[VdfnModuloCounterNode] Error - Encountered Modulo of 0!" << std::endl;
				return false;
		}

		//Check if a reset signal has arrived
		if( this->m_pReset && this->m_pReset->GetUpdateCounter() > this->m_iResetLastChange )
		{
			this->m_iResetLastChange = this->m_pReset->GetUpdateCounter();
			this->m_oValue = this->m_pReset->GetValue();
			vdfnout << "[Counter] Reset"  << std::endl;
		}
		else
			this->m_oValue += this->m_pDelta->GetValue();

		//m_oValue = m_oValue - T( int( m_oValue / m_oModulo ) ) * m_oModulo;
		while( this->m_oValue >= m_oModuloValue )
			this->m_oValue -= m_oModuloValue;
		while( this->m_oValue < 0 )
			this->m_oValue += m_oModuloValue;
		this->m_pOut->SetValue( this->m_oValue, TVdfnModuloCounterNode<T>::GetUpdateTimeStamp() );
		vdfnout << "[ModuloCounter] changed to " << this->m_oValue << std::endl;
		return true;
	}

	/**
	 * To prevent that a change of the modulo inport triggers an evaluation
	 * and thus adds the current value of the Delta-Port more than once,
	 * we override CalcUpdateNeededScore to only regard the Delta-Port
	 */
	unsigned int CalcUpdateNeededScore() const
	{
		return this->m_pDelta->GetUpdateCounter();
	}

private:
	TVdfnPort<T>	*m_pModulo;
	T				m_oModuloValue;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNMODULOCOUNTERNODE_H
