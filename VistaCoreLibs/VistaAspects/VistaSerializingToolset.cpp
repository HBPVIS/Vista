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
void VistaSerializingToolset::Swap4(void *p)
{
  register unsigned char t;

  t=((unsigned char*)p)[0]; ((unsigned char*)p)[0]=((unsigned char*)p)[3]; ((unsigned char*)p)[3]=t;
  t=((unsigned char*)p)[1]; ((unsigned char*)p)[1]=((unsigned char*)p)[2]; ((unsigned char*)p)[2]=t;
}
;

void VistaSerializingToolset::Swap2(void *p)
{
  register unsigned char t;
  t=((unsigned char*)p)[0]; ((unsigned char*)p)[0]=((unsigned char*)p)[1]; ((unsigned char*)p)[1]=t;

}

void VistaSerializingToolset::Swap8(void *p)
{
  register unsigned char t;

  t=((unsigned char*)p)[0]; ((unsigned char*)p)[0]=((unsigned char*)p)[7]; ((unsigned char*)p)[7]=t;
  t=((unsigned char*)p)[1]; ((unsigned char*)p)[1]=((unsigned char*)p)[6]; ((unsigned char*)p)[6]=t;
  t=((unsigned char*)p)[2]; ((unsigned char*)p)[2]=((unsigned char*)p)[5]; ((unsigned char*)p)[5]=t;
  t=((unsigned char*)p)[3]; ((unsigned char*)p)[3]=((unsigned char*)p)[4]; ((unsigned char*)p)[4]=t;
}

VistaSerializingToolset::eEndianess VistaSerializingToolset::GetPlatformEndianess()
{
	if(thisMachine == VST_NONEENDIAN)
	{
		uint32 iVal = 0; // make sure this int is 32bit wide
		unsigned char ucTmp[4]; // 32bit == 4 byte, unsigned, please
		ucTmp[0] = 1; // set the first one to 1
		ucTmp[1] = ucTmp[2] = ucTmp[3] = 0; // set all others to 0

		// ucTmp[0-3] = 01 00 00 00

		// now look at this as an int32
		iVal = *((uint32*)ucTmp);

		// do we think this is 1?
		if(iVal == 1)
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

void VistaSerializingToolset::Swap(void *p, int iLength)
{
	switch(iLength)
	{
	case 2:
		Swap2(p);
		break;
	case 4:
		Swap4(p);
		break;
	case 8:
		Swap8(p);
		break;
	default:
		{
			if((iLength % 8) == 0)
			{
				Swap(p, 8, iLength/8);
			}
			else if((iLength%4) == 0)
			{
				Swap(p, 4, iLength/4);
			}
			else if((iLength%2) == 0)
			{
				Swap(p,2, iLength/2);
			}
		break;
		}
	}
}

void VistaSerializingToolset::Swap(void *p, int iLength, int iCount)
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

	unsigned char *pc = (unsigned char*) p;
	for (int j=0; j<iCount; ++j)
	{
		pSwapFunc(pc);
		pc += iLength;
	}
#else
	unsigned char *pc = (unsigned char*)p;
	for(int j=0; j < iCount; ++j)
	{
		Swap(pc, iLength);
//        iCount += iLength;
		pc += iLength;
	}
#endif
}

