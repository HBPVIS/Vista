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
// $Id$

#ifndef _VISTAKEYBOARDDRIVER_H
#define _VISTAKEYBOARDDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"

#include "VistaDeviceDriver.h"
#include "VistaDeviceSensor.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a base class that defines data handling for keyboards. Subclasses decide
 * about window attachments and other state issues. Feel free to define your
 * very own keyboard device, one does not have to use this interface. But it can
 * simplify access for different windowing toolkits, as it defines a logical
 * layout. It tries to store
 *
 * - key values (as int) without any assumption about the encoding
 * - modifier values (as int) without any assumption about the encoding.
 *
 * subclasses can determines semantics of keys and call the UpdateKey() method
 * to store a new key to the history.
 */
class VISTADEVICEDRIVERSAPI IVistaKeyboardDriver : public IVistaDeviceDriver
{
public:
	virtual ~IVistaKeyboardDriver();

	/**
	 * memory layout of a keyboard measure. quite plain.
	 */
	struct _sKeyboardMeasure
	{
		int m_nKey,
			m_nModifier;
	};

	/**
	 * in case you want to add your onw transcoders for keyboard measures, this
	 * token can be handy. Register this with the functor reflectionable type,
	 * and things should work as expected.
	 * @return the transcoder type string used for registering the get-functors
	 */
	static std::string GetKeyboardTranscodeName();

	/**
	 * should return sizeof(_sKeyboardMeasure)
	 */
	virtual unsigned int GetSensorMeasureSize() const;
protected:
	IVistaKeyboardDriver();

	/**
	 * shorthand notation, as the keyboard only has 1 sensor :)
	 */
	void MeasureStart(microtime dTs);

	/**
	 * subclass add-keystroke API. adds a new value for keys and mods
	 * to this keyboard's history
	 * @return false iff no slot was available for writing (history size not set?)
	 */
	bool UpdateKey(int nKey, int nModifier);

	/**
	 * shorthand notation, as the keyboard only has 1 sensor :)
	 */
	void MeasureStop();
private:
};

class VISTADEVICEDRIVERSAPI VistaKeyboardDriverTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder();
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAKEYBOARDDRIVER_H
