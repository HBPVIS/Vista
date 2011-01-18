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

#ifndef __VISTADTRACKSDKDRIVERCONFIG_H
#define __VISTADTRACKSDKDRIVERCONFIG_H

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
#define VISTADTRACKSDKDRIVEREXPORT __declspec(dllexport)
#define VISTADTRACKSDKDRIVERIMPORT __declspec(dllimport)
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE_EXPORT
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTADTRACKSDKDRIVEREXPORT
#define VISTADTRACKSDKDRIVERIMPORT
#endif

// Define VISTADTRACKSDKDRIVERAPI for DLL builds
#ifdef VISTADTRACKSDKDRIVERDLL
#ifdef VISTADTRACKSDKDRIVERDLL_EXPORTS
#define VISTADTRACKSDKDRIVERAPI VISTADTRACKSDKDRIVEREXPORT
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE
#else
#define VISTADTRACKSDKDRIVERAPI VISTADTRACKSDKDRIVERIMPORT
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTADTRACKSDKDRIVERAPI
#endif

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTADTRACKSDKDRIVERCONFIG_H





