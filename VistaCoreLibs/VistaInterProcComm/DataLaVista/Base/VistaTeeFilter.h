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
// $Id: VistaTeeFilter.h 22867 2011-08-07 15:29:00Z dr165799 $

#ifndef IDLVISTATEEFILTER_H
#define IDLVISTATEEFILTER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaInPlaceFilter.h>
#include <deque>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaDataPacket;
class DLVistaPacketQueue;
class VistaMutex;
class VistaThreadEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAINTERPROCCOMMAPI IDLVistaTeeFilter : public IDLVistaInPlaceFilter
{
public:
	IDLVistaTeeFilter();
	~IDLVistaTeeFilter();


	virtual bool AttachOutputComponent(IDLVistaPipeComponent * pComp);
	virtual bool DetachOutputComponent(IDLVistaPipeComponent * pComp);

	virtual int GetNumberOfOutbounds() const;

	virtual IDLVistaPipeComponent *GetOutboundByIndex(int iIndex) const;

protected:
	VistaMutex *GrabMutex(VistaMutex *pMutex, bool bBlock);
	int GetPacketIndex(IDLVistaDataPacket *) const;

	void UpdatePacketIndex(IDLVistaDataPacket *pPacket, int iIndex);

protected:
	VistaMutex *m_pMutexIn, *m_pMutexOut, *m_pLockEmpty, *m_pModifyOutbounds;
	VistaThreadEvent *m_pRecycleEvent;

private:
	std::deque<IDLVistaPipeComponent *> *m_pOutbounds;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //DLVISTATEEPIPE_H

