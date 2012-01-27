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
// $Id: VistaOpenSGWebInterface.h 21463 2011-05-24 08:20:37Z dr165799 $

#ifndef _VISTAOPENSGWEBINTERFACE_H_
#define _VISTAOPENSGWEBINTERFACE_H_

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaKernelOpenSGExtConfig.h"

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGWebInterface
{
public:
	VistaOpenSGWebInterface( VistaSystem* pVistaSystem,
								const int iPort = 8888 );
	~VistaOpenSGWebInterface();

	void SetWebPort( const int iPort );
	int GetWebPort() const;

	void SetIsEnabled( const bool bSet );
	bool GetIsEnabled() const;

private:
	VistaSystem*		m_pVistaSystem;
	struct DataWrapper;
	DataWrapper*		m_pDataWrapper;
	bool				m_bEnabled;
	int					m_iPort;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGWEBINTERFACE_H_
