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

#include <cstring>
#include <cstdio>

#include <string>
#include <iostream>

#include "VistaByteBufferDeSerializer.h"
#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaSerializingToolset.h>
#include <VistaAspects/VistaSerializable.h>

#include <VistaInterProcComm/VistaInterProcCommOut.h>

using namespace std;


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaByteBufferDeSerializer::VistaByteBufferDeSerializer()
: m_bDoSwap((VistaSerializingToolset::GetPlatformEndianess() == VistaSerializingToolset::VST_LITTLEENDIAN)),
  m_pExternalBuffer(0), m_bDeleteAfterUsage(false)
{
}

VistaByteBufferDeSerializer::~VistaByteBufferDeSerializer()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaByteBufferDeSerializer::DoRead(unsigned char* pBuf, int iSize, bool bSwap)
{
	unsigned int iSwapSize = (unsigned int)iSize;
	//printf("iSize=%d, vecsize = %d\n", iSize, m_vecBuffer.size());

	if(!m_pExternalBuffer)
	{
		if(m_vecBuffer.size() < (unsigned int)iSize)
		{
			printf("VistaByteBufferDeSerializer::DoRead() -- could read (%d), should read %d, nulling result\n",
			int(m_vecBuffer.size()), iSize);
			iSize = (int)m_vecBuffer.size();
		}
		//read data from internal buffer
		for(int i=0; i < iSize; ++i)
		{
			pBuf[i] = m_vecBuffer[0];
			m_vecBuffer.pop_front();
		}
	}
	else
	{
		if(m_iExternalSize-m_iCurrentBufferPos < iSize)
		{
			printf("VistaByteBufferDeSerializer::DoRead() -- could read (%d), should read %d, nulling result\n",
			m_iExternalSize-m_iCurrentBufferPos, iSize);
			iSize = m_iExternalSize-m_iCurrentBufferPos;
		}
		//read data from externally set buffer
		memcpy(pBuf, &(m_pExternalBuffer[m_iCurrentBufferPos]), iSize*sizeof(char));
		m_iCurrentBufferPos += iSize;
	}

	if(bSwap && GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*) pBuf, iSwapSize);
	return (iSwapSize==iSize);  // we indicate success iff we had enough bytes to read
}


bool VistaByteBufferDeSerializer::GetByteorderSwapFlag() const
{
	return m_bDoSwap;
}

void VistaByteBufferDeSerializer::SetByteorderSwapFlag(const bool bDoesIt )
{
	m_bDoSwap = bDoesIt;
}

int VistaByteBufferDeSerializer::ReadShort16( VistaType::ushort16 &us16Val)
{
	if(!DoRead((unsigned char*)&us16Val, sizeof(us16Val)))
		return -1;
	return sizeof(VistaType::ushort16);
}

int VistaByteBufferDeSerializer::ReadInt32( VistaType::sint32 &si32Val)
{
	if(!DoRead((unsigned char*)&si32Val, sizeof(si32Val)))
		return -1;
	return sizeof(VistaType::sint32);
}

int VistaByteBufferDeSerializer::ReadInt32( VistaType::uint32 &si32Val)
{
	if(!DoRead((unsigned char*)&si32Val, sizeof(VistaType::uint32)))
		return -1;
	return sizeof(VistaType::sint32);
}

 int VistaByteBufferDeSerializer::ReadInt64( VistaType::sint64 &si64Val)
{
	if(!DoRead((unsigned char*)&si64Val, sizeof(si64Val)))
		return -1;
	return sizeof(si64Val);
}

int VistaByteBufferDeSerializer::ReadUInt64( VistaType::uint64 &ui64Val)
{
	if(!DoRead((unsigned char*)&ui64Val, sizeof(ui64Val)))
		return -1;
	return sizeof(ui64Val);
}

 int VistaByteBufferDeSerializer::ReadFloat32( VistaType::float32 &fVal)
{
	if( !DoRead((unsigned char*)&fVal, sizeof(fVal)))
		return -1;
	return sizeof(fVal);

}

 int VistaByteBufferDeSerializer::ReadFloat64( VistaType::float64 &f64Val)
{
	if(!DoRead((unsigned char*)&f64Val, sizeof(f64Val)))
		return -1;
	return sizeof(f64Val);

}

 int VistaByteBufferDeSerializer::ReadDouble( double &dDoubleVal)
{
	if(!DoRead((unsigned char*)&dDoubleVal, sizeof(dDoubleVal)))
		return -1;
	return sizeof(dDoubleVal);

}

 int VistaByteBufferDeSerializer::ReadString(string &sIn, const int iMaxLen)
{
	sIn.resize(iMaxLen);
	if(!DoRead((unsigned char*)sIn.data(), iMaxLen, false))
		return -1;
	return iMaxLen;
}

 int VistaByteBufferDeSerializer::ReadDelimitedString(string &sString, char cDelim )
 {
	 sString.erase();

	 char pcTmp[2];
	 pcTmp[0] = 0x00;
	 pcTmp[1] = 0x00;

	 int iLength = 1;
	 int iLen = 0; /**< measure length */
	 do
	 {
		 int iRead=0;
		 if((iRead=ReadRawBuffer((void*)&pcTmp[0], iLength))==iLength)
		 {
			 if(pcTmp[0] == cDelim)
			 {
				 break; // leave loop
			 }
			 else
			 {
				 sString.append(string(&pcTmp[0]));
				 ++iLen;
			 }
		 }
		 else
		 {
			 vipcerr << "Should read: " << iLength
				 << ", but read: " << iRead << endl;
			 break;
		 }
	 }
	 while(true);

	 return iLen;
 }

int VistaByteBufferDeSerializer::ReadRawBuffer(void *pBuffer, int iLen)
{
	/** @todo check me */
	if(!DoRead((unsigned char*)pBuffer, iLen, false))
		return -1;
	return iLen;

}

 int VistaByteBufferDeSerializer::ReadBool(bool &bVal)
{
	if(!DoRead((unsigned char*)&bVal, sizeof(bVal)))
		return -1;
	return sizeof(bVal);

}

 int VistaByteBufferDeSerializer::ReadDoubleName( const char *sVarName, double &dDouble)
{
	 return ReadDouble(dDouble);
}

 int VistaByteBufferDeSerializer::ReadShort16Name( const char *sVarName, VistaType::ushort16 &us16Val)
{
	 return ReadShort16(us16Val);
}

 int VistaByteBufferDeSerializer::ReadInt32Name( const char *sVarName, VistaType::sint32 &si32Val)
{
	 return ReadInt32(si32Val);
}

int VistaByteBufferDeSerializer::ReadInt32Name( const char *sVarName, VistaType::uint32 &si32Val)
{
	 return ReadInt32(si32Val);
}


int VistaByteBufferDeSerializer::ReadInt64Name( const char *sVarName, VistaType::sint64 &si64Val)
{
	return ReadInt64(si64Val);
}

int VistaByteBufferDeSerializer::ReadUInt64Name( const char *sVarName, VistaType::uint64 &ui64Val)
{
	return ReadUInt64(ui64Val);
}

 int VistaByteBufferDeSerializer::ReadFloat32Name( const char *sVarName, VistaType::float32 &fVal)
{
	return ReadFloat32(fVal);
}

 int VistaByteBufferDeSerializer::ReadFloat64Name( const char *sVarName, VistaType::float64 &f64Val)
{
	return ReadFloat64(f64Val);
}

 int VistaByteBufferDeSerializer::ReadStringName(const char *sVarName, string &sVal, int iMaxLength)
{
	return ReadString(sVal, (int)iMaxLength);
}

int VistaByteBufferDeSerializer::ReadStringName(const char *sVarName, string &sVal, char cDelim)
{
	return ReadString(sVal, (char)cDelim);
}


 int VistaByteBufferDeSerializer::ReadRawBufferName(const char *sVarName, void *pBuffer, int iLen)
{
	return ReadRawBuffer(pBuffer, iLen);
}

 int VistaByteBufferDeSerializer::ReadBoolName(const char *sVarName, bool &bVal)
{
	return ReadBool(bVal);
}

 int VistaByteBufferDeSerializer::ReadSerializable(IVistaSerializable &obj)
{
	return obj.DeSerialize(*this);
}

bool VistaByteBufferDeSerializer::FillBuffer(const char *pcBuff, int iLength)
{
	ClearBuffer();
	for(int i=0; i < iLength; ++i)
	{
		m_vecBuffer.push_back((unsigned char)pcBuff[i]); /** @todo speed this up */
	}
	return true;
}

bool VistaByteBufferDeSerializer::SetBuffer(const char *pcBuff, int iLength, bool bDeleteAfterUse /*=false*/)
{
	ClearBuffer();
	m_pExternalBuffer = pcBuff;
	m_iExternalSize = iLength;
	m_iCurrentBufferPos = 0;
	m_bDeleteAfterUsage = bDeleteAfterUse;
	return true;
}

void VistaByteBufferDeSerializer::ClearBuffer()
{
	m_vecBuffer.clear();
	if(m_bDeleteAfterUsage)
		delete[] (char*)m_pExternalBuffer;
	m_iCurrentBufferPos = -1;
	m_bDeleteAfterUsage = false;
	m_pExternalBuffer = (char*)0x00000000;
}

unsigned int VistaByteBufferDeSerializer::GetTailSize() const
{
	if(!m_pExternalBuffer)
	{
		// using internal dequeu, tail size = number
		// of elements in queue
		return (int)m_vecBuffer.size();
	}
	else
	{
		// external buffer, rest to read is
		// iSize - curPos
		return m_iExternalSize - m_iCurrentBufferPos;
	}
}

const char *VistaByteBufferDeSerializer::GetBuffer() const
{
	if(m_pExternalBuffer)
		return m_pExternalBuffer;
	return (const char*)&m_vecBuffer[0];
}

const char *VistaByteBufferDeSerializer::GetReadHead() const
{
	if(m_pExternalBuffer)
		return &m_pExternalBuffer[m_iCurrentBufferPos];
	return (const char*)&m_vecBuffer[m_iCurrentBufferPos];
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


