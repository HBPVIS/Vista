/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#if defined(VISTA_THREADING_SPROC)

#ifndef _VISTASPROCTHREADIMP_H
#define _VISTASPROCTHREADIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaThreadImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPriority;
class VistaThread;
class VistaThreadEvent;
class sproc_hlp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaSPROCThreadImp : public IVistaThreadImp
{
public:
	VistaSPROCThreadImp(const VistaThread &);
	virtual ~VistaSPROCThreadImp();

	virtual bool Run( ) ;
	virtual bool Suspend() ;
	virtual bool Resume() ;
	virtual bool Join() ;
	virtual bool Abort() ;

	virtual bool     SetPriority   ( const VistaPriority & ) ;
	virtual void GetPriority   (VistaPriority &) const ;

	static IVistaThreadImp *CreateThreadImp(const VistaThread &);

	/**
	 * give the processor away temporarily
	 */
	void YieldThread();

	void SetCancelAbility(const bool bOkToCancel);
	bool CanBeCancelled() const;


	/**
	 * Method that is to be performed BEFORE departed fork starts execution
	 */
	virtual void PreRun() ;

	/**
	 * Method that is to be performed AFTER forked work is done
	 * DO NOT CALL THIS METHOD AT ANY COST!
	 */
	virtual void PostRun();

	virtual long GetThreadIdentity() const;
	static long GetCallingThreadIdentity() const;

protected:
	const VistaThread  &m_rThread; /**< @todo think about this */
	VistaThreadEvent *m_pFinalizeEvent;
	sproc_hlp         *m_pHlp;
	bool     m_bIsRunning;
	bool     m_bCanBeCancelled;
	long        irixPID;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAWIN32THREADMP_H


#endif // VISTA_THREADING_POSIX

