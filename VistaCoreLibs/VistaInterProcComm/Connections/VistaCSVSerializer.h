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

#ifndef _VISTACSVSERIALIZER_H
#define _VISTACSVSERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaAspects/VistaSerializer.h>

#include <sstream>

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
 * VistaCSVSerializer
 *
 *  Serializer class for "character separated values" buffers. You can choose a separator character
 * ( ';' is standard ) and the serializer will write values to an output string stream
 *  separated by these separator characters.
 *
 *  There could be another implementation with a "marking" character, to mark occurrences of m_cSeparator
 *  in values. In the current implementation, m_cSeparator must not occur in any value !!!
 */
class VISTAINTERPROCCOMMAPI VistaCSVSerializer : public IVistaSerializer
{
private:

	std::ostringstream m_streamOutput;

	char               m_cSeparator;
protected:
public:
	//! @remark m_cSeparator must not occur in any value !!!
	VistaCSVSerializer(char separator = ';');

	virtual ~VistaCSVSerializer();

	const char* GetBuffer() const;
	void GetBufferString(std::string & strBuffer) const;
	int   GetBufferSize() const;

	virtual int WriteShort16(  ushort16 us16Val);
	virtual int WriteInt32(  sint32 si32Val) ;
	virtual int WriteInt32(  uint32 si32Val) ;
	virtual int WriteInt64(  sint64 si64Val) ;
	virtual int WriteUInt64(  uint64 si64Val) ;
	virtual int WriteFloat32(  float32 fVal);
	virtual int WriteFloat64(  float64 f64Val);
	virtual int WriteDouble(  double dVal) ;
	virtual int WriteRawBuffer( const void *pBuffer, const int iLen) ;
	virtual int WriteBool( bool bVal) ;

	virtual int WriteString( const std::string &sString) ;
	virtual int WriteShort16Name( const char *sVarName,  ushort16 us16Val) ;
	virtual int WriteInt32Name( const char *sVarName,  sint32 si32Val) ;
	virtual int WriteInt32Name( const char *sVarName,  uint32 si32Val) ;
	virtual int WriteInt64Name( const char *sVarName,  sint64 si64Val) ;
	virtual int WriteUInt64Name( const char *sVarName,  uint64 si64Val) ;
	virtual int WriteFloat32Name( const char *sVarName,  float32 fVal) ;
	virtual int WriteFloat64Name( const char *sVarName,  float64 f64Val);
	virtual int WriteDoubleName( const char *sVarName,  double ) ;
	virtual int WriteStringName( const char *sVarName,  const std::string &) ;
	virtual int WriteRawBufferName(const char *sVarName, const void *pBuffer, const int iLen) ;
	virtual int WriteBoolName(const char *sVarName,  bool bVal) ;

	virtual int WriteSerializable(const IVistaSerializable &obj);
	virtual int WriteDelimitedString( const std::string &sString, char cDelim = '\0');

	void ClearBuffer();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACSVSERIALIZER_H


