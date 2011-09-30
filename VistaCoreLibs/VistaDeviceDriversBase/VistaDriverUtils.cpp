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
// $Id: VistaDriverUtils.cpp 22128 2011-07-01 11:30:05Z dr165799 $

#include "VistaDriverUtils.h"

#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#include "VistaDeviceDriver.h"
#include "DriverAspects/VistaDriverMeasureHistoryAspect.h"

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

	bool LoadCreationMethodFromPlugin( const std::string &strPathToPlugin,
		                               VistaDriverPlugin *pStoreTo  )
	{
		if( pStoreTo->m_pTranscoder == NULL )
			return false;

		// make a test for existence?
		pStoreTo->m_Plugin = VistaDLL::Open( strPathToPlugin );
		if(pStoreTo->m_Plugin)
		{
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetDeviceClassName");
			if(name)
			{
				typedef const char *(*GetNameMethod)();
				typedef IVistaDriverCreationMethod *(*CrMethod)(IVistaTranscoderFactoryFactory *);

				GetNameMethod f = (GetNameMethod)name;
				const char *pcName = f(); // claim name
				pStoreTo->m_strDriverClassName = (pcName ? std::string(pcName) : "");

				VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetCreationMethod");
				if(sym)
				{
					CrMethod m = (CrMethod)sym;
					pStoreTo->m_pMethod = m(pStoreTo->m_pTranscoder);
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


	bool LoadTranscoderFromPlugin( const std::string &strPathToPlugin,
				                           VistaDriverPlugin *pStoreTo )
	{
		// make a test for existence?		
		pStoreTo->m_TranscoderDll = VistaDLL::Open( strPathToPlugin );
		if(pStoreTo->m_TranscoderDll)
		{
			typedef IVistaTranscoderFactoryFactory *(*CrMethod)();
			VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pStoreTo->m_TranscoderDll, "CreateTranscoderFactoryFactory");
			if(sym)
			{
				CrMethod m = (CrMethod)sym;
				pStoreTo->m_pTranscoder = m(); // call m
				if(pStoreTo->m_pTranscoder)
					return true;
				else
				{
					VistaDLL::Close( pStoreTo->m_TranscoderDll );
					pStoreTo->m_TranscoderDll = NULL;
					return false;
				}
			}
			else
			{
				VistaDLL::Close( pStoreTo->m_TranscoderDll );
				pStoreTo->m_TranscoderDll = NULL;
				return false;
			}
		}
		else
			vddout << VistaDLL::GetError() << std::endl;

		return false;
	}

	bool DisposeTranscoderFromPlugin( VistaDriverPlugin *pPlug )
	{
		if( pPlug->m_Plugin || pPlug->m_pMethod )
			return false;

		if(pPlug->m_TranscoderDll)
		{
			if( pPlug->m_pTranscoder )
			{
				VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pPlug->m_TranscoderDll, "OnUnloadTranscoderFactoryFactory");
				if(sym)
				{
					typedef bool (*UnloadM)(IVistaTranscoderFactoryFactory*);
					UnloadM unload = (UnloadM)sym;
					unload(pPlug->m_pTranscoder);
				}
				else
					IVistaReferenceCountable::refdown(pPlug->m_pTranscoder);
			}
			pPlug->m_pTranscoder = NULL;

			VistaDLL::Close( pPlug->m_TranscoderDll );
			pPlug->m_TranscoderDll = NULL;
		}

		return true;
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
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pPlug->m_Plugin, "UnloadCreationMethod");
			if(name)
			{
				// ok, we have an unload method. lets query and cast it!
				typedef void (*UnloadMt)(IVistaDriverCreationMethod*);
				UnloadMt unloadFunc = (UnloadMt)name;

				unloadFunc(pPlug->m_pMethod); // call
//				if(bDeleteCm)
//					delete pPlug->m_pMethod;
				pPlug->m_pMethod = NULL;
			}
			else
			{
//				pPlug->m_pMethod->OnUnload(); // call directly

				// currently, there is no explicit dispose in plugin structure
				// even worse: no method to get hold of a generic destructor
				// we should think about this (ref-count?)
				// so we get rid of the getters and setters, but not of the method
				// itself... damn...

				if(bDeleteCm)
					IVistaReferenceCountable::refdown(pPlug->m_pMethod);

				// but instead: ignore the problem...
				pPlug->m_pMethod = NULL;
			}
		}

		if(pPlug->m_Plugin)
		{
			VistaDLL::Close( pPlug->m_Plugin ); // hopefully, the OS will take care for
			                                     // not *really* releasing resources
												 // iff we opened the SO more than once
			pPlug->m_Plugin = NULL;
		}

		DisposeTranscoderFromPlugin( pPlug );
		pPlug->m_strDriverClassName = std::string(); // clear name

		return true;
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

