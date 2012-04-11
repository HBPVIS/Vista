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

#if defined(WIN32)
// OpenSG warnings
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif

#include "VistaOpenSGModelOptimizer.h"

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimer.h>
#include <VistaTools/VistaFileSystemFile.h>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGAction.h>
#include <OpenSG/OSGGraphOp.h>
#include <OpenSG/OSGGraphOpSeq.h>
#include <OpenSG/OSGVerifyGeoGraphOp.h>
#include <OpenSG/OSGVerifyGraphOp.h>
#include <OpenSG/OSGStripeGraphOp.h>
#include <OpenSG/OSGMergeGraphOp.h>
#include <OpenSG/OSGSharePtrGraphOp.h>
#include <OpenSG/OSGPruneGraphOp.h>
#include <OpenSG/OSGGraphOpFactory.h>
#include <OpenSG/OSGMaterialMergeGraphOp.h>
#include <OpenSG/OSGGeoFunctions.h>
#include <OpenSG/OSGLineChunk.h>
#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGBaseFunctions.h>

#include <list>




/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class OSGGeometryTraversal
{
public:    
	OSGGeometryTraversal() {}

	virtual ~OSGGeometryTraversal() {}

	std::list<osg::NodePtr>& GetNodes()
	{
		return m_liGeomNodes;
	}

	std::list<osg::GeometryPtr>& GetGeometries()
	{
		return m_liGeometries;
	}

	osg::Action::ResultE Process( osg::NodePtr& pNode )
	{
		if( pNode->getCore()->getType().isDerivedFrom( osg::Geometry::getClassType() ) )
		{
			m_liGeomNodes.push_back( pNode );
			osg::GeometryPtr pGeom = osg::GeometryPtr::dcast( pNode->getCore() );
			m_liGeometries.push_back( pGeom );
		}
		return osg::Action::Continue;
	}

private:
	std::list<osg::NodePtr>     m_liGeomNodes;
	std::list<osg::GeometryPtr> m_liGeometries;
};


bool Optimize( osg::NodePtr pModelNode, int nOptimizationMode, bool bVerbose )
{
	try
	{
		VistaTimer oTimer;

		// we always verify
		if( bVerbose )
			vstr::outi() << "[VistaOpenSGModelOptimizer]: Performing Graph verification" << std::endl;		
		osg::VerifyGraphOp oVerifyOp;
		oVerifyOp.setVerbose( bVerbose );
		oVerifyOp.traverse( pModelNode );
		if( bVerbose )
		{
			vstr::outi() << vstr::singleindent << "optimization took " << vstr::formattime( 1000 * oTimer.GetLifeTime(), 1 ) << "ms" << std::endl;
		}

		if( bVerbose )
			vstr::outi() << "[VistaOpenSGModelOptimizer]: Performing Geometry verification" << std::endl;		
		osg::VerifyGeoGraphOp oVerifyGeoOp;
		oVerifyGeoOp.traverse( pModelNode );
		if( bVerbose )
		{
			vstr::outi() << vstr::singleindent << "optimization took " << vstr::formattime( 1000 * oTimer.GetLifeTime(), 1 ) << "ms" << std::endl;
		}

		
		if( nOptimizationMode & VistaOpenSGModelOptimizer::OPT_MERGE_MATERIALS )
		{
			if( bVerbose )
				vstr::outi() << "[VistaOpenSGModelOptimizer]: Performing Material Merge" << std::endl;		
			oTimer.ResetLifeTime();
			osg::MaterialMergeGraphOp oMaterialMergeOp;
			oMaterialMergeOp.traverse( pModelNode );
			//graphop->addGraphOp(&matMerge);
			if( bVerbose )
			{
				vstr::outi() << vstr::singleindent << "optimization took " << vstr::formattime( 1000 * oTimer.GetLifeTime(), 1 ) << "ms" << std::endl;
			}
		}


		if( nOptimizationMode & VistaOpenSGModelOptimizer::OPT_CONVERT_GEOMS )
		{
			if( bVerbose )
				vstr::outi() << "[VistaOpenSGModelOptimizer]: Converting Geometries" << std::endl;	

			oTimer.ResetLifeTime();
			OSGGeometryTraversal oTraversalHelper;
			osg::traverse( pModelNode, 
				osg::osgTypedMethodFunctor1ObjPtrCPtrRef<osg::Action::ResultE, OSGGeometryTraversal, osg::NodePtr>(
																&oTraversalHelper, &OSGGeometryTraversal::Process ) ); 
			
			std::list<osg::GeometryPtr> &geos = oTraversalHelper.GetGeometries();
			int overallVertexCount = 0;
			int count = 0;
			for( std::list<osg::GeometryPtr>::const_iterator itGeom = geos.begin();
				itGeom != geos.end(); ++itGeom )
			{
				const int vCount = osg::createSingleIndex( (*itGeom) );
				overallVertexCount += vCount;			
				std::cout << ++count << "/" << geos.size() << ": " << vCount << " vertices" << std::endl;
			}
			if( bVerbose )
			{
				vstr::outi() << vstr::singleindent << "optimization took " << vstr::formattime( 1000 * oTimer.GetLifeTime(), 1 ) << "ms" << std::endl;
			}
			
		}

		if( nOptimizationMode & VistaOpenSGModelOptimizer::OPT_MERGE_GEOMETRIES )
		{
			if( bVerbose )
				vstr::outi() << "[VistaOpenSGModelOptimizer]: Merging Geometries" << std::endl;	
			oTimer.ResetLifeTime();
			osg::MergeGraphOp oMerge;		
			oMerge.traverse( pModelNode );
			if( bVerbose )
			{
				vstr::outi() << vstr::singleindent << "optimization took " << vstr::formattime( 1000 * oTimer.GetLifeTime(), 1 ) << "ms" << std::endl;
			}
		}

		if( nOptimizationMode & VistaOpenSGModelOptimizer::OPT_STRIPE_GEOMETRIES )
		{
			if( bVerbose )
				vstr::outi() << "[VistaOpenSGModelOptimizer]: Performing Geometry Striping" << std::endl;	
			oTimer.ResetLifeTime();
			osg::StripeGraphOp oStripeOp;
			oStripeOp.traverse( pModelNode );
			if( bVerbose )
			{
				vstr::outi() << vstr::singleindent << "optimization took " << vstr::formattime( 1000 * oTimer.GetLifeTime(), 1 ) << "ms" << std::endl;
			}
		}

	}
	catch( std::exception& oException )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer]: Exception during optimization: "
					<< oException.what() << std::endl;
		return false;
	}
	catch( ... )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer]: Exception during optimization" << std::endl;
		return false;
	}
	
	return true;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOpenSGModelOptimizer::OptimizeNode( IVistaNode* pNode,
											int nOptimizationMode,
											bool bVerbose  )
{
	VistaNode* pNodeInstance = static_cast<VistaNode*>( pNode );
	VistaOpenSGNodeData* pData = dynamic_cast<VistaOpenSGNodeData*>( pNodeInstance->GetData() );
	if( pData == NULL )
	{
		vstr::outi() << "###ERROR### [VistaOpenSGModelOptimizer::OptimizeNode] --"
				<< "passed node is no OpenSG node" << std::endl;
		return false;
	}

	osg::NodePtr pOSGNode = pData->GetNode();

	return Optimize( pOSGNode, nOptimizationMode, bVerbose );

}

bool VistaOpenSGModelOptimizer::OptimizeFile( const std::string& sFilename,
											const std::string& sOutputFilename,
											int nOptimizationMode,
											bool bVerbose )
{
	bool bDoExit = false;

	// @todo: how to check if osg already is initialized?
	OSG::osgInit( 0, NULL );
	//bDoExit = true;


	VistaFileSystemFile oFile( sFilename );
	if( oFile.Exists() == false )
	{
		vstr::errp() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "File [" << sFilename << "] does not exist!" << std::endl;
		return false;
	}
	
	osg::NodePtr pModelNode;
	if( bVerbose )
	{
		vstr::outi() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "Laoding file [" << sFilename << "]" << std::endl;
	}

	osg::GraphOpSeq* pGraphOp = new osg::GraphOpSeq;
	// we always verify
	pGraphOp->addGraphOp( new osg::VerifyGraphOp );
	pGraphOp->addGraphOp( new osg::VerifyGeoGraphOp );

	//if( nOptimizationMode & VistaOpenSGModelOptimizer::OPT_MERGE_MATERIALS )
	//{
	//	pGraphOp->addGraphOp( new osg::MaterialMergeGraphOp );
	//}

	//if( nOptimizationMode & VistaOpenSGModelOptimizer::OPT_STRIPE_GEOMETRIES )
	//{
	//	pGraphOp->addGraphOp( new osg::StripeGraphOp );
	//}

	try
	{
		pModelNode = osg::SceneFileHandler::the().read( sFilename.c_str(), pGraphOp );
	}
	catch( std::exception& oException )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer]: Exception during optimization loading: "
					<< oException.what() << std::endl;
		return false;
	}
	catch( ... )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer]: Exception during optimization loading" << std::endl;
		return false;
	}
	if( pModelNode == NULL )
	{
		vstr::errp() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "Loading file [" << sFilename << "] failed!" << std::endl;
		return false;
	}

	if( Optimize( pModelNode, nOptimizationMode, bVerbose ) == false )
	{
		return false;
	}

	std::string sOutName;
	if( sOutputFilename.empty() )
	{
		size_t nDotPos = sFilename.rfind( '.' );
		if( nDotPos != std::string::npos )
		{
			sOutName = sFilename;
			sOutName.replace( nDotPos, 1, "_opt." );
		}
		else
		{
			// shouldnt happen
			sOutName += sFilename + "_opt";
		}
	}
	else
	{
		sOutName = sOutputFilename;
	}

	if( bVerbose )
	{
		vstr::outi() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "Writing optimized file [" << sOutName << "]" << std::endl;
	}
	bool bRes = osg::SceneFileHandler::the().write( pModelNode, sOutName.c_str() );	

	if( bDoExit )
		OSG::osgExit();

	return bRes;
}

bool VistaOpenSGModelOptimizer::OptimizeAndSaveNode( IVistaNode* pNode,
													const std::string& sOutputFilename,
													int nOptimizationMode,
													bool bVerbose )
{
	VistaNode* pNodeInstance = static_cast<VistaNode*>( pNode );
	VistaOpenSGNodeData* pData = dynamic_cast<VistaOpenSGNodeData*>( pNodeInstance->GetData() );	
	if( pData == NULL )
	{
		vstr::outi() << "###ERROR### [VistaOpenSGModelOptimizer::OptimizeNode] --"
				<< "passed node is no OpenSG node" << std::endl;
		return false;
	}

	osg::NodePtr pOSGNode = pData->GetNode();

	if( Optimize( pOSGNode, nOptimizationMode, bVerbose ) == false )
	{
		return false;
	}

	pData->SetNode( pOSGNode );

	return osg::SceneFileHandler::the().write( pOSGNode, sOutputFilename.c_str() );	
}

IVistaNode* VistaOpenSGModelOptimizer::LoadAutoOptimizedFile( VistaSceneGraph* pSceneGraph,
												 const std::string& sFilename,
												 int nOptimizationMode,
												 const std::string& sDumpDataFormat,
												 bool bCompareTimestamps,
												 bool bVerbose )
{
	if( VistaFileSystemFile( sFilename ).Exists() == false )
	{
		vstr::errp() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "File [" << sFilename << "] does not exist!" << std::endl;
		return false;
	}

	std::string sOptSting;
	if( nOptimizationMode != OPT_NONE )
	{			
		if( nOptimizationMode & OPT_MERGE_MATERIALS )
		{
			sOptSting += "_MM";
		}
		if( nOptimizationMode & OPT_STRIPE_GEOMETRIES )
		{
			sOptSting += "_STRP";
		}
		if( nOptimizationMode & OPT_MERGE_GEOMETRIES )
		{
			sOptSting += "_MG";
		}
		if( nOptimizationMode & OPT_CONVERT_GEOMS )
		{
			sOptSting += "_CG";
		}
	}

	std::size_t nDotPos = sFilename.rfind( '.' );
	std::string sOwnExtension = sFilename.substr( nDotPos + 1 );
	assert( nDotPos != std::string::npos );

	std::string sOptLoadName = sFilename.substr( 0, nDotPos )
								+ "_opt" + sOptSting;

	if( sDumpDataFormat == "ac" )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "Data format [ac] not supported for writing - using [bin] instead" << std::endl;
		sOptLoadName += ".bin";
	}
	else
	{
		sOptLoadName += "." + sDumpDataFormat;
	}

	VistaFileSystemFile oOptFile( sOptLoadName );
	bool bDoOptimize = true;
	if( oOptFile.Exists() )
	{
		// File has already been optimized - check if it has to be rebuilt
		if( bCompareTimestamps == false )
		{
			bDoOptimize = false;
		}
		else if( oOptFile.GetLastModifiedDate() > VistaFileSystemFile( sFilename ).GetLastModifiedDate() )
		{
			bDoOptimize = false;
		}
		else if( bVerbose )
		{
			vstr::outi() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "Optimized file [" << sOptLoadName 
				<< "] exists, but is outdated - recreating it" << std::endl;
		}
	}

	if( bDoOptimize )
	{
		if( OptimizeFile( sFilename, sOptLoadName, nOptimizationMode, bVerbose ) == false )
			return NULL;
	}

	try
	{
		if( bVerbose )
		{
			vstr::outi() << "[VistaOpenSGModelOptimizer::OptimizeFile] -- "
				<< "Loading optimized file [" << sOptLoadName << "]" << std::endl;
		}
		IVistaNode* pNode = pSceneGraph->LoadNode( sOptLoadName, VistaSceneGraph::OPT_NONE );
		return pNode;
	}
	catch( std::exception& oException )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer]: Exception while loading optimized file: "
					<< oException.what() << std::endl;
		return false;
	}
	catch( ... )
	{
		vstr::warnp() << "[VistaOpenSGModelOptimizer]: Exception while loading optimized file" << std::endl;
		return false;
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

