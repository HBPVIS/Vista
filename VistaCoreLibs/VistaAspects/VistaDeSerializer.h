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

#ifndef _VISTADESERIALIZER_H
#define _VISTADESERIALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"
#include <VistaBase/VistaBaseTypes.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSerializable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class serves as a simple base class for home-grown serialization methods.
 * Serializing means something like "preserving an object's state into a kind of
 * serial representation e.g. stream or similiar". The basic idea behind it is
 * that an object usually is composed of base-types (int, doubles etc.) or
 * other objects that are composed of basetypes at some level and so on. So usually
 * what you want to do is to send an object in a stream to a different machine
 * and assemble another instance with the same attribute values on this machine.
 * In our teminology all objects that are to be serialized have to be  of the base
 * type "IVistaSerializable" which does define the methods "Serialize" and "DeSerialize"
 * which do what is depicted above. They use the interface of a serializer or
 * deserialzer in order to streamline their basetypes or hand the serializer/desirializer
 * to their complex aggregates. Note that this mechanism sounds simple, but has its
 * very own advantages and disadvantages.<br>
 * This interface supports the idea of writing variables that are assigned to names
 * in order to enable a non-linear retrieval with this deserializer (which may not be
 * supported by all DeSerializers available).
 * So all methods are available either in the form of simple methods or with the
 * prefix "Name" to the method name (e.g. ReadInt32() and ReadInt32Name()). The first
 * one forces that any peer has to know the correct order of the incoming data, while
 * the other one allows that incoming data is stored in tables and retrieved later
 * on. Note that this is currently only the sketch of an idea and might not be appropriate
 * at all.
 */
class VISTAASPECTSAPI IVistaDeSerializer
{
private:
protected:

	/**
	 * DeSerializers are only interfaces, they do not really get constructed.
	 */
	IVistaDeSerializer() {};
public:

	/**
	 * DeSerializers are only interfaces, they do not really get destructed.
	 */
	virtual ~IVistaDeSerializer() {};


	/**
	 *
	 */
	virtual int ReadShort16( ushort16 &us16Val) = 0;

	/**
	 *
	 */
	virtual int ReadInt32( sint32 &si32Val) = 0;


	/**
	 *
	 */
	virtual int ReadInt32( uint32 &si32Val) = 0;


	/**
	 *
	 */
	virtual int ReadInt64( sint64 &si64Val) = 0;


	/**
	 *
	 */
	virtual int ReadUInt64( uint64 &si64Val) = 0;


	/**
	 *
	 */
	virtual int ReadFloat32( float32 &fVal) = 0;

	/**
	 *
	 */
	virtual int ReadFloat64( float64 &f64Val) = 0;

	/**
	 *
	 */
	virtual int ReadDouble( double &dVal ) = 0;

	/**
	 *
	 */
	virtual int ReadRawBuffer(void *pBuffer, int iLen) = 0;

	/**
	 *
	 */
	virtual int ReadBool(bool &bVal) = 0;

	/**
	 *
	 */
	virtual int ReadString(std::string &, const int iMaxLen) = 0;

	/**
	 *
	 */
	virtual int ReadDelimitedString(std::string &, char cDelim = '\0') = 0;

	/**
	 *
	 */
	virtual int ReadShort16Name( const char *sVarName, ushort16 &us16Val) = 0;

	/**
	 *
	 */
	virtual int ReadInt32Name( const char *sVarName, sint32 &si32Val) = 0;

	/**
	 *
	 */
	virtual int ReadInt32Name( const char *sVarName, uint32 &ui32Val) = 0;

	/**
	 *
	 */
	virtual int ReadInt64Name( const char *sVarName, sint64 &si64Val) = 0;

	virtual int ReadUInt64Name( const char *sVarName, uint64 &si64Val) = 0;

	/**
	 *
	 */
	virtual int ReadFloat32Name( const char *sVarName, float32 &fVal) = 0;

	/**
	 *
	 */
	virtual int ReadFloat64Name( const char *sVarName, float64 &f64Val) = 0;

	/**
	 *
	 */
	virtual int ReadDoubleName( const char *sVarName, double &dDoubleVal) = 0;


	/**
	 *
	 */
	virtual int ReadStringName(const char *sVarName, std::string &sVal, int iMaxLen) = 0;

	/**
	 *
	 */
	virtual int ReadStringName(const char *sVarName, std::string &sVal, char cDelim = '\0') = 0;
	/**
	 *
	 */
	virtual int ReadRawBufferName(const char *sVarName, void *pBuffer, int iLen) = 0;

	/**
	 *
	 */
	virtual int ReadBoolName(const char *sVarName, bool &bVal) = 0;

	/**
	 *
	 */
	virtual int ReadSerializable(IVistaSerializable &) = 0;

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( ushort16 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( sint32 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( uint32 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( sint64 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( uint64 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( float32 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( float64 &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( std::string &);

	/**
	 *
	 */
	virtual IVistaDeSerializer &operator>> ( bool &);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTASYSTEM_H
