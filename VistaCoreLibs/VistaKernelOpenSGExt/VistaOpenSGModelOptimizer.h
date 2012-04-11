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

#ifndef _VISTAOPENSGMODELOPTIMIZER_H_
#define _VISTAOPENSGMODELOPTIMIZER_H_

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

class IVistaNode;
class VistaSceneGraph;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGModelOptimizer
{
public:
	enum
	{
		OPT_MERGE_MATERIALS		= 1<<0,
		OPT_STRIPE_GEOMETRIES	= 1<<1,
		OPT_MERGE_GEOMETRIES	= 1<<2,
		OPT_CONVERT_GEOMS		= 1<<3,
		// preset combinations
		OPT_NONE				= 0,
		OPT_DEFAULT				= OPT_MERGE_MATERIALS | OPT_MERGE_GEOMETRIES,
		OPT_ALL					= ~0
	};

	static bool OptimizeNode( IVistaNode* pNode,
						int nOptimizationMode = OPT_DEFAULT,
						bool bVerbose = true );
	static bool OptimizeAndSaveNode( IVistaNode* pNode,
						const std::string& sOutputFilename,
						int nOptimizationMode = OPT_DEFAULT,
						bool bVerbose = true );
	static bool OptimizeFile( const std::string& sFilename,
						const std::string& sOutputFilename = "",
						int nOptimizationMode = OPT_DEFAULT,
						bool bVerbose = true  );
	static IVistaNode* LoadAutoOptimizedFile( VistaSceneGraph* pSceneGraph,
						const std::string& sFilename,
						int nOptimizationMode = OPT_DEFAULT,
						const std::string& sDumpDataFormat = "bin",
						bool bCompareTimestamps = true,
						bool bVerbose = true  );
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGMODELOPTIMIZER_H_
