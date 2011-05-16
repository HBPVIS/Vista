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

#ifndef _VISTAOPENSGGRAPHICSBRIDGE_H
#define _VISTAOPENSGGRAPHICSBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsBridge.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/OpenSG/VistaOpenSGSystemClassFactory.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGPolygonChunk.h>
#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGBlendChunk.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif
/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class Vista2DText;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGGeometryData : public IVistaGeometryData
{
public:
	VistaOpenSGGeometryData();
	virtual ~VistaOpenSGGeometryData();

	osg::GeometryPtr GetGeometry() const;
	bool GetGeometry( osg::GeometryPtr & geom ) const;
	bool SetGeometry( const osg::GeometryPtr & geom );

	osg::PolygonChunkPtr GetAttrChunk() const;
	bool SetAttrChunk(osg::PolygonChunkPtr ptr);

	VistaRenderingAttributes GetRenderingAttributes() const;
	bool GetRenderingAttributes( VistaRenderingAttributes & renderingattributes ) const;
	bool SetRenderingAttributes( const VistaRenderingAttributes & renderingattributes );

	osg::UInt32 GetOsgFaceType() const;
	void SetOsgFaceType(osg::UInt32 nType);

private:
	osg::GeometryRefPtr          m_ptrGeometry;
	VistaRenderingAttributes m_pRenderingAttributes;
	osg::PolygonChunkRefPtr           m_ptrAttrChunk;
	osg::UInt32 m_nCachedFaceType;
};

/*============================================================================*/
class VISTAKERNELAPI VistaOpenSGGraphicsBridge : public IVistaGraphicsBridge
{
public:

	// additional debuging for the OSG scene graph content
	void DebugOSG();

	bool CalcVertexNormals(IVistaGeometryData*, const float &fCreaseAngle = 0.524f); // 30 degrees
	bool CalcFaceNormals(IVistaGeometryData*);

	VistaColorRGB GetBackgroundColor() const;
	void SetBackgroundColor(const VistaColorRGB & color);

	int  GetNumberOfVertices(IVistaGeometryData*) const;
	int  GetNumberOfFaces(IVistaGeometryData*) const;

	int  GetNumberOfColors(IVistaGeometryData*) const;
	int  GetNumberOfCoords(IVistaGeometryData*) const;
	int  GetNumberOfNormals(IVistaGeometryData*) const;
	int  GetNumberOfTextureCoords(IVistaGeometryData*) const;

	VistaVertexFormat GetVertexFormat(IVistaGeometryData* pData) const;
	VistaGeometry::faceType GetFaceType(IVistaGeometryData* pData) const;

	bool GetVertices(std::vector<VistaIndexedVertex>& vertices,
					 IVistaGeometryData*) const
	{
		return false; /** @todo implment...or not...gonna cut the graphics-backend abstraction down anyway!*/
	}

	bool GetCoordinates(std::vector<VistaVector3D>& coords,
						IVistaGeometryData*) const;

	bool GetTextureCoords2D(std::vector<VistaVector3D>& textureCoords2D,
							IVistaGeometryData*) const;

	bool GetNormals(std::vector<VistaVector3D>& normals, IVistaGeometryData*) const;
	bool GetNormals(std::vector<float>& coords, IVistaGeometryData*) const;

	bool GetColors(std::vector<VistaColorRGB>& colorsRGB, IVistaGeometryData*) const;

	bool GetFaces(std::vector<int>& faces, IVistaGeometryData*) const;
	bool GetCoordinates(std::vector<float>& coords, IVistaGeometryData*) const;

	/**
	 * Gives indices for all triangles in the geometry
	 * ! Returns the indices of Vertices, Normals, or Both
	 */
	bool GetTrianglesVertexIndices( std::vector<int>& vecVertexIndices,
									IVistaGeometryData* pData ) const;
	bool GetTrianglesNormalIndices( std::vector<int>& vecNormalIndices,
									IVistaGeometryData* pData ) const;
	bool GetTrianglesVertexAndNormalIndices( std::vector<int>& vecVertexIndices,
									std::vector<int>& vecNormalIndices,
									IVistaGeometryData* pData) const;

	int  GetCoordinateIndex(const int idx, IVistaGeometryData*) const;
	bool GetCoordinate(const int idx, float fCoord[3], IVistaGeometryData*) const;
	VistaVector3D GetCoordinate(const int idx, IVistaGeometryData*) const;

	int  GetNormalIndex(const int idx, IVistaGeometryData*) const;
	bool GetNormal(const int idx, float fNormal[3], IVistaGeometryData* pData) const;

	VistaVector3D GetNormal(const int idx, IVistaGeometryData*) const;

	int  GetColorIndex(const int idx, IVistaGeometryData*) const;
	VistaColorRGB GetColor(const int idx, IVistaGeometryData*) const;

	int  GetTextureCoordIndex(const int idx, IVistaGeometryData*) const;
	bool GetTextureCoord(const int idx, float fTexCoord[3],  IVistaGeometryData*) const;

	VistaVector3D GetTextureCoord(const int idx, IVistaGeometryData*) const;

	bool SetCoordinates(const int startIdx, const std::vector<VistaVector3D>& coords, IVistaGeometryData*);
	bool SetCoordinates(const int startIdx, const std::vector<float>& coords, IVistaGeometryData*);

	bool SetCoordinate(const int idx, const float coord[3], IVistaGeometryData*);
	bool SetCoordinate(const int idx, const VistaVector3D& coord, IVistaGeometryData*);
	bool SetCoordIndex(const int idx, const int value, IVistaGeometryData*);
	bool SetCoordIndices(const int startIdx, const std::vector<int>& indices, IVistaGeometryData*)
	{
		return false; /** @todo */
	}

	bool SetTextureCoords2D(const int startIdx,
							const std::vector<VistaVector3D>& textureCoords2D,
							IVistaGeometryData*);

	bool SetTextureCoords2D(const int startIdx,
							const std::vector<float>& textureCoords2D,
							IVistaGeometryData*);


	bool SetTextureCoord2D(const int idx, const float coord[3], IVistaGeometryData*);

	bool SetTextureCoord2D(const int idx, const VistaVector3D& coord, IVistaGeometryData* pData);

	bool SetTextureCoordIndex(const int idx, const int value, IVistaGeometryData*);

	bool SetTextureCoordsIndices(const int startIdx,
								 const std::vector<int>& indices,
								 IVistaGeometryData*)
	{
		return false; /** @todo */
	}

	bool SetNormals(const int startIdx, const std::vector<float>& normals, IVistaGeometryData*);
	bool SetNormals(const int startIdx,
					const std::vector<VistaVector3D>& normals,
					IVistaGeometryData*);
	bool SetNormal(const int idx, const float normal[3], IVistaGeometryData*);
	bool SetNormal(const int idx, const VistaVector3D& normal, IVistaGeometryData*);
	bool SetNormalIndex(const int idx, const int value, IVistaGeometryData*);
	bool SetNormalIndices(const int startIdx,
						  const std::vector<int>& indices,
						  IVistaGeometryData*)
	{
		return false; /** @todo */
	}

	bool SetColors(const int startIdx,
				   const std::vector<VistaColorRGB>& colors,
				   IVistaGeometryData*);

	bool SetColors(const int startIdx,
				   const int bufferLength,
				   float* colors, IVistaGeometryData*);

	bool SetColor(const int idx, const VistaColorRGB& color, IVistaGeometryData*);
	bool SetColorIndex(const int idx, const int value, IVistaGeometryData*);
	bool SetColorIndices(const int startIdx,
						 const std::vector<int>& indices,
						 IVistaGeometryData*)
	{
		return false; /** @todo */
	}

	bool SetColor(const VistaColorRGB & color, IVistaGeometryData*);

	bool SetTexture(const std::string &, IVistaGeometryData*);
	bool DeleteTexture(IVistaGeometryData*);

	bool ScaleGeometry(const float, IVistaGeometryData*);
	bool ScaleGeometry(const float[3], IVistaGeometryData*);
	bool ScaleGeometry(const float, const float, const float, IVistaGeometryData*);

	bool GetBoundingBox(VistaVector3D& min, VistaVector3D& max, IVistaGeometryData* pData) const;

	bool GetFaceBoundingBox(const int idx,
							VistaVector3D& min,
							VistaVector3D& max,
							IVistaGeometryData* pData) const;

	bool GetFaceCoords(const int idx,
					   VistaVector3D& a,
					   VistaVector3D& b,
					   VistaVector3D& c,
					   IVistaGeometryData* pData) const;

	bool GetFaceVertices(const int idx,
						 int& vertexId0,
						 int& vertexId1,
						 int& vertexId2,
						 IVistaGeometryData* pData) const;

	/**
	 * \param	coords	appropriate coordinate-indices are pushed_back onto coords
	 */
	 bool GetFaceCoords(const int idx, std::vector<int> &coords, int &nMod, IVistaGeometryData *) const;


	bool GetRenderingAttributes(VistaRenderingAttributes& attr, IVistaGeometryData* pData) const;
	bool SetRenderingAttributes(const VistaRenderingAttributes& attr, IVistaGeometryData* pData);
	float GetTransparency(IVistaGeometryData *) const;
	bool SetTransparency(const float& transparency, IVistaGeometryData* pData);

	bool CreateIndexedGeometry(const std::vector<VistaIndexedVertex>& vertices,
							   const std::vector<VistaVector3D>& coords,
							   const std::vector<VistaVector3D>& textureCoords2D,
							   const std::vector<VistaVector3D>& normals,
							   const std::vector<VistaColorRGB>& colorsRGB,
							   const VistaVertexFormat& vFormat,
							   const VistaGeometry::faceType fType,
							   IVistaGeometryData*);

	bool CreateIndexedGeometry
		  (const std::vector<VistaIndexedVertex>& vertices,
		   const std::vector<float>& coords,
			 const std::vector<float>& textureCoords,
			 const std::vector<float>& normals,
			 const std::vector<VistaColorRGB>& colors,
			 const VistaVertexFormat& vFormat,
			 const VistaGeometry::faceType fType,
			 IVistaGeometryData*);

	virtual bool GetIsStatic(IVistaGeometryData *);
	virtual void SetIsStatic(bool bIsStatic, IVistaGeometryData *);

	/**
	 * @return returns true if culling is enabled globally, false otherwise
	 */
	bool GetCullingEnabled() const;
	/**
	 * @param enables global culling if true, disables otherwise
	 */
	void SetCullingEnabled(bool bCullingEnabled);

	/**
	 * @return returns true if occlusion culling is enabled globally,
	           false otherwise
	 */
	bool GetOcclusionCullingEnabled() const;
	/**
	 * @param enables global occlusion culling if true, disables otherwise
	 */
	void SetOcclusionCullingEnabled( bool bOclCullingEnabled );

	/**
	 * @return returns true if bounding box drawing around scenegraph
	           nodes is enabled, false otherwise
	 */
	bool GetBBoxDrawingEnabled() const;
	/**
	 * @param enables bounding box drawing around scenegraph nodes if true,
	          disables otherwise
	 */
	void SetBBoxDrawingEnabled(bool bState);


	// av006ce I don't think this is necessary with OpenSG
	virtual bool BeginEdit(IVistaGeometryData*) { return true; }
	virtual bool EndEdit(IVistaGeometryData*) { return true; }

	virtual bool AddFace(const int vertexId0,
						 const int vertexId1,
						 const int vertexId2,
						 int& faceId,
						 IVistaGeometryData*);

	virtual bool DeleteFace(const int faceId, bool deleteVertices, IVistaGeometryData*);
	virtual int  AddVertex(const VistaVector3D& pos, IVistaGeometryData*);
	virtual bool DeleteVertex(const int vertexId, IVistaGeometryData*);

	virtual bool SetTexture(const int id,
							const int width,
							const int height,
							const int bpp,
							unsigned char *cBuffer,
							bool bHasAlpha,
							IVistaGeometryData *pData);

	virtual	bool CreateMaterialTable(void)
	{
		m_vMaterialTable.clear();
		return true;
	}

	// convenience function in/for the OpenSG bridge
	bool SetMaterial(const VistaMaterial & material, IVistaGeometryData*);

	bool SetMaterialIndex(const int& materialIndex, IVistaGeometryData*);

	// doesn't make much sense in OpenSG
	bool BindToVistaMaterialTable (IVistaGeometryData*) { return true; }

	int	 GetNumberOfMaterials (void) { return (int)m_vMaterialTable.size(); }

	/**
	 * @todo
	 * this one is really dangerous!!!!
	 * any material used in an opensg geometry will be destroyed with the geometry object,
	 * IS THIS METHOD IN USE SOMEWEHRE!!!??
	 */
	int	 AddMaterial		  (const VistaMaterial & material)
	{
		// get material properties from ViSTA object
		float amb[3], dif[3], spe[3], emi[3], opa, shi, alpha;
		material.GetAmbientColor(amb);
		material.GetDiffuseColor(dif);
		material.GetSpecularColor(spe);
		material.GetEmissionColor(emi);
		material.GetOpacity(opa);
		material.GetShininess(shi);
		alpha = opa;

		osg::MaterialChunkPtr matchunk = osg::MaterialChunk::create();
		beginEditCP(matchunk);
		{
		matchunk->setAmbient(osg::Color4f(amb[0],amb[1],amb[2],alpha));
		matchunk->setDiffuse(osg::Color4f(dif[0],dif[1],dif[2],alpha));
		matchunk->setSpecular(osg::Color4f(spe[0],spe[1],spe[2],alpha));
		matchunk->setEmission(osg::Color4f(emi[0],emi[1],emi[2],alpha));
		matchunk->setShininess(shi);
		matchunk->setLit(true);
		}
		endEditCP(matchunk);


		// store the retrieved Information in an OpenSG object
		osg::ChunkMaterialPtr stm = osg::ChunkMaterial::create();
		beginEditCP(stm);
		{
		stm->addChunk(matchunk);
		}
		endEditCP(stm);

		// cache all OpenSG data to prevent inefficient material-merge-scene-graph-operations
		m_vOSGMatTab.push_back(stm);

		m_vMaterialTable.push_back(material);
		m_vMaterialTable.back().SetMaterialIndex( (int)m_vMaterialTable.size() - 1 );
		return (int)m_vMaterialTable.size() - 1;
	}

	bool GetOSGMaterialByIndex(int iIndex, osg::ChunkMaterialPtr & stm);

	virtual bool GetMaterialByIndex(int iIndex, VistaMaterial &oIn) const;
	virtual bool GetMaterialByName(const std::string &sName, VistaMaterial &oIn) const;

	// little helper to get the osg::RenderAction from the DisplayBridge
	osg::RenderAction * GetRenderAction() const { return m_pRenderAction; }

	IVistaGeometryData* NewGeometryData();
	bool DeleteGeometryData(IVistaGeometryData*);

	//** misc
	float GetFrameRate() const;
	unsigned int GetFrameCount() const;

	//Constructor/Destructor
	VistaOpenSGGraphicsBridge(osg::RenderAction * pAction,
							   VistaOpenSGSystemClassFactory * pFactory );

	virtual ~VistaOpenSGGraphicsBridge()
	{}

private:
	bool SetTexture(osg::ImagePtr image, IVistaGeometryData* pData);

	bool m_bShowCursor;

	VistaOpenSGGraphicsBridge();
	VistaOpenSGSystemClassFactory * m_pFactory;
	osg::RenderAction * m_pRenderAction;

	std::vector<VistaMaterial>   m_vMaterialTable;
	std::vector<osg::ChunkMaterialPtr> m_vOSGMatTab;
	std::map<std::string,osg::ImagePtr> m_mTextureTable;

	// internal helper to avoid loading the same image twice from disc
	osg::ImagePtr GetCachedOrLoadImage( const std::string &sFileName );

};


#endif

