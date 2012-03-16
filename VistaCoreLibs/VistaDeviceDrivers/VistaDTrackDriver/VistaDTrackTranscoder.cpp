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

#include "VistaDTrackCommonShare.h"

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>


#include <cstring>
#include <cstdio>

#include <string>

namespace
{
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

		static std::string GetTypeString() { return "VistaDTrackGlobalsTranscode"; }

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

			VistaDTrackMeasures::sGlobalMeasure *m = (VistaDTrackMeasures::sGlobalMeasure*)&(pMeasure->m_vecMeasures[0]);
			if(nIdx == 0)
				dScalar = m->m_nFrameCount;
			else
				dScalar = m->m_nTimeStamp;

			return true;
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

		static std::string GetTypeString() { return "VistaDTrackBodyTranscode"; }

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
			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
			return VistaVector3D(float(m->m_nPos[0]),
				float(m->m_nPos[1]),
				float(m->m_nPos[2]));
		}

		bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &oValue) const
		{
			if(!pMeasure)
				return false;

			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
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

			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
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

			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
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

			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];
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

			VistaDTrackMeasures::sBaseMasure *m = (VistaDTrackMeasures::sBaseMasure*)&(*pMeasure).m_vecMeasures[0];

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

			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];

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
			VistaDTrackMeasures::sBodyMeasure *m = (VistaDTrackMeasures::sBodyMeasure*)&(*pMeasure).m_vecMeasures[0];

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

		static std::string GetTypeString() { return "VistaDTrackStickTranscode"; }

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
			VistaDTrackMeasures::sStickMeasure *m = (VistaDTrackMeasures::sStickMeasure*)&(*pMeasure).m_vecMeasures[0];
			if( nIndex < 8)
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

			VistaDTrackMeasures::sStickMeasure *m = (VistaDTrackMeasures::sStickMeasure*)&(*pMeasure).m_vecMeasures[0];
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

			VistaDTrackMeasures::sStickMeasure *m = (VistaDTrackMeasures::sStickMeasure*)&(*pMeasure).m_vecMeasures[0];
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
			VistaDTrackMeasures::sStickMeasure *m = (VistaDTrackMeasures::sStickMeasure*)&(*pMeasure).m_vecMeasures[0];

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


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaDTrackStick2Transcode : public VistaDTrackBodyTranscode
	{
	public:
		VistaDTrackStick2Transcode()
			: VistaDTrackBodyTranscode()
		{
			// 5 info, 8 buttons, 8 controllers, 
			m_nNumberOfScalars = 8 + 8 + 2;
		}

		static std::string GetTypeString() { return "VistaDTrackStick2Transcode"; }

		REFL_INLINEIMP(VistaDTrackStick2Transcode, VistaDTrackBodyTranscode);
	};


	class VistaDTrackStick2ScalarGet : public VistaDTrackBodyScalarGet
	{
	public:
		VistaDTrackStick2ScalarGet()
			: VistaDTrackBodyScalarGet("DSCALAR",
			"VistaDTrackStick2Transcode", "dtrack Stick2 dscalar get") {}


		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
			double &dScalar, unsigned int nIndex) const
		{
			if(!pMeasure)
				return false;

			// normalize index
			const VistaDTrackMeasures::sStick2Measure* pStick2Measure =
						reinterpret_cast<const VistaDTrackMeasures::sStick2Measure*>( &(*pMeasure).m_vecMeasures[0] );
			if( nIndex < 8)
			{
				// trying to retrieve virtual button state
				// note that we map the virtual scalar index 5 .. 5+nNumberOfButtons
				// from a single bit mask which is stored in m_vecMeasures[2]

				dScalar =  ( pStick2Measure->m_nButtonMask & ( 1 << nIndex ) ) ? 1.0 : 0.0;
				return true;
			}
			else if( nIndex < 16 )
			{
				dScalar = pStick2Measure->m_anControllers[nIndex - 8];
				return true;
			}
			else
			{
				switch(nIndex)
				{
				case 16:
					dScalar = pStick2Measure->m_nId;
					return true;
				case 17:
					dScalar = pStick2Measure->m_nQuality;
					return true;
				default:
					break;
				}
			}
			return false;
		}
	};

	class VistaDTrackStick2ButtonGet : public IVistaMeasureTranscode::TTranscodeIndexedGet<bool>
	{
	public:
		VistaDTrackStick2ButtonGet()
			: IVistaMeasureTranscode::TTranscodeIndexedGet<bool>("BUTTONS",
				"VistaDTrackStick2Transcode", "Flystick2 buttons")
		{
		}

		virtual unsigned int GetNumberOfIndices() const { return 32; }

		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
							bool &bScalar, unsigned int nIndex) const
		{
			if(!pMeasure)
				return false;

			const VistaDTrackMeasures::sStick2Measure* pStickMeasure
						= pMeasure->getRead<VistaDTrackMeasures::sStick2Measure>();

			if( nIndex >= pStickMeasure->m_nNumberButtonValues )
				return false;

			bScalar = ( pStickMeasure->m_nButtonMask & ( 1 << nIndex ) ) != 0;
			return true;
		}
	};

	class VistaDTrackStick2OriGet : public IVistaMeasureTranscode::CQuatGet
	{
	public:
		VistaDTrackStick2OriGet()
			: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
			"VistaDTrackStick2Transcode", "dtrack Stick2 ori get") {}

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

			VistaDTrackMeasures::sStick2Measure *m = (VistaDTrackMeasures::sStick2Measure*)&(*pMeasure).m_vecMeasures[0];
			VistaTransformMatrix t (float(m->m_anRot[0]), float(m->m_anRot[3]), float(m->m_anRot[6]), 0,
									float(m->m_anRot[1]), float(m->m_anRot[4]), float(m->m_anRot[7]), 0,
									float(m->m_anRot[2]), float(m->m_anRot[5]), float(m->m_anRot[8]), 0,
									0    , 0    , 0    , 1);

			qQuat = VistaQuaternion(t);
			return true;
		}
	};

	class VistaDTrackStick2PosGet : public IVistaMeasureTranscode::CV3Get
	{
	public:
		VistaDTrackStick2PosGet()
			: IVistaMeasureTranscode::CV3Get("POSITION",
			"VistaDTrackStick2Transcode", "dtrack Stick2 pos get") {}

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

			VistaDTrackMeasures::sStick2Measure *m = (VistaDTrackMeasures::sStick2Measure*)&(*pMeasure).m_vecMeasures[0];
			v3Pos[0] = float(m->m_nPos[0]);
			v3Pos[1] = float(m->m_nPos[1]);
			v3Pos[2] = float(m->m_nPos[2]);
			return true;
		}
	};

	class VistaDTrackNamedStick2Get : public IVistaMeasureTranscode::CUIntGet
	{
	public:
		enum eTp
		{
			TAG_BTMASK = 0
		};

		VistaDTrackNamedStick2Get(eTp nIdx,
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
			VistaDTrackMeasures::sStick2Measure *m = (VistaDTrackMeasures::sStick2Measure*)&(*pMeasure).m_vecMeasures[0];

			switch( m_nIdx )
			{
			case TAG_BTMASK:
				{
					dVal = (unsigned int)(int(m->m_nButtonMask));
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

	static IVistaPropertyGetFunctor *SaStick2Get[] =
	{
		new VistaDTrackStick2ScalarGet,
		new VistaDTrackStick2ButtonGet,
		new VistaDTrackStick2OriGet,
		new VistaDTrackStick2PosGet,
		new VistaDTrackNamedStick2Get( VistaDTrackNamedStick2Get::TAG_BTMASK,
		"BTMASK",
		"VistaDTrackStick2Transcode",
		"Buttonmask as reported from the dtrack" ),
		NULL
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaDTrackHandTranscode : public VistaDTrackBodyTranscode
	{
	public:
		VistaDTrackHandTranscode()
			: VistaDTrackBodyTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaDTrackHandTranscode"; }

		REFL_INLINEIMP(VistaDTrackHandTranscode, VistaDTrackBodyTranscode);
	};


	class VistaDTrackHandScalarGet : public VistaDTrackBodyScalarGet
	{
	public:
		VistaDTrackHandScalarGet()
			: VistaDTrackBodyScalarGet("DSCALAR",
			"VistaDTrackHandTranscode", "dtrack hand dscalar get") {}


		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
			double &dScalar, unsigned int nIndex) const
		{
			if(!pMeasure)
				return false;

			//// normalize index
			//const VistaDTrackMeasures::sHandMeasure* pHandMeasure =
			//			reinterpret_cast<const VistaDTrackMeasures::sHandMeasure*>( &(*pMeasure).m_vecMeasures[0] );
			//if( nIndex < 8)
			//{
			//	// trying to retrieve virtual button state
			//	// note that we map the virtual scalar index 5 .. 5+nNumberOfButtons
			//	// from a single bit mask which is stored in m_vecMeasures[2]

			//	dScalar =  ( pHandMeasure->m_nButtonMask & ( 1 << nIndex ) ) ? 1.0 : 0.0;
			//	return true;
			//}
			//else if( nIndex < 16 )
			//{
			//	dScalar = pHandMeasure->m_anControllers[nIndex - 8];
			//	return true;
			//}
			//else
			//{
			//	switch(nIndex)
			//	{
			//	case 16:
			//		dScalar = pHandMeasure->m_nId;
			//		return true;
			//	case 17:
			//		dScalar = pHandMeasure->m_nQuality;
			//		return true;
			//	default:
			//		break;
			//	}
			//}
			return false;
		}
	};

	class VistaDTrackHandOriGet : public IVistaMeasureTranscode::CQuatGet
	{
	public:
		VistaDTrackHandOriGet()
			: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
			"VistaDTrackHandTranscode", "dtrack Hand ori get") {}

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

			VistaDTrackMeasures::sHandMeasure *m = (VistaDTrackMeasures::sHandMeasure*)&(*pMeasure).m_vecMeasures[0];
			VistaTransformMatrix matTrans;
			matTrans.SetBasisMatrix( m->m_anBackRotation );

			qQuat = VistaQuaternion( matTrans );
			return true;
		}
	};

	class VistaDTrackHandPosGet : public IVistaMeasureTranscode::CV3Get
	{
	public:
		VistaDTrackHandPosGet()
			: IVistaMeasureTranscode::CV3Get("POSITION",
			"VistaDTrackHandTranscode", "dtrack Hand pos get") {}

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

			VistaDTrackMeasures::sHandMeasure *m = (VistaDTrackMeasures::sHandMeasure*)&(*pMeasure).m_vecMeasures[0];
			v3Pos.SetValues( m->m_anBackPosition );
			return true;
		}
	};

	class VistaDTrackHandFingerPositionGet : public IVistaMeasureTranscode::CV3Get
	{
	public:
		VistaDTrackHandFingerPositionGet( int nFingerIndex,
								const std::string &strName,
								const std::string &strClass,
								const std::string &strDesc)
			: IVistaMeasureTranscode::CV3Get( strName, strClass, strDesc )
			, m_nFingerIndex( nFingerIndex )
		{
		}


		VistaVector3D GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaVector3D v3Pos;
			GetValue( pMeasure, v3Pos );
			return v3Pos;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure,
							VistaVector3D& v3Val ) const
		{
			bool bRet = true;
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			if( m_nFingerIndex >= pHandMeasure->m_nNumberOfFingers )
				return false;

			const VistaDTrackMeasures::sHandMeasure::Finger& oFinger = pHandMeasure->m_aFingers[m_nFingerIndex];
			v3Val.SetValues( oFinger.m_anPosition );
			return bRet;
		}
	private:
		int m_nFingerIndex;
	};

	class VistaDTrackHandFingerOrientationGet : public IVistaMeasureTranscode::CQuatGet
	{
	public:
		VistaDTrackHandFingerOrientationGet( int nFingerIndex,
								const std::string &strName,
								const std::string &strClass,
								const std::string &strDesc)
			: IVistaMeasureTranscode::CQuatGet( strName, strClass, strDesc )
			, m_nFingerIndex( nFingerIndex )
		{
		}


		VistaQuaternion GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaQuaternion v3Pos;
			GetValue( pMeasure, v3Pos );
			return v3Pos;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure,
							VistaQuaternion& v3Val ) const
		{
			bool bRet = true;
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			if( m_nFingerIndex >= pHandMeasure->m_nNumberOfFingers )
				return false;

			const VistaDTrackMeasures::sHandMeasure::Finger& oFinger = pHandMeasure->m_aFingers[m_nFingerIndex];
			VistaTransformMatrix matRot;
			matRot.SetBasisMatrix( oFinger.m_anRotation );
			v3Val = VistaQuaternion( matRot );
			return bRet;
		}
	private:
		int m_nFingerIndex;
	};

	class VistaDTrackHandAllFingerPositionsGet : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaVector3D> >
	{
	public:
		VistaDTrackHandAllFingerPositionsGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaVector3D> >
				( "FINGER_POSITIONS", "VistaDTrackHandTranscode",
				   "vector with positions of fingers")
		{
		}
		virtual std::vector<VistaVector3D> GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			std::vector<VistaVector3D> vecPositions;
			GetValue( pMeasure, vecPositions );
			return vecPositions;
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure, std::vector<VistaVector3D>& vecOut ) const
		{
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			vecOut.resize( (std::size_t)pHandMeasure->m_nNumberOfFingers );
			for( int i = 0; i < pHandMeasure->m_nNumberOfFingers; ++i )
				vecOut[i] = VistaVector3D( pHandMeasure->m_aFingers[i].m_anPosition );
			return true;
		}
	};

	class VistaDTrackHandAllFingerOrientationsGet : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaQuaternion> >
	{
	public:
		VistaDTrackHandAllFingerOrientationsGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaQuaternion> >
				( "FINGER_ORIENTATIONS", "VistaDTrackHandTranscode",
				   "vector with positions of fingers")
		{
		}
		virtual std::vector<VistaQuaternion> GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			std::vector<VistaQuaternion> vecPositions;
			GetValue( pMeasure, vecPositions );
			return vecPositions;
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure, std::vector<VistaQuaternion>& vecOut ) const
		{
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			vecOut.resize( (std::size_t)pHandMeasure->m_nNumberOfFingers );
			for( int i = 0; i < pHandMeasure->m_nNumberOfFingers; ++i )
			{
				VistaTransformMatrix matTransform;
				matTransform.SetBasisMatrix( pHandMeasure->m_aFingers[i].m_anRotation );
				vecOut[i] = VistaQuaternion( matTransform );
			}
			return true;
		}
	};

	static IVistaPropertyGetFunctor *SaHandGet[] =
	{
		new VistaDTrackHandScalarGet,
		new VistaDTrackHandOriGet,
		new VistaDTrackHandPosGet,
		new VistaDTrackHandAllFingerPositionsGet,
		new VistaDTrackHandAllFingerOrientationsGet,
		new VistaDTrackHandFingerPositionGet( 0, "FINGER1_POSITION", "VistaDTrackHandTranscode", "Position of thumb" ),
		new VistaDTrackHandFingerPositionGet( 1, "FINGER2_POSITION", "VistaDTrackHandTranscode", "Position of index finger" ),
		new VistaDTrackHandFingerPositionGet( 2, "FINGER3_POSITION", "VistaDTrackHandTranscode", "Position of middle finger" ),
		new VistaDTrackHandFingerPositionGet( 3, "FINGER4_POSITION", "VistaDTrackHandTranscode", "Position of ring finger" ),
		new VistaDTrackHandFingerPositionGet( 4, "FINGER5_POSITION", "VistaDTrackHandTranscode", "Position of little finger" ),
		new VistaDTrackHandFingerOrientationGet( 0, "FINGER1_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of thumb" ),
		new VistaDTrackHandFingerOrientationGet( 1, "FINGER2_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of index finger" ),
		new VistaDTrackHandFingerOrientationGet( 2, "FINGER3_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of middle finger" ),
		new VistaDTrackHandFingerOrientationGet( 3, "FINGER4_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of ring finger" ),
		new VistaDTrackHandFingerOrientationGet( 4, "FINGER5_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of little finger" ),
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

		static std::string GetTypeString() { return "VistaDTrackMarkerTranscode"; }

		REFL_INLINEIMP(VistaDTrackMarkerTranscode, IVistaMeasureTranscode);
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

			VistaDTrackMeasures::sMarkerMeasure *m = (VistaDTrackMeasures::sMarkerMeasure*)&(*pMeasure).m_vecMeasures[0];
			return VistaVector3D( float(m->m_nPos[0]),
				float(m->m_nPos[1]), float(m->m_nPos[2]) );
		}

		bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &oValue) const
		{
			if(!pMeasure)
				return false;

			VistaDTrackMeasures::sMarkerMeasure *m = (VistaDTrackMeasures::sMarkerMeasure*)&(*pMeasure).m_vecMeasures[0];
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

	class VistaDTrackDriverTranscodeFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		VistaDTrackDriverTranscodeFactoryFactory()
		{
			CreateCreators(m_creators);
		}

		~VistaDTrackDriverTranscodeFactoryFactory()
		{
			CleanupCreators(m_creators);
		}

		typedef std::map<std::string,ICreateTranscoder*> CRMAP;
		CRMAP m_creators;

		static void CreateCreators(CRMAP &mp)
		{
			mp["MARKER"]  = new TCreateTranscoder<VistaDTrackMarkerTranscode>;
			mp["STICK"]  = new TCreateTranscoder<VistaDTrackStickTranscode>;
			mp["STICK2"]  = new TCreateTranscoder<VistaDTrackStick2Transcode>;
			mp["HAND"]  = new TCreateTranscoder<VistaDTrackHandTranscode>;
			mp["BODY"]  = new TCreateTranscoder<VistaDTrackBodyTranscode>;
			mp["GLOBAL"]  = new TCreateTranscoder<VistaDTrackGlobalsTranscode>;
			//mp["MEASURE"]  = new TCreateTranscoder<VistaDTrackMeasureTranscode>;
			mp["MEASURE"]  = NULL;
		}

		static void CleanupCreators( CRMAP &mp )
		{
			for( CRMAP::iterator it = mp.begin(); it != mp.end(); ++it )
				delete (*it).second;

			mp.clear();
		}

		virtual IVistaMeasureTranscoderFactory *CreateFactoryForType( const std::string &strTypeName )
		{
			CRMAP::const_iterator it = m_creators.find( strTypeName );
			if( it == m_creators.end() )
				return NULL;
			return (*it).second->Create();
		}


		virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory *fac )
		{
			delete fac;
		}

		static void OnUnload()
		{
			CRMAP mp;
			CreateCreators(mp);
			for( CRMAP::iterator it = mp.begin(); it != mp.end(); ++it )
			{
				if( (*it).second )
					(*it).second->OnUnload();
			}
			CleanupCreators(mp);
		}
	};

}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#if VISTADTRACKTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaDTrackDriverTranscodeFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaDTrackDriverTranscodeFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaDTrackDriverTranscodeFactoryFactory)

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

