/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#include "VdfnObjectRegistry.h" 
#include <VistaAspects/VistaNameable.h>

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnObjectRegistry::VdfnObjectRegistry()
{
}

VdfnObjectRegistry::~VdfnObjectRegistry()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


IVistaTransformable *VdfnObjectRegistry::GetObjectTransform( const std::string &strName ) const
{
	IVistaNameable *pNameable;
	IVistaTransformable *pTrans = NULL;
	GetObject(strName, pNameable, pTrans);
	return pTrans;
}


IVistaNameable      *VdfnObjectRegistry::GetObject( const std::string &strName ) const
{
	IVistaNameable *pNameable = NULL;
	IVistaTransformable *pTrans;
	GetObject(strName, pNameable, pTrans);
	return pNameable;
}


bool VdfnObjectRegistry::GetObject( const std::string &strName,
				IVistaNameable *&pObj,
				IVistaTransformable *&pTrans ) const
{
	OBJMAP::const_iterator cit = m_mpObjects.find(strName);
	if( cit == m_mpObjects.end() )
		return false;

	pObj = (*cit).second.first;
	pTrans = (*cit).second.second;

	return true;
}


bool VdfnObjectRegistry::SetObject( const std::string &strName, 
				IVistaNameable *pObj, 
				IVistaTransformable *pTrans )
{
	if(m_mpObjects.find(strName) != m_mpObjects.end())
		return false;

	m_mpObjects[strName] = OBJ(pObj, pTrans);
	return true;
}


bool VdfnObjectRegistry::RemObject( const std::string &strName )
{
	OBJMAP::iterator it = m_mpObjects.find(strName);
	if(it == m_mpObjects.end() )
		return false;

	m_mpObjects.erase(it);

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


