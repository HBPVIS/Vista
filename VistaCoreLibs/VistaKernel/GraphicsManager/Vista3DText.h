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
// $Id: Vista3DText.h 20730 2011-03-30 15:56:24Z dr165799 $

#ifndef _IVISTA3DTEXT_H
#define _IVISTA3DTEXT_H

#include <string>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaBoundingBox;
class VistaColorRGB;
class VistaVector3D;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI IVista3DText
{
public:
	// Constructor
	//virtual IVista3DFontInterface();

	// Deconstructor
	virtual ~IVista3DText();

	virtual void		SetText(const std::string &Text ) = 0;
	virtual std::string	GetText() const                   = 0;

	virtual float       GetFontSize() const          = 0;
	virtual void        SetFontSize(float fFontSize) = 0;

	virtual float       GetFontDepth() const = 0;
	virtual void        SetFontDepth(float fDepth) = 0;

	virtual bool        FontHasChar(char c) const    = 0;

	virtual bool        SetFontName(const std::string &sFontName) = 0;
	virtual std::string GetFontName() const = 0;


	virtual VistaColorRGB GetFontColor() const = 0;
	virtual void            SetFontColor(const VistaColorRGB &) = 0;

protected:
	IVista3DText();
private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_IVISTA3DTEXTINTERFACE_H

