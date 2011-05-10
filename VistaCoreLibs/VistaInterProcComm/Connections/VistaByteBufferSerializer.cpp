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

#include "VistaByteBufferSerializer.h"
#include <VistaAspects/VistaSerializingToolset.h>
#include <VistaAspects/VistaSerializable.h>

#include <cstring>
#include <cstdio>
#include <cassert>
#include <string>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaByteBufferSerializer::VistaByteBufferSerializer(unsigned int uiInitialBufferSize)
: IVistaSerializer(),
  m_iWriteHead(0),
  //m_uiCapacity(0),
  m_bRetrimSize(true)
{
	m_vecBuffer.resize(uiInitialBufferSize);
	m_uiCapacity = (unsigned int)m_vecBuffer.capacity();
	if( uiInitialBufferSize > 0 )
		m_pHead = &(m_vecBuffer[0]);
	else
		m_pHead = NULL;
	m_bDoSwap = (VistaSerializingToolset::GetPlatformEndianess() == VistaSerializingToolset::VST_LITTLEENDIAN);
}

VistaByteBufferSerializer::~VistaByteBufferSerializer()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
int VistaByteBufferSerializer::WriteValue(char *pcValue, int iLength)
{
	int iSum = m_iWriteHead+iLength;
	if(m_uiCapacity < (unsigned int)iSum)
	{
		// we shall not retrim the size
		if(!m_bRetrimSize)
		{
			// try to adjust the size according to the space left
			iLength > iSum-(int)m_uiCapacity ? 0 : iSum-(int)m_uiCapacity;

			if(iLength == 0) // no more space left
				return -1;
		}
		else
		{
			// resize, make some space
			// 2*oldCapacity+oldSpace+newNeed (iSum)
			m_vecBuffer.resize((2*m_uiCapacity)+iSum);

			// cache capacity
			m_uiCapacity = (unsigned int)m_vecBuffer.capacity();

			// STL vectors might copy memory to
			// a new region if necessary, se recache the head pointer
			m_pHead = &(m_vecBuffer[0]);
		}
	}
	register char *p = pcValue;
	register char *pIn = m_pHead+m_iWriteHead;

	memcpy(pIn, p, iLength*sizeof(char));
	m_iWriteHead += iLength;

	return iLength;
}

bool VistaByteBufferSerializer::GetByteorderSwapFlag() const
{
	return m_bDoSwap;
}


void VistaByteBufferSerializer::SetByteorderSwapFlag(const bool bDoesIt )
{
	m_bDoSwap = bDoesIt;
}

int VistaByteBufferSerializer::WriteShort16(  ushort16 us16Val)
{
	fprintf (stderr, "FOOBAR\n");
      if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&us16Val, sizeof(ushort16));
	return WriteValue((char*)&us16Val, sizeof(ushort16));
}

int VistaByteBufferSerializer::WriteInt32(  sint32 si32Val)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&si32Val, sizeof(sint32));
	return WriteValue((char*)&si32Val, sizeof(si32Val));
}

int VistaByteBufferSerializer::WriteInt32(  uint32 si32Val)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&si32Val, sizeof(uint32));
	return WriteValue((char*)&si32Val, sizeof(si32Val));
}

int VistaByteBufferSerializer::WriteInt64(  sint64 si64Val)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&si64Val, sizeof(sint64));
	return WriteValue((char*)&si64Val, sizeof(si64Val));
}

int VistaByteBufferSerializer::WriteUInt64(  uint64 ui64Val)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&ui64Val, sizeof(uint64));
	return WriteValue((char*)&ui64Val, sizeof(ui64Val));
}

int VistaByteBufferSerializer::WriteFloat32(  float32 fVal)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&fVal, sizeof(float32));
	return WriteValue((char*)&fVal, sizeof(fVal));
}

int VistaByteBufferSerializer::WriteFloat64(  float64 f64Val)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&f64Val, sizeof(float64));
	return WriteValue((char*)&f64Val, sizeof(f64Val));
}

int VistaByteBufferSerializer::WriteDouble(  double dVal)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&dVal, sizeof(double));
	return WriteValue((char*)&dVal, sizeof(dVal));
}

int VistaByteBufferSerializer::WriteDelimitedString( const string &sString, char delim)
{
	int iLength = (int)sString.size();
	//int iSize //= WriteValue((unsigned char*)&iLength, sizeof(int));
	int iSize = WriteValue((char*) sString.c_str(), iLength);
	iSize += WriteValue((char*) &delim, 1);
	return iSize;
}

int VistaByteBufferSerializer::WriteString( const string &sString)
{
	int iLength = (int)sString.size();
	//int iSize //= WriteValue((unsigned char*)&iLength, sizeof(int));
	int iSize = WriteValue((char*)sString.c_str(), iLength);
	return iSize;
}

int VistaByteBufferSerializer::WriteRawBuffer( const void *pBuffer, const int iLen)
{
	return WriteValue((char*)pBuffer, iLen);
}

int VistaByteBufferSerializer::WriteBool( bool bVal)
{
	if(GetByteorderSwapFlag())
		VistaSerializingToolset::Swap((void*)&bVal, sizeof(bool));
	return WriteValue((char*)&bVal, sizeof(bVal));
}


int VistaByteBufferSerializer::WriteShort16Name( const char *sVarName,  ushort16 us16Val)
{
	return WriteShort16(us16Val);
}

int VistaByteBufferSerializer::WriteInt32Name( const char *sVarName,  sint32 si32Val)
{
	return WriteInt32(si32Val);
}

int VistaByteBufferSerializer::WriteInt32Name( const char *sVarName,  uint32 si32Val)
{
	return WriteInt32(si32Val);
}


int VistaByteBufferSerializer::WriteInt64Name( const char *sVarName,  sint64 si64Val)
{
	return WriteInt64(si64Val);
}

int VistaByteBufferSerializer::WriteUInt64Name( const char *sVarName,  uint64 ui64Val)
{
	return WriteUInt64(ui64Val);
}


int VistaByteBufferSerializer::WriteFloat32Name( const char *sVarName,  float32 fVal)
{
	return WriteFloat32(fVal);
}

int VistaByteBufferSerializer::WriteFloat64Name( const char *sVarName,  float64 f64Val)
{
	return WriteFloat64(f64Val);
}

int VistaByteBufferSerializer::WriteDoubleName( const char *sVarName,  double dVal)
{
	return WriteDouble(dVal);
}

int VistaByteBufferSerializer::WriteStringName( const char *sVarName,  const string &sVal)
{
	return WriteString(sVal);
}

int VistaByteBufferSerializer::WriteRawBufferName(const char *sVarName, const void *pBuffer, const int iLen)
{
	return WriteRawBuffer(pBuffer, iLen);
}

int VistaByteBufferSerializer::WriteBoolName(const char *sVarName,  bool bVal)
{
	return WriteBool(bVal);
}

int VistaByteBufferSerializer::WriteSerializable(const IVistaSerializable &obj)
{
	return obj.Serialize(*this);
}

char *VistaByteBufferSerializer::GetBuffer() const
{
	return const_cast<char *>(m_pHead);
}

int   VistaByteBufferSerializer::GetBufferSize() const
{
	return m_iWriteHead;
//    return m_vecBuffer.size();
}

void VistaByteBufferSerializer::GetBuffer(vector<unsigned char> &veStore)
{
	veStore.resize(m_iWriteHead);
	memcpy(&veStore[0], m_pHead, m_iWriteHead);
}

void VistaByteBufferSerializer::ClearBuffer()
{
	//m_vecBuffer.clear();
	//m_pHead = &m_vecBuffer[0];
	m_iWriteHead = 0;
}

void VistaByteBufferSerializer::SetBufferCapacity(int iNewSize)
{
	if(m_bRetrimSize)
	{
		m_vecBuffer.resize(iNewSize);
		m_uiCapacity = (unsigned int)m_vecBuffer.capacity();
		m_pHead = &(m_vecBuffer[0]);
	}
}


int  VistaByteBufferSerializer::GetBufferCapacity() const
{
	return m_uiCapacity; // this is always correct, even for adopted buffers
}

void VistaByteBufferSerializer::SetBuffer(char *pvBuffer,
										   int iBufferSize,
										   int iWriteHead)
{
	assert( iBufferSize > 0 );
	if(pvBuffer == NULL)
	{
		// relink to internal buffer
		m_vecBuffer.resize(iBufferSize);
		m_pHead = &(m_vecBuffer[0]);
		m_uiCapacity = (int)m_vecBuffer.capacity();
		m_iWriteHead = 0;
		m_bRetrimSize = true;
	}
	else
	{
		// link to external buffer
		m_bRetrimSize = false;
		m_pHead = pvBuffer;
		m_iWriteHead = iWriteHead;
		m_uiCapacity = iBufferSize;
	}
}

void VistaByteBufferSerializer::DumpCurrentBuffer() const
{
	printf("VistaByteBufferSerializer::DumpCurrentBuffer() -- size = %d\n",
			int(m_vecBuffer.size()));

   for(int i=0; i < m_iWriteHead; ++i)
	{
		printf("%02x ", m_vecBuffer[i]);
		if(i && (i%16==0))
			printf("\n");
	}
	printf("\n");
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


