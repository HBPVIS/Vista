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

#ifndef _VISTAGRAPHICSBRIDGE_H
#define _VISTAGRAPHICSBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaGeometry.h>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
//class VistaVertex;
class VistaVertexFormat;
class VistaFace;
class VistaVector3D;
class VistaPlane;
class VistaRay;
class VistaColorRGB;
class VistaMaterial;

class VistaRenderingAttributes;

class Vista2DDrawingObject;
class Vista2DText;
class Vista2DBitmap;
class Vista2DLine;
class Vista2DRectangle;
class VistaGeometry;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaGeometryData
{
	friend class VistaGeometry;
public:
	virtual ~IVistaGeometryData(){};
protected:

};

class VISTAKERNELAPI IVistaGraphicsBridge
{
	//friend class VistaSG;
	//friend class VistaGraphicsManager;
	//friend class Vista2DDrawingObject;
	//friend class Vista2DBitmap;
public:

	virtual bool CalcVertexNormals(IVistaGeometryData*, const float &fCreaseAngle = 0.524f) = 0; // 30 degrees
	virtual bool CalcFaceNormals(IVistaGeometryData*) = 0;

	virtual VistaColorRGB GetBackgroundColor() const =0;
	virtual void SetBackgroundColor(const VistaColorRGB & color) =0;

	virtual int  GetNumberOfVertices(IVistaGeometryData*) const =0;
	virtual int  GetNumberOfFaces(IVistaGeometryData*) const =0;

	virtual int  GetNumberOfColors(IVistaGeometryData*) const =0;
	virtual int  GetNumberOfCoords(IVistaGeometryData*) const =0;
	virtual int  GetNumberOfNormals(IVistaGeometryData*) const =0;
	virtual int  GetNumberOfTextureCoords(IVistaGeometryData*) const =0;

//	virtual bool IsIndexed(IVistaGeometryData*) const =0;
	virtual VistaVertexFormat	GetVertexFormat(IVistaGeometryData*) const =0;
	virtual VistaGeometry::faceType	GetFaceType(IVistaGeometryData*) const =0;

	// non-indexed geometry
//	virtual bool GetVertices(vector<VistaVertex>& vertices, IVistaGeometryData*) const =0;

	// indexed geometry
	virtual bool GetVertices(std::vector<VistaIndexedVertex>& vertices, IVistaGeometryData*) const =0;
	virtual bool GetCoordinates(std::vector<VistaVector3D>& coords, IVistaGeometryData*) const =0;
	virtual bool GetTextureCoords2D(std::vector<VistaVector3D>& textureCoords2D, IVistaGeometryData*) const =0;
	virtual bool GetNormals(std::vector<VistaVector3D>& normals, IVistaGeometryData*) const =0;
	virtual bool GetNormals(std::vector<float>& normals, IVistaGeometryData*) const =0;
	virtual bool GetColors(std::vector<VistaColorRGB>& colorsRGB, IVistaGeometryData*) const =0;

	virtual bool GetFaces(std::vector<int>& faces, IVistaGeometryData*) const =0;
	virtual bool GetCoordinates(std::vector<float>& coords, IVistaGeometryData*) const =0;

	/**
	 * Gives indices for all triangles in the geometry
	 * ! Returns the indices of Vertices, Normals, or Both
	 */
	virtual bool GetTrianglesVertexIndices( std::vector<int>& vecVertexIndices,
									IVistaGeometryData* pData ) const = 0;
	virtual bool GetTrianglesNormalIndices( std::vector<int>& vecNormalIndices,
									IVistaGeometryData* pData ) const = 0;
	virtual bool GetTrianglesVertexAndNormalIndices( std::vector<int>& vecVertexIndices,
									std::vector<int>& vecNormalIndices,
									IVistaGeometryData* pData) const = 0;

	virtual int  GetCoordinateIndex(const int idx, IVistaGeometryData*) const =0;
	virtual bool GetCoordinate(const int idx, float fCoord[3], IVistaGeometryData*) const =0;
	virtual VistaVector3D GetCoordinate(const int idx, IVistaGeometryData*) const =0;

	virtual int  GetNormalIndex(const int idx, IVistaGeometryData*) const =0;
	virtual bool GetNormal(const int idx, float fNormal[3], IVistaGeometryData*) const =0;
	virtual VistaVector3D GetNormal(const int idx, IVistaGeometryData*) const =0;

	virtual int  GetColorIndex(const int idx, IVistaGeometryData*) const =0;
	virtual VistaColorRGB GetColor(const int idx, IVistaGeometryData*) const =0;

	virtual int  GetTextureCoordIndex(const int idx, IVistaGeometryData*) const =0;
	virtual bool GetTextureCoord(const int idx, float fTexCoord[3],  IVistaGeometryData*) const =0;
	virtual VistaVector3D GetTextureCoord(const int idx, IVistaGeometryData*) const =0;

	virtual bool SetCoordinates(const int startIdx, const std::vector<VistaVector3D>& coords, IVistaGeometryData*) =0;
	virtual bool SetCoordinates(const int startIdx, const std::vector<float>& coords, IVistaGeometryData*) =0;

	virtual bool SetCoordinate(const int idx, const float coord[3], IVistaGeometryData*) =0;
	virtual bool SetCoordinate(const int idx, const VistaVector3D& coord, IVistaGeometryData*) =0;
	virtual bool SetCoordIndex(const int idx, const int value, IVistaGeometryData*) =0;
	virtual bool SetCoordIndices(const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) =0;

	virtual bool SetTextureCoords2D(const int startIdx, const std::vector<VistaVector3D>& textureCoords2D, IVistaGeometryData*) =0;
	virtual bool SetTextureCoords2D(const int startIdx, const std::vector<float>& textureCoords2D, IVistaGeometryData*) =0;

	virtual bool SetTextureCoord2D(const int idx, const float coord[3], IVistaGeometryData*) =0;
	virtual bool SetTextureCoord2D(const int idx, const VistaVector3D& coord, IVistaGeometryData* pData) =0;

	virtual bool SetTextureCoordIndex(const int idx, const int value, IVistaGeometryData*) =0;
	virtual bool SetTextureCoordsIndices(const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) =0;

	virtual bool SetNormals(const int startIdx, const std::vector<float>& normals, IVistaGeometryData*) =0;
	virtual bool SetNormals(const int startIdx, const std::vector<VistaVector3D>& normals, IVistaGeometryData*) =0;

	virtual bool SetNormal(const int idx, const float normal[3], IVistaGeometryData*) =0;
	virtual bool SetNormal(const int idx, const VistaVector3D& normal, IVistaGeometryData*) =0;
	virtual bool SetNormalIndex(const int idx, const int value, IVistaGeometryData*) =0;
	virtual bool SetNormalIndices(const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) =0;

	virtual bool SetColors(const int startIdx, const std::vector<VistaColorRGB>& colors, IVistaGeometryData*) =0;
	virtual bool SetColors(const int startIdx, const int bufferLength, float* colors, IVistaGeometryData*) =0;

	virtual bool SetColor(const int idx, const VistaColorRGB& color, IVistaGeometryData*) =0;
	virtual bool SetColorIndex(const int idx, const int value, IVistaGeometryData*) =0;
	virtual bool SetColorIndices(const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) =0;

	virtual bool SetColor(const VistaColorRGB & color, IVistaGeometryData*) =0;

	virtual bool SetMaterial(const VistaMaterial & material, IVistaGeometryData*) =0;
	virtual bool BindToVistaMaterialTable	(IVistaGeometryData*) = 0;
	virtual int	 GetNumberOfMaterials		(void) = 0;
	virtual int	 AddMaterial				(const VistaMaterial &Material) = 0;
	virtual bool SetMaterialIndex			(const int& materialIndex, IVistaGeometryData*) =0;
	virtual bool GetMaterialByIndex(int iIndex, VistaMaterial &oIn) const = 0;
	virtual bool GetMaterialByName(const std::string &sName, VistaMaterial &oIn) const = 0;

	virtual bool SetTexture(const std::string &, IVistaGeometryData*) =0;
	virtual bool SetTexture(const int id,        /**< unknown parameter ? */
			                const int width,     /**< width in pixel */
			                const int height,    /**< height in pixel */
			                const int bpp,       /**< bits per color channel (8,32) */
			                unsigned char *data, /**< pointer to pixelbuffer RGB(A) */
			                bool bHasAlpha,      /**< pixel data has alpha or not RGBA iff true, RGB else */
			                IVistaGeometryData*) =0;
	virtual bool DeleteTexture(IVistaGeometryData*) =0;

	virtual bool ScaleGeometry(const float, IVistaGeometryData*) =0;
	virtual bool ScaleGeometry(const float[3], IVistaGeometryData*) =0;
	virtual bool ScaleGeometry(const float, const float, const float, IVistaGeometryData*) =0;

	virtual bool GetBoundingBox(VistaVector3D& min, VistaVector3D& max, IVistaGeometryData*) const =0;
	virtual bool GetFaceBoundingBox(const int idx, VistaVector3D& min, VistaVector3D& max, IVistaGeometryData*) const =0;
	virtual bool GetFaceCoords(const int idx, VistaVector3D& a,VistaVector3D& b,VistaVector3D& c, IVistaGeometryData*) const =0;
	virtual bool GetFaceCoords(const int idx, std::vector<int> &coords, int &nMod, IVistaGeometryData*) const = 0;
	virtual bool GetFaceVertices(const int idx, int& vertexId0, int& vertexId1, int& vertexId2, IVistaGeometryData* pData) const =0;

	virtual bool GetRenderingAttributes(VistaRenderingAttributes& attr, IVistaGeometryData*) const =0;
	virtual	bool SetRenderingAttributes(const VistaRenderingAttributes& attr, IVistaGeometryData*) = 0;

	virtual float GetTransparency(IVistaGeometryData *) const = 0;
	virtual bool SetTransparency(const float& transparency, IVistaGeometryData*) = 0;

//	virtual bool CreateGeometry(const std::vector<VistaVertex>& vertices, const VistaVertexFormat& vFormat,
//		const VistaGeometry::faceType fType, IVistaGeometryData*) =0;

	virtual bool CreateIndexedGeometry(const std::vector<VistaIndexedVertex>& vertices,
										const std::vector<VistaVector3D>& coords,
										const std::vector<VistaVector3D>& textureCoords2D,
										const std::vector<VistaVector3D>& normals,
										const std::vector<VistaColorRGB>& colorsRGB,
										const VistaVertexFormat& vFormat,
										const VistaGeometry::faceType fType,
										IVistaGeometryData*) =0;

	virtual bool CreateIndexedGeometry
		(const std::vector<VistaIndexedVertex>& vertices,
		 const std::vector<float>& coords,
		 const std::vector<float>& textureCoords,
		 const std::vector<float>& normals,
		 const std::vector<VistaColorRGB>& colors,
		 const VistaVertexFormat& vFormat,
		 const VistaGeometry::faceType fType,
		 IVistaGeometryData*) =0;

	virtual bool GetIsStatic(IVistaGeometryData *) = 0;
	virtual void SetIsStatic(bool bIsStatic, IVistaGeometryData *) = 0;

	/**
	 * @return returns true if culling is enabled globally, false otherwise
	 */
	virtual bool GetCullingEnabled() const = 0;
	/**
	 * @param enables global culling if true, disables otherwise
	 */
	virtual void SetCullingEnabled(bool bCullingEnabled) = 0;

	/**
	 * @return returns true if occlusion culling is enabled globally,
	           false otherwise
	 */
	virtual bool GetOcclusionCullingEnabled() const = 0;
	/**
	 * @param enables global occlusion culling if true, disables otherwise
	 */
	virtual void SetOcclusionCullingEnabled( bool bOclCullingEnabled ) = 0;

	/**
	 * @return returns true if bounding box drawing around scenegraph
	           nodes is enabled, false otherwise
	 */
	virtual bool GetBBoxDrawingEnabled() const = 0;
	/**
	 * @param enables bounding box drawing around scenegraph nodes if true,
	          disables otherwise
	 */
	virtual void SetBBoxDrawingEnabled(bool bState) = 0;


	/** Begin edit session
	*/
	virtual bool BeginEdit(IVistaGeometryData*) = 0;
	/** End edit session
	*/
	virtual bool EndEdit(IVistaGeometryData*) = 0;

	/** Add new face defined by vertices 0,1,2
	  * @param int faceId : faceId of the new face can be specified.
	*/
	virtual bool AddFace(const int vertexId0, const int vertexId1, const int vertexId2, int& faceId, IVistaGeometryData*) = 0;
	/** Delete face faceId. Resulting isolated vertices will be deleted if
	  * deleteVertices is true
	  * @param int faceId : face to be deleted
	  * @param bool deleteVertices : delete isolated vertices
	  * @return bool true/false
	*/
	virtual bool DeleteFace(const int faceId, bool deleteVertices, IVistaGeometryData*) = 0;
	/** Create a new vertex at a given position.
	  * @return int new vertexId
	*/
	virtual int AddVertex(const VistaVector3D& pos, IVistaGeometryData*) = 0;
	/** Delete vertex vertexId and all incident faces
	*/
	virtual bool DeleteVertex(const int vertexId, IVistaGeometryData*) = 0;

	virtual IVistaGeometryData* NewGeometryData() =0;
	virtual bool DeleteGeometryData(IVistaGeometryData*) =0;
	VistaGeometry* NewGeometry(IVistaGeometryData* pData);

	//misc
	virtual float	GetFrameRate() const =0;
	virtual unsigned int GetFrameCount() const =0;
	virtual	bool	CreateMaterialTable	(void) = 0;


	virtual ~IVistaGraphicsBridge();

protected:
	IVistaGraphicsBridge();


};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif

