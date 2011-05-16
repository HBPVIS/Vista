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

#if defined(WIN32)
#pragma warning(disable: 4275)
#pragma warning(disable: 4267)
#pragma warning(disable: 4251)
#pragma warning(disable: 4231)
#endif

#include "VistaOpenSGPerMaterialShader.h"

#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>

#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGSHLParameterChunk.h>

#include <iostream>

#include "VistaKernelOpenSGExtOut.h"

////////////////////////////////////
////////////////////////////////////
/////    CShaderUniformParam   /////
////////////////////////////////////
////////////////////////////////////

VistaOpenSGPerMaterialShader::CShaderUniformParam
			::CShaderUniformParam( const std::string& sName,
							const bool bParam )
: m_eType( CShaderUniformParam::BOOL )
, m_iData( bParam )
, m_sName( sName )
{
}
VistaOpenSGPerMaterialShader::CShaderUniformParam
			::CShaderUniformParam( const std::string& sName,
							const int iParam )
: m_eType( CShaderUniformParam::INT )
, m_sName( sName )
, m_iData( iParam )
{
}
VistaOpenSGPerMaterialShader::CShaderUniformParam
			::CShaderUniformParam( const std::string& sName,
							const float fParam )
: m_eType( CShaderUniformParam::FLOAT )
, m_sName( sName )
{
	m_afData[0] = fParam;
}
VistaOpenSGPerMaterialShader::CShaderUniformParam
			::CShaderUniformParam( const std::string& sName,
							const float afParam[],
							const int iVecSize )
: m_sName( sName )
{
	switch( iVecSize )
	{
		case 1:
			m_eType = CShaderUniformParam::FLOAT;
			m_afData[0] = afParam[0];
			break;
		case 2:
			m_eType = CShaderUniformParam::VEC2;
			m_afData[0] = afParam[0];
			m_afData[1] = afParam[1];
			break;
		case 3:
			m_eType = CShaderUniformParam::VEC3;
			m_afData[0] = afParam[0];
			m_afData[1] = afParam[1];
			m_afData[2] = afParam[2];
			break;
		case 4:
			m_eType = CShaderUniformParam::VEC4;
			m_afData[0] = afParam[0];
			m_afData[1] = afParam[1];
			m_afData[2] = afParam[2];
			m_afData[3] = afParam[3];
			break;
		
	}
}
VistaOpenSGPerMaterialShader::CShaderUniformParam
			::CShaderUniformParam( const PARAM_TYPE eType )
: m_eType( eType )
{
	switch( m_eType )
	{
		case CShaderUniformParam::INVALID:
		case CShaderUniformParam::BOOL:
		case CShaderUniformParam::INT:
		case CShaderUniformParam::FLOAT:
		case CShaderUniformParam::VEC2:
		case CShaderUniformParam::VEC3:
		case CShaderUniformParam::VEC4:
			m_eType = CShaderUniformParam::INVALID;
			break;
		case CShaderUniformParam::OSG_LIGHT_MASK:
		case CShaderUniformParam::OSG_ACTIVE_LIGHTS:
		case CShaderUniformParam::OSG_CAMERA_POSITION:
			break;
	}
}

////////////////////////////////////
////////////////////////////////////
/////        CShaderData       /////
////////////////////////////////////
////////////////////////////////////
struct VistaOpenSGPerMaterialShader::CShaderData
{
public:
	CShaderData()
	: m_pShader( osg::SHLChunk::create() )
	{
	}
	~CShaderData() {}
public:
	osg::SHLChunkRefPtr m_pShader;
};
/*
class VistaOpenSGPerMaterialShader::CShaderData
{
public:
	CShaderData()
	: shlShader( SHLChunk::create() )
	{
	}

	virtual ~CShaderData()
	{
	}

	bool SetShadersFromFile( std::istream& sVertexShader,
					std::istream& sFragmentShader )
	{
		
		beginEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
			if( shlShader->readVertexProgram( sVertexShader ) == false )
			{
				std::cerr << "Error: could not read fragment shader" << std::endl;
				endEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
				return false;
			}			
			if( shlShader->readFragmentProgram( sFragmentShader ) == false )
			{
				std::cerr << "Error: could not read fragment shader" << std::endl;
				endEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
				return false;
			}
			shlShader->setUniformParameter("OSGLight0Active", 0);
			shlShader->setUniformParameter("OSGLight1Active", 0);
			shlShader->setUniformParameter("OSGLight2Active", 0);
			shlShader->setUniformParameter("OSGLight3Active", 0);
			shlShader->setUniformParameter("OSGLight4Active", 0);
			shlShader->setUniformParameter("OSGLight5Active", 0);
			shlShader->setUniformParameter("OSGLight6Active", 0);
			shlShader->setUniformParameter("OSGLight7Active", 0);
		endEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
		return true;
	}

	bool SetShadersFromString( const std::string& sVertexShader,
					const std::string& sFragmentShader )
	{
		beginEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
			if( shlShader->readVertexProgram( sVertexShader.c_str() ) == false )
			{
				std::cerr << "Error: could not read fragment shader" << std::endl;
				endEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
				return false;
			}			
			if( shlShader->readFragmentProgram( sVertexShader.c_str() ) == false )
			{
				std::cerr << "Error: could not read fragment shader" << std::endl;
				endEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
				return false;
			}
			shlShader->setUniformParameter("OSGLight0Active", 0);
			shlShader->setUniformParameter("OSGLight1Active", 0);
			shlShader->setUniformParameter("OSGLight2Active", 0);
			shlShader->setUniformParameter("OSGLight3Active", 0);
			shlShader->setUniformParameter("OSGLight4Active", 0);
			shlShader->setUniformParameter("OSGLight5Active", 0);
			shlShader->setUniformParameter("OSGLight6Active", 0);
			shlShader->setUniformParameter("OSGLight7Active", 0);

		endEditCP( shlShader, SHLChunk::FragmentProgramFieldMask );
		return true;
	}

	SHLChunkRefPtr shlShader;	
};
*/

//////////////////////////////////////////////////
//////////////////////////////////////////////////
///// CShaderVistaOpenSGPerMaterialShaderData  /////
//////////////////////////////////////////////////
//////////////////////////////////////////////////


VistaOpenSGPerMaterialShader::VistaOpenSGPerMaterialShader()
: m_pShaderData( new CShaderData )
{
}

VistaOpenSGPerMaterialShader::~VistaOpenSGPerMaterialShader()
{	
	delete m_pShaderData;
}

bool VistaOpenSGPerMaterialShader::ApplyToGeometry( VistaGeometry *pGeometry )
{
	VistaOpenSGGeometryData *pGeomData
					= static_cast<VistaOpenSGGeometryData*>( pGeometry->GetData() );
	
	osg::GeometryPtr pOSGGeometry = pGeomData->GetGeometry();
	osg::ChunkMaterialPtr pOSGMaterial( 
		osg::ChunkMaterialPtr::dcast( pOSGGeometry->getMaterial() ) );
	if( pOSGMaterial == osg::NullFC )
		return false;

	beginEditCP( pOSGMaterial );
		pOSGMaterial->addChunk( m_pShaderData->m_pShader );
	endEditCP( pOSGMaterial );

	return true;	
}

bool VistaOpenSGPerMaterialShader::ApplyToOSGMaterial( void* pMaterial )
{
	osg::ChunkMaterialPtr* pOSGMaterial = (osg::ChunkMaterialPtr*)( pMaterial );
	if( pOSGMaterial == NULL || (*pOSGMaterial) == osg::NullFC )
		return false;
	
	beginEditCP( *pOSGMaterial );
		(*pOSGMaterial)->addChunk( m_pShaderData->m_pShader );
	endEditCP( *pOSGMaterial );

	return true;	
}

bool VistaOpenSGPerMaterialShader::ApplyToGeometry( VistaGeometry *pGeometry,
						const std::vector<CShaderUniformParam>& vecUniformParams )
{
	if( vecUniformParams.empty() )
		return ApplyToGeometry( pGeometry );

	VistaOpenSGGeometryData *pGeomData
			= static_cast<VistaOpenSGGeometryData*>( pGeometry->GetData() );
	
	osg::GeometryPtr pOSGGeometry = pGeomData->GetGeometry();
	osg::ChunkMaterialPtr pOSGMaterial( 
		osg::ChunkMaterialPtr::dcast( pOSGGeometry->getMaterial() ) );
	if( pOSGMaterial == osg::NullFC )
	{
		//create our own material
		pOSGMaterial = osg::ChunkMaterial::create();
		osg::beginEditCP( pOSGGeometry, osg::Geometry::MaterialFieldMask );
			pOSGGeometry->setMaterial( pOSGMaterial );
		osg::endEditCP( pOSGGeometry, osg::Geometry::MaterialFieldMask );		
	}

	return ApplyToOSGMaterial( &pOSGMaterial, vecUniformParams );
}

bool VistaOpenSGPerMaterialShader::ApplyToOSGMaterial( void* pMaterial,
							const std::vector<CShaderUniformParam>& vecUniformParams)
{
	osg::ChunkMaterialPtr* pOSGMaterial = (osg::ChunkMaterialPtr*)( pMaterial );
	if( pOSGMaterial == NULL || (*pOSGMaterial) == osg::NullFC )
		return false;
	
	osg::SHLParameterChunkPtr pSHLParams = osg::SHLParameterChunk::create();
	beginEditCP( pSHLParams );
		pSHLParams->setSHLChunk( m_pShaderData->m_pShader );
		for( std::vector<CShaderUniformParam>::const_iterator
				citParam = vecUniformParams.begin();
				citParam != vecUniformParams.end();
				++citParam )
		{
			switch( (*citParam).m_eType )
			{			
				case CShaderUniformParam::BOOL:
					pSHLParams->setUniformParameter( 
								(*citParam).m_sName.c_str(), 
								( (*citParam).m_iData != 0 ) );
					break;
				case CShaderUniformParam::INT:
					pSHLParams->setUniformParameter( 
								(*citParam).m_sName.c_str(), 
								(*citParam).m_iData );
					break;
				case CShaderUniformParam::FLOAT:
					pSHLParams->setUniformParameter( 
								(*citParam).m_sName.c_str(),
								osg::Real32( (*citParam).m_afData[0] ) );
					break;
				case CShaderUniformParam::VEC2:
					pSHLParams->setUniformParameter( 
								(*citParam).m_sName.c_str(), 
								osg::Vec2f( (*citParam).m_afData ) );
					break;
				case CShaderUniformParam::VEC3:
					pSHLParams->setUniformParameter( 
								(*citParam).m_sName.c_str(), 
								osg::Vec3f( (*citParam).m_afData ) );
					break;
				case CShaderUniformParam::VEC4:
					pSHLParams->setUniformParameter( 
								(*citParam).m_sName.c_str(), 
								osg::Vec4f( (*citParam).m_afData ) );
					break;
				case CShaderUniformParam::OSG_LIGHT_MASK:
					pSHLParams->setUniformParameter( "OSGActiveLightMask", 0 );
					break;
				case CShaderUniformParam::OSG_ACTIVE_LIGHTS:
					pSHLParams->setUniformParameter( "OSGLight0Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight1Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight2Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight3Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight4Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight5Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight6Active", 0 );
					pSHLParams->setUniformParameter( "OSGLight7Active", 0 );
					break;
				case CShaderUniformParam::OSG_CAMERA_POSITION:
					pSHLParams->setUniformParameter( "OSGCameraPosition", 0 );
					break;
				case CShaderUniformParam::INVALID:
				default:
					break;
			}
		}
	endEditCP( pSHLParams );

	beginEditCP( *pOSGMaterial );
		(*pOSGMaterial)->addChunk( pSHLParams );
		(*pOSGMaterial)->addChunk( m_pShaderData->m_pShader );
	endEditCP( *pOSGMaterial );
	return true;
}

bool VistaOpenSGPerMaterialShader::RemoveFromGeometry( VistaGeometry *pGeometry )
{
	VistaOpenSGGeometryData *pGeomData
			= static_cast<VistaOpenSGGeometryData*>( pGeometry->GetData() );
	
	//First: Get Material
	osg::GeometryPtr pOSGGeometry = pGeomData->GetGeometry();
	osg::ChunkMaterialPtr pOSGMaterial( 
		osg::ChunkMaterialPtr::dcast( pOSGGeometry->getMaterial() ) );
	if( pOSGMaterial == osg::NullFC )
	{
		//create our own material
		pOSGMaterial = osg::ChunkMaterial::create();
		osg::beginEditCP( pOSGGeometry, osg::Geometry::MaterialFieldMask );
			pOSGGeometry->setMaterial( pOSGMaterial );
		osg::endEditCP( pOSGGeometry, osg::Geometry::MaterialFieldMask );		
	}
	return RemoveFromOSGMaterial( &pOSGMaterial );
}

bool VistaOpenSGPerMaterialShader::RemoveFromOSGMaterial( void* pMaterial )
{
	osg::ChunkMaterialPtr* pOSGMaterial = (osg::ChunkMaterialPtr*)( pMaterial );
	if( pOSGMaterial == NULL || (*pOSGMaterial) == osg::NullFC )
		return false;

	//Check if there's a SHLParametersChunk
	osg::SHLParameterChunkPtr pOSGSHLParams( osg::SHLParameterChunkPtr::dcast(
				(*pOSGMaterial)->find( osg::SHLParameterChunk::getClassType() ) ) );
	if( pOSGSHLParams != osg::NullFC )
	{
		beginEditCP( *pOSGMaterial );
			(*pOSGMaterial)->subChunk( pOSGSHLParams );
		endEditCP( *pOSGMaterial );
	}

	//Get the SHLChunk, and remove it
	osg::SHLChunkPtr pOSGSHLChunk( osg::SHLChunkPtr::dcast(
				(*pOSGMaterial)->find( osg::SHLChunk::getClassType() ) ) );
	if( pOSGSHLChunk == osg::NullFC )
		return false;
	
	beginEditCP( *pOSGMaterial );
		(*pOSGMaterial)->subChunk( pOSGSHLChunk );
	endEditCP( *pOSGMaterial );
	
	return true;
}

bool VistaOpenSGPerMaterialShader::SetShadersFromFile(
									const std::string& sVertexShaderFile,
									const std::string& sFragmentShaderFile )
{
	std::ifstream oVertexShader( sVertexShaderFile.c_str() );
	if( oVertexShader.good() == false )
	{
		vosgextout << "Error: could not load vertex shader file \""
			<< sVertexShaderFile << "\"" << std::endl;
		return false;
	}
	std::ifstream oFragmentShader( sFragmentShaderFile.c_str() );
	if( oFragmentShader.good() == false )
	{
		vosgextout << "Error: could not load fragment shader file \""
			<< sFragmentShaderFile << "\"" << std::endl;
		return false;
	}
	beginEditCP( m_pShaderData->m_pShader, osg::SHLChunk::FragmentProgramFieldMask );
		if( m_pShaderData->m_pShader->readVertexProgram( oVertexShader ) == false )
		{
			vosgexterr << "Error: could not read fragment shader" << std::endl;
			endEditCP( m_pShaderData->m_pShader, osg::SHLChunk::FragmentProgramFieldMask );
			return false;
		}			
		if( m_pShaderData->m_pShader->readFragmentProgram( oFragmentShader ) == false )
		{
			vosgexterr << "Error: could not read fragment shader" << std::endl;
			endEditCP( m_pShaderData->m_pShader, osg::SHLChunk::FragmentProgramFieldMask );
			return false;
		}
	endEditCP( m_pShaderData->m_pShader, osg::SHLChunk::FragmentProgramFieldMask );
	return true;
}
	
bool VistaOpenSGPerMaterialShader::SetShadersFromString( 
							const std::string& sVertexShader,
							const std::string& sFragmentShader )
{
	beginEditCP( m_pShaderData->m_pShader, osg::SHLChunk::FragmentProgramFieldMask );
		m_pShaderData->m_pShader->setVertexProgram( sVertexShader );
		m_pShaderData->m_pShader->setFragmentProgram( sFragmentShader );
	endEditCP( m_pShaderData->m_pShader, osg::SHLChunk::FragmentProgramFieldMask );
	return true;	
}

bool VistaOpenSGPerMaterialShader::AddUniformParameter(
									const CShaderUniformParam& oParam )
{
	beginEditCP( m_pShaderData->m_pShader );
		switch( oParam.m_eType )
		{			
			case CShaderUniformParam::BOOL:
				m_pShaderData->m_pShader->setUniformParameter( 
							oParam.m_sName.c_str(), 
							( oParam.m_iData != 0 ) );
				break;
			case CShaderUniformParam::INT:
				m_pShaderData->m_pShader->setUniformParameter( 
							oParam.m_sName.c_str(), 
							oParam.m_iData );
				break;
			case CShaderUniformParam::FLOAT:
				m_pShaderData->m_pShader->setUniformParameter( 
							oParam.m_sName.c_str(),
							osg::Real32( oParam.m_afData[0] ) );
				break;
			case CShaderUniformParam::VEC2:
				m_pShaderData->m_pShader->setUniformParameter( 
							oParam.m_sName.c_str(), 
							osg::Vec2f( oParam.m_afData ) );
				break;
			case CShaderUniformParam::VEC3:
				m_pShaderData->m_pShader->setUniformParameter( 
							oParam.m_sName.c_str(), 
							osg::Vec3f( oParam.m_afData ) );
				break;
			case CShaderUniformParam::VEC4:
				m_pShaderData->m_pShader->setUniformParameter( 
							oParam.m_sName.c_str(), 
							osg::Vec4f( oParam.m_afData ) );
				break;
			case CShaderUniformParam::OSG_LIGHT_MASK:
				m_pShaderData->m_pShader->setUniformParameter( "OSGActiveLightMask", 0 );
				break;
			case CShaderUniformParam::OSG_ACTIVE_LIGHTS:
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight0Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight1Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight2Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight3Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight4Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight5Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight6Active", 0 );
				m_pShaderData->m_pShader->setUniformParameter( "OSGLight7Active", 0 );
				break;
			case CShaderUniformParam::OSG_CAMERA_POSITION:
				m_pShaderData->m_pShader->setUniformParameter( "OSGCameraPosition", 0 );
				break;
			case CShaderUniformParam::INVALID:
			default:
				endEditCP( m_pShaderData->m_pShader );
				return false;
		}
	endEditCP( m_pShaderData->m_pShader );
	return true;
}

