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

#include "VistaDTrackDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceDriversOut.h>

#include "../VistaDriverConnectionAspect.h"
#include "../VistaDriverSensorMappingAspect.h"
#include "../VistaDriverMeasureHistoryAspect.h"
#include "../VistaDriverLoggingAspect.h"
#include "../VistaDriverProtocolAspect.h"

#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaCSVDeSerializer.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include <string>
#include <iostream>
#include <assert.h>
using namespace std;

class IVistaDTrackProtocol
{
public:
	virtual ~IVistaDTrackProtocol() {}
	virtual bool SendAttachString(VistaConnection *pCon) const = 0;
	virtual bool SendDetachString(VistaConnection *pCon) const = 0;
	virtual bool SendEnableString(VistaConnection *pCon) const = 0;
	virtual bool SendDisableString(VistaConnection *pCon) const = 0;
	virtual bool SendStatusString(VistaConnection *pCon, bool &bIdle) const = 0;
};

class VistaDTrack1Protocol : public IVistaDTrackProtocol
{
public:

	virtual bool SendAttachString(VistaConnection *pCon) const
	{
		pCon->WriteRawBuffer("dtrack 10 3\0",13);
		VistaTimeUtils::Sleep(100);

		return true;
	}

	virtual bool SendDetachString(VistaConnection *pCon) const
	{
			// stop tracking data output
		pCon->WriteRawBuffer("dtrack 32\0",11);
		pCon->WaitForSendFinish();

		// stop measurement
		pCon->WriteRawBuffer("dtrack 10 0\0",13);
		pCon->WaitForSendFinish();

		// wait a bit just for the sake of it
		VistaTimeUtils::Sleep(100);

		return true;
	}

	virtual bool SendEnableString(VistaConnection *pCon) const
	{
		if(pCon->WriteRawBuffer("dtrack 31\0", 10)==10)
		{
			VistaTimeUtils::Sleep(100);
			return true;
		}
		return false;
	}

	virtual bool SendDisableString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack 32\0", 10)==10);
		VistaTimeUtils::Sleep(100);
		return true;
	}

	virtual bool SendStatusString(VistaConnection *pCon, bool &bIdle) const
	{
		// not supported by dtrack1
		return false;
	}
};

class VistaDTrack2Protocol : public IVistaDTrackProtocol
{
public:
	virtual bool SendAttachString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack2 init\0", 13) == 13);
	}

	virtual bool SendDetachString(VistaConnection *pCon) const
	{
		if( SendDisableString(pCon) )
			pCon->WaitForSendFinish(); // no information given
		return true;
	}

	virtual bool SendEnableString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack2 tracking start\0", 23) == 23);
	}

	virtual bool SendDisableString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack2 tracking stop\0", 22) == 22);
	}

	virtual bool SendStatusString(VistaConnection *pCon, bool &bIdle) const
	{
		if(pCon->WriteRawBuffer( "dtrack2 get status active\0", 26) == 26)
		{
			std::string strAnswer;
			pCon->ReadDelimitedString( strAnswer ); // read until 0x00 or failure

			// should evaluate the answer, how?
			return true;
		}
		return false;
	}
};

class VistaDTrackProtocolAspect : public IVistaDriverProtocolAspect
{
public:
	VistaDTrackProtocolAspect( VistaDTrackDriver *pDriver )
		: m_pDriver( pDriver ),
		  m_pProtocol(NULL)
	{
	}

	bool SetProtocol( const _cVersionTag &oTag )
	{
		if(m_pDriver)
		{
			if(VistaAspectsComparisonStuff::StringEquals( oTag.m_strProtocolName, "dtrack1", false ))
			{
				return ((m_pProtocol = new VistaDTrack1Protocol) != NULL);
			}
			else if(VistaAspectsComparisonStuff::StringEquals( oTag.m_strProtocolName, "dtrack2", false ))
			{
				return ((m_pProtocol = new VistaDTrack2Protocol) != NULL);
			}
		}

		return false;
	}

	IVistaDTrackProtocol *GetProtocol() const
	{
		return m_pProtocol;
	}

private:
	VistaDTrackDriver *m_pDriver;
	IVistaDTrackProtocol *m_pProtocol;
};
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
/**
 * memory layout of the base measure
 */
struct _sBaseMasure
{
	double m_nId,
		   m_nQuality;
};

/**
 * this is a dummy structure to read off the values in a generic
 * way. the other structures are simple access functions for the
 * transcode getters!
 */
struct _sGenericMeasure
{
	double m_anField[18];
};


// ############################################################################
// DECODER API
// ############################################################################

static bool ReadNDoubles(IVistaDeSerializer *pDeSer,
			unsigned int nCount,
			VistaSensorMeasure::MEASUREVEC &vecOut,
			unsigned int &nOffset)
{
	_sGenericMeasure *dField = (_sGenericMeasure*)&vecOut[0];
	for(unsigned int n=0; n < nCount; ++n)
	{
		pDeSer->ReadDouble(dField->m_anField[nOffset++]);
	}
	return true;
}


class ILineDecode
{
public:
	ILineDecode(unsigned int nMarkerType)
		: m_nMarkerType(nMarkerType)
	{
	}

	virtual ~ILineDecode() {}

	virtual int ReadAllBlocksWithOffset(VistaByteBufferDeSerializer*,
		VistaSensorMeasure::MEASUREVEC &vecOut,
			unsigned int nOffset) = 0;

	virtual int ReadMarkerType(VistaByteBufferDeSerializer*,
		 unsigned int &nMarkerType) = 0;

	virtual int ReadSingleBlock(VistaByteBufferDeSerializer*,
		VistaSensorMeasure::MEASUREVEC &vecOut) = 0;

	std::vector<int>       m_vecBlockLength;
protected:
	unsigned int m_nMarkerType;
};


class FrDecode : public ILineDecode
{
public:
	FrDecode(unsigned int nMarkerType)
		: ILineDecode(nMarkerType)
	{}


	virtual int ReadAllBlocksWithOffset(VistaByteBufferDeSerializer *pDeSer,
		VistaSensorMeasure::MEASUREVEC &vecOut,
								unsigned int nOffset)
	{
		// fr is read off
		std::string strFrameCnt;
		pDeSer->ReadDelimitedString(strFrameCnt, 0x0d);
		_sGlobalMeasure *m = (_sGlobalMeasure*)&vecOut[0];

		m->m_nFrameCount = int(VistaAspectsConversionStuff::ConvertToDouble(strFrameCnt));
		return 1;
	}

	virtual int ReadMarkerType(VistaByteBufferDeSerializer*,
			unsigned int &nMarkerType)
	{
		nMarkerType = m_nMarkerType;
		return 0;
	}

	virtual int ReadSingleBlock(VistaByteBufferDeSerializer*,
		VistaSensorMeasure::MEASUREVEC &vecOut)
	{
		_sGlobalMeasure *m = (_sGlobalMeasure*)&vecOut[0];

		m->m_nFrameCount = -1;
		m->m_nTimeStamp  = 1.0;
		return 0;
	}
};

class GenDecode : public ILineDecode
{
public:
	GenDecode(unsigned int nSensorType,
			   const std::vector<int> &vecBlockLengths)
	: ILineDecode(nSensorType),
	  m_pFragDecode(new VistaCSVDeSerializer(' '))
	{
		 m_vecBlockLength = vecBlockLengths;
	}

	~GenDecode()
	{
		delete m_pFragDecode;
	}
	virtual int ReadMarkerType(VistaByteBufferDeSerializer *pDeSer,
			unsigned int &nMarkerType)
	{
		// read marker type has the task to read off the
		// number of samples in this record (hence the name ;)
		// if there are 0 markers in the set, there is no ' '
		// to delimit the end of this.
		// we should link the posix regexp here
		// but for now, we simply copy the read delimited string
		// from the serializer and pimp it to read ' ' OR '\r'
		// which marks the end of the record.
		/** @todo link posix and boost */
		std::string sString;

		char pcTmp[2];
		pcTmp[0] = 0x00;
		pcTmp[1] = 0x00;

		int iLength = 1;
		int iLen = 0; /**< measure length */
		do
		{
			int iRead=0;
			if((iRead=pDeSer->ReadRawBuffer((void*)&pcTmp[0], iLength))==iLength)
			{
				if(pcTmp[0] == ' ' || pcTmp[0] == '\r')
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
				vstr::errf() << "Should read: " << iLength
					<< ", but read: " << iRead << std::endl;
				break;
			}
		}
		while(true);


		// read off number of sensors read
		unsigned int nNum = VistaAspectsConversionStuff::ConvertToInt(sString);
		nMarkerType = m_nMarkerType;
		return nNum;

	}

	virtual int ReadAllBlocksWithOffset(VistaByteBufferDeSerializer *pDeSer,
		VistaSensorMeasure::MEASUREVEC &vecOut,
								unsigned int nOffset)
	{
		std::string strBlock, strBeg;
		unsigned int nInitial = nOffset;
		char c;
		std::vector<int>::const_iterator cit = m_vecBlockLength.begin();
		++cit; // skip first token length
		for(; cit != m_vecBlockLength.end(); ++cit)
		{
			// prepare next block
			// read off '['
			pDeSer->ReadRawBuffer(&c, sizeof(char));
			//pDeSer->ReadDelimitedString(strBeg, '[');
			pDeSer->ReadDelimitedString(strBlock, ']');
			// load csv with strBlock
			m_pFragDecode->FillBuffer(strBlock);
			// read block-lengths of doubles
			ReadNDoubles(m_pFragDecode, (*cit), vecOut, nOffset);
		}
		return nOffset - nInitial; // return number of double read
	}

	virtual int ReadSingleBlock(VistaByteBufferDeSerializer*pDeSer,
		VistaSensorMeasure::MEASUREVEC &vecOut)
	{
		std::string strBlock, strBeg;
		pDeSer->ReadDelimitedString(strBeg, '[');
		pDeSer->ReadDelimitedString(strBlock, ']');
		// load csv with strBlock
		m_pFragDecode->FillBuffer(strBlock);
		// read block-lengths of doubles

		unsigned int nOffset = 0;
		ReadNDoubles(m_pFragDecode, (unsigned int)(double(vecOut.size())/sizeof(double)), vecOut, nOffset);

		return nOffset;
	}


private:
	VistaCSVDeSerializer *m_pFragDecode;

};

// ############################################################################
// ############################################################################


// ############################################################################
// ATTACH/DETACH SEQUENCE WHEN CONNECTED
// ############################################################################

class VistaDTrackAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaDTrackAttachSequence(VistaDTrackProtocolAspect *pProtocol,
		                       bool bNotAttachOnly = false)
		: m_bSendStartTracking(bNotAttachOnly),
		  m_pProtocol(pProtocol)
	{
	}

	virtual bool operator()(VistaConnection *pCon)
	{
		if(!pCon->GetIsOpen())
			if(pCon->Open() == false)
				return false;


		// start tracking
		if(m_bSendStartTracking)
		{
			return (m_pProtocol->GetProtocol()->SendAttachString(pCon));
			//pCon->WriteRawBuffer("dtrack 31\0",11);
		}
		return true;
	}

	bool GetSendStartTracking() const { return m_bSendStartTracking; }
	void SetSendStartTracking(bool bSend) { m_bSendStartTracking = bSend; }

private:
	bool m_bSendStartTracking;
	VistaDTrackProtocolAspect *m_pProtocol;
};

class VistaDTrackDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaDTrackDetachSequence(VistaDTrackProtocolAspect *pProtocol,
		                       bool bNotDetachOnly = false)
		: m_bSendStopTracking(bNotDetachOnly),
		  m_pProtocol(pProtocol)
	{
	}

	virtual bool operator()(VistaConnection *pCon)
	{
		// nothing to do, we expect this connection to be open
		if(!pCon->GetIsOpen())
			return false;

		if(m_bSendStopTracking)
		{
			m_pProtocol->GetProtocol()->SendDetachString(pCon);
		}
		pCon->Close();
		return !pCon->GetIsOpen();
	}

	bool GetSendStopTracking() const { return m_bSendStopTracking; }
	void SetSendStopTracking(bool bSend) { m_bSendStopTracking = bSend; }
private:
	bool m_bSendStopTracking;
	VistaDTrackProtocolAspect *m_pProtocol;
};

// ############################################################################

// ############################################################################
// TRANSCODERS
// ############################################################################

class VistaDTrackGlobalsTranscode : public IVistaMeasureTranscode
{
public:
	VistaDTrackGlobalsTranscode()
	{
		// inherited
		m_nNumberOfScalars = 2;
	}
	REFL_INLINEIMP(VistaDTrackGlobalsTranscode, IVistaMeasureTranscode);
};

class VistaDTrackGlobalsScalarGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaDTrackGlobalsScalarGet()
		: IVistaMeasureTranscode::CScalarDoubleGet("DSCALAR",
		  "VistaDTrackGlobalsTranscode", "get global dtrack measures (frame cnt/ts)")
	{}


	bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
						 double &dScalar,
						 unsigned int nIdx) const
	{
		if(!pMeasure || nIdx > 2)
			return false;

		_sGlobalMeasure *m = (_sGlobalMeasure*)&(pMeasure->m_vecMeasures[0]);
		if(nIdx == 0)
			dScalar = m->m_nFrameCount;
		else
			dScalar = m->m_nTimeStamp;

		return true;
	}
};

class VistaDTrackGlobalsFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDTrackGlobalsTranscode;
	}
};

static IVistaPropertyGetFunctor *SaGlobalGet[] =
{
	new VistaDTrackGlobalsScalarGet,
	NULL
};

// #########################################################################

class VistaDTrackBodyTranscode : public IVistaMeasureTranscode
{
public:
	VistaDTrackBodyTranscode()
	{
		m_nNumberOfScalars = 5;
	}

	virtual ~VistaDTrackBodyTranscode() {}

protected:
private:

	REFL_INLINEIMP(VistaDTrackBodyTranscode, IVistaMeasureTranscode);

};

class VistaDTrackBodyPosGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaDTrackBodyPosGet()
		: IVistaMeasureTranscode::CV3Get("POSITION",
		"VistaDTrackBodyTranscode", "dtrack body position") {}

	VistaVector3D GetValue(const VistaSensorMeasure *pMeasure) const
	{
		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
		return VistaVector3D(float(m->m_nPos[0]),
							  float(m->m_nPos[1]),
							  float(m->m_nPos[2]));
	}

	bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &oValue) const
	{
		if(!pMeasure)
			return false;

		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
		oValue[0] = float(m->m_nPos[0]);
		oValue[1] = float(m->m_nPos[1]);
		oValue[2] = float(m->m_nPos[2]);
		return true;
	}
};

class VistaDTrackBodyOriGet : public IVistaMeasureTranscode::CQuatGet
{
public:
	VistaDTrackBodyOriGet()
		: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
		"VistaDTrackBodyTranscode", "dtrack body orientation") {}

	VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaQuaternion q;
		GetValue(pMeasure, q);
		return q;
	};

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaQuaternion &qQuat) const
	{
		if(!pMeasure)
			return false;

		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
		VistaTransformMatrix t (float(m->m_anRot[0]), float(m->m_anRot[3]), float(m->m_anRot[6]), 0,
			float(m->m_anRot[1]), float(m->m_anRot[4]), float(m->m_anRot[7]), 0,
			float(m->m_anRot[2]), float(m->m_anRot[5]), float(m->m_anRot[8]), 0,
			0    , 0    , 0    , 1);

		qQuat = VistaQuaternion(t);
		return true;
	}
};

class VistaDTrackBodyMatrixGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
{
public:
	VistaDTrackBodyMatrixGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>("ROTMATRIX",
		"VistaDTrackBodyTranscode", "dtrack body orientation as a 4x4 matrix")
	{}

	VistaTransformMatrix GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaTransformMatrix m;
		GetValue(pMeasure, m);
		return m;
	};

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaTransformMatrix &mt) const
	{
		if(!pMeasure)
			return false;

		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
		mt = VistaTransformMatrix(float(m->m_anRot[0]), float(m->m_anRot[3]), float(m->m_anRot[6]), 0,
			float(m->m_anRot[1]), float(m->m_anRot[4]), float(m->m_anRot[7]), 0,
			float(m->m_anRot[2]), float(m->m_anRot[5]), float(m->m_anRot[8]), 0,
			0    , 0    , 0    , 1);

		return true;
	}
};

class VistaDTrackBodyPoseMatrixGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
{
public:
	VistaDTrackBodyPoseMatrixGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>("POSEMATRIX",
		"VistaDTrackBodyTranscode", "dtrack body pose (trans+rot) as a 4x4 matrix")
	{}

	VistaTransformMatrix GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaTransformMatrix m;
		GetValue(pMeasure, m);
		return m;
	};

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaTransformMatrix &mt) const
	{
		if(!pMeasure)
			return false;

		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
		mt = VistaTransformMatrix(float(m->m_anRot[0]), float(m->m_anRot[3]), float(m->m_anRot[6]), 0,
			float(m->m_anRot[1]), float(m->m_anRot[4]), float(m->m_anRot[7]), 0,
			float(m->m_anRot[2]), float(m->m_anRot[5]), float(m->m_anRot[8]), 0,
			0    , 0    , 0    , 1);
		mt.SetTranslation( VistaVector3D(float(m->m_nPos[0]), float(m->m_nPos[1]), float(m->m_nPos[2]) ) );

		return true;
	}
};

class VistaDTrackBaseScalarGet : public IVistaMeasureTranscode::CScalarDoubleGet
{
public:
	VistaDTrackBaseScalarGet (const std::string &strPropName,
		const std::string &strClassName, const std::string &strDesc)
		: IVistaMeasureTranscode::CScalarDoubleGet(strPropName,
		strClassName, strDesc) {}

	bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
		double &dScalar, unsigned int nIndex) const
	{
		if(!pMeasure || nIndex >= 2)
			return false;

		_sBaseMasure *m = (_sBaseMasure*)&(*pMeasure).m_vecMeasures[0];

		switch(nIndex)
		{
		case 0: // id
			dScalar = m->m_nId;
			break;
		case 1: // qu
			dScalar = m->m_nQuality;
			break;
		default:
			return false;
		}

		return true;
	}

};

class VistaDTrackBodyScalarGet : public VistaDTrackBaseScalarGet
{
public:
	VistaDTrackBodyScalarGet()
		: VistaDTrackBaseScalarGet ("DSCALAR",
		"VistaDTrackBodyTranscode", "dtrack body scalar get") {}

	VistaDTrackBodyScalarGet(const std::string &strPropName,
		const std::string &strClassName, const std::string &strDesc)
		: VistaDTrackBaseScalarGet (strPropName,
		strClassName, strDesc) {}

	bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
		double &dScalar, unsigned int nIndex) const
	{
		if(!pMeasure || nIndex >= 5)
			return false;

		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];

		switch(nIndex)
		{
		case 2: // ex
		case 3: // ey
		case 4: // ez
			dScalar = m->m_nEuler[ nIndex - 2 ]; // shift back by 2
			//dScalar = (*m_pMeasure).m_vecMeasures[(nIndex-2) + m_nEulerOffset];
			break;
		default:
			return VistaDTrackBaseScalarGet::GetValueIndexed(pMeasure, dScalar, nIndex);;
		}

		return true;
	}
};

class VistaDTrackNamedScalarGet : public IVistaMeasureTranscode::TTranscodeValueGet<double>
{
	public:
		enum eTp
		{
			TAG_ID=0,
			TAG_QUALITY,
			TAG_EX,
			TAG_EY,
			TAG_EZ,
			TAG_LAST
		};
		VistaDTrackNamedScalarGet(eTp nIdx,
								   const std::string &strName,
								   const std::string &strRefClass,
								   const std::string &strDesc)
	: IVistaMeasureTranscode::TTranscodeValueGet<double>(strName, strRefClass, strDesc),
					m_nIdx(nIdx)
	{}

	double GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		double d = 0.0;
		GetValue(pMeasure, d);
		return d;
	}

	bool GetValue( const VistaSensorMeasure *pMeasure, double &dVal ) const
	{
		bool bRet = true;
		_sBodyMeasure *m = (_sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];

		switch(m_nIdx)
		{
			case TAG_ID:
				dVal = m->m_nId;
				break;
			case TAG_QUALITY:
				dVal = m->m_nQuality;
				break;
			case TAG_EX:
			case TAG_EY:
			case TAG_EZ:
				dVal = m->m_nEuler[ TAG_LAST - m_nIdx ];
				break;
			default:
				bRet = false;
				break;
		}
		return bRet;
	}


	private:

	eTp m_nIdx;
};

class VistaDTrackBodyFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDTrackBodyTranscode;
	}
};

static IVistaPropertyGetFunctor *SaBodyGet[] =
{
	new VistaDTrackBodyPosGet,
	new VistaDTrackBodyOriGet,
	new VistaDTrackBodyScalarGet,
	new VistaDTrackBodyMatrixGet,
	new VistaDTrackBodyPoseMatrixGet,
	new VistaDTrackNamedScalarGet( VistaDTrackNamedScalarGet::TAG_ID,
								 "RAWID",
								 "VistaDTrackBodyTranscode",
								 "The raw id of the body as reported from the dtrack" ),
	new VistaDTrackNamedScalarGet( VistaDTrackNamedScalarGet::TAG_QUALITY,
									"QUALITY",
									"VistaDTrackBodyTranscode",
									"Quality as reported by the dtrack driver" ),
	new VistaDTrackNamedScalarGet( VistaDTrackNamedScalarGet::TAG_EX,
									"EULER_X",
									"VistaDTrackBodyTranscode",
									"Euler angles X" ),
	new VistaDTrackNamedScalarGet( VistaDTrackNamedScalarGet::TAG_EY,
									"EULER_Y",
									"VistaDTrackBodyTranscode",
									"Euler angles Y" ),
	new VistaDTrackNamedScalarGet( VistaDTrackNamedScalarGet::TAG_EZ,
									"EULER_Z",
									"VistaDTrackBodyTranscode",
									"Euler angles Z" ),

	NULL
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VistaDTrackStickTranscode : public VistaDTrackBodyTranscode
{
public:
	VistaDTrackStickTranscode()
		: VistaDTrackBodyTranscode()
	{
		m_nNumberOfScalars = 8 + 5;
	}

	REFL_INLINEIMP(VistaDTrackStickTranscode, VistaDTrackBodyTranscode);
};


class VistaDTrackStickScalarGet : public VistaDTrackBodyScalarGet
{
public:
	VistaDTrackStickScalarGet()
		: VistaDTrackBodyScalarGet("DSCALAR",
		"VistaDTrackStickTranscode", "dtrack stick dscalar get") {}


	bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
		double &dScalar, unsigned int nIndex) const
	{
		if(!pMeasure)
			return false;

		// normalize index
		_sStickMeasure *m = (_sStickMeasure*)&(*pMeasure).m_vecMeasures[0];
		if(nIndex >= 0 && nIndex < 8)
		{
			// trying to retrieve virtual button state
			// note that we map the virtual scalar index 5 .. 5+nNumberOfButtons
			// from a single bit mask which is stored in m_vecMeasures[2]

			dScalar = (int(m->m_nButtonState) & ( 1 << nIndex )) ? 1.0 : 0.0;
			return true;
		}
		else
		{
			switch(nIndex)
			{
			case 8:
				dScalar = m->m_nId;
				return true;
			case 9:
				dScalar = m->m_nQuality;
				return true;
			case 10:
			case 11:
			case 12:
				dScalar = m->m_nEuler[nIndex - 10];
				return true;
			default:
				break;
			}
		}
		return false;
	}
};

class VistaDTrackStickOriGet : public IVistaMeasureTranscode::CQuatGet
{
public:
	VistaDTrackStickOriGet()
		: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
		"VistaDTrackStickTranscode", "dtrack stick ori get") {}

	VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaQuaternion q;
		GetValue(pMeasure, q);
		return q;
	};

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaQuaternion &qQuat) const
	{
		if(!pMeasure)
			return false;

		_sStickMeasure *m = (_sStickMeasure*)&(*pMeasure).m_vecMeasures[0];
		VistaTransformMatrix t (float(m->m_anRot[0]), float(m->m_anRot[3]), float(m->m_anRot[6]), 0,
								 float(m->m_anRot[1]), float(m->m_anRot[4]), float(m->m_anRot[7]), 0,
								 float(m->m_anRot[2]), float(m->m_anRot[5]), float(m->m_anRot[8]), 0,
								 0    , 0    , 0    , 1);

		qQuat = VistaQuaternion(t);
		return true;
	}
};

class VistaDTrackStickPosGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaDTrackStickPosGet()
		: IVistaMeasureTranscode::CV3Get("POSITION",
		"VistaDTrackStickTranscode", "dtrack stick pos get") {}

	VistaVector3D GetValue(const VistaSensorMeasure *pMeasure) const
	{
		VistaVector3D v3;
		GetValue(pMeasure, v3);
		return v3;
	}

	bool GetValue(const VistaSensorMeasure * pMeasure, VistaVector3D &v3Pos) const
	{
		if(!pMeasure)
			return false;

		_sStickMeasure *m = (_sStickMeasure*)&(*pMeasure).m_vecMeasures[0];
		v3Pos[0] = float(m->m_nPos[0]);
		v3Pos[1] = float(m->m_nPos[1]);
		v3Pos[2] = float(m->m_nPos[2]);
		return true;
	}
};

class VistaDTrackNamedStickGet : public IVistaMeasureTranscode::CUIntGet
{
	public:
		enum eTp
		{
			TAG_BTMASK = 0
		};

		VistaDTrackNamedStickGet(eTp nIdx,
								const std::string &strName,
								const std::string &strClass,
								const std::string &strDesc)
	: IVistaMeasureTranscode::CUIntGet( strName, strClass, strDesc ),
		m_nIdx(nIdx)
		{
		}


	unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
	{
		unsigned int nVal = 0;
		GetValue(pMeasure,nVal);
		return nVal;
	}

	bool GetValue( const VistaSensorMeasure *pMeasure,
					 unsigned int &dVal ) const
	{
		bool bRet = true;
		_sStickMeasure *m = (_sStickMeasure*)&(*pMeasure).m_vecMeasures[0];

		switch( m_nIdx )
		{
			case TAG_BTMASK:
			{
				dVal = (unsigned int)(int(m->m_nButtonState));
				break;
			}
			default:
				bRet = false;
				break;
		}

		return bRet;
	}
	private:
		eTp m_nIdx;

};

class VistaDTrackStickFactory : public IVistaMeasureTranscoderFactory
{
public:
	VistaDTrackStickFactory(unsigned int nNumberOfButtons)
		: m_nNumberOfButtons(nNumberOfButtons)
	{
	}

	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDTrackStickTranscode;
	}

	unsigned int m_nNumberOfButtons;
};


static IVistaPropertyGetFunctor *SaStickGet[] =
{
	new VistaDTrackStickScalarGet,
	new VistaDTrackStickOriGet,
	new VistaDTrackStickPosGet,
	new VistaDTrackNamedStickGet( VistaDTrackNamedStickGet::TAG_BTMASK,
								   "BTMASK",
								   "VistaDTrackStickTranscode",
								   "Buttonmask as reported from the dtrack" ),
	NULL
};
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VistaDTrackMarkerTranscode : public IVistaMeasureTranscode
{
public:
	VistaDTrackMarkerTranscode()
	{
		// inherited
		m_nNumberOfScalars = 2;
	}

	virtual ~VistaDTrackMarkerTranscode() {}

	REFL_INLINEIMP(VistaDTrackMarkerTranscode, IVistaMeasureTranscode);
};

class VistaDTrackMarkerFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaDTrackMarkerTranscode;
	}
};

class VistaDTrackMarkerPosGet : public IVistaMeasureTranscode::CV3Get
{
public:
	VistaDTrackMarkerPosGet()
		: IVistaMeasureTranscode::CV3Get("POSITION",
		"VistaDTrackMarkerTranscode", "dtrack marker position get") {}

	VistaVector3D GetValue(const VistaSensorMeasure *pMeasure) const
	{
		if(!pMeasure)
			return VistaVector3D();

		_sMarkerMeasure *m = (_sMarkerMeasure*)&(*pMeasure).m_vecMeasures[0];
		return VistaVector3D( float(m->m_nPos[0]),
			float(m->m_nPos[1]), float(m->m_nPos[2]) );
	}

	bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &oValue) const
	{
		if(!pMeasure)
			return false;

		_sMarkerMeasure *m = (_sMarkerMeasure*)&(*pMeasure).m_vecMeasures[0];
		oValue[0] = float(m->m_nPos[0]);
		oValue[1] = float(m->m_nPos[1]);
		oValue[2] = float(m->m_nPos[2]);

		return true;
	}
};

class VistaDTrackMarkerScalarGet : public VistaDTrackBaseScalarGet
{
public:
	VistaDTrackMarkerScalarGet()
		: VistaDTrackBaseScalarGet("DSCALAR",
		"VistaDTrackMarkerTranscode", "dtrack marker scalar get") {}
};


static IVistaPropertyGetFunctor *SaMarkerGet[] =
{
	new VistaDTrackMarkerPosGet,
	new VistaDTrackMarkerScalarGet,
	NULL
};

// ############################################################################
// CREATION METHOD
// ############################################################################

class VistaDTrackCreationMethod : public IVistaDriverCreationMethod
{
public:
	virtual IVistaDeviceDriver *operator()()
	{
		return new VistaDTrackDriver;
	}
};

// hide static member of this module in anonymouse namespace
namespace
{
	VistaDTrackCreationMethod *SDTrackCreate = NULL;
}

IVistaDriverCreationMethod *VistaDTrackDriver::GetDriverFactoryMethod()
{
	if( SDTrackCreate == NULL)
	{
		SDTrackCreate = new VistaDTrackCreationMethod;
		SDTrackCreate->RegisterSensorType( "STICK",
				                           sizeof(_sStickMeasure),
				                           60,
				                           new VistaDTrackStickFactory(8),
				                           "VistaDTrackStickTranscode" );
		SDTrackCreate->RegisterSensorType( "BODY",
				                           sizeof(_sBodyMeasure),
				                           60,
				                           new VistaDTrackBodyFactory,
				                           "VistaDTrackBodyTranscode" );

		SDTrackCreate->RegisterSensorType( "MARKER",
				                           sizeof(_sMarkerMeasure),
				                           60,
				                           new VistaDTrackMarkerFactory,
				                           "VistaDTrackMarkerTranscode");
		SDTrackCreate->RegisterSensorType( "GLOBAL",
				                           sizeof(_sGlobalMeasure),
				                           60,
				                           new VistaDTrackGlobalsFactory,
				                           "VistaDTrackGlobalsTranscode" );

		/** @todo still has to be implemented: a transcode for marker targets. */
		SDTrackCreate->RegisterSensorType( "MEASURE",
				                           sizeof(_sMeasureMeasure),
				                           60,
				                           NULL,
				                           "VistaDTrackMeasureTranscode");

	}

	return SDTrackCreate;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDTrackDriver::VistaDTrackDriver()
: IVistaDeviceDriver(),
  m_pConnection(new VistaDriverConnectionAspect),
  m_pProtocol( NULL ),
  m_pSensors(new VistaDriverSensorMappingAspect),
  m_vecPacketBuffer(8192),
  m_pDeSerializer(new VistaByteBufferDeSerializer),
  m_pLine(new VistaByteBufferDeSerializer),
  m_nMarkerType(~0),
  m_nGlobalType(~0),
  m_bAttachOnly(false)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);
	m_pProtocol = new VistaDTrackProtocolAspect(this);
	RegisterAspect( m_pProtocol );

	// prepare connections
	m_pConnection->SetConnection(0, NULL, "DATACONNECTION", true);
	m_pConnection->SetConnection(1, NULL, "CONTROLCONNECTION", true);

	m_pConnection->SetAttachSequence(1, new VistaDTrackAttachSequence(m_pProtocol, m_bAttachOnly));
	m_pConnection->SetDetachSequence(1, new VistaDTrackDetachSequence(m_pProtocol, m_bAttachOnly));

	RegisterAspect(m_pConnection);

	// register sensor type STICK measuring 18 values
	unsigned int nStickType  =  m_pSensors->RegisterType("STICK",  sizeof(_sStickMeasure), 60,new VistaDTrackStickFactory(8));

	// register sensor type BODY measuring 17 values (same as stick without button)
	unsigned int nBodyType   =  m_pSensors->RegisterType("BODY",   sizeof(_sBodyMeasure), 60, new VistaDTrackBodyFactory);

	// register sensort type MARKER catching id,qu and 3dof
	m_nMarkerType =  m_pSensors->RegisterType("MARKER",  sizeof(_sMarkerMeasure), 60, new VistaDTrackMarkerFactory);


	// register type global, catching frame count and (optional) clock
	m_nGlobalType =  m_pSensors->RegisterType("GLOBAL",  sizeof(_sGlobalMeasure), 60, new VistaDTrackGlobalsFactory);

	// register type MEAUSRE, measuring id, qu, bt, pos (no euler) and rot
	// IAR: fix factory once the 6mt type is needed
	unsigned int nMeasureType = m_pSensors->RegisterType("MEASURE",sizeof(_sMeasureMeasure), 60, NULL);


	RegisterAspect(m_pSensors);


	// create fr decoder
	m_mapDecoder.insert(DECODEMAP::value_type("fr", new FrDecode(m_nGlobalType)));
	// create ts decoder
	m_mapDecoder.insert(DECODEMAP::value_type("ts", new FrDecode(m_nGlobalType)));

	std::vector<int> vTmp;
	vTmp.push_back(2); // 2 doubles for the header
	vTmp.push_back(6); // 6 for the pos / euler frame
	vTmp.push_back(9); // 9 for the rot matrix

	// 6d: normal body
	// [id qu][sx sy sz ex ey ez][b0 b1 b2 b3 b4 b5 b6 b7 b8]
	m_mapDecoder.insert(DECODEMAP::value_type("6d", new GenDecode(nBodyType, vTmp)));

	// 6df: flystick
	// [id qu bt][sx sy sz ex ey ez][b0 b1 b2 b3 b4 b5 b6 b7 b8]
	vTmp[0] = 3; // 3 for the header, the rest is unaltered
	m_mapDecoder.insert(DECODEMAP::value_type("6df", new GenDecode(nStickType, vTmp)));


	// 6dmt, measuring tool
	// [id qu bt][sx sy sz][b0 b1 b2 b3 b4 b5 b6 b7 b8]
	vTmp[1] = 3; // same here for the pos /euler frame
	m_mapDecoder.insert(DECODEMAP::value_type("6dmt", new GenDecode(nMeasureType, vTmp)));

	// 3d: single marker,
	// [id qu][sx sy sz]
	vTmp.clear(); // only two blocks
	vTmp.push_back(2); // 2 first
	vTmp.push_back(3); // then 3 doubles
	m_mapDecoder.insert(DECODEMAP::value_type("3d", new GenDecode(m_nMarkerType, vTmp)));

}

VistaDTrackDriver::~VistaDTrackDriver()
{
	UnregisterAspect(m_pConnection, false);
	delete m_pConnection;

	UnregisterAspect(m_pSensors, false);
	delete m_pSensors;

	UnregisterAspect(m_pProtocol, false);
	delete m_pProtocol;


	// generically delete all sensors
	DeleteAllSensors();


	delete m_pDeSerializer;
	delete m_pLine;

	for(DECODEMAP::iterator it = m_mapDecoder.begin();
		it != m_mapDecoder.end(); ++it)
	{
		delete (*it).second;
	}
}

bool VistaDTrackDriver::DoSensorUpdate(VistaType::microtime dTs)
{
// 	cout << "VistaDTrackDriver::DoSensorUpdate("
//              << dTs << ")" << std::endl;

	bool bTimeout = false;
	int nRet = m_pConnection->GetCommand(0, (void*)&m_vecPacketBuffer[0], 8192, 5000, &bTimeout);

	if(nRet == -1)
	{
		vstr::warnf() << "VistaDTrackDriver::DoSensorUpdate() - GetCommand Failed";
		return false;
	}

	if(bTimeout)
	{
		vstr::warnf() << "VistaDTrackDriver::DoSensorUpdate() - Timeout";
		return false;
	}

 	//cout << "Read [" << nRet << "] bytes from connection\n";

	(*m_pDeSerializer).SetBuffer( &m_vecPacketBuffer[0], nRet, false );

	VistaDriverLoggingAspect *pLog
		= static_cast<VistaDriverLoggingAspect*>(
			GetAspectById(VistaDriverLoggingAspect::GetAspectId()));

	//std::vector<double> vTmp(8192);
	bool bRet = false;
	while((*m_pDeSerializer).GetTailSize())
	{
		std::string strLine;
		(*m_pDeSerializer).ReadDelimitedString(strLine, 0x0a);

		if(pLog)
		{
			pLog->Log(dTs, this, strLine);
		}

		//cout << "L: " << strLine << std::endl;
		(*m_pLine).SetBuffer( reinterpret_cast<const VistaType::ubyte8*>( strLine.c_str() ), (int)strLine.size(), false);

		std::string strType;
		(*m_pLine).ReadDelimitedString(strType, ' ');
		// lookup decoder
		DECODEMAP::iterator it = m_mapDecoder.find(strType);
		if( it != m_mapDecoder.end() )
		{
			// determine number of values to read
			unsigned int nMarkerType = ~0;

			// we read off the first part of the sentence
			// the decoder will store its type information
			// in nMarkerType, if the record contains more than
			// one entry, this value is returned as nNumValues
			int nNumValues = (*(*it).second).ReadMarkerType(m_pLine, nMarkerType);

			if(nMarkerType == m_nGlobalType)
			{
				// this is kinda hacked
				unsigned int nSensorId = ~0;
				// the art protocol does not assign ids to global information
				// like frame count and timestamp, even more unfortunate,
				// the order of the sentences in one record is not guaranteed
				// although it looks as if fr and ts always proceed
				// a record. However, we do want to record that data
				// as a "virtual" sensor, where we map fr count to be sensor 0
				// and ts sensor 1, so the user can decide on using this
				// sensor as input type "GLOBAL"
				if(strType == "fr")
					nSensorId = 0;
				else if(strType == "ts")
					nSensorId = 1;

				unsigned int nMappedId = m_pSensors->GetSensorId(nMarkerType, nSensorId);
				VistaDeviceSensor *pSen = GetSensorByIndex(nMappedId);
				if(pSen)
				{
					m_pHistoryAspect->MeasureStart(pSen, dTs);
					VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSen);
					assert(pM);

					unsigned int nIdx = 0;
					(*(*it).second).ReadAllBlocksWithOffset(m_pLine, (*pM).m_vecMeasures, nIdx);
					m_pHistoryAspect->MeasureStop(pSen);
					pSen->SetUpdateTimeStamp(dTs);
					bRet = true;
				}
				// the else case is not critical, as up to now,
				// all global sentences are fed in one block which
				// is clearly separated by the 0xd 0xa ending
			}
			else
			{
				// so far, all other records contain at least a preamble
				// with [id qu ...], we read off this information
				// to a temporary buffer, as we need to know where to
				// store the upcoming information
				VistaSensorMeasure::MEASUREVEC vTmp((*it).second->m_vecBlockLength[0]*sizeof(double));
				unsigned int nMarkerIndex = 0;
				for(int n=0; n < nNumValues; ++n)
				{
					//cout << "Trying to Read sensor [" << n << "]\n";
					// figure out which sensor to use:
					if((*(*it).second).ReadSingleBlock(m_pLine, vTmp) == (int)vTmp.size()/(int)sizeof(double))
					{
						// vTmp[0]: id
						// vTmp[1]: qu
						// vTmp[2]: bt (if (*it).second->m_vecBlockLength() == 3)

						// determine type related index
						VistaDeviceSensor *pSen = NULL;

						if(nMarkerType == m_nMarkerType)
						{
							// markers are special as they have no real id!
							// so we map the markers as they come to the markers
							// as given in the configuration file
							unsigned int nNum = m_pSensors->GetNumRegisteredSensorsForType(nMarkerType);
							if(nMarkerIndex < nNum)
							{
								// ok, we still have a marker left to read
								unsigned int nSensorId = m_pSensors->GetSensorId(nMarkerType, nMarkerIndex);

								// increase the marker index
								nMarkerIndex = (++nMarkerIndex) % (m_pSensors->GetNumRegisteredSensorsForType(nMarkerType)+1);

								// retrieve sensor
								pSen = GetSensorByIndex(nSensorId);
							} // else: we have no more virtual marker left to read
						}
						else
						{
							// no marker, retrieve sensor naturally
							_sGenericMeasure *m = (_sGenericMeasure*)&vTmp[0];

							unsigned int nSensorId = m_pSensors->GetSensorId(nMarkerType, (unsigned int)m->m_anField[0]);
							// retrieve from internal map
							pSen = GetSensorByIndex(nSensorId);
						}

						// markers are counted 'as seen'
						// so we map them to the index % as many as are registered


						// we will store vTmp[0] and vTmp[1] in the resulting measure
						// of the sensor later on,

						//cout << "Sensor(" << pSen << "; " << nMarkerType << ", " << vTmp[0] << ", " << nMarkerIndex << ")";
						if(pSen)
						{
							 //cout << " -- FOUND\n";

							m_pHistoryAspect->MeasureStart(pSen, dTs);
							VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSen);
							assert(pM);
							unsigned int nIdx = (unsigned int)(double(vTmp.size())/sizeof(double));
							double *dField = (double*)&vTmp[0];
							_sGenericMeasure *m = (_sGenericMeasure*)&(*pM).m_vecMeasures[0];

							for(unsigned int l=0; l < nIdx; ++l)
								m->m_anField[l] = dField[l];

							(*(*it).second).ReadAllBlocksWithOffset(m_pLine,
															   (*pM).m_vecMeasures,
															   nIdx);

//							for(int i=0; i < (*pM).m_vecMeasures.size(); ++i)
//								std::cout << (*pM).m_vecMeasures[i] << " ";
//							std::cout  << std::endl;

							// we skip two eventually pending whitespace or 0x0d
							// in order to relax the parsing constraint for the
							// next block
//							char c;
//							m_pLine->ReadRawBuffer(&c, sizeof(char));
							m_pHistoryAspect->MeasureStop(pSen);
							pSen->SetUpdateTimeStamp(dTs);
							bRet = true;
						}
						else
						{
							//cout << " -- NOT FOUND\n";

							// we could not find the sensor for this packet,
							// maybe it is not configured, so we simply read
							// the rest of the string off and continue
							VistaSensorMeasure::MEASUREVEC vSkip(32*sizeof(double));
							unsigned int nIdx = 0;
							(*(*it).second).ReadAllBlocksWithOffset(m_pLine, vSkip, nIdx);
							char c;
							m_pLine->ReadRawBuffer(&c, sizeof(char));
						}
					}
				}
			}
		}

		//cout << "(TS) : " << (*m_pDeSerializer).GetTailSize() << std::endl;
		if((*m_pDeSerializer).GetTailSize() == 1)
		{
			//cout << "(TS == 1)\n";
			// the dtrack2 protocol seems to be buggy sometimes
			// as some stoopid in munich forgot to remove the trailing
			// '\0' when sending the dtrack ascii packet, so this parser
			// gets confused. However, when the tailsize is 1, no real
			// value is encoded in the size-1-string, so we simply skip it here
			// the 'old' dtrack does not contain this. Go and beat ART.
			char c;
			(*m_pDeSerializer).ReadRawBuffer(&c, sizeof(char));
		}
	}

	return bRet;
}

bool VistaDTrackDriver::PhysicalEnable(bool bEnable)
{
	if(!m_bAttachOnly) // will not send enable/disable only on specific request
	{
		if(bEnable)
		{
			return m_pProtocol->GetProtocol()->SendEnableString(m_pConnection->GetConnection(1));

		}
		else
		{
			return m_pProtocol->GetProtocol()->SendDisableString(m_pConnection->GetConnection(1));
		}
	}
	return true;
}

bool VistaDTrackDriver::GetAttachOnly() const
{
	return m_bAttachOnly;
}

void VistaDTrackDriver::SetAttachOnly(bool bAttach)
{
	VistaDTrackAttachSequence *pAttach =
		dynamic_cast<VistaDTrackAttachSequence*>(m_pConnection->GetAttachSequence(1));

	if(pAttach)
		pAttach->SetSendStartTracking(!bAttach);

	VistaDTrackDetachSequence *pDetach =
		dynamic_cast<VistaDTrackDetachSequence*>(m_pConnection->GetDetachSequence(1));

	if(pDetach)
		pDetach->SetSendStopTracking(!bAttach);

	m_bAttachOnly = bAttach;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

