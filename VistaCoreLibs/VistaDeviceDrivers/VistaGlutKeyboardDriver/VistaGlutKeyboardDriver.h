	/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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

#ifndef _VISTAGLUTKEYBOARDDRIVER_H
#define _VISTAGLUTKEYBOARDDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <map>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAGLUTKEYBOARDDRIVER_STATIC)
#ifdef VISTAGLUTKEYBOARDDRIVER_EXPORTS
#define VISTAGLUTKEYBOARDDRIVERAPI __declspec(dllexport)
#else
#define VISTAGLUTKEYBOARDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAGLUTKEYBOARDDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverAbstractWindowAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAGLUTKEYBOARDDRIVERAPI VistaGlutKeyboardDriver : public IVistaKeyboardDriver
{
public:
	VistaGlutKeyboardDriver(IVistaDriverCreationMethod *);
	virtual ~VistaGlutKeyboardDriver();

	static void KeyDownFunction(unsigned char, int, int);
	static void KeyUpFunction(unsigned char, int, int);
	static void SpecialKeyDownFunction(int, int, int);
	static void SpecialKeyUpFunction(int, int, int);
protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
private:
	static void SetKeyValue(VistaGlutKeyboardDriver *,
							unsigned char ucKey,
							bool bIsUp,
							int nModifier);
	static void SetSpecialKeyValue(VistaGlutKeyboardDriver *pKeyboard,
								   int nKeyValue,
								   bool bIsUp,
								   int nModifier);

	struct _sKeyHlp
	{
		_sKeyHlp()
			: m_nKey(-1),
			  m_nModifier(-1),
			  m_bUpKey(false)
		{}

		_sKeyHlp( int nKey, int nModifier, bool bUp )
			: m_nKey(nKey), m_nModifier(nModifier), m_bUpKey(bUp)
		{
		}


		int m_nKey,
			m_nModifier;
		bool m_bUpKey;
	};

	std::vector< _sKeyHlp > m_vecKeyVec;
	bool m_bLastFrameValue;

	VistaDriverAbstractWindowAspect *m_pWindowAspect;
};

class VISTAGLUTKEYBOARDDRIVERAPI VistaGlutKeyboardDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaGlutKeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory *fac);
	virtual IVistaDeviceDriver *CreateDriver();
protected:
private:
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGGLUTKEYBOARDDRIVER_H
