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

#if !defined _VISTAWINDOW_H
#define _VISTAWINDOW_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaDisplayEntity.h"

#include <vector>


/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplay;
class VistaViewport;
class IVistaDisplayBridge;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaWindow is ...
 */
class VISTAKERNELAPI VistaWindow  : public VistaDisplayEntity
{
	friend class IVistaDisplayBridge;
	friend class VistaDisplayManager;

public:
	virtual ~VistaWindow();

	void *GetWindowId() const;

	VistaDisplay *GetDisplay() const;
	std::string GetDisplayName() const;

	unsigned int GetNumberOfViewports() const;
	std::list<std::string> GetViewportNames() const;

	VistaViewport *GetViewport(unsigned int iIndex) const;
	std::vector<VistaViewport *> &GetViewports();

	virtual void Debug ( std::ostream & out ) const;

	/**
	 * Get/set window properties. The following keys are understood:
	 *
	 * STEREO               -   [bool]
	 * POSITION             -   [list of ints - 2 items]
	 * SIZE                 -   [list of ints - 2 items]
	 * DRAW_BORDER          -   [bool]
	 * FULLSCREEN           -   [bool]
	 * TITLE                -   [string]
	 * DISPLAY_NAME         -   [string][read only]
	 * NUMBER_OF_VIEWPORTS  -   [int][read only]
	 * VIEWPORT_NAMES       -   [list of strings][read only]
	 */

	class VISTAKERNELAPI VistaWindowProperties : public IVistaDisplayEntityProperties
	{
		friend class VistaWindow;

	public:
		enum {
			MSG_STEREO_CHANGE = IVistaDisplayEntityProperties::MSG_LAST,
			MSG_POSITION_CHANGE,
			MSG_SIZE_CHANGE,
			MSG_FULLSCREEN_CHANGE,
			MSG_TITLE_CHANGE,
			MSG_LAST
		};

		bool  SetName(const std::string &sName);

		bool GetStereo() const;
		bool SetStereo(const bool bStereo);

		bool GetPosition(int& x, int& y) const;
		bool SetPosition(const int x, const int y);

		bool GetSize(int& w, int& h) const;
		bool SetSize(const int w, const int h);

		bool SetFullScreen(bool bFullScreen);
		bool GetFullScreen() const;

		std::string GetTitle() const;
		bool SetTitle(const std::string& strTitle);

		virtual std::string GetReflectionableType() const;

	protected:
		virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;
		virtual ~VistaWindowProperties();

	private:
		VistaWindowProperties(VistaWindow *, IVistaDisplayBridge *);
	};

	VistaWindowProperties *GetWindowProperties() const;

protected:
	virtual IVistaDisplayEntityProperties *CreateProperties();
	VistaWindow(VistaDisplay *pDisplay,
				 IVistaDisplayEntityData *pData,
				 IVistaDisplayBridge *pBridge);

	VistaDisplay       *m_pDisplay;

private:
	std::vector<VistaViewport *>   m_vecViewports;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif // !defined(_VISTAWINDOW_H)
