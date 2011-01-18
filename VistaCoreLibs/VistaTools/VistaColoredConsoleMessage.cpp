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

#include "VistaColoredConsoleMessage.h"

#ifdef WIN32
#include <Windows.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
using namespace std;


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   PrintError prints the given String in Red on std::cout      */
/*                                                                            */
/*============================================================================*/
void VistaColoredConsoleMessage::PrintError(string errormsg)
	{		
#ifdef WIN32			
		HANDLE hStderr;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		WORD wOldColorAttrs;

		hStderr = GetStdHandle(STD_ERROR_HANDLE);

		GetConsoleScreenBufferInfo(hStderr, &csbiInfo);

		//saves current console info
		wOldColorAttrs = csbiInfo.wAttributes;
		
		//sets color to red
		SetConsoleTextAttribute(hStderr, FOREGROUND_RED | 
            FOREGROUND_INTENSITY);
		
		cerr << errormsg;
		
		//sets color back 
		SetConsoleTextAttribute(hStderr, wOldColorAttrs);
#else
			cerr << "\033[31m" << errormsg << "\033[0m" << endl;
#endif
	}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   PrintWarning prints the given String in blue on std::cout   */
/*                                                                            */
/*============================================================================*/
void VistaColoredConsoleMessage::PrintWarning(string warningmsg)
	{		
#ifdef WIN32			
		HANDLE hStdout;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		WORD wOldColorAttrs;

		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

		GetConsoleScreenBufferInfo(hStdout, &csbiInfo);

		//saves current console info
		wOldColorAttrs = csbiInfo.wAttributes;
		
		//sets color to blue
		SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | 
            FOREGROUND_INTENSITY);

		cout << warningmsg << endl;
		
		//sets color back 
		SetConsoleTextAttribute(hStdout, wOldColorAttrs);
#else
			cout << "\033[34m" << warningmsg << "\033[0m" << endl;
#endif
	}

