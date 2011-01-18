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

#ifndef _VISTAGLUTKEYBOARDDRIVER_H
#define _VISTAGLUTKEYBOARDDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaDeviceDriversBase/VistaKeyboardDriver.h>
#include <map>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverWindowAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaGlutKeyboardDriver : public IVistaKeyboardDriver
{
public:
	VistaGlutKeyboardDriver();
	virtual ~VistaGlutKeyboardDriver();

	static IVistaDriverCreationMethod *GetDriverCreationMethod();

	static void KeyDownFunction(unsigned char, int, int);
	static void KeyUpFunction(unsigned char, int, int);
	static void SpecialKeyDownFunction(int, int, int);
	static void SpecialKeyUpFunction(int, int, int);
protected:
	virtual bool DoSensorUpdate(microtime dTs);
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

	VistaDriverWindowAspect *m_pWindowAspect;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGGLUTKEYBOARDDRIVER_H
