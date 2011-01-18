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

#include "VistaDisplayBridge.h"

#include "VistaDisplaySystem.h"
#include "VistaDisplay.h"
#include "VistaWindow.h"
#include "VistaViewport.h"
#include "VistaProjection.h"

#include <VistaKernel/VistaKernelOut.h>

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
IVistaDisplayBridge::IVistaDisplayBridge()
{
}

IVistaDisplayBridge::~IVistaDisplayBridge()
{
#ifdef DEBUG
	vkernout << " [VistaDisplayBridge] >> DESTRUCTOR <<" << endl;
#endif
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UpdateDisplaySystems                                        */
/*                                                                            */
/*============================================================================*/
bool IVistaDisplayBridge::UpdateDisplaySystems(VistaDisplayManager *pDisplayManager)
{
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserverUpdate                                              */
/*                                                                            */
/*============================================================================*/
void IVistaDisplayBridge::ObserverUpdate(IVistaObserveable *pObserveable,
										 int msg,
										 int ticket,
										 VistaDisplaySystem *pTarget)
{
	// let the toolkit-specific implementation decide on how to react 
	// to this...
	// we just provide this empty body, in case an implementation does not 
	// need to observe anything...
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   NewDisplaySystem                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem *IVistaDisplayBridge::NewDisplaySystem(VistaVirtualPlatform *pReferenceFrame,
														   IVistaDisplayEntityData *pData,
														   VistaDisplayManager *pDisplayManager)
{
	VistaDisplaySystem *pDS = new VistaDisplaySystem(pDisplayManager, pReferenceFrame, pData, this);
	pDS->InitProperties();
	return pDS;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   NewDisplay                                                  */
/*                                                                            */
/*============================================================================*/
VistaDisplay *IVistaDisplayBridge::NewDisplay(IVistaDisplayEntityData *pData,
											   VistaDisplayManager *pDisplayManager)
{
	VistaDisplay *pD = new VistaDisplay(pDisplayManager, pData, this);
	pD->InitProperties();
	return pD;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   NewWindow                                                   */
/*                                                                            */
/*============================================================================*/
VistaWindow *IVistaDisplayBridge::NewWindow(VistaDisplay *pDisplay,
											 IVistaDisplayEntityData *pData)
{
	VistaWindow *pW = new VistaWindow(pDisplay, pData, this);
	pW->InitProperties();
	return pW;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   NewViewport                                                 */
/*                                                                            */
/*============================================================================*/
VistaViewport *IVistaDisplayBridge::NewViewport(VistaDisplaySystem *pDisplaySystem,
												 VistaWindow *pWindow,
												 IVistaDisplayEntityData *pData)
{
	VistaViewport *pV = new VistaViewport(pDisplaySystem, pWindow, pData, this);
	pV->InitProperties();
	return pV;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   NewProjection                                               */
/*                                                                            */
/*============================================================================*/
VistaProjection *IVistaDisplayBridge::NewProjection(VistaViewport *pViewport,
													 IVistaDisplayEntityData *pData)
{
	VistaProjection *pP = new VistaProjection(pViewport, pData, this);
	pP->InitProperties();
	return pP;
}


