/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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

#ifndef __VISTAVRPNDRIVERCONFIG_H
#define __VISTAVRPNDRIVERCONFIG_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning (disable: 4786)
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VRPNDRIVER_STATIC) 
#ifdef VRPNDRIVER_EXPORTS
#define VRPNDRIVERAPI __declspec(dllexport)
#else
#define VRPNDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VRPNDRIVERAPI
#endif

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTAVRPNDRIVERCONFIG_H





