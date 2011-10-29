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
// $Id: VistaOpenSGParticles.cpp$

#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4275)
#pragma warning(disable: 4267)
#pragma warning(disable: 4251)
#pragma warning(disable: 4231)
#endif

#include "VistaOpenSGParticles.h"

#include <VistaBase/VistaVersion.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaExtensionNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>

#include <VistaTools/VistaFileSystemFile.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGParticles.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGImageFunctions.h>
#include <OpenSG/OSGImageFileHandler.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGBlendChunk.h>
#include <OpenSG/OSGTexGenChunk.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>
#include <OpenSG/OSGSimpleTexturedMaterialBase.h>
#include <OpenSG/OSGSHLChunk.h>

using namespace osg;


struct VistaOpenSGParticlesData
{
	ParticlesPtr  ptrParticles;
	MFPnt3f      *pPositionsField;
	MFPnt3f		 *pSecPositionsField;
	MFVec3f      *pSizesField;
	MFColor4f    *pColors4Field;
	MFColor3f    *pColors3Field;
	bool          bHasAlpha;

	SHLChunkPtr	  mSHLChunk; //for shader quads
	SimpleTexturedMaterialPtr mParticleTexture; //for shader quads
	VistaOpenSGParticlesData(ParticlesPtr ptr, const bool bWithAlpha = false)
		: ptrParticles(ptr)
		, pPositionsField(NULL)
		, pSecPositionsField(NULL)
		, pColors4Field(NULL)
		, pColors3Field(NULL)
		, pSizesField(NULL)
		, bHasAlpha(bWithAlpha)
	{
		CPEditAll(ptrParticles);

		// set "immersive billboarding" mode
		ptrParticles->setMode(Particles::ViewDirQuads);

		// create positions field
		GeoPositions3fPtr pos = GeoPositions3f::create();
		ptrParticles->setPositions(pos);
		pPositionsField = pos->getFieldPtr();

		// create secpositions field
		GeoPositions3fPtr secpos = GeoPositions3f::create();
		ptrParticles->setSecPositions(secpos);
		pSecPositionsField = secpos->getFieldPtr();

		// create color field (either 3f or 4f)
		GeoColorsPtr col;
		if(GetHasAlpha())
		{
			GeoColors4fPtr ptr = GeoColors4f::create();
			col = ptr;
			pColors4Field = ptr->getFieldPtr();
		}
		else
		{
			GeoColors3fPtr ptr = GeoColors3f::create();
			col = ptr;
			pColors3Field = ptr->getFieldPtr();
		}

		ptrParticles->setColors(col);

		pSizesField = ptrParticles->getMFSizes();
		ptrParticles->setMaterial(SimpleMaterial::create());
	}

	bool GetHasAlpha() const {return bHasAlpha;}
	void SetHasAlpha(const bool state)
	{
		if(state == GetHasAlpha())
			return;

		//else
		bHasAlpha = state;

		GeoColorsPtr col;
		if(GetHasAlpha())
		{
			GeoColors4fPtr ptr = GeoColors4f::create();
			col = ptr;
			pColors4Field = ptr->getFieldPtr();
			pColors3Field = NULL;
		}
		else
		{
			GeoColors3fPtr ptr = GeoColors3f::create();
			col = ptr;
			pColors3Field = ptr->getFieldPtr();
			pColors4Field = NULL;
		}

		CPEdit(ptrParticles, Particles::ColorsFieldMask);
		ptrParticles->setColors(col);
	}
};

VistaOpenSGParticles::VistaOpenSGParticles(VistaSceneGraph *pSG,
						VistaGroupNode *pParentNode,
						int eParticleMode )
: m_pParticleNode(NULL)
, m_pData(NULL)
, m_eParticleMode( eParticleMode )
{
	m_pParticleNode = pSG->NewExtensionNode(pParentNode, NULL);
	NodePtr ptrNode = static_cast<VistaOpenSGExtensionNodeData*>
		(m_pParticleNode->GetData())->GetNode();

	// create OpenSG particles
	m_pData = new VistaOpenSGParticlesData(Particles::create());
	beginEditCP(ptrNode, Node::CoreFieldMask);
	ptrNode->setCore(m_pData->ptrParticles);
	endEditCP  (ptrNode, Node::CoreFieldMask);
}

VistaOpenSGParticles::~VistaOpenSGParticles()
{
	DetachAndDelete();
	delete m_pData;
}

void VistaOpenSGParticles::DetachAndDelete()
{
	VistaGroupNode* pP = m_pParticleNode->GetParent();
	if(pP)
		pP->DisconnectChild(m_pParticleNode);

	delete m_pParticleNode->GetExtension();
	delete m_pParticleNode;
}


void VistaOpenSGParticles::BeginEdit() const
{
	beginEditCP(m_pData->ptrParticles);
}

void VistaOpenSGParticles::EndEdit() const
{
	endEditCP(m_pData->ptrParticles);
}

void VistaOpenSGParticles::SetColorsWithAlpha(const bool bWithAlpha) const
{
	m_pData->SetHasAlpha(bWithAlpha);
}

void VistaOpenSGParticles::SetNumParticles( const int nSize, const bool bUseSeparateColors,
										   const bool bUseSeparateSizes ) const
{
	BeginEdit();
	m_pData->pPositionsField->resize(nSize, Pnt3f  (0, 0, 0));
	//if( m_eParticleMode == PM_VIEWERQUADS ) I THINK I NEED THIS ALWAYS ...
		m_pData->pSecPositionsField->resize(nSize, Pnt3f  (0, 0, 0));

	if(m_pData->GetHasAlpha())
		m_pData->pColors4Field   ->resize(bUseSeparateColors ? nSize : 1, Color4f(1.0f, 1.0f, 1.0f, 1.0f));
	else
		m_pData->pColors3Field   ->resize(bUseSeparateColors ? nSize : 1, Color3f(1.0f, 1.0f, 1.0f));

	m_pData->pSizesField    ->resize(bUseSeparateSizes  ? nSize : 1, Vec3f  (1.0f, 1.0f, 1.0f));
	EndEdit();
}

int VistaOpenSGParticles::GetNumParticles() const
{
	return m_pData->pPositionsField->size();
}

int VistaOpenSGParticles::GetNumColors() const
{
	if(m_pData->GetHasAlpha())
		return m_pData->pColors4Field->size();
	else
		return m_pData->pColors3Field->size();
}

bool VistaOpenSGParticles::GetColorsWithAlpha() const
{
	return m_pData->GetHasAlpha();
}

int VistaOpenSGParticles::GetNumSizes() const
{
	return m_pData->pSizesField->size();
}

float* VistaOpenSGParticles::GetParticlePositions3fField(const int &idx) const
{
	return (float*)&m_pData->pPositionsField[idx][0];
}


float* VistaOpenSGParticles::GetParticleColors3fField(const int &idx) const
{
	return (float*)&m_pData->pColors3Field[idx][0];
}

float* VistaOpenSGParticles::GetParticleColors4fField(const int &idx) const
{
	return (float*)&m_pData->pColors4Field[idx][0];
}

float* VistaOpenSGParticles::GetParticleColorsXfField(const int &idx) const
{
	if(m_pData->GetHasAlpha())
		return GetParticleColors4fField(idx);
	else
		return GetParticleColors3fField(idx);
}

float* VistaOpenSGParticles::GetParticleSizes3fField(const int &idx) const
{
	return (float*)&m_pData->pSizesField[idx][0];
}

VistaExtensionNode *VistaOpenSGParticles::GetParticleNode() const
{
	return m_pParticleNode;
}


static inline float gaussian2(const float &x, const float &y, const float &sigma)
{
	//SQRT(PI) = 2.506628274631000502415765284811045253006
	return osg::osgexp(-0.5f * (x*x + y*y) / (sigma*sigma)) / (2.506628274631000502415765284811045253006f * sigma);
}

static void makeGaussBlobAlpha(ImagePtr &img, const float &sigma = 6.3f, const float &m = 10.0f)
{
	assert(img != osg::NullFC);
	assert(img->hasAlphaChannel());
	beginEditCP(img);
	osg::UInt8 bpp = img->getBpp();
	osg::UInt8 *tex = img->getData();
	int width = img->getWidth();
	int height = img->getHeight();

	// modify alpha
	for(int y=0; y < height; y++)
	{
		for(int x=0; x < width; x++)
		{
			// clean border
			if(x==0 || x==width-1 || y==0 || y==height-1)
			{
				tex[bpp*(y*width+x) + bpp-1] = 0;
			}
			else
			{
			  osg::UInt8 pv = (osg::UInt8)osg::osgMin(255.0f, m * gaussian2(x-width*0.5f, y-height*0.5f, sigma)*256.0f);
				tex[bpp*(y*width+x) +  bpp-1] = pv;
				/*5. * gaussian2(x-dim2, y-dim2, (dim*0.2))*/
			}
		}
	}
	endEditCP(img);
}


static osg::ChunkMaterialPtr createTexturedMaterial(osg::ImagePtr ptrImage)
{
	ChunkMaterialPtr ptrChunkMat;
	{
		MaterialChunkPtr ptrMat;
		TextureChunkPtr  ptrTex;
		TexGenChunkPtr   ptrTexGen;
		BlendChunkPtr    ptrBlend;

		// create chunks
		ptrChunkMat = ChunkMaterial::create();
		ptrMat      = MaterialChunk::create();
		ptrTex      =  TextureChunk::create();
		ptrTexGen   =   TexGenChunk::create();
		ptrBlend    =    BlendChunk::create();

		// get rid of begin/endEdit calls
		CPEditAll(ptrChunkMat);
		CPEditAll(ptrMat);
		CPEditAll(ptrTex);
		CPEditAll(ptrTexGen);
		CPEditAll(ptrBlend);

		// add all chunks to the material
		ptrChunkMat->addChunk(ptrMat);
		ptrChunkMat->addChunk(ptrTex);
		ptrChunkMat->addChunk(ptrTexGen);
		ptrChunkMat->addChunk(ptrBlend);

		// set parameters
		ptrMat->setDiffuse(Color4f(1,1,1,1));
		ptrMat->setLit(false);

		ptrTex->setImage( ptrImage );
		ptrTex->setEnvMode( GL_REPLACE );
		ptrTex->setMagFilter( GL_LINEAR );

		if(ptrImage->hasAlphaChannel())
		{
			ptrBlend->setSrcFactor(GL_SRC_ALPHA);
			ptrBlend->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
			ptrBlend->setAlphaFunc(GL_NONE);
			ptrBlend->setAlphaValue(0);
		}
	}
	return ptrChunkMat;

}

void VistaOpenSGParticles::SetUseGaussBlobTexture(const int &size, const float &sigma, const float &m) const
{
	ImagePtr pImage = Image::create();
	beginEditCP(pImage);
		pImage->set(Image::OSG_RGBA_PF, size, size);
		pImage->clear(0xFF);
	endEditCP  (pImage);
	makeGaussBlobAlpha(pImage, sigma, m);

	beginEditCP(m_pData->ptrParticles,  Particles::MaterialFieldMask);
		m_pData->ptrParticles->setMaterial( createTexturedMaterial(pImage) );
	endEditCP  (m_pData->ptrParticles,  Particles::MaterialFieldMask);
}



void VistaOpenSGParticles::SetMaterialTransparency(const float &a)	const
{
	VISTA_THROW_NOT_IMPLEMENTED
	//ChunkMaterialPtr ptrChMat	= ChunkMaterialPtr::dcast
	//	(m_pData->ptrParticles->getMaterial());
	//assert(ptrChMat);

	//MaterialChunkPtr ptrMatChunk = MaterialChunkPtr::dcast(
	//	ptrChMat->find(MaterialChunk::getClassType());
	//beginEditCP(ptrMatChunk, MaterialChunk::TransparencyFieldMask);
	//ptrMat
	//endEditCP  (ptrMatChunk, MaterialChunk::TransparencyFieldMask);
}

void VistaOpenSGParticles::SetGLTexEnvMode       (const GLenum &eMode) const
{
	ChunkMaterialPtr ptrChunkMat =
		ChunkMaterialPtr::dcast(m_pData->ptrParticles->getMaterial());
	assert(ptrChunkMat);

	TextureChunkPtr ptrTexChunk = TextureChunkPtr::dcast(
		ptrChunkMat->find(TextureChunk::getClassType()));

	if(!ptrTexChunk)
	{
		ptrTexChunk = TextureChunk::create();
		beginEditCP(ptrChunkMat, ChunkMaterial::ChunksFieldMask);
		ptrChunkMat->addChunk(ptrTexChunk);
		endEditCP  (ptrChunkMat, ChunkMaterial::ChunksFieldMask);
	}

	beginEditCP(ptrTexChunk, TextureChunk::EnvModeFieldMask);
	ptrTexChunk->setEnvMode(eMode);
	endEditCP   (ptrTexChunk, TextureChunk::EnvModeFieldMask);
}

void VistaOpenSGParticles::SetGLBlendSrcFactor(const GLenum &eMode) const
{
	ChunkMaterialPtr ptrChunkMat =
		ChunkMaterialPtr::dcast(m_pData->ptrParticles->getMaterial());
	assert(ptrChunkMat);

	BlendChunkPtr ptrBlendChunk = BlendChunkPtr::dcast(
		ptrChunkMat->find(BlendChunk::getClassType()));
	if(!ptrBlendChunk)
	{
		ptrBlendChunk = BlendChunk::create();
		beginEditCP(ptrChunkMat, ChunkMaterial::ChunksFieldMask);
		ptrChunkMat->addChunk(ptrBlendChunk);
		endEditCP  (ptrChunkMat, ChunkMaterial::ChunksFieldMask);
	}

	beginEditCP(ptrBlendChunk, BlendChunk::SrcFactorFieldMask);
	ptrBlendChunk->setSrcFactor(eMode);
	endEditCP  (ptrBlendChunk, BlendChunk::SrcFactorFieldMask);
}

void VistaOpenSGParticles::SetGLBlendDestFactor(const GLenum &eMode) const
{
	ChunkMaterialPtr ptrChunkMat =
		ChunkMaterialPtr::dcast(m_pData->ptrParticles->getMaterial());
	assert(ptrChunkMat);

	BlendChunkPtr ptrBlendChunk = BlendChunkPtr::dcast(
		ptrChunkMat->find(BlendChunk::getClassType()));
	if(!ptrBlendChunk)
	{
		ptrBlendChunk = BlendChunk::create();
		beginEditCP(ptrChunkMat, ChunkMaterial::ChunksFieldMask);
		ptrChunkMat->addChunk(ptrBlendChunk);
		endEditCP  (ptrChunkMat, ChunkMaterial::ChunksFieldMask);
	}

	beginEditCP(ptrBlendChunk, BlendChunk::DestFactorFieldMask);
	ptrBlendChunk->setDestFactor(eMode);
	endEditCP  (ptrBlendChunk, BlendChunk::DestFactorFieldMask);
}

void VistaOpenSGParticles::SetDrawOrder(const VistaOpenSGParticles::eDrawOrder &mode) const
{
	beginEditCP(m_pData->ptrParticles, Particles::DrawOrderFieldMask);
	m_pData->ptrParticles->setDrawOrder(mode);
	endEditCP  (m_pData->ptrParticles, Particles::DrawOrderFieldMask);
}

bool VistaOpenSGParticles::SetUseSpriteImage(const std::string &strFileName)  const
{
	VistaFileSystemFile oFile( strFileName );
	if( oFile.Exists() == false )
	{
		vstr::warnp() << "VistaOpenSGParticles::SetUseSpriteImage() -- "
			<< "File [" << strFileName << "] does not exist!" << std::endl;
		return false;
	}
	if (m_pData->ptrParticles->getMode() == Particles::ShaderQuads)
	{
		SetNormal();
		SetSimpleTexturedMaterial(strFileName);
		SetParticleShader();
	}
	else
	//Really don't know what to do else then. We only got those two kinds of Particles implemented yet.
	{		
		beginEditCP(m_pData->ptrParticles,  Particles::MaterialFieldMask);
			m_pData->ptrParticles->setMaterial(createTexturedMaterial(
					ImageFileHandler::the().read(strFileName.c_str())));
		endEditCP  (m_pData->ptrParticles,  Particles::MaterialFieldMask);
	}
	return true;
}

void VistaOpenSGParticles::SetMode(const int &mode)
{
	beginEditCP(m_pData->ptrParticles, Particles::ModeFieldMask);

	switch(mode)
	{
		case PM_VIEWERQUADS:
		{
			m_pData->ptrParticles->setMode(Particles::ViewerQuads);
			m_pData->pSecPositionsField->resize( 0, Pnt3f(0, 0, 0));
			break;
		}
		case PM_DIRECTEDQUADS:
		{
			m_pData->ptrParticles->setMode(Particles::ShaderQuads);
			m_pData->pSecPositionsField->resize( m_pData->pPositionsField->getSize(), Pnt3f(0, 0, 0) );
			break;
		}
		default:
			break;
	}
	

	endEditCP(m_pData->ptrParticles, Particles::ModeFieldMask);

	m_eParticleMode = mode;

}


float* VistaOpenSGParticles::GetSecParticlePositions3fField(const int &idx) const
{
	return (float*)&m_pData->pSecPositionsField[idx][0];
}

void VistaOpenSGParticles::CopyPosToSecpos() const
{
	float *secpos;
	float *pos;

	pos = GetParticlePositions3fField();
	secpos = GetSecParticlePositions3fField();

	int i=0;
	for (i=0; i < (int)m_pData->pSecPositionsField->size(); i++)
	{
		secpos[3*i+0] = pos[3*i+0];
		secpos[3*i+1] = pos[3*i+1];
		secpos[3*i+2] = pos[3*i+2];
	}

}

void VistaOpenSGParticles::SetNormal() const
{
	beginEditCP(m_pData->ptrParticles, Particles::DrawOrderFieldMask);

	osg::GeoNormals3fPtr norms = osg::GeoNormals3f::create();
	norms->push_back(Vec3f(0,1,0));

	m_pData->ptrParticles->setNormals(norms);

	endEditCP(m_pData->ptrParticles, Particles::DrawOrderFieldMask);

}


void VistaOpenSGParticles::SetSimpleTexturedMaterial(const std::string &FileName) const
{
    m_pData->mParticleTexture = osg::SimpleTexturedMaterial::create();

    osg::ImagePtr pImage = osg::Image::create();
	pImage->read(FileName.c_str());

	osg::beginEditCP(m_pData->mParticleTexture);
    m_pData->mParticleTexture->setLit( true );
    m_pData->mParticleTexture->setImage( pImage );
    m_pData->mParticleTexture->setEnvMode( GL_MODULATE );
    m_pData->mParticleTexture->setMagFilter( GL_LINEAR_MIPMAP_LINEAR );


    osg::BlendChunkPtr bl= osg::BlendChunk::create();
	if(pImage->hasAlphaChannel())
	{
    osg::beginEditCP(bl);
        bl->setSrcFactor(GL_SRC_ALPHA);
        bl->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
		bl->setAlphaFunc(GL_GREATER);
		bl->setAlphaValue(0.1f);
    osg::endEditCP(bl);
	}

    m_pData->mParticleTexture->addChunk(bl);
	osg::endEditCP(m_pData->mParticleTexture);

    m_pData->ptrParticles->setMaterial(m_pData->mParticleTexture);
}

void VistaOpenSGParticles::SetParticleShader() const
{
	// vertex shader program.
	static std::string vp_program =
	"\n"
	"#define pPos gl_MultiTexCoord1\n"
	"#define pSecPos gl_MultiTexCoord2\n"
	"#define pSize gl_MultiTexCoord3\n"
	"#define pOffset gl_Vertex\n"
	"\n"
	"varying float brightness;\n"
	"\n"
	"void main(void)\n"
	"{\n"
	"\n"
	"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
	"\n"
	"vec4 pos;\n"
	"vec3 diff_secpos_pos = vec3(pSecPos-pPos);\n"
	"float move_length = length(diff_secpos_pos);\n"
	"diff_secpos_pos = diff_secpos_pos / move_length;\n"
	"vec3 cameraPosition = normalize(vec3(gl_ModelViewMatrixInverse*vec4(0,0,0,1.0)-pPos));"
	"vec3 other_direction = (-cross(diff_secpos_pos,cameraPosition));\n"
	"\n"
	"gl_FrontColor = gl_Color;\n"
	"brightness = pSize[2];\n"
	"\n"
	"pos = pPos - vec4(pSize[0] * ( pOffset[0] - 0.5 )* diff_secpos_pos,0)\n"
	"           + vec4(pSize[1] * pOffset[1] * other_direction,0);\n"
	"gl_Position = gl_ModelViewProjectionMatrix * pos;\n"
	"}\n";

	//fragment shader program.
		static std::string fp_program =
	"\n"
	"uniform sampler2D myTexture;\n"
	"\n"
	"varying float brightness;\n"
	"\n"
	"void main(void)\n"
	"{\n"
	"\n"
	"   //gl_FragColor = vec4(my_color, 0);\n"
	"	vec4 tex2d = texture2D(myTexture, vec2(gl_TexCoord[0]));\n"
	"	float yval = 0.0;\n"
	"	float uval = 0.0;\n"
	"	float vval = 0.0;\n"
	"	float rval = 0.0;\n"
	"	float gval = 0.0;\n"
	"	float bval = 0.0;\n"
	"	yval = 0.3*tex2d.r + 0.6*tex2d.g + 0.1*tex2d.b;\n"
	"	uval = -0.3*tex2d.r - 0.6*tex2d.g + 0.9*tex2d.b;\n"
	"	vval = 0.7*tex2d.r - 0.6*tex2d.g - 0.1*tex2d.b;\n"
	"   yval = yval * (1.0+6.0*brightness);\n"
	"	if (yval>1.0) {yval=1.0;}\n"
	//"\n"
	//"	rval = yval+vval;\n"
	//"	gval = yval - 0.166667*uval - 0.5*vval;\n"
	//"	bval = yval + uval;\n"
	//"\n"
	//"	rval = rval*(1.0+gl_Color.r);\n"
	//"	if (rval > 1.0) {rval = 1.0;}\n"
	//"\n"
	//"	gval = gval*(1.0+gl_Color.g);\n"
	//"	if (gl_Color.g == 0.0) {gval = 0.0;}\n"
	//"\n"
	//"	bval = bval*(1.0+gl_Color.b);\n"
	//"	if (bval > 255.0) {bval = 255.0;}\n"
	"	rval = yval+vval;\n"
	"	gval = yval - 0.166667*uval - 0.5*vval;\n"
	"	bval = yval + uval;\n"
	"\n"
	"	gl_FragColor  = vec4(rval*(1.0+gl_Color.r),gval*(1.0-gl_Color.g),bval*(1.0-gl_Color.b),tex2d.a*gl_Color.a);\n"
	"}\n";

	SetShaderPrograms(vp_program,fp_program);

}

//The following functions could be an All-In-One Solution. I decided to leave them seperate so one couly easily re-use them. Still they're hidden as Private!
void VistaOpenSGParticles::SetShaderPrograms(const std::string &VertexShaderProgram, const std::string &FragmentShaderProgram) const
{
    m_pData->mSHLChunk = osg::SHLChunk::create();
	osg::beginEditCP(m_pData->mSHLChunk);
    m_pData->mSHLChunk->setVertexProgram(VertexShaderProgram);
	m_pData->mSHLChunk->setFragmentProgram(FragmentShaderProgram);
	osg::endEditCP(m_pData->mSHLChunk);

	osg::beginEditCP(m_pData->mParticleTexture);
	m_pData->mParticleTexture->addChunk(m_pData->mSHLChunk);
	osg::endEditCP(m_pData->mParticleTexture);

	SetUniformParameter("myTexture",0);
}

void VistaOpenSGParticles::SetUniformParameter(const std::string &UniformName, const int &NumValue) const
{
	osg::beginEditCP(m_pData->mSHLChunk);
	m_pData->mSHLChunk->setUniformParameter(UniformName.c_str(),NumValue);
	osg::endEditCP(m_pData->mSHLChunk);
}

int VistaOpenSGParticles::GetMode() const
{
	return m_eParticleMode;
}
