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
		  m_nAvgUpdFreq(0)
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

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
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


