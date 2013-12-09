/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif

#include "VistaOpenSGPhongShader.h"

#include "VistaOpenSGPerMaterialShader.h"

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>

#include <VistaBase/VistaStreamUtils.h>

#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGMaterialGroup.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGSHLParameterChunk.h>
#include <OpenSG/OSGNode.h>

#include <queue>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

const std::string VistaOpenSGPhongShader::m_sVertexShader = \
"varying vec3 v3WorldNormal, v3WorldPos;\n"
"void main()\n"
"{	\n"
"	v3WorldNormal = normalize( gl_NormalMatrix * gl_Normal );	\n"		
"	v3WorldPos = vec3( gl_ModelViewMatrix * gl_Vertex );\n"
"	gl_TexCoord[0] = gl_MultiTexCoord0;		\n"
"	gl_Position = ftransform();\n"
"	gl_FrontColor = gl_Color;\n"
"	gl_BackColor = gl_Color;\n"
"}\n";

const std::string VistaOpenSGPhongShader::m_sFragmentShader = \
"uniform sampler2D tex; \n"
"varying vec3 v3WorldNormal, v3WorldPos; \n"
"uniform bool OSGLight0Active; \n"
"uniform bool OSGLight1Active; \n"
"uniform bool OSGLight2Active; \n"
"uniform bool OSGLight3Active; \n"
"uniform bool OSGLight4Active; \n"
"uniform bool OSGLight5Active; \n"
"uniform bool OSGLight6Active; \n"
"uniform bool OSGLight7Active; \n"
"\n"
"uniform bool IsTextured; \n"
"\n"
"void ProcessLight( in int iLight, \n"
"				in vec3 v3Normal, \n"
"				inout vec4 v4Color ) \n"
"{\n"
"	vec4 v4LocalColor = gl_FrontMaterial.ambient * gl_LightSource[iLight].ambient; \n"
"\n"
"	vec3 v3LightDir; \n"
"	float fAttenuation = 1.0; \n"
"	\n"
"	// calculating Light Direction and Attenuation based on light type \n"
"	if( gl_LightSource[iLight].position.w == 0.0 ) \n"
"	{\n"
"		// directional light\n"
"		v3LightDir = normalize(gl_LightSource[iLight].position.xyz);\n"
"	}\n"
"	else // point or spot light\n"
"	{\n"
"		// point light\n"
"		vec3 v3Aux = gl_LightSource[iLight].position.xyz - v3WorldPos;\n"
"		v3LightDir = normalize(v3Aux);\n"
"		float fDist = length(v3Aux);\n"
"		fAttenuation = 1.0 / ( gl_LightSource[iLight].constantAttenuation +\n"
"								gl_LightSource[iLight].linearAttenuation * fDist +\n"
"								gl_LightSource[iLight].quadraticAttenuation * fDist * fDist );\n"
"								\n"
"		if( gl_LightSource[iLight].spotCutoff <= 90.0 )	\n"
"		{\n"
"		 	// it's a spotlight\n"
"			vec3 v3SpotDir = normalize( gl_LightSource[iLight].spotDirection );\n"
"		\n"
"			float fCosSpotAngle = dot( -v3LightDir, v3SpotDir );\n"
"			//return vec4( gl_LightSource[iLight].spotDirection, 1.0 );\n"
"				\n"
"			if( fCosSpotAngle < gl_LightSource[iLight].spotCosCutoff )\n"
"			{\n"
"				// outside the spot\n"
"				fAttenuation = 0.0;\n"
"			}\n"
"			else\n"
"			{\n"
"				// inside the main spot area\n"
"				fAttenuation *= pow( fCosSpotAngle, gl_LightSource[iLight].spotExponent );\n"
"			}\n"
"		}\n"
"	}\n"
"\n"
"	// Ambient Term\n"
"	v4LocalColor += gl_FrontMaterial.ambient * gl_LightSource[iLight].ambient;\n"
"\n"
"	vec3 v3HalfVec = normalize( -normalize(v3WorldPos) + v3LightDir );\n"
"\n"
"	float fNdotL, fNdotHV;\n"
"	fNdotL = dot( v3Normal, v3LightDir );\n"
"	if( fNdotL > 0.0 )\n"
"	{\n"
"		fNdotHV = max( dot( v3Normal, v3HalfVec ), 0.0 );\n"
"\n"
"		//Diffuse Term\n"
"		v4LocalColor += gl_Color * gl_LightSource[iLight].diffuse\n"
"				* fNdotL;\n"
"		//Specular Term\n"
"       if( gl_FrontMaterial.shininess > 0.0 )\n"
"		  v4LocalColor += gl_FrontMaterial.specular * gl_LightSource[iLight].specular\n"
"				* fNdotL * max( pow( fNdotHV, gl_FrontMaterial.shininess ), 0.0 );\n"
"	}\n"
"\n"
"	v4LocalColor *= fAttenuation;\n"
"	v4Color += v4LocalColor;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"	// a fragment shader can't write a varying variable, hence we need\n"
"	//a new variable to store the normalized interpolated normal \n"
"	vec3 v3Normal = normalize( v3WorldNormal );\n"
"	if( gl_FrontFacing == false ) //render back side\n"
"		v3Normal = -v3Normal;\n"
"\n"
"	vec4 v4Color = gl_FrontLightModelProduct.sceneColor;\n"
"\n"
"	if( OSGLight0Active )\n"
"		ProcessLight( 0, v3Normal, v4Color );\n"
"	if( OSGLight1Active )\n"
"		ProcessLight( 1, v3Normal, v4Color );\n"
"	if( OSGLight2Active )\n"
"		ProcessLight( 2, v3Normal, v4Color );\n"
"	if( OSGLight3Active )\n"
"		ProcessLight( 3, v3Normal, v4Color );\n"
"	if( OSGLight4Active )\n"
"		ProcessLight( 4, v3Normal, v4Color );\n"
"	if( OSGLight5Active )\n"
"		ProcessLight( 5, v3Normal, v4Color );\n"
"	if( OSGLight6Active )\n"
"		ProcessLight( 6, v3Normal, v4Color );\n"
"	if( OSGLight7Active )\n"
"		ProcessLight( 7, v3Normal, v4Color );\n"
"\n"
"	v4Color[3] = gl_FrontMaterial.diffuse[3];\n"
"	if( IsTextured )\n"
"		v4Color = v4Color * texture2D( tex, gl_TexCoord[0].st );\n"
"\n"	
"	gl_FragColor = v4Color;\n"
"}\n";


class OSGTraversalHelper
{
public:
	OSGTraversalHelper( VistaOpenSGPerMaterialShader* pShader )
	: m_pShader( pShader )
	{
	}

	osg::Action::ResultE TraverseAddCallback( osg::NodePtr& pOSGNode )
	{
		osg::ChunkMaterialPtr pOSGMaterial = osg::NullFC;

		osg::MaterialGroupPtr pOSGMaterialGroup 
				= osg::MaterialGroupPtr::dcast( pOSGNode->getCore() );	
		if( pOSGMaterialGroup != osg::NullFC )
		{
			pOSGMaterial = osg::ChunkMaterialPtr::dcast( pOSGMaterialGroup->getMaterial() );
		}
		else
		{
			osg::GeometryPtr pOSGGeometry = osg::GeometryPtr::dcast( pOSGNode->getCore() );
			if( pOSGGeometry != osg::NullFC )
			{
				pOSGMaterial = osg::ChunkMaterialPtr::dcast( pOSGGeometry->getMaterial() );
			}
		}

		if( pOSGMaterial != osg::NullFC )
		{
			//First: determine if the geometry is textured
			osg::TextureChunkPtr pOSGTexChunk( osg::TextureChunkPtr::dcast(
					pOSGMaterial->find( osg::TextureChunk::getClassType(), 0 ) ) );
			bool bTextured = false;
			if( pOSGTexChunk != osg::NullFC )
			{
				bTextured = true;
			}
			else
			{
				osg::SimpleTexturedMaterialPtr pTexMat 
							= osg::SimpleTexturedMaterialPtr::dcast( pOSGMaterial );
				if( pTexMat != osg::NullFC && pTexMat->getImage() )
					bTextured = true;
			}
			std::vector<VistaOpenSGPerMaterialShader::CShaderUniformParam> vecParams;
			vecParams.push_back( VistaOpenSGPerMaterialShader::CShaderUniformParam(
														"IsTextured", bTextured ) );
			
			if( m_pShader->ApplyToOSGMaterial( &pOSGMaterial, vecParams ) == false )
			{
				vstr::warnp() << "[OSGPhongShader]: Could not apply Phong shader!" << std::endl;
			}
			//else
			//	return osg::Action::Skip;
		}	
	        
		return osg::Action::Continue;
	}

	osg::Action::ResultE TraverseRemoveCallback( osg::NodePtr& pOSGNode )
	{
		osg::ChunkMaterialPtr pOSGMaterial = osg::NullFC;

		osg::MaterialGroupPtr pOSGMaterialGroup 
				= osg::MaterialGroupPtr::dcast( pOSGNode->getCore() );	
		if( pOSGMaterialGroup != osg::NullFC )
		{
			pOSGMaterial = osg::ChunkMaterialPtr::dcast( pOSGMaterialGroup->getMaterial() );
		}
		else
		{
			osg::GeometryPtr pOSGGeometry = osg::GeometryPtr::dcast( pOSGNode->getCore() );
			if( pOSGGeometry != osg::NullFC )
			{
				pOSGMaterial = osg::ChunkMaterialPtr::dcast( pOSGGeometry->getMaterial() );
			}
		}

		if( pOSGMaterial != osg::NullFC )
		{			
			if( m_pShader->RemoveFromOSGMaterial( &pOSGMaterial ) == false )
			{
				vstr::warnp() << "[OSGPhongShader]: Could not remove Phong shader!" << std::endl;
			}
			//else
			//	return osg::Action::Skip;
		}	
	        
		return osg::Action::Continue;
	}
private:
	VistaOpenSGPerMaterialShader* m_pShader;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaOpenSGPhongShader::VistaOpenSGPhongShader()
: m_pShader( new VistaOpenSGPerMaterialShader )
, m_bEnabled( true )
, m_sVertexShaderFile( "" )
, m_sFragmentShaderFile( "" )
{
	m_pShader->SetShadersFromString( m_sVertexShader, m_sFragmentShader );
	m_pShader->SetUniformParameter( VistaOpenSGPerMaterialShader::CShaderUniformParam(
				VistaOpenSGPerMaterialShader::CShaderUniformParam::OSG_ACTIVE_LIGHTS ) );
}
VistaOpenSGPhongShader::VistaOpenSGPhongShader(
									const std::string& sVertexShaderFile,
									const std::string& sFragmentShaderFile )
: m_pShader( new VistaOpenSGPerMaterialShader )
, m_bEnabled( true )
, m_sVertexShaderFile( sVertexShaderFile )
, m_sFragmentShaderFile( sFragmentShaderFile )
{
	m_pShader->SetShadersFromFile( m_sVertexShaderFile, m_sFragmentShaderFile );
	m_pShader->SetUniformParameter( VistaOpenSGPerMaterialShader::CShaderUniformParam(
				VistaOpenSGPerMaterialShader::CShaderUniformParam::OSG_ACTIVE_LIGHTS ) );
}
VistaOpenSGPhongShader::~VistaOpenSGPhongShader()
{
	delete m_pShader;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOpenSGPhongShader::ReloadFromFile()
{
	if( m_sFragmentShaderFile.empty() || m_sVertexShaderFile.empty() )
		return false;
	return m_pShader->SetShadersFromFile( m_sVertexShaderFile, m_sFragmentShaderFile );
}
bool VistaOpenSGPhongShader::ReloadFromFile( const std::string& sVertexShaderFile,
						const std::string& sFragmentShaderFile )
{
	m_sVertexShaderFile = sVertexShaderFile;
	m_sFragmentShaderFile = sFragmentShaderFile;
	return m_pShader->SetShadersFromFile( m_sVertexShaderFile, m_sFragmentShaderFile );
}


bool VistaOpenSGPhongShader::ApplyToGeometry( VistaGeometry *pGeometry )
{
	VistaOpenSGGeometryData *pGeomData
					= static_cast<VistaOpenSGGeometryData*>( pGeometry->GetData() );
	
	osg::GeometryPtr pOSGGeometry = pGeomData->GetGeometry();
	osg::ChunkMaterialPtr pOSGMaterial( 
		osg::ChunkMaterialPtr::dcast( pOSGGeometry->getMaterial() ) );
	if( pOSGMaterial == osg::NullFC )
		return false;

	//First: determine if the geometry is textured
	osg::TextureChunkPtr pOSGTexChunk( osg::TextureChunkPtr::dcast(
			pOSGMaterial->find( osg::TextureChunk::getClassType(), 0 ) ) );
	bool bTextured = false;
	if( pOSGTexChunk != osg::NullFC )
	{
		bTextured = true;
	}
	else
	{
		osg::SimpleTexturedMaterialPtr pTexMat 
					= osg::SimpleTexturedMaterialPtr::dcast( pOSGMaterial );
		if( pTexMat != osg::NullFC && pTexMat->getImage() )
			bTextured = true;
	}

	std::vector<VistaOpenSGPerMaterialShader::CShaderUniformParam> vecParams;
	vecParams.push_back( VistaOpenSGPerMaterialShader::CShaderUniformParam(
												"IsTextured", bTextured ) );
	return m_pShader->ApplyToGeometry( pGeometry, vecParams );
}
bool VistaOpenSGPhongShader::RemoveFromGeometry( VistaGeometry *pGeometry )
{
	return m_pShader->RemoveFromGeometry( pGeometry );
}

bool VistaOpenSGPhongShader::ApplyToAllGeometries( IVistaNode* pNode )
{
	//std::queue<IVistaNode*> queNodes;
	//queNodes.push( pNode );
	//IVistaNode* pCurrentNode;
	//VistaGeomNode* pGeomNode;
	//while( queNodes.empty() == false )
	//{
	//	pCurrentNode = queNodes.front();
	//	queNodes.pop();
	//	if( pCurrentNode->GetType() == VISTA_GEOMNODE )
	//	{
	//		pGeomNode = static_cast<VistaGeomNode*>( pCurrentNode );
	//		ApplyToGeometry( pGeomNode->GetGeometry() );
	//	}
	//	else if( pCurrentNode->CanHaveChildren() )
	//	{
	//		VistaGroupNode* pGroup = static_cast<VistaGroupNode*>( pCurrentNode );
	//		for( unsigned int i = 0; i < pGroup->GetNumChildren(); ++i )
	//			queNodes.push( pGroup->GetChild( i ) );
	//	}
	//}

	//blech... why is there a difference between IVistaNode and VistaNode?
	VistaNode* pSameNode = dynamic_cast<VistaNode*>( pNode );
	if( pSameNode == NULL )
		return false;
	VistaOpenSGNodeData* pData = static_cast<VistaOpenSGNodeData*>( pSameNode->GetData() );
	osg::NodePtr pOSGNode = pData->GetNode();

	OSGTraversalHelper pHelper( m_pShader );
	osg::traverse( pOSGNode, osg::osgTypedMethodFunctor1ObjPtrCPtrRef<
			osg::Action::ResultE, OSGTraversalHelper, osg::NodePtr>( 
				&pHelper, &OSGTraversalHelper::TraverseAddCallback ) );
	return true;
}
bool VistaOpenSGPhongShader::RemoveFromAllGeometries( IVistaNode* pNode )
{
	//std::queue<IVistaNode*> queNodes;
	//queNodes.push( pNode );
	//IVistaNode* pCurrentNode;
	//VistaGeomNode* pGeomNode;
	//while( queNodes.empty() == false )
	//{
	//	pCurrentNode = queNodes.front();
	//	queNodes.pop();
	//	if( pCurrentNode->GetType() == VISTA_GEOMNODE )
	//	{
	//		pGeomNode = static_cast<VistaGeomNode*>( pCurrentNode );
	//		RemoveFromGeometry( pGeomNode->GetGeometry() );
	//	}
	//	else if( pCurrentNode->CanHaveChildren() )
	//	{
	//		VistaGroupNode* pGroup = static_cast<VistaGroupNode*>( pCurrentNode );
	//		for( unsigned int i = 0; i < pGroup->GetNumChildren(); ++i )
	//			queNodes.push( pGroup->GetChild( i ) );
	//	}
	//}
	//return true;

	//blech... why is there a difference between IVistaNode and VistaNode?
	VistaNode* pSameNode = dynamic_cast<VistaNode*>( pNode );
	if( pSameNode == NULL )
		return false;
	VistaOpenSGNodeData* pData = static_cast<VistaOpenSGNodeData*>( pSameNode->GetData() );
	osg::NodePtr pOSGNode = pData->GetNode();

	OSGTraversalHelper pHelper( m_pShader );
	osg::traverse( pOSGNode, osg::osgTypedMethodFunctor1ObjPtrCPtrRef<
			osg::Action::ResultE, OSGTraversalHelper, osg::NodePtr>( 
				&pHelper, &OSGTraversalHelper::TraverseRemoveCallback ) );
	return true;
}


bool VistaOpenSGPhongShader::GetIsEnabled() const
{
	return m_bEnabled;
}
bool VistaOpenSGPhongShader::SetIsEnabled( const bool bSet )
{
	// mean MEAN hack!
	if( bSet == m_bEnabled )
		return false;

	if( bSet )
	{
		if( ReloadFromFile() == false )
			m_pShader->SetShadersFromString( m_sVertexShader, m_sFragmentShader );
	}
	else
	{
		m_pShader->SetShadersFromString( "", "" );
	}

	m_bEnabled = bSet;
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

