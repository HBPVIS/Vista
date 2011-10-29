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
// $Id: VdfnTypeConvertNode.h 22867 2011-08-07 15:29:00Z dr165799 $

#ifndef _VDFNTYPECONVERTNODE_H
#define _VDFNTYPECONVERTNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
#include "VdfnUtil.h"

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
 * class template for conversion operations from one type to another.
 * is a container to store a conversion function from tFrom to tTo (CFAssign).
 *
 * @inport{in,tFrom,mandatory,the inport value and type to convert to tTo}
 * @outport{out,tTo,the value of in represented using type tTo}
 *
 * Conversion using this node is done by applying a user defined conversion
 * function from tFrom to tTo. In case users want a different conversion, it
 * is suggested to re-instatiate this template using a different conversion
 * function during construction and re-register the node type using a different
 * name. The conversion function can be free floating function and is performed
 * by a copy-in, so it can be expensive.
 */
template<class tFrom, class tTo>
class TVdfnTypeConvertNode : public IVdfnNode
{
public:
	typedef tTo (*CFAssign)( const tFrom& );
	typedef TVdfnPort<tFrom> FromPort;
	typedef TVdfnPort<tTo>   ToPort;

	TVdfnTypeConvertNode( CFAssign assign )
		: IVdfnNode(),
		  m_pFrom(NULL),
		  m_pTo(new TVdfnPort<tTo>),
		  m_CfAssign(assign)
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<tFrom> >);
		RegisterOutPort( "out", m_pTo );
	}


	~TVdfnTypeConvertNode() {}

	bool PrepareEvaluationRun()
	{
		m_pFrom = VdfnUtil::GetInPortTyped<FromPort*>("in", this);
		return GetIsValid();
	}
protected:
	bool DoEvalNode()
	{
		const tFrom &vIn = m_pFrom->GetValueConstRef();
		m_pTo->SetValue( m_CfAssign(vIn), GetUpdateTimeStamp() );
		return true;
	}
private:
	FromPort *m_pFrom;
	ToPort   *m_pTo;
	CFAssign  m_CfAssign;
};

/**
 * templated node constructor, constructs nodes of type TVdfnTypeConvertNode
 */
template<class tFrom, class tTo>
class VdfnTypeConvertNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	typedef typename TVdfnTypeConvertNode<tFrom, tTo>::CFAssign AssignF;

	/**
	 * @param fct the conversion function to use for all instances created by
	          this creator.
	 */
	VdfnTypeConvertNodeCreate( AssignF fct = NULL )
		: VdfnNodeFactory::IVdfnNodeCreator(),
		  m_CFct( fct )
	{
		if( m_CFct == NULL )
			m_CFct = &VistaConversion::ConvertType<tTo, tFrom>;
	}

	/**
	 * creates an TVdfnTypeConvertNode, unconditional, no argument.
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		return new TVdfnTypeConvertNode<tFrom, tTo>( m_CFct );
	}

private:
	AssignF m_CFct;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNTYPECONVERTNODE_H

