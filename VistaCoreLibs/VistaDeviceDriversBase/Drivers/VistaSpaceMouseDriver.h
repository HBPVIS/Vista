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

#ifndef _VISTASPACEMOUSEDRIVER_H
#define _VISTASPACEMOUSEDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"

#include "../VistaDeviceDriver.h"
#include <VistaBase/VistaVectorMath.h>


#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver for serial line connection style space mice. Use it when
 * you have such an old device. The device driver expects a serial line connection,
 * as it has to twiddle a bit with timeouts due to serial transmission, so be
 * aware of that. The proper settings can be found in the manual of the
 * device.
 *
 * @todo more documentation on this
 */
class VISTADEVICEDRIVERSAPI VistaSpaceMouseDriver : public IVistaDeviceDriver
{

public:
	VistaSpaceMouseDriver();
	~VistaSpaceMouseDriver();

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();
	virtual unsigned int GetSensorMeasureSize() const;
	virtual bool PostUpdate();

public:
	// ########################################################################
	// COMMANDTABLE
	// ########################################################################
	bool CmdBeep(int nDuration);
	bool CmdKeyboard();
	bool CmdSetMode(int nDominant,int nTranslation,int nRotation);
	bool CmdRequestData();
	bool CmdSetNullRadius(int nRadius);
	bool CmdReset(void);
	bool CmdGetDeviceName(std::string &sName);

protected:
	//bool AttachConnection(VistaConnection *);
	//bool DetachConnection(VistaConnection *pCon);
	bool DoSensorUpdate(microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);
private:
	// ########################################################################
	// HELPERS
	// ########################################################################
	unsigned char EncodeValue(unsigned int nValue);
	unsigned int  DecodeValue(unsigned char cKey);
	float         BuildCoordinate(unsigned char *cBuffer);


	static char SCodeTable[16];
	VistaDriverConnectionAspect *m_pConnection;
	VistaVector3D m_v3Pos;
	VistaQuaternion m_qOri;
	std::vector<double> m_nVecButtonStates;

};

/**
 * the memory layout of a space mouse measure.
 */
struct _sSpaceMouseMeasure
{
	float  m_nPos[4],  // make it 4, that's easier to convert for the vista
					   // vector
		   m_nOri[4];
	float  m_nRotAx[4]; // raw values for the rotation axis

	double m_nButtonStates[9];
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASPACEMOUSEDRIVER_H
