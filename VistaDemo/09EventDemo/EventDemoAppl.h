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
/*
 * $Id$
 */

#ifndef _EVENTDEMOAPPL_H
#define _EVENTDEMOAPPL_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaCentralEventHandler.h>
#include "DemoHandler.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class GreedyObserver;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * EventDemoAppl is only a template for own aplications based on ViSTA
 * 
 * @AUTHOR  Andreas Gerndt
 * @DATE    07.09.2001
 */    
class EventDemoAppl
{
public:
    // CONSTRUCTORS / DESTRUCTOR
    EventDemoAppl( int argc = 0, char  *argv[] = NULL );
    virtual ~EventDemoAppl();

    // IMPLEMENTATION
    void   Run  ();

    // ANOTHER
    void   CreateScene ();


// MEMBERS
private:
        VistaSystem					m_vistaSystem;
        DemoHandler*				m_pEventHandler;
        VistaEventManager*			m_pEventManager;
        GreedyObserver*				m_pGreedyObserver;
};


#endif // _EVENTDEMOAPPL_H
