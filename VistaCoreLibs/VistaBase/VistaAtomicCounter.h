/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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
// $Id: VistaDeSerializer.h 4719 2009-09-10 09:29:58Z tbeer $

#ifndef _VISTAATOMICCOUNTER_H_
#define _VISTAATOMICCOUNTER_H_

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

template< class baseType >
class TVistaAtomicCounter
{
public:
	virtual ~TVistaAtomicCounter() {}

	operator baseType() const { return m_value; }
	virtual void Set( baseType value ) { m_value = value; }
	baseType Get() const { return m_value; };
	virtual void Add( baseType value)  = 0;
	virtual void Sub( baseType value ) = 0;
	virtual void Inc() = 0;
	virtual void Dec() = 0;
	virtual bool DecAndTestNull() = 0;
	virtual bool AddAndTestNegative( baseType value ) = 0;
	virtual baseType ExchangeAndAdd( baseType value ) = 0;

	void operator= ( baseType value ) { Set(value); }
	void operator+=( baseType value ) { Add(value); }
	void operator-=( baseType value ) { Sub(value); }


	void operator++()
	{
		Inc();
	} // prefix operator

	TVistaAtomicCounter &operator++( int )
	{
		Inc();
		return *this;
	} // postfix operator

	void operator--()
	{
		Dec();
	} // prefix operator

	void operator--( int )
	{
		Dec();
	} // postfix operator
protected:
	TVistaAtomicCounter( baseType initialValue = 0 )
	: m_value( initialValue )
	{

	}

	volatile baseType m_value;
};

class VISTABASEAPI VistaSigned32Atomic : public TVistaAtomicCounter<VistaType::sint32>
{
public:
	VistaSigned32Atomic( VistaType::sint32 initialValue = 0 );

	virtual void Add( VistaType::sint32 value);
	virtual void Sub( VistaType::sint32 value );
	virtual void Inc();
	virtual void Dec();
	virtual bool DecAndTestNull();
	virtual bool AddAndTestNegative( VistaType::sint32 value );
	virtual VistaType::sint32 ExchangeAndAdd( VistaType::sint32 value );
};

class VISTABASEAPI VistaSigned64Atomic : public TVistaAtomicCounter<VistaType::sint64>
{
public:
	VistaSigned64Atomic( VistaType::sint64 initialValue = 0 );

	virtual void Add( VistaType::sint64 value);
	virtual void Sub( VistaType::sint64 value );
	virtual void Inc();
	virtual void Dec();
	virtual bool DecAndTestNull();
	virtual bool AddAndTestNegative( VistaType::sint64 value );
	virtual VistaType::sint64 ExchangeAndAdd( VistaType::sint64 value );
};


#endif /* _VISTAATOMICCOUNTER_H_ */


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/


