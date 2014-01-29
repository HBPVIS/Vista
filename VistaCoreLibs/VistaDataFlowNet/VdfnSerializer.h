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

#ifndef _VDFNSERIALIZER_H
#define _VDFNSERIALIZER_H


/*
 * this file is used to collect serialize adapters for standard datatypes.
 * can be used as an example for custom types. Should be included before
 * IVdfnNode.h and IVdfnPort.h.
 */

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include <VistaBase/VistaVectorMath.h>
#include <VistaMath/VistaBoundingBox.h>
#include <VistaMath/VistaVector.h>
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>
#include <vector>
#include <map>
#include <deque>
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
 * operator to deserialize vectors.
 */
VISTADFNAPI IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer,
		                                     VistaVector<float,4> &v3 );

/**
 * operator to serialize vectors
 */
VISTADFNAPI IVistaSerializer &operator<<( IVistaSerializer &oSer,
		                                   const VistaVector<float,4> &v3 );

/**
 * operator to deserialize vec 3d
 */
VISTADFNAPI IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer,
		                                     VistaVector3D &v3 );

/**
 * operator to serialize vec 3d
 */
VISTADFNAPI IVistaSerializer &operator<<( IVistaSerializer &oSer,
		                                   const VistaVector3D &v3 );

/**
 * operator to serialize axis and angle
 */
VISTADFNAPI IVistaSerializer &operator<<( IVistaSerializer &oSer,
		                                   const VistaAxisAndAngle &v3 );

/**
 * operator to deserialize axis and angle
 */
VISTADFNAPI IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer,
		                                     VistaAxisAndAngle &v3 );


/**
 * templated operator to deserialize vectors of type T
 */
template<class T>
IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, std::vector<T> &v )
{
	VistaType::uint32 size = 0;
	oSer.ReadInt32(size);
	v.resize(size);
	for(typename std::vector<T>::size_type n=0; n < size; ++n)
		oSer >> v[n];
	return oSer;
}

/**
 * templated operator to serialize vectors of type T
 */
template<class T>
IVistaSerializer &operator<<( IVistaSerializer &oSer, const std::vector<T> &v )
{
	typename std::vector<T>::size_type size = v.size();
	oSer.WriteInt32((VistaType::sint32)size);
	for(typename std::vector<T>::size_type n=0; n < size; ++n)
		oSer << v[n];
	return oSer;
}

/**
 * templated operator to deserialize maps from T to C. can be expensive to
 * use, at it will deserialize and then does a copy-in.
 */
template<class T, class C>
IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, std::map<T, C> &m )
{
	VistaType::uint32 size = 0;
	oSer.ReadInt32(size);
	T first;
	C second;
	for(typename std::map<T, C>::size_type n=0; n < size; ++n)
	{
		oSer >> first;
		oSer >> second;
		m[first] = second;
	}
	return oSer;
}

/**
 * serializer to write a map from T to C
 */
template<class T, class C>
IVistaSerializer &operator<<( IVistaSerializer &oSer, const std::map<T, C> &m )
{
	typename std::map<T, C>::size_type size = m.size();
	oSer.WriteInt32((VistaType::sint32)size);
	typename std::map<T, C>::const_iterator iterEnd = m.end();
	for(typename std::map<T, C>::const_iterator iter = m.begin(); iter != iterEnd; ++iter)
	{
		oSer << iter->first;
		oSer << iter->second;
	}
	return oSer;
}


/**
 * deserializer for a deque of type T. tries to resize (will call the default
 * constructor of type T and write over the single elements.
 */
template<class T>
IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, std::deque<T> &v )
{
	VistaType::uint32 size = 0;
	oSer.ReadInt32(size);
	v.resize(size);
	for(typename std::deque<T>::size_type n=0; n < size; ++n)
		oSer >> v[n];
	return oSer;
}

/**
 * a serializer for deque of type T.
 */
template<class T>
IVistaSerializer &operator<<( IVistaSerializer &oSer, const std::deque<T> &v )
{
	typename std::deque<T>::size_type size = v.size();
	oSer.WriteInt32((VistaType::sint32)size);
	for(typename std::deque<T>::size_type n=0; n < size; ++n)
		oSer << v[n];
	return oSer;
}

/**
 * a deserializer for quaternions
 */
VISTADFNAPI IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaQuaternion &v3 );

/**
 * a serializer for quaternions
 */
VISTADFNAPI IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaQuaternion &v3 );

/**
 * a deserializer for transform matrices
 */
VISTADFNAPI IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaTransformMatrix &v3 );

/**
 * a serializer for transform matrices
 */
VISTADFNAPI IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaTransformMatrix &v3 );

/**
 * a deserializer for bounding boxes
 */
VISTADFNAPI IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaBoundingBox &bb );

/**
 * a serializer for bounding boxes
 */
VISTADFNAPI IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaBoundingBox &bb );

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNSERIALIZER_H

