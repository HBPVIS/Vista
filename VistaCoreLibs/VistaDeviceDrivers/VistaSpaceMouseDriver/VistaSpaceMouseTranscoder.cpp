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
// $Id: VistaSpaceMouseDriver.cpp 5570 2010-10-26 20:21:11Z ingoassenmacher $

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include "VistaSpaceMouseCommonShare.h"

namespace
{
	class VistaSpaceMouseMeasureTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaSpaceMouseMeasureTranscode()
		{
			// inherited
			m_nNumberOfScalars = 9;
		}

		virtual ~VistaSpaceMouseMeasureTranscode() {}
		static std::string GetTypeString() { return "VistaSpaceMouseMeasureTranscode"; }

		REFL_INLINEIMP(VistaSpaceMouseMeasureTranscode, IVistaMeasureTranscode);
	};

	class VistaSpaceMousePosTranscode : public IVistaMeasureTranscode::CV3Get
	{
	public:
		VistaSpaceMousePosTranscode()
			: IVistaMeasureTranscode::CV3Get("POSITION",
			VistaSpaceMouseMeasureTranscode::GetTypeString(),
			"spacemouse axes translational coords") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaSpaceMouseMeasures::sSpaceMouseMeasure *m = (VistaSpaceMouseMeasures::sSpaceMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
			return VistaVector3D(m->m_nPos);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaSpaceMouseRotAxisTranscode : public IVistaMeasureTranscode::CV3Get
	{
	public:
		VistaSpaceMouseRotAxisTranscode()
			: IVistaMeasureTranscode::CV3Get("ROTAXIS",
			VistaSpaceMouseMeasureTranscode::GetTypeString(),
			"spacemouse axes for rotation (raw)") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaSpaceMouseMeasures::sSpaceMouseMeasure *m = (VistaSpaceMouseMeasures::sSpaceMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
			return VistaVector3D(m->m_nRotAx);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaSpaceMouseOriTranscode : public IVistaMeasureTranscode::CQuatGet
	{
	public:
		VistaSpaceMouseOriTranscode()
			: IVistaMeasureTranscode::CQuatGet("ORIENTATION",
			VistaSpaceMouseMeasureTranscode::GetTypeString(),
			"spacemouse axes rotational coords") {}

		virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaSpaceMouseMeasures::sSpaceMouseMeasure *m = (VistaSpaceMouseMeasures::sSpaceMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
			return VistaQuaternion(m->m_nOri[0], m->m_nOri[1], m->m_nOri[2], m->m_nOri[3]);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};


	class VistaSpacemouseScalarTranscode : public IVistaMeasureTranscode::CScalarDoubleGet
	{
	public:
		VistaSpacemouseScalarTranscode()
			: IVistaMeasureTranscode::CScalarDoubleGet("DSCALAR",
			VistaSpaceMouseMeasureTranscode::GetTypeString(),
			"spacemouse keys (0-8)") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure || nIndex > 8)
				return false;

			VistaSpaceMouseMeasures::sSpaceMouseMeasure *m = (VistaSpaceMouseMeasures::sSpaceMouseMeasure*)&(*pMeasure).m_vecMeasures[0];
			dScalar = m->m_nButtonStates[nIndex];
			return true;
		}
	};

	static IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaSpaceMousePosTranscode,
		new VistaSpaceMouseOriTranscode,
		new VistaSpacemouseScalarTranscode,
		new VistaSpaceMouseRotAxisTranscode,
		NULL
	};
}

class VistaSpacemouseTranscoderFactory : public TDefaultTranscoderFactory<VistaSpaceMouseMeasureTranscode>
{
public:
	VistaSpacemouseTranscoderFactory()
		: TDefaultTranscoderFactory<VistaSpaceMouseMeasureTranscode>(VistaSpaceMouseMeasureTranscode::GetTypeString())
	{}
};

#if VISTASPACEMOUSETRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaSpacemouseTranscoderFactory> )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaSpacemouseTranscoderFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaSpacemouseTranscoderFactory>)

