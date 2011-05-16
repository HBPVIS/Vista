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

#include "VistaExceptionBase.h"
#include "VistaBaseOut.h"

#include <string>
#include <vector>
#include <iostream>

#include <cstdio>
#include <cmath>
#include <cstdlib>

using namespace std;

#if defined(LINUX)
#include <execinfo.h>
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaExceptionBase::VistaExceptionBase(const char *pcExMsg,
					 const char *pcExSource, int iExLine, int iExNum) throw()
: std::exception(),
  m_sExceptionText(pcExMsg),
  m_sExceptionSource(pcExSource),
  m_iExceptionLine(iExLine),
  m_iExceptionNumber(iExNum)
{
#if defined(LINUX)
	/// @todo fixme?!
	// I do hope, that this is no "OutOfMemory"-Exception ;)
	void *array[25]; // we trace 25 levels
	int nSize = backtrace(array, 25);
	char **symbols = backtrace_symbols(array, nSize);
	for(int i=0; i < nSize; ++i)
	{
		m_sBacktrace = m_sBacktrace + string(symbols[i]) + string("\n");
	}

	free(symbols);
#endif
}


// PRIVATE!
VistaExceptionBase::VistaExceptionBase()
: std::exception()
{
}


VistaExceptionBase::~VistaExceptionBase() throw()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
const char* VistaExceptionBase::what() const throw()
{
	return m_sExceptionText.c_str();
}

int    VistaExceptionBase::GetExceptionNumber() const
{
	return m_iExceptionNumber;
}

string VistaExceptionBase::GetExceptionText() const
{
	return m_sExceptionText;
}

int    VistaExceptionBase::GetExceptionLine() const
{
	return m_iExceptionLine;
}

string VistaExceptionBase::GetExceptionSource() const
{
	return m_sExceptionSource;
}

void VistaExceptionBase::PrintException() const
{
	vbaseerr << GetPrintStatement() << endl;
}

string VistaExceptionBase::GetPrintStatement() const
{
	//char buffer[8192];
	string sText = GetExceptionText();
	string sSource = GetExceptionSource();
	vector<unsigned char> vecString(220     // for the decorative text
						 +sText.size()          // for the user given text
						 +sSource.size()        // for the source file text
						 +(int)ceil(log((float)GetExceptionNumber()+1))+1 // ln(dig) ~ number of digits-1
						 +(int)ceil(log((float)GetExceptionLine()+1))+1,// ln(dig) ~ number of digits-1
						 0); // init with 0
#ifdef WIN32
 #if defined (_MSC_VER)
  #if ( _MSC_VER >= 1400 )
	_snprintf_s((char*)&vecString[0], vecString.size(), vecString.size(), "VistaExceptionBase() -- Exception [%d]\n===============================================\n%s\n===============================================\nLocation: %s\nLine:     %d\n===============================================\n",
		GetExceptionNumber(), GetExceptionText().c_str(), GetExceptionSource().c_str(), GetExceptionLine());
 #else
	_snprintf((char*)&vecString[0], 8192, "VistaExceptionBase() -- Exception [%d]\n===============================================\n%s\n===============================================\nLocation: %s\nLine:     %d\n===============================================\n",
		GetExceptionNumber(), GetExceptionText().c_str(), GetExceptionSource().c_str(), GetExceptionLine());
 #endif // _MSC_VER >= 1400
 #else
	sprintf((char*)&vecString[0], "VistaExceptionBase() -- Exception [%d]\n===============================================\n%s\n===============================================\nLocation: %s\nLine:     %d\n===============================================\n",
		GetExceptionNumber(), GetExceptionText().c_str(), GetExceptionSource().c_str(), GetExceptionLine());
 #endif // _MSCVER
#else // WIN32
	snprintf((char*)&vecString[0], 8192, "VistaExceptionBase() -- Exception [%d]\n===============================================\n%s\n===============================================\nLocation: %s\nLine:     %d\n===============================================\n",
		GetExceptionNumber(), GetExceptionText().c_str(), GetExceptionSource().c_str(), GetExceptionLine());
#endif
#if defined(SUNOS) || _MSC_VER < 1400
	return string((char*)&vecString[0]);
#else
	return string(vecString.begin(), vecString.end());
#endif
}

string VistaExceptionBase::GetBacktraceString() const
{
	return m_sBacktrace;
}

void   VistaExceptionBase::PrintBacktrace() const
{
	vbaseerr << m_sBacktrace << endl;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
