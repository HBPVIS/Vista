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

#include "VistaDriverUtils.h"

#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#include "VistaDeviceDriver.h"
#include "VistaDriverMeasureHistoryAspect.h"

#include "VistaDeviceDriversOut.h"

#include <cmath>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
namespace VddUtil
{
	bool LoadDriverFromPlugin( const std::string &strPathToPlugin,
		                       VistaDriverPlugin *pStoreTo )
	{
		// make a test for existence?
		pStoreTo->m_Plugin = VistaDLL::Open( strPathToPlugin );
		if(pStoreTo->m_Plugin)
		{
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetDeviceClassName");
			if(name)
			{
				typedef const char *(*GetNameMethod)();
				typedef IVistaDeviceDriver *(*CrMethod)(int);

				GetNameMethod f = (GetNameMethod)name;
				const char *pcName = f(); // claim name
				pStoreTo->m_strDriverClassName = (pcName ? std::string(pcName) : "");

				VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "CreateDevice");
				if(sym)
				{
					CrMethod m = (CrMethod)sym;
					pStoreTo->m_pDriver = m(0); // !!! what about the 0?
					if(pStoreTo->m_pDriver)
					{
						// ok, everything is nice so far,
						return true;
					}
					else
					{
						VistaDLL::Close( pStoreTo->m_Plugin );
						return false;
					}
				}
			}
			else
			{
				VistaDLL::Close( pStoreTo->m_Plugin );
				pStoreTo->m_Plugin = NULL;
			}
		}
		else
			vddout << VistaDLL::GetError() << std::endl;

		return false;
	}

	bool LoadCreationMethodFromPlugin( const std::string &strPathToPlugin,
		                               VistaDriverPlugin *pStoreTo  )
	{
		// make a test for existence?
		pStoreTo->m_Plugin = VistaDLL::Open( strPathToPlugin );
		if(pStoreTo->m_Plugin)
		{
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetDeviceClassName");
			if(name)
			{
				typedef const char *(*GetNameMethod)();
				typedef IVistaDriverCreationMethod *(*CrMethod)();

				GetNameMethod f = (GetNameMethod)name;
				const char *pcName = f(); // claim name
				pStoreTo->m_strDriverClassName = (pcName ? std::string(pcName) : "");

				VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetCreationMethod");
				if(sym)
				{
					CrMethod m = (CrMethod)sym;
					pStoreTo->m_pMethod = m();
					if(pStoreTo->m_pMethod)
					{
						// ok, everything is nice so far,
						return true;
					}
					else
					{
						VistaDLL::Close( pStoreTo->m_Plugin );
						pStoreTo->m_Plugin = NULL;
						return false;
					}
				}
				else
				{
					VistaDLL::Close( pStoreTo->m_Plugin );
					pStoreTo->m_Plugin = NULL;
					return false;
				}
			}
			else
			{
				VistaDLL::Close( pStoreTo->m_Plugin );
				pStoreTo->m_Plugin = NULL;
			}
		}
		else
			vddout << VistaDLL::GetError() << std::endl;
		return false;
	}

	bool DisposePlugin( VistaDriverPlugin *pPlug, bool bDeleteCm )
	{
		if( pPlug->m_pDriver )
		{
			// currently, there is no explicit dispose in plugin structure
			delete pPlug->m_pDriver;
			pPlug->m_pDriver = NULL;
		}

		if( pPlug->m_pMethod )
		{
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pPlug->m_Plugin, "Unload");
			if(name)
			{
				// ok, we have an unload method. let query and cast it!
				typedef void (*UnloadMt)(IVistaDriverCreationMethod*);
				UnloadMt ul = (UnloadMt)name;

				ul(pPlug->m_pMethod); // call

				if(bDeleteCm)
					delete pPlug->m_pMethod;

				pPlug->m_pMethod = NULL;
			}
			else
			{
				pPlug->m_pMethod->OnUnload(); // call directly

				// currently, there is no explicit dispose in plugin structure
				// even worse: no method to get hold of a generic destructor
				// we should think about this (ref-count?)
				// so we get rid of the getters and setters, but not of the method
				// itself... damn...

				if(bDeleteCm)
					delete pPlug->m_pMethod;

				// but instead: ignore the problem...
				pPlug->m_pMethod = NULL;
			}
		}

		if(pPlug->m_Plugin)
		{
			VistaDLL::Close( pPlug->m_Plugin ); // hopefully, the OS will take care for
			                                     // not *really* releasing resources
												 // iff we opened the SO more than once
		}
		return true;
	}
	bool SetupSensorHistory( IVistaDeviceDriver *pDriver, VistaDeviceSensor *pSensor,
					  IVistaDriverCreationMethod *pCrMethod,
					  const std::string &strSensorTypeName,
											unsigned int nDesiredReadSlots,
											unsigned int nReadLengthInMsecs )
	{
		unsigned int nType = pCrMethod->GetTypeFor( strSensorTypeName );
		if(nType == ~0)
			return false;

		unsigned int nUpdateEstimator = pCrMethod->GetUpdateEstimatorFor( nType );
		unsigned int nSlotLength = pCrMethod->GetMeasureSizeFor( nType );

		// calculation:
		// the update estimator gives the refresh rate in Hz (best-case)
		// so, for a 15 frame min update, we need:
		// rt: 1000/estimator -> msecs per sample
		// mr: 1000/nReadLengthInMsecs -> msecs per frame (max. read-time)
		// mr/rt = # additional packets we need in order not to create
		// an overrun during write
		unsigned int nDriverSize = int(std::ceil((double(nReadLengthInMsecs)/double(1000.0f/double(nUpdateEstimator)))));

		VistaDriverMeasureHistoryAspect *pHistory =
			dynamic_cast<VistaDriverMeasureHistoryAspect*>( pDriver->GetAspectById(
					VistaDriverMeasureHistoryAspect::GetAspectId()) );

		// set 2 times the estimated driver size (as paranoia factor ;)
		return pHistory->SetHistorySize( pSensor, nDesiredReadSlots, 2*nDriverSize, nSlotLength );
	}

	bool InitVdd()
	{
		//if( IVistaTimerImp::GetSingleton() )
		//	return true;

		// no timer set... but we need one...
		// note: this is a dangling pointer, we hope somebody will clean up
		// after us. Not nice, but the concept has to settle.
		//IVistaTimerImp::SetSingleton( new VistaDefaultTimerImp );
		return true;
	}

	bool ExitVdd()
	{
		//delete IVistaTimerImp::GetSingleton();
		//IVistaTimerImp::SetSingleton(NULL);
		return true;
	}

}

