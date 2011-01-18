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

#ifndef _VISTAOPENSGPHONGSHADER_H_
#define _VISTAOPENSGPHONGSHADER_H_

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

class VistaGeometry;
class IVistaNode;
class VistaOpenSGPerMaterialShader;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGPhongShader
{
public:
	VistaOpenSGPhongShader();
	VistaOpenSGPhongShader( const std::string& sVertexShaderFile,
							const std::string& sFragmentShaderFile );
	~VistaOpenSGPhongShader();

	bool ReloadFromFile();
	bool ReloadFromFile( const std::string& sVertexShaderFile,
						const std::string& sFragmentShaderFile );

	bool ApplyToGeometry( VistaGeometry *pGeometry );
	bool RemoveFromGeometry( VistaGeometry *pGeometry );

	bool ApplyToAllGeometries( IVistaNode* pNode );
	bool RemoveFromAllGeometries( IVistaNode* pNode );

	bool GetIsEnabled() const;
	bool SetIsEnabled( const bool bSet );

private:
	VistaOpenSGPerMaterialShader* m_pShader;
	bool					m_bEnabled;

	std::string m_sVertexShaderFile;
	std::string m_sFragmentShaderFile;

	static const std::string m_sVertexShader;
	static const std::string m_sFragmentShader;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGPHONGSHADER_H_
