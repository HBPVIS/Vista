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

#include "VistaCSVSerializer.h"
#include <VistaAspects/VistaSerializingToolset.h>
#include <VistaAspects/VistaSerializable.h>

#include <cstdio>
#include <cstdlib>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaCSVSerializer::VistaCSVSerializer(char separator)
:m_cSeparator (separator)
{
}

VistaCSVSerializer::~VistaCSVSerializer()
{}

int VistaCSVSerializer::WriteShort16 (  ushort16 us16Val)
{
	m_streamOutput << us16Val << m_cSeparator;
	return  sizeof(us16Val);
}

int VistaCSVSerializer::WriteInt32(  sint32 si32Val)
{
	m_streamOutput << si32Val << m_cSeparator;
	return  sizeof(si32Val);
}

int VistaCSVSerializer::WriteInt32(  uint32 si32Val)
{
	m_streamOutput << si32Val << m_cSeparator;
	return  sizeof(si32Val);
}

int VistaCSVSerializer::WriteInt64(  sint64 si64Val)
{
#if defined(WIN32)
	const char *specifier = "%I64d";
#elif defined(LINUX)
	const char *specifier = "%lld";
#else
	const char *specifier = "%d";
#endif
	// 2^64 yields 20 digits + trailing zero
	// so we use 32byte, should be enough
	char buffer[32];

	sprintf(buffer, specifier, si64Val);


	m_streamOutput << buffer << m_cSeparator;
	return  sizeof(si64Val);
}

int VistaCSVSerializer::WriteUInt64(  uint64 ui64Val)
{
#if defined(WIN32)
	const char *specifier = "%I64u";
#elif defined(LINUX)
	const char *specifier = "%lld";
#else
	const char *specifier = "%d";
#endif
	// 2^64 yields 20 digits + trailing zero
	// so we use 32byte, should be enough
	char buffer[32];
	sprintf(buffer, specifier, ui64Val);

	m_streamOutput << buffer << m_cSeparator;
	return  sizeof(ui64Val);
}


int VistaCSVSerializer::WriteFloat32(  float32 fVal)
{
	m_streamOutput << fVal << m_cSeparator;
	return  sizeof(fVal);
}

int VistaCSVSerializer::WriteFloat64(  float64 f64Val)
{
	m_streamOutput << f64Val << m_cSeparator;
	return  sizeof(f64Val);
}

int VistaCSVSerializer::WriteDouble(  double dVal)
{
	m_streamOutput << dVal << m_cSeparator;
	return  sizeof(dVal);
}

int VistaCSVSerializer::WriteString( const string &sString)
{
	m_streamOutput << sString << m_cSeparator;
	return (int)sString.length();
}

int VistaCSVSerializer::WriteDelimitedString( const string &sString, char cDelim)
{
	int iRet = WriteString(sString);
		char c[2];
		c[0] = cDelim;
		c[1] = 0x0;

		iRet += WriteString(std::string(c));

		return iRet;
}


int VistaCSVSerializer::WriteRawBuffer( const void *pBuffer, const int iLen)
{
	m_streamOutput.write ((char*)pBuffer, iLen);
	m_streamOutput << m_cSeparator;
	return iLen;
}

int VistaCSVSerializer::WriteBool( bool bVal)
{
	m_streamOutput << bVal << m_cSeparator;
	return  sizeof(bVal);
}


int VistaCSVSerializer::WriteShort16Name( const char *sVarName,  ushort16 us16Val)
{
	return WriteShort16(us16Val);
}

int VistaCSVSerializer::WriteInt32Name( const char *sVarName,  sint32 si32Val)
{
	return WriteInt32(si32Val);
}

int VistaCSVSerializer::WriteInt32Name( const char *sVarName,  uint32 si32Val)
{
	return WriteInt32(si32Val);
}

int VistaCSVSerializer::WriteInt64Name( const char *sVarName,  sint64 si64Val)
{
	return WriteInt64(si64Val);
}

int VistaCSVSerializer::WriteUInt64Name( const char *sVarName,  uint64 ui64Val)
{
	return WriteUInt64(ui64Val);
}


int VistaCSVSerializer::WriteFloat32Name( const char *sVarName,  float32 fVal)
{
	return WriteFloat32(fVal);
}

int VistaCSVSerializer::WriteFloat64Name( const char *sVarName,  float64 f64Val)
{
	return WriteFloat64(f64Val);
}

int VistaCSVSerializer::WriteDoubleName( const char *sVarName,  double dVal)
{
	return WriteDouble(dVal);
}

int VistaCSVSerializer::WriteStringName( const char *sVarName,  const string &sVal)
{
	return WriteString(sVal);
}

int VistaCSVSerializer::WriteRawBufferName(const char *sVarName, const void *pBuffer, const int iLen)
{
	return WriteRawBuffer(pBuffer, iLen);
}

int VistaCSVSerializer::WriteBoolName(const char *sVarName,  bool bVal)
{
	return WriteBool(bVal);
}

int VistaCSVSerializer::WriteSerializable(const IVistaSerializable &obj)
{
	return obj.Serialize(*this);
}

const char* VistaCSVSerializer::GetBuffer() const
{
	return m_streamOutput.str().c_str();
}

void VistaCSVSerializer::GetBufferString(std::string & strBuffer) const
{
	strBuffer = m_streamOutput.str();
}


int   VistaCSVSerializer::GetBufferSize() const
{
	return (int)m_streamOutput.str().length();
}


void VistaCSVSerializer::ClearBuffer()
{
	m_streamOutput.flush();
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

