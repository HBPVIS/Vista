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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaDirectXGamepadPlugin.h"
#include "VistaDirectXGamepad.h"

#include <VistaKernel/InteractionManager/VistaDriverWindowAspect.h>

#if defined(WIN32)

BOOL APIENTRY DllMain( HANDLE hModule, 
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif //__VISTADIRECTXGAMEPADCONFIG_H


namespace 
{
	/**
	 * This is a proxy class for the original driver creation
	 * method. The plugin dll is defined in the scope of Vista,
	 * as we need a window handle for this driver to function
	 * properly. For that, we inject currently a kernel
	 * dependency to the VistaWindowAspect, which we create and
	 * register with each new driver.
	 * for all the other methods, we do a simple forwarding.
	 */
	class CreateProxy : public IVistaDriverCreationMethod
	{
	private:
		IVistaDriverCreationMethod *m_pOrig;

	public:
		CreateProxy( IVistaDriverCreationMethod *pOriginal )
			: m_pOrig(pOriginal)
		{

		}

		virtual IVistaDeviceDriver *operator()() 
		{
			IVistaDeviceDriver *pDriver = (*m_pOrig)(); // create driver

			// sanity check...
			VistaDirectXGamepad *pGp = dynamic_cast<VistaDirectXGamepad*>(pDriver);
			if(!pGp)
			{
				delete pDriver;
				return NULL;
			}

			// this is the important change: inject a driver window aspect
			// (this one comes as a dependency from the kernel!)
			pGp->RegisterAspect( new VistaDriverWindowAspect );

			return pGp;
		}

		// the rest of this API is a simple forwarder!

		virtual unsigned int RegisterSensorType( const std::string &strName,
			unsigned int nMeasureSize,
			unsigned int nUpdateEstimator,
			IVistaMeasureTranscoderFactory *pFac,
			const std::string &strTranscoderTypeString)
		{
			return m_pOrig->RegisterSensorType(strName, nMeasureSize,
				nUpdateEstimator, pFac, strTranscoderTypeString);
		}


		virtual bool         UnregisterType(const std::string &strType,
			bool bDeleteFactories = false)
		{
			return m_pOrig->UnregisterType(strType, bDeleteFactories);
		}

		virtual unsigned int GetTypeFor( const std::string &strType ) const
		{
			return m_pOrig->GetTypeFor(strType);
		}

		virtual bool GetTypeNameFor( unsigned int nTypeId, std::string &strDest ) const
		{
			return m_pOrig->GetTypeNameFor(nTypeId, strDest);
		}

		virtual unsigned int GetMeasureSizeFor( unsigned int nType ) const
		{
			return m_pOrig->GetMeasureSizeFor(nType);
		}

		virtual unsigned int GetUpdateEstimatorFor(unsigned int nType ) const
		{
			return m_pOrig->GetUpdateEstimatorFor(nType);
		}

		virtual std::list<std::string> GetTypeNames() const
		{
			return m_pOrig->GetTypeNames();
		}

		virtual std::list<unsigned int> GetTypes() const
		{
			return m_pOrig->GetTypes();
		}


		virtual IVistaMeasureTranscoderFactory *GetTranscoderFactoryForSensor(unsigned int nType) const
		{
			return m_pOrig->GetTranscoderFactoryForSensor(nType);
		}

		virtual IVistaMeasureTranscoderFactory *GetTranscoderFactoryForSensor(const std::string &strTypeName) const
		{
			return m_pOrig->GetTranscoderFactoryForSensor(strTypeName);
		}
	};

	CreateProxy *SpFactory = NULL;
}

extern "C" VISTADIRECTXGAMEPADPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int)
{
	VistaDirectXGamepad *pPad = new VistaDirectXGamepad;
	pPad->RegisterAspect( new VistaDriverWindowAspect );
	return pPad;
}


extern "C" VISTADIRECTXGAMEPADPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new CreateProxy(  VistaDirectXGamepad::GetDriverFactoryMethod() );
	}
	return SpFactory;
}

extern "C" VISTADIRECTXGAMEPADPLUGINAPI const char *GetDeviceClassName()
{
	return "DIRECTXGAMEPAD";
}


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


