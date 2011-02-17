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

#include "VistaEnvironment.h"

#include <stdlib.h>
#ifdef WIN32
#define SYSTEM "WIN32"
#include <Windows.h>
#pragma warning(disable: 4996)
#elif IRIX
#define SYSTEM "IRIX"
#elif LINUX
#define SYSTEM "LINUX"
#elif SUNOS
#define SYSTEM "SUNOS"
#else
#define SYSTEM "HPUX"
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static std::string CheckedReturn(char *pcString)
{
	if(pcString)
		return std::string(pcString);
	else
		return std::string("");
}


/*============================================================================*/
/* CONSTRUCTORS                                                               */
/*============================================================================*/

VistaEnvironment::VistaEnvironment()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


std::string VistaEnvironment::GetCompileFlagSYSTEM()
{
	return SYSTEM;
};


std::string VistaEnvironment::GetCompileTime()
{
	return __TIME__;
};


std::string VistaEnvironment::GetCompileDate()
{
	return __DATE__;
};

std::string VistaEnvironment::GetHostname()
{
#ifdef WIN32
	return CheckedReturn(getenv("Computername"));
#else
	return CheckedReturn(getenv("HOSTNAME"));
#endif
}


std::string VistaEnvironment::GetOSystem()
{
#ifdef WIN32
	return CheckedReturn(getenv("OS"));
#else
	std::string a = CheckedReturn(getenv("OSNAME"));
	a = a + " " + CheckedReturn(getenv("R_OSVERSION"));
	return a;
#endif
}

std::string VistaEnvironment::GetCPUType()
{
#ifdef WIN32
	return CheckedReturn(getenv("PROCESSOR_ARCHITECTURE"));
#else
	return CheckedReturn(getenv("R_HWARCH"));
#endif
}

std::string VistaEnvironment::GetNumberOfProcessors()
{
#ifdef WIN32
	return CheckedReturn(getenv("NUMBER_OF_PROCESSORS"));
#else
	return CheckedReturn(getenv("R_HW_NPROC"));
#endif
}


std::string VistaEnvironment::GetMemory()
{
#ifdef WIN32
	return CheckedReturn((getenv("REALMEM")==0)?("No entry"):(getenv("REALMEM")));
#else
	return CheckedReturn(getenv("R_HW_REALMEM"));
#endif
}

std::string VistaEnvironment::GetEnv(const std::string &sEnv)
{
	return CheckedReturn(getenv(sEnv.c_str()));
};

void VistaEnvironment::SetEnv(const std::string &sKey, const std::string &sValue)
{
	putenv((char*)(sKey+std::string("=")+sValue).c_str());
};