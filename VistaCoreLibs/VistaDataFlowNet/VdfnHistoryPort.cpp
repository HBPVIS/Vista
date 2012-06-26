/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#include "VdfnHistoryPort.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnHistoryPortData::VdfnHistoryPortData( const VistaMeasureHistory &oHistory,
									IVistaMeasureTranscode *pTranscode,
									VistaDeviceSensor *pSensor)
		: m_oHistory(oHistory),
		  m_pTranscode(pTranscode),
		  m_pSensor(pSensor),
		  m_nNewMeasures(0),
		  m_nRealNewMeasures(0),
		  m_nUpdateIndex(0),
		  m_nAvgDriverUpdTime(0),
		  m_nAvgUpdFreq(0),
		  m_nLastSelializedMeasure(0)
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifndef VISTA_TRANSMIT_INCREMENTAL_HISTORIES
IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,
							   VistaSensorMeasure &oMeasure)
{
	oDeSer >> oMeasure.m_nMeasureIdx
		 >> oMeasure.m_nMeasureTs
		 >> oMeasure.m_nSwapTime
		 >> oMeasure.m_nEndianess;
#if defined(DEBUG)
	if( oMeasure.m_nEndianess != VistaSerializingToolset::GetPlatformEndianess() )
	{
		vstr::warnp() << "[VistaDeSerializer]: Endianess mismatch" << std::endl;
	}
#endif

	unsigned int nSize = 0;
	oDeSer >> nSize;
	oMeasure.m_vecMeasures.resize(nSize);

	// reading the raw buffer as is assumes at this stage that the content
	// here has a good endianess.
	oDeSer.ReadRawBuffer( &oMeasure.m_vecMeasures[0], nSize );
	return oDeSer;
}

IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,
							   VistaMeasureHistory &oHist)
{
	/** @todo hmm... this effectively can f*ck up all concurrent */
	// readers to this history, as it is part of a sensor that
	// might be attached to the sensor of this history.
	oDeSer >> oHist.m_nClientReadSize
		>> oHist.m_nDriverWriteSize
		>> oHist.m_nUpdateTs     // this is worst... can be used as a read trigger for waiting readers
		>> oHist.m_nMeasureCount // same here
		>> oHist.m_nSwapCount    // not as severe, only updated single threaded
		>> oHist.m_nSnapshotWriteHead;

	VistaType::uint32 nFirst, nNext, nCurrent, nSize;
	nFirst = nNext = nCurrent = nSize = 0;
	oDeSer >> nSize
		   >> nFirst
		   >> nNext
		   >> nCurrent;


	// this should not really happen, but I am lazy now, so if it
	// happens, the next line will force it to work properly
	if(oHist.m_rbHistory.GetRawAccess().size() != nSize)
	{
		oHist.m_rbHistory.ResizeBuffer(nSize);
	}

	oHist.m_rbHistory.SetFirst(nFirst);
	oHist.m_rbHistory.SetNext(nNext);
	oHist.m_rbHistory.SetCurrent(nCurrent);

	// read off the single measures index by index
	// we should think about optimizing that and only transferring
	// the different slots and not all of them
	for(unsigned int n=0; n < nSize; ++n)
	{
		oDeSer >> *(oHist.m_rbHistory.index(n));
	}

	return oDeSer;
}

IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,
							   VdfnHistoryPortData *pPort )
{
	oDeSer >> const_cast<VistaMeasureHistory&>((*pPort).m_oHistory)
		   >> (*pPort).m_nNewMeasures
		   >> (*pPort).m_nRealNewMeasures
		   >> (*pPort).m_nUpdateIndex
		   >> (*pPort).m_nAvgDriverUpdTime
		   >> (*pPort).m_nAvgUpdFreq;

	return oDeSer;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SERIALIZING
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IVistaSerializer   &operator<<(IVistaSerializer &oSer,
							   const VistaSensorMeasure &oMeasure )
{
	oSer << oMeasure.m_nMeasureIdx
		 << oMeasure.m_nMeasureTs
		 << oMeasure.m_nSwapTime
		 << oMeasure.m_nEndianess
		 << static_cast<VistaType::uint32>(oMeasure.m_vecMeasures.size());

	// we are writing the measure vec as a raw buffer
	// this will lead to problems, once this data is transferred between machines
	// with a different endianess. One solution could be: send an endianess flag
	// along and let the transcoders read off the memory using a ByteBufferSerializer
	// with the sending endianess. But there might be a devil in the details then...
	oSer.WriteRawBuffer( &oMeasure.m_vecMeasures[0], (int)oMeasure.m_vecMeasures.size() );

	return oSer;
}

IVistaSerializer   &operator<<(IVistaSerializer &oSer,
							   const VistaMeasureHistory &oHist )
{
	// it is possible that a driver is writing concurrently to this history.
	// that means that some values might be crashed during the read off this
	// history. The current past should be ok to read off, so we ignore this
	// here. However, we have to think about concurrent device access in a cluster
	// environment. Does not seem to be easy.
	oSer << oHist.m_nClientReadSize
		 << oHist.m_nDriverWriteSize
		 << oHist.m_nUpdateTs
		 << oHist.m_nMeasureCount
		 << oHist.m_nSwapCount
		 << oHist.m_nSnapshotWriteHead
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetBufferSize())
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetFirst())
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetNext())
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetCurrentVal());

	const TVistaRingBuffer<VistaSensorMeasure>::Container_type &vCont = oHist.GetReadBuffer().GetRawAccess();

	for(TVistaRingBuffer<VistaSensorMeasure>::Container_type::const_iterator cit = vCont.begin();
		cit != vCont.end(); ++cit)
	{
		oSer << *cit;
	}
	return oSer;
}

IVistaSerializer   &operator<<(IVistaSerializer &oSer,
							   const VdfnHistoryPortData *pPort )
{
	oSer << (*pPort).m_oHistory
		 << (*pPort).m_nNewMeasures
		 << (*pPort).m_nRealNewMeasures
		 << (*pPort).m_nUpdateIndex
		 << (*pPort).m_nAvgDriverUpdTime
		 << (*pPort).m_nAvgUpdFreq;

	return oSer;
}

#else // use incremental histories

int SerializeMeasure( IVistaSerializer& oSer,
					   const VistaSensorMeasure& oMeasure )
{
	int nRet = 0;
	nRet += oSer.WriteInt32( oMeasure.m_nMeasureIdx );
	nRet += oSer.WriteDouble( oMeasure.m_nMeasureTs );
	nRet += oSer.WriteDouble( oMeasure.m_nSwapTime );
	nRet += oSer.WriteInt32( oMeasure.m_nEndianess );
	nRet += oSer.WriteInt32( VistaType::uint32( oMeasure.m_vecMeasures.size() ) );
	nRet += oSer.WriteRawBuffer( &oMeasure.m_vecMeasures[0], (int)oMeasure.m_vecMeasures.size() );
	return nRet;
}

int DeSerializeMeasure( IVistaDeSerializer& oDeSer,
					   VistaSensorMeasure& oMeasure)
{
	int nRet = 0;
	nRet += oDeSer.ReadInt32( oMeasure.m_nMeasureIdx );
	nRet += oDeSer.ReadDouble( oMeasure.m_nMeasureTs );
	nRet += oDeSer.ReadDouble( oMeasure.m_nSwapTime );
	nRet += oDeSer.ReadInt32( oMeasure.m_nEndianess );
#if defined(DEBUG)
	if( oMeasure.m_nEndianess != VistaSerializingToolset::GetPlatformEndianess() )
	{
		vstr::warnp() << "[VistaDeSerializer]: Endianess mismatch" << std::endl;
	}
#endif

	VistaType::uint32 nSize = 0;
	nRet += oDeSer.ReadInt32( nSize );
	oMeasure.m_vecMeasures.resize( nSize );

	// reading the raw buffer as is assumes at this stage that the content
	// here has a good endianess.
	nRet += oDeSer.ReadRawBuffer( &oMeasure.m_vecMeasures[0], nSize );
	return nRet;
}

int SerializeHistoryIncremental( IVistaSerializer& oSer,
							   const VistaMeasureHistory& oHist,
							   const unsigned int m_nLastSerializeCount )
{
	// it is possible that a driver is writing concurrently to this history.
	// that means that some values might be crashed during the read off this
	// history. The current past should be ok to read off, so we ignore this
	// here. However, we have to think about concurrent device access in a cluster
	// environment. Does not seem to be easy.
	int nRet = 0;
	nRet += oSer.WriteInt32( oHist.m_nClientReadSize );
	nRet += oSer.WriteInt32( oHist.m_nDriverWriteSize );
	nRet += oSer.WriteDouble( oHist.m_nUpdateTs );
	nRet += oSer.WriteInt32( oHist.m_nMeasureCount );
	nRet += oSer.WriteInt32( oHist.m_nSwapCount );
	nRet += oSer.WriteInt32( oHist.m_nSnapshotWriteHead );

	VistaType::uint32 nNewMeasures = oHist.m_nMeasureCount - m_nLastSerializeCount;
	// never transmit more measures than the data size
	nNewMeasures = std::min( nNewMeasures, (VistaType::uint32)oHist.m_rbHistory.GetBufferSize() );
	nRet += oSer.WriteInt32( nNewMeasures );
	
	TVistaRingBuffer<VistaSensorMeasure>::const_iterator itMeasure
				= oHist.m_rbHistory.index( oHist.m_nSnapshotWriteHead );
	for( VistaType::uint32 i = 0; i < nNewMeasures; ++i )
	{
		--itMeasure;
		nRet += SerializeMeasure( oSer, (*itMeasure) );
	}
	
	return nRet;
}

int DeSerializeHistoryIncremental( IVistaDeSerializer& oDeSer,
							   VistaMeasureHistory& oHist,
							   const unsigned int m_nLastSerializeCount )
{
	/** @todo hmm... this effectively can f*ck up all concurrent */
	// readers to this history, as it is part of a sensor that
	// might be attached to the sensor of this history.
	int nRet = 0;
	nRet += oDeSer.ReadInt32( oHist.m_nClientReadSize );
	nRet += oDeSer.ReadInt32( oHist.m_nDriverWriteSize );
	nRet += oDeSer.ReadDouble( oHist.m_nUpdateTs );
	nRet += oDeSer.ReadInt32( oHist.m_nMeasureCount );
	nRet += oDeSer.ReadInt32( oHist.m_nSwapCount );
	nRet += oDeSer.ReadInt32( oHist.m_nSnapshotWriteHead );

	VistaType::uint32 nNewMeasures;
	nRet += oDeSer.ReadInt32( nNewMeasures );

	assert( nNewMeasures == std::min( oHist.m_nMeasureCount - m_nLastSerializeCount,
			(VistaType::uint32)oHist.m_rbHistory.GetBufferSize() ) );
	
	TVistaRingBuffer<VistaSensorMeasure>::iterator itMeasure
				= oHist.m_rbHistory.index( oHist.m_nSnapshotWriteHead );
	for( VistaType::uint32 i = 0; i < nNewMeasures; ++i )
	{
		--itMeasure;
		nRet += DeSerializeMeasure( oDeSer, (*itMeasure) );
	}
	
	return nRet;
}


IVistaSerializer   &operator<<( IVistaSerializer& oSer,
							   const VdfnHistoryPortData* pPort )
{
	int nRet = SerializeHistoryIncremental( oSer, pPort->m_oHistory, pPort->m_nLastSelializedMeasure );
	nRet += oSer.WriteInt32( (VistaType::uint32)pPort->m_nNewMeasures );
	nRet += oSer.WriteInt32( (VistaType::uint32)pPort->m_nRealNewMeasures );
	nRet += oSer.WriteInt32( (VistaType::uint32)pPort->m_nUpdateIndex );
	nRet += oSer.WriteDouble( pPort->m_nAvgDriverUpdTime );
	nRet += oSer.WriteDouble( pPort->m_nAvgUpdFreq );

	pPort->m_nLastSelializedMeasure = pPort->m_oHistory.m_nMeasureCount;
	return oSer;
}

IVistaDeSerializer &operator>>( IVistaDeSerializer& oDeSer,
							   VdfnHistoryPortData* pPort )
{
	int nRet = DeSerializeHistoryIncremental( oDeSer,
							const_cast<VistaMeasureHistory& >( pPort->m_oHistory ),
							pPort->m_nLastSelializedMeasure );
	VistaType::uint32 nDummy;
	nRet += oDeSer.ReadInt32( nDummy );
	pPort->m_nNewMeasures = nDummy;
	nRet += oDeSer.ReadInt32( nDummy );
	pPort->m_nRealNewMeasures = nDummy;
	nRet += oDeSer.ReadInt32( nDummy );
	pPort->m_nUpdateIndex = nDummy;
	nRet += oDeSer.ReadDouble( pPort->m_nAvgDriverUpdTime );
	nRet += oDeSer.ReadDouble( pPort->m_nAvgUpdFreq );

	pPort->m_nLastSelializedMeasure = pPort->m_oHistory.m_nMeasureCount;
	return oDeSer;
}



#endif
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


