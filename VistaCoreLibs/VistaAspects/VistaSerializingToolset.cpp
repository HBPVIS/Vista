/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#include "VistaSerializingToolset.h"
#include <VistaBase/VistaBaseTypes.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static VistaSerializingToolset::eEndianess thisMachine = VistaSerializingToolset::VST_NONEENDIAN;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSerializingToolset::VistaSerializingToolset()
{
}

VistaSerializingToolset::~VistaSerializingToolset()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaSerializingToolset::Swap4( void* pBuffer )
{
	register VistaType::byte cTmp;
	register VistaType::byte* pCharBuffer = reinterpret_cast<VistaType::byte*>( pBuffer );

	cTmp = pCharBuffer[0]; 
	pCharBuffer[0] = pCharBuffer[3];
	pCharBuffer[3] = cTmp;

	cTmp = pCharBuffer[1];
	pCharBuffer[1] = pCharBuffer[2];
	pCharBuffer[2] = cTmp;
}
;

void VistaSerializingToolset::Swap2( void* pBuffer )
{
	register VistaType::byte cTmp;
	register VistaType::byte* pCharBuffer = reinterpret_cast<VistaType::byte*>( pBuffer );

	cTmp = pCharBuffer[0];
	pCharBuffer[0] = pCharBuffer[1];
	pCharBuffer[1] = cTmp;

}

void VistaSerializingToolset::Swap8( void* pBuffer )
{
	register VistaType::byte cTmp;
	register VistaType::byte* pCharBuffer = reinterpret_cast<VistaType::byte*>( pBuffer );

	cTmp  =pCharBuffer[0];
	pCharBuffer[0] = pCharBuffer[7];
	pCharBuffer[7] = cTmp;

	cTmp = pCharBuffer[1];
	pCharBuffer[1] = pCharBuffer[6];
	pCharBuffer[6] = cTmp;

	cTmp = pCharBuffer[2];
	pCharBuffer[2] = pCharBuffer[5];
	pCharBuffer[5]  =cTmp;

	cTmp = pCharBuffer[3];
	pCharBuffer[3] = pCharBuffer[4];
	pCharBuffer[4] = cTmp;
}

VistaSerializingToolset::eEndianess VistaSerializingToolset::GetPlatformEndianess()
{
	if(thisMachine == VST_NONEENDIAN)
	{
		union
		{
			VistaType::uint32 m_nVal;
			VistaType::byte m_ucTmp[4];
		} iVal;
		
		iVal.m_ucTmp[0] = 1; // set the first one to 1
		iVal.m_ucTmp[1] = iVal.m_ucTmp[2] = iVal.m_ucTmp[3] = 0; // set all others to 0

		// ucTmp[0-3] = 01 00 00 00

		// do we think this is 1?
		if(iVal.m_nVal == 1)
			thisMachine = VST_LITTLEENDIAN; // well, we do... this is big-endian, then
		else
			thisMachine = VST_BIGENDIAN; // well, no, we do not... this is littleendian? (we might inspect this closer, but we do not ;)

		// ok, after the first call to this method, thisMachine is != NONEENDIAN
		// that means that every subsequent call on the same machine will
		// simply return the flag that is kept in thisMachine
	}

	// return the value that is valid for this machine
	return thisMachine;
}

void VistaSerializingToolset::Swap( void* pBuffer, int iLength )
{
	switch( iLength )
	{
	case 2:
		Swap2( pBuffer );
		break;
	case 4:
		Swap4( pBuffer );
		break;
	case 8:
		Swap8( pBuffer );
		break;
	default:
		{
			if( ( iLength % 8) == 0 )
			{
				Swap( pBuffer, 8, iLength/8 );
			}
			else if( (iLength % 4) == 0 )
			{
				Swap( pBuffer, 4, iLength/4 );
			}
			else if( (iLength % 2) == 0 )
			{
				Swap( pBuffer, 2, iLength/2 );
			}
			break;
		}
	}
}

void VistaSerializingToolset::Swap( void* pBuffer, int iLength, int iCount )
{
#if 1
	void (*pSwapFunc)(void *);
	switch (iLength)
	{
	case 2:
		pSwapFunc = Swap2;
		break;
	case 4:
		pSwapFunc = Swap4;
		break;
	case 8:
		pSwapFunc = Swap8;
		break;
	default:
		return;
	}

	VistaType::byte* pc = reinterpret_cast<VistaType::byte*>( pBuffer );
	for( int j = 0; j < iCount; ++j )
	{
		pSwapFunc(pc);
		pc += iLength;
	}
#else
	VistaType::byte* pc = reinterpret_cast<VistaType::byte*>( pBuffer );
	for(int j=0; j < iCount; ++j)
	{
		Swap(pc, iLength);
		//        iCount += iLength;
		pc += iLength;
	}
#endif
}

