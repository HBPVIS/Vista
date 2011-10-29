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
// $Id: VistaViewport.h 20730 2011-03-30 15:56:24Z dr165799 $

#if !defined _VISTAVIEWPORT_H
#define _VISTAVIEWPORT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>


#include "VistaDisplayEntity.h"


/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplaySystem;
class VistaWindow;
class VistaProjection;
class IVistaDisplayBridge;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaViewport is ...
 */
class VISTAKERNELAPI VistaViewport  : public VistaDisplayEntity
{
	friend class IVistaDisplayBridge;
	friend class VistaDisplayManager;

public:
	virtual ~VistaViewport();

	VistaDisplaySystem *GetDisplaySystem() const;
	std::string GetDisplaySystemName() const;

	VistaWindow        *GetWindow() const;
	std::string GetWindowName() const;

	VistaProjection    *GetProjection() const;
	std::string GetProjectionName() const;


	virtual void Debug ( std::ostream & out ) const;

	/**
	 * Get/set viewport properties. The following keys are understood:
	 *
	 * POSITION             -   [list of ints - 2 items]
	 * SIZE                 -   [list of ints - 2 items]
	 * DISPLAY_SYSTEM_NAME  -   [string][read only]
	 * WINDOW_NAME          -   [string][read only]
	 * PROJECTION_NAME      -   [string][read only]
	 */

	class VISTAKERNELAPI VistaViewportProperties : public IVistaDisplayEntityProperties
	{
	public:

		enum
		{
			MSG_POSITION_CHANGE = IVistaDisplayEntityProperties::MSG_LAST,
			MSG_SIZE_CHANGE,
			MSG_LAST
		};

		bool SetName(const std::string &sName);

		bool GetPosition(int& x, int& y) const;
		bool SetPosition(const int x, const int y);

		bool GetSize(int& w, int& h) const;
		bool SetSize(const int w, const int h);

		virtual std::string GetReflectionableType() const;
	protected:
		virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;

	private:
		VistaViewportProperties(
			VistaDisplayEntity *,
			IVistaDisplayBridge *);
		virtual ~VistaViewportProperties();
		friend class VistaViewport;
	};

	VistaViewportProperties *GetViewportProperties() const;

protected:
	virtual IVistaDisplayEntityProperties *CreateProperties();
	VistaViewport  (VistaDisplaySystem *pDisplaySystem,
					 VistaWindow *pWindow,
					 IVistaDisplayEntityData *pData,
					 IVistaDisplayBridge *pBridge);

	void SetProjection(VistaProjection *pProjection);

private:
	VistaDisplaySystem     *m_pDisplaySystem;
	VistaWindow            *m_pWindow;
	VistaProjection        *m_pProjection;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAVIEWPORT_H)
