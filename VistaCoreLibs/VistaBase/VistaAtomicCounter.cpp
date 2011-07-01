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
// $Id: VistaDeSerializer.cpp 4719 2009-09-10 09:29:58Z tbeer $

#include "VistaAtomicCounter.h"

#if defined(WIN32)
#include <windows.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaSigned32Atomic::VistaSigned32Atomic( VistaType::sint32 initialValue )
: TVistaAtomicCounter<VistaType::sint32>(initialValue)
  {

  }


VistaSigned64Atomic::VistaSigned64Atomic( VistaType::sint64 initialValue )
: TVistaAtomicCounter<VistaType::sint64>( initialValue )
{

}

// __LP64__ accounts for mac 64 snow leopard and above.
#if defined(__GNUC__) && ( defined(i386) || defined(__i386__) || defined(__x86_64__) || defined(__LP64__) )

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VistaSigned32Atomic
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// hand-crafted assembler for 32bit singed integer on intel machines / gnu-c compilers,
	// though the assembler instructions are supported by a number of other compilers, too.

	void VistaSigned32Atomic::Add( VistaType::sint32 value)
	{
		__asm__ __volatile__ ("lock; add{l} {%1,%0|%0,%1}"
				  	        : "+m" (m_value) : "ir" (value) : "memory");
	}

	void VistaSigned32Atomic::Sub( VistaType::sint32 value )
	{
		__asm__ __volatile__ ("lock; sub{l} {%1,%0|%0,%1}"
				  : "+m" (m_value)
				  : "ir" (value)
				  : "memory");
	}

	void VistaSigned32Atomic::Inc()
	{
		__asm__ __volatile__ ("lock; inc{l} %0"
				  : "+m" (m_value)
				  :
				  : "memory");
	}


	void VistaSigned32Atomic::Dec()
	{
		__asm__ __volatile__ ("lock; dec{l} %0"
				  : "+m" (m_value) : : "memory");
	}

	bool VistaSigned32Atomic::DecAndTestNull()
	{
		unsigned char c;
		__asm__ __volatile__("lock; dec{l} %0; sete %1"
				 :"+m" (m_value), "=qm" (c)
				 : : "memory");
		return c != 0;
	}

	bool VistaSigned32Atomic::AddAndTestNegative( VistaType::sint32 value )
	{
		unsigned char c;
		__asm__ __volatile__("lock; add{l} {%2,%0|%0,%2}; sets %1"
				 :"+m" (m_value), "=qm" (c)
				 :"ir" (value) : "memory");
		return c != 0;
	}

	VistaType::sint32 VistaSigned32Atomic::ExchangeAndAdd( VistaType::sint32 value )
	{
		VistaType::sint32 res;
		__asm__ __volatile__ ("lock; xadd{l} {%0,%1|%1,%0}"
				: "=r" (res), "+m" (m_value)
							: "0" (value)
							: "memory");
		return res;
	}


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VistaSigned64Atomic
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	 inline void VistaSigned64Atomic::Add( VistaType::sint64 value)
	 {
			__asm__ __volatile__ ("lock; add{q} {%1,%0|%0,%1}"
					  : "+m" (m_value) : "ir" (value) : "memory");
	 }

	 inline void VistaSigned64Atomic::Sub( VistaType::sint64 value )
	 {
			__asm__ __volatile__ ("lock; sub{q} {%1,%0|%0,%1}"
					  : "+m" (m_value) : "ir" (value) : "memory");
	 }

	 void VistaSigned64Atomic::Inc()
	 {
			__asm__ __volatile__ ("lock; inc{q} %0"
					  : "+m" (m_value) : : "memory");
	 }

	 void VistaSigned64Atomic::Dec()
	 {
			__asm__ __volatile__ ("lock; dec{q} %0"
					  : "+m" (m_value) : : "memory");
	 }

	 bool VistaSigned64Atomic::DecAndTestNull()
	 {
			unsigned char c;
			__asm__ __volatile__("lock; dec{q} %0; sete %1"
					 :"+m" (m_value), "=qm" (c)
					 : : "memory");
			return c != 0;
	 }

	 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 value )
	 {
			unsigned char c;
			__asm__ __volatile__("lock; add{q} {%2,%0|%0,%2}; sets %1"
					 :"+m" (m_value), "=qm" (c)
					 :"ir" (value) : "memory");
			return c != 0;
	 }

	 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 value )
	 {
		 long res;
		__asm__ __volatile__ ("lock; xadd{q} {%0,%1|%1,%0}"
				: "=r" (res), "+m" (m_value)
							: "0" (value)
							: "memory");
		return res;
	 }

#elif defined(WIN32) // we use that for win64, too as it is *always* defined according to msdn on windows
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// VistaSigned32Atomic
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		inline void VistaSigned32Atomic::Add( VistaType::sint32 value)
		{
			InterlockedExchangeAdd( (LONG*)&m_value, (LONG)value);
		}

		inline void VistaSigned32Atomic::Sub( VistaType::sint32 value )
		{
			InterlockedExchangeAdd( (LONG*)&m_value, (LONG)-value);
		}

		void VistaSigned32Atomic::Inc()
		{
			Add(1);
		}


		void VistaSigned32Atomic::Dec()
		{
			Sub(1);
		}

		bool VistaSigned32Atomic::DecAndTestNull()
		{
			// test whether the old value was 1, so a decrease would render m_value to 0
			return ( InterlockedExchangeAdd((LONG*)&m_value, -1) == 1 );
		}

		bool VistaSigned32Atomic::AddAndTestNegative( VistaType::sint32 value )
		{
			return ( InterlockedExchangeAdd((LONG*)&m_value, (LONG)-1) < 0 );
		}

		VistaType::sint32 VistaSigned32Atomic::ExchangeAndAdd( VistaType::sint32 value )
		{
			return InterlockedExchangeAdd( (LONG*)&m_value, (LONG)value );
		}


		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// VistaSigned64Atomic
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		 inline void VistaSigned64Atomic::Add( VistaType::sint64 value)
		 {
			 InterlockedExchangeAdd64( (LONGLONG*)&m_value, (LONGLONG)value);
		 }

		 inline void VistaSigned64Atomic::Sub( VistaType::sint64 value )
		 {
			 InterlockedExchangeAdd64( (LONGLONG*)&m_value, (LONGLONG)-value);
		 }

		 void VistaSigned64Atomic::Inc()
		 {
			 Add(1);
		 }

		 void VistaSigned64Atomic::Dec()
		 {
			 Sub(1);
		 }

		 bool VistaSigned64Atomic::DecAndTestNull()
		 {
			 // test whether the old value was 1, so a decrease would render m_value to 0
			 return ( InterlockedExchangeAdd64((LONGLONG*)&m_value, (LONGLONG)-1) == 1 );
		 }

		 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 value )
		 {
			 return ( InterlockedExchangeAdd64((LONGLONG*)&m_value, (LONGLONG)-1) < 0 );
		 }

		 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 value )
		 {
			 return InterlockedExchangeAdd64( (LONGLONG*)&m_value, (LONGLONG)value );
		 }
#else
	#error machine type not known for atomics.
#endif
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

