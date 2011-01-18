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

#ifndef __VISTAWIIMOTEDRIVERCONFIG_H
#define __VISTAWIIMOTEDRIVERCONFIG_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning (disable: 4786)
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Shared library support
#ifdef WIN32
#define VISTAWIIMOTEDRIVEREXPORT __declspec(dllexport)
#define VISTAWIIMOTEDRIVERIMPORT __declspec(dllimport)
#else
#define VISTAWIIMOTEDRIVEREXPORT
#define VISTAWIIMOTEDRIVERIMPORT
#endif

// Define VISTAWIIMOTEDRIVERAPI for DLL builds
#ifdef VISTAWIIMOTEDRIVERDLL
#ifdef VISTAWIIMOTEDRIVERDLL_EXPORT
#define VISTAWIIMOTEDRIVERAPI VISTAWIIMOTEDRIVEREXPORT
#else
#define VISTAWIIMOTEDRIVERAPI VISTAWIIMOTEDRIVERIMPORT
#endif
#else
#define VISTAWIIMOTEDRIVERAPI
#endif

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTAWIIMOTEDRIVERCONFIG_H





