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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaGeometryFactory.h"

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <vector>
using namespace std;

/*============================================================================*/
/*  HELPERS                                                                   */
/*============================================================================*/

bool ReadColorFromProplist( const VistaPropertyList& oPropList,
								   const std::string& sPropName,
								   VistaColor& oColorTarget )
{
	return oPropList.GetValue<VistaColor>( sPropName, oColorTarget );
}


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaGeometryFactory::VistaGeometryFactory(VistaSceneGraph *pSG)
: m_pSG(pSG)
{
}

VistaGeometryFactory::~VistaGeometryFactory()
{
	m_pSG = NULL;
}
/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

VistaSceneGraph* VistaGeometryFactory::GetSG() const
{
	return m_pSG;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   createPlaneGeo                                              */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreatePlane( float xsize, float zsize,
												  int xres, int zres,
												  int normal,
												  VistaColor color)
{

	VistaVertexFormat vertexFormat;
	vertexFormat.coordinate = VistaVertexFormat::COORDINATE;
	vertexFormat.color = VistaVertexFormat::COLOR_NONE;
	vertexFormat.normal = VistaVertexFormat::NORMAL;
	vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

	vector<VistaIndexedVertex> index;
	vector<float> coords;
	vector<float> textureCoords;
	vector<float> normals;
	vector<VistaColor> colors;

	VistaIndexedVertex ci;
	ci.SetColorIndex(0);
	ci.SetNormalIndex(0);

	float normalVec[3][3] = { {1,0,0}, {0,1,0}, {0,0,1} };

	normals.push_back(normalVec[normal][0]);
	normals.push_back(normalVec[normal][1]);
	normals.push_back(normalVec[normal][2]);

	float val[3];

	val[(normal + 2) % 3] = -xsize/2.0f;
	val[(normal + 0) % 3] = 0.0f;
	val[(normal + 1) % 3] = zsize/2.0f;

	for(int i = 0; i < zres+1; i++)
	{
		for(int j = 0; j < xres; j++)
		{
			coords.push_back(val[0]);
			coords.push_back(val[1]);
			coords.push_back(val[2]);
			switch(normal)
			{
				case Vista::X:
					textureCoords.push_back(1-(xsize/2+val[(normal + 2) % 3])/xsize);
					textureCoords.push_back((zsize/2+val[(normal + 1) % 3])/zsize);
					break;
				case Vista::Y:
					textureCoords.push_back((xsize/2+val[(normal + 2) % 3])/xsize);
					textureCoords.push_back(1-(zsize/2+val[(normal + 1) % 3])/zsize);
					break;
				case Vista::Z:
					textureCoords.push_back((zsize/2+val[(normal + 1) % 3])/zsize);
					textureCoords.push_back((xsize/2+val[(normal + 2) % 3])/xsize);
					break;
			}
			val[(normal + 2) % 3] += xsize/xres;
		}

		coords.push_back(val[0]);
		coords.push_back(val[1]);
		coords.push_back(val[2]);

		switch(normal)
		{
			case Vista::X:
				textureCoords.push_back(1-(xsize/2+val[(normal + 2) % 3])/xsize);
				textureCoords.push_back((zsize/2+val[(normal + 1) % 3])/zsize);
				break;
			case Vista::Y:
				textureCoords.push_back((xsize/2+val[(normal + 2) % 3])/xsize);
				textureCoords.push_back(1-(zsize/2+val[(normal + 1) % 3])/zsize);
				break;
			case Vista::Z:
				textureCoords.push_back((zsize/2+val[(normal + 1) % 3])/zsize);
				textureCoords.push_back((xsize/2+val[(normal + 2) % 3])/xsize);
				break;
		}
		val[(normal + 2) % 3] = -xsize/2.0f;;
		val[(normal + 1) % 3] -= zsize/zres;
	}

	for(int i = 0; i <= xres*zres; i += xres+1)
	{
		for(int j = 0; j < xres; j++)
		{
			// top right triangle of a quad
			ci.SetCoordinateIndex(i+j);
			ci.SetTextureCoordinateIndex(i+j);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+j+1);
			ci.SetTextureCoordinateIndex(i+j+1);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+j+xres+2);
			ci.SetTextureCoordinateIndex(i+j+xres+2);
			index.push_back(ci);

			// bottom left triangle of a quad
			ci.SetCoordinateIndex(i+j+xres+1);
			ci.SetTextureCoordinateIndex(i+j+xres+1);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+j);
			ci.SetTextureCoordinateIndex(i+j);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+j+xres+2);
			ci.SetTextureCoordinateIndex(i+j+xres+2);
			index.push_back(ci);
		}
	}

	VistaGeometry* ret = GetSG()->NewIndexedGeometry
		(index,coords,textureCoords,normals,colors,vertexFormat,VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
	ret->SetColor(color);
	return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateBoxGeo                                                */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateBox (float xsize, float ysize, float zsize,
												 int hor, int vert, int depth,
												 VistaColor color)
{
	VistaVertexFormat vertexFormat;
	vertexFormat.coordinate = VistaVertexFormat::COORDINATE;
	vertexFormat.color = VistaVertexFormat::COLOR_NONE;
	vertexFormat.normal = VistaVertexFormat::NORMAL;
	vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

	vector<VistaIndexedVertex> index;
	vector<float> coords;
	vector<float> textureCoords;
	vector<float> normals;
	vector<VistaColor> colors;

	VistaIndexedVertex ci;
	VistaVector3D vec;

	int basepoint = 0;
	int a,b,c,d;
	int x,y,pl;

	VistaVector3D size(xsize,  ysize,  zsize);
	VistaVector3D step(xsize / hor, ysize / vert, zsize / depth);
	VistaVector3D res((float) hor, (float) vert, (float) depth);

	static int inds[6][2] =  { {0,1}, {0,1}, {2,1}, {2,1}, {0,2}, {0,2} };
	static int signs[6][2] = { {1,1}, {-1,1}, {-1,1}, {1,1}, {1,-1}, {1,1} };
	static int asigns[6] = { 1, -1, 1, -1, 1, -1 };

	ci.SetColorIndex(0);

	for(pl = 0; pl < 6; pl++)
	{
		int axis = 3 - inds[pl][0] - inds[pl][1];

		for(y = 0; y <= res[inds[pl][1]] ; y++)
		{
			for(x = 0; x <= res[inds[pl][0]]; x++)
			{
				vec[ inds[pl][0] ] = (x * step[inds[pl][0]] - size[inds[pl][0]] / 2) * signs[pl][0];
				vec[ inds[pl][1] ] = (y * step[inds[pl][1]] - size[inds[pl][1]] / 2) * signs[pl][1];
				vec[ axis ] = size[ axis ] * asigns[ pl ] / 2;

				coords.push_back(vec[0]);
				coords.push_back(vec[1]);
				coords.push_back(vec[2]);

				//std::cout << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;

				textureCoords.push_back(x / (float) res[inds[pl][0]]);
				textureCoords.push_back(y / (float) res[inds[pl][1]]);
			 }
		 }
	}

	// normals
	normals.push_back(0.0);
	normals.push_back(0.0);
	normals.push_back(1.0);

	normals.push_back(0.0);
	normals.push_back(0.0);
	normals.push_back(-1.0);

	normals.push_back(1.0);
	normals.push_back(0.0);
	normals.push_back(0.0);

	normals.push_back(-1.0);
	normals.push_back(0.0);
	normals.push_back(0.0);

	normals.push_back(0.0);
	normals.push_back(1.0);
	normals.push_back(0.0);

	normals.push_back(0.0);
	normals.push_back(-1.0);
	normals.push_back(0.0);

	for(pl = 0; pl < 6; pl++)
	{
		ci.SetNormalIndex(pl);
		for(y = 0; y < res[inds[pl][1]]; y++)
		{
			int h = int(res[inds[pl][0]]);

			a = basepoint + (y + 1) * (h + 1);
			b = basepoint +  y      * (h + 1);
			ci.SetCoordinateIndex(a);
			ci.SetTextureCoordinateIndex(a);
			index.push_back(ci);
			ci.SetCoordinateIndex(b);
			ci.SetTextureCoordinateIndex(b);
			index.push_back(ci);
			for(x = 1; x < h; x++)
			{
				a = basepoint +  y      * (h + 1) + x;
				b = basepoint + (y + 1) * (h + 1) + x;
				c = basepoint +  y      * (h + 1);
				d = basepoint + (y + 1) * (h + 1);

				/*
				ci.SetCoordinateIndex(a);
				ci.SetTextureCoordinateIndex(a);
				index.push_back(ci);

				ci.SetCoordinateIndex(a);
				ci.SetTextureCoordinateIndex(a);
				index.push_back(ci);
				ci.SetCoordinateIndex(d);
				ci.SetTextureCoordinateIndex(d);
				index.push_back(ci);
				ci.SetCoordinateIndex(b);
				ci.SetTextureCoordinateIndex(b);
				index.push_back(ci);

				ci.SetCoordinateIndex(a);
				ci.SetTextureCoordinateIndex(a);
				index.push_back(ci);
				ci.SetCoordinateIndex(b);
				ci.SetTextureCoordinateIndex(b);
				index.push_back(ci);
				ci.SetCoordinateIndex(c);
				ci.SetTextureCoordinateIndex(c);
				index.push_back(ci);

				ci.SetCoordinateIndex(a);
				ci.SetTextureCoordinateIndex(a);
				index.push_back(ci);
				ci.SetCoordinateIndex(b);
				ci.SetTextureCoordinateIndex(b);
				index.push_back(ci);
				*/
			}

			c = basepoint +  y      * (h + 1);
			d = b;
			a = basepoint + (y + 1) * (h + 1) + h;
			b = basepoint +  y      * (h + 1) + h;
			ci.SetCoordinateIndex(a);
			ci.SetTextureCoordinateIndex(a);
			index.push_back(ci);

			ci.SetCoordinateIndex(c);
			ci.SetTextureCoordinateIndex(c);
			index.push_back(ci);
			ci.SetCoordinateIndex(b);
			ci.SetTextureCoordinateIndex(b);
			index.push_back(ci);
			ci.SetCoordinateIndex(a);
			ci.SetTextureCoordinateIndex(a);
			index.push_back(ci);


		}
		basepoint += int((res[inds[pl][0]] + 1.f) * (res[inds[pl][1]] + 1.f) );
	}

	VistaGeometry *ret = GetSG()->NewIndexedGeometry
		(index,coords,textureCoords,normals,colors,vertexFormat,VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
	ret->SetColor(color);
	return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateCircleGeo                                             */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateDisk(float pR,
												   int resC, int resD,
												   int normal,
												   VistaColor color)
{
	VistaVertexFormat vertexFormat;
	vertexFormat.coordinate = VistaVertexFormat::COORDINATE;
	vertexFormat.color = VistaVertexFormat::COLOR_NONE;
	vertexFormat.normal = VistaVertexFormat::NORMAL;
	vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;


	vector<VistaIndexedVertex> index;
	vector<float> textureCoords;
	vector<float> normals;
	vector<float> coords;
	vector<VistaColor> colors;

	VistaIndexedVertex ci;
	ci.SetColorIndex(0);
	ci.SetNormalIndex(0);

	float normalVec[3][3] = { {1,0,0}, {0,1,0}, {0,0,1} };

	normals.push_back(normalVec[normal][0]);
	normals.push_back(normalVec[normal][1]);
	normals.push_back(normalVec[normal][2]);

	coords.push_back(0.0);
	coords.push_back(0.0);
	coords.push_back(0.0); // Middle

	textureCoords.push_back(0.5f);
	textureCoords.push_back(0.5f);

	float val[3];
	val[(normal + 0) % 3] = 0.0f;

	float step = 2.0f/resC;
	float stepR = pR/resD;

	if(step == 0.0f)
		return NULL;

	for(int j = 1; j <= resD; j++)
	{
		for(int i = 0; i < resC; i++)
		{
			val[(normal + 2) % 3] = (float)(j*stepR*std::cos(i*2*Vista::Pi/resC));
			val[(normal + 1) % 3] = (float)(-j*stepR*std::sin(i*2*Vista::Pi/resC));

			coords.push_back(val[0]);
			coords.push_back(val[1]);
			coords.push_back(val[2]);
			switch(normal)
			{
				case Vista::X:
					textureCoords.push_back((float)((-j*stepR*std::cos(i*2*Vista::Pi/resC))/pR+1)/2);
					textureCoords.push_back((float)((-j*stepR*std::sin(i*2*Vista::Pi/resC))/pR+1)/2);
					break;
				case Vista::Y:
					textureCoords.push_back((float)((j*stepR*std::cos(i*2*Vista::Pi/resC))/pR+1)/2);
					textureCoords.push_back((float)((j*stepR*std::sin(i*2*Vista::Pi/resC))/pR+1)/2);
					break;
				case Vista::Z:
					textureCoords.push_back((float)((-j*stepR*std::sin(i*2*Vista::Pi/resC))/pR+1)/2);
					textureCoords.push_back((float)((j*stepR*std::cos(i*2*Vista::Pi/resC))/pR+1)/2);
					break;
			}
		}
	}

	int i = 1;
	for(i = 1; i < resC; i++)
	{
		ci.SetCoordinateIndex(0);
		ci.SetTextureCoordinateIndex(0);
		index.push_back(ci);
		ci.SetCoordinateIndex(i);
		ci.SetTextureCoordinateIndex(i);
		index.push_back(ci);
		ci.SetCoordinateIndex(i+1);
		ci.SetTextureCoordinateIndex(i+1);
		index.push_back(ci);
	}
	ci.SetCoordinateIndex(0);
	ci.SetTextureCoordinateIndex(0);
	index.push_back(ci);
	ci.SetCoordinateIndex(i);
	ci.SetTextureCoordinateIndex(i);
	index.push_back(ci);
	ci.SetCoordinateIndex(1);
	ci.SetTextureCoordinateIndex(1);
	index.push_back(ci);

	for(int j = 0; j <= resD-2; j++)
	{
		for(i = 1+j*resC; i < resC+j*resC; i++)
		{
			ci.SetCoordinateIndex(i);
			ci.SetTextureCoordinateIndex(i);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+resC);
			ci.SetTextureCoordinateIndex(i+resC);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+resC+1);
			ci.SetTextureCoordinateIndex(i+resC+1);
			index.push_back(ci);

			ci.SetCoordinateIndex(i);
			ci.SetTextureCoordinateIndex(i);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+1);
			ci.SetTextureCoordinateIndex(i+1);
			index.push_back(ci);
			ci.SetCoordinateIndex(i+resC+1);
			ci.SetTextureCoordinateIndex(i+resC+1);
			index.push_back(ci);
		}
		ci.SetCoordinateIndex(i);
		ci.SetTextureCoordinateIndex(i);
		index.push_back(ci);
		ci.SetCoordinateIndex(i+resC);
		ci.SetTextureCoordinateIndex(i+resC);
		index.push_back(ci);
		ci.SetCoordinateIndex(1+j*resC+resC);
		ci.SetTextureCoordinateIndex(1+j*resC+resC);
		index.push_back(ci);

		ci.SetCoordinateIndex(i);
		ci.SetTextureCoordinateIndex(i);
		index.push_back(ci);
		ci.SetCoordinateIndex(1+j*resC);
		ci.SetTextureCoordinateIndex(1+j*resC);
		index.push_back(ci);
		ci.SetCoordinateIndex(1+j*resC+resC);
		ci.SetTextureCoordinateIndex(1+j*resC+resC);
		index.push_back(ci);
	}
	VistaGeometry *ret = GetSG()->NewIndexedGeometry
		(index,coords,textureCoords,normals,colors,vertexFormat,VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
	ret->SetColor(color);
	return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateConeGeo                                               */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateCone(
				float botRad, float topRad, float height,
				int resC, int resD, int resY, VistaColor color,
				bool bBottom, bool bTop, bool bSides)
				// resD := Diameter resolution
				// resC := Circumference resolution
				// resY := resolution on the Y axis
{
	VistaVertexFormat vertexFormat;
	vertexFormat.coordinate = VistaVertexFormat::COORDINATE;
	vertexFormat.color = VistaVertexFormat::COLOR_NONE;
	vertexFormat.normal = VistaVertexFormat::NORMAL;
	vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;


	vector<VistaIndexedVertex> index;
	vector<float> coords;
	vector<float> textureCoords;
	vector<float> normals;
	vector<VistaColor> colors;

	VistaIndexedVertex ci;
	ci.SetColorIndex(0);
	ci.SetNormalIndex(0);

	coords.push_back(0.0);
	coords.push_back(-height/2.0f);
	coords.push_back(0.0); // Middle of the bottom

	textureCoords.push_back(0.5);
	textureCoords.push_back(0.5); // Middle of the bottom

	float step = 2.0f/resC;
	float stepR = botRad/resD;
	float stepYR = (botRad-topRad)/resY;
	float stepY = height/resY;
	float y = -height/2.0f;
	float r = botRad;

	if(step == 0.0f)
		return NULL;

	// Coords Bottom circle
	if(bBottom)
	{
		for(int j = 1; j <= resD; j++)
		{
			for(int i = 0; i < resC; i++)
			{
				coords.push_back((float)(j*stepR*std::cos(i*2*Vista::Pi/resC)));
				coords.push_back(-height/2.0f);
				coords.push_back((float)(-j*stepR*std::sin(i*2*Vista::Pi/resC)));

				textureCoords.push_back((float)((-j*stepR*std::cos(i*2*Vista::Pi/resC))/botRad+1)/2);
				textureCoords.push_back((float)((j*stepR*std::sin(i*2*Vista::Pi/resC))/botRad+1)/2);
			}
		}
	}

	// Coords Ring
	if(bSides)
	{
		for(int j = 0; j < resY+1; j++)
		{
			for(int i = 0; i < resC; i++)
			{
				coords.push_back((float)(r*std::cos(i*2*Vista::Pi/resC)));
				coords.push_back(y);
				coords.push_back((float)(-r*std::sin(i*2*Vista::Pi/resC)));

				textureCoords.push_back((float)i/resC);
				textureCoords.push_back((float)j/(resY/*+2*/));
			}
			y += stepY;
			r = r - stepYR;
		}
	}

	// Coords Top circle
	if(bTop)
	{
		float stepR1 = topRad/resD;
		for(int j = 1; j <= resD; j++)
		{
			for(int i = 0; i < resC; i++)
			{
				coords.push_back((float)(j*stepR1*std::cos(i*2*Vista::Pi/resC)));
				coords.push_back(height/2.0f);
				coords.push_back((float)(-j*stepR1*std::sin(i*2*Vista::Pi/resC)));

				textureCoords.push_back((float)((j*stepR1*std::cos(i*2*Vista::Pi/resC))/topRad+1)/2);
				textureCoords.push_back((float)((j*stepR1*std::sin(i*2*Vista::Pi/resC))/topRad+1)/2);
			}
		}
		coords.push_back(0);
		coords.push_back(height/2.0f);
		coords.push_back(0);

		textureCoords.push_back(0.5);
		textureCoords.push_back(0.5);
	}

	// Adding the last texture coords
	// cause of double using the geocoords but single use of texcoords
	if(bSides)
	{
		for(int i = 0; i < resY+2; i++)
		{
			textureCoords.push_back(1);
			textureCoords.push_back((float)i/(resY/*+2*/));
		}
	}

	//Creating normals
	normals.resize( 3*resC + 6 );
	std::vector<float>::iterator itNormalEntry = normals.begin();
	float fYComponent;
	float fFactor = atan( ( botRad - topRad ) / height );
	fYComponent = sin( fFactor );
	fFactor = cos( fFactor );
	(*itNormalEntry) = 0.0f;
	(*++itNormalEntry) = -1.0f;
	(*++itNormalEntry) = 0.0f;
	(*++itNormalEntry) = 0.0f;
	(*++itNormalEntry) = 1.0f;
	(*++itNormalEntry) = 0.0f;
	for( int i = 0; i < resC; ++i )
	{
		float fAngle = i * 2 * Vista::Pi / resC;
		(*++itNormalEntry) = fFactor * cos( fAngle );
		(*++itNormalEntry) = fYComponent;
		(*++itNormalEntry) = -fFactor * sin( fAngle );
	}

	// Building the geometry
	int texSize = (int)textureCoords.size() / 2;
	int ind = 1;
	if(bBottom)
	{
		ci.SetNormalIndex(1);
		for(ind = 1; ind < resC; ind++)
		{
			ci.SetCoordinateIndex(ind+1);
			ci.SetTextureCoordinateIndex(ind+1);			
			index.push_back(ci);
			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			index.push_back(ci);
			ci.SetCoordinateIndex(0);
			ci.SetTextureCoordinateIndex(0);
			index.push_back(ci);
		}
		ci.SetCoordinateIndex(1);
		ci.SetTextureCoordinateIndex(1);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind);
		ci.SetTextureCoordinateIndex(ind);
		index.push_back(ci);
		ci.SetCoordinateIndex(0);
		ci.SetTextureCoordinateIndex(0);
		index.push_back(ci);


		for(int j = 0; j <= resD-2; j++)
		{
			for(ind = 1+j*resC; ind < resC+j*resC; ind++)
			{
				ci.SetCoordinateIndex(ind+resC+1);
				ci.SetTextureCoordinateIndex(ind+resC+1);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+resC);
				ci.SetTextureCoordinateIndex(ind+resC);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind);
				ci.SetTextureCoordinateIndex(ind);
				index.push_back(ci);

				ci.SetCoordinateIndex(ind);
				ci.SetTextureCoordinateIndex(ind);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+1);
				ci.SetTextureCoordinateIndex(ind+1);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+resC+1);
				ci.SetTextureCoordinateIndex(ind+resC+1);
				index.push_back(ci);
			}
			ci.SetCoordinateIndex(1+j*resC+resC);
			ci.SetTextureCoordinateIndex(1+j*resC+resC);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+resC);
			ci.SetTextureCoordinateIndex(ind+resC);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			index.push_back(ci);

			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			index.push_back(ci);
			ci.SetCoordinateIndex(1+j*resC);
			ci.SetTextureCoordinateIndex(1+j*resC);
			index.push_back(ci);
			ci.SetCoordinateIndex(1+j*resC+resC);
			ci.SetTextureCoordinateIndex(1+j*resC+resC);
			index.push_back(ci);
		}
	}

	// sides
	if(bSides)
	{

		if(bBottom)
		{
			ind = resD*resC+1;
		}
		else
		{
			ind = 1;
		}
		for(int j = 0; j < resY-1; j++)
		{
			for(int n = 0; n < resC-1; n++)
			{
				ci.SetCoordinateIndex(ind);
				ci.SetTextureCoordinateIndex(ind);
				ci.SetNormalIndex(2+n);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+resC+1);
				ci.SetTextureCoordinateIndex(ind+resC+1);
				ci.SetNormalIndex(3+n);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+resC);
				ci.SetTextureCoordinateIndex(ind+resC);
				ci.SetNormalIndex(2+n);
				index.push_back(ci);

				ci.SetCoordinateIndex(ind+1);
				ci.SetTextureCoordinateIndex(ind+1);
				ci.SetNormalIndex(3+n);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+resC+1);
				ci.SetTextureCoordinateIndex(ind+resC+1);
				ci.SetNormalIndex(3+n);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind);
				ci.SetTextureCoordinateIndex(ind);
				ci.SetNormalIndex(2+n);
				index.push_back(ci);
				ind++;
			}

			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			ci.SetNormalIndex(1+resC);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+1);
			//ci.SetTextureCoordinateIndex(ind+1);
			ci.SetTextureCoordinateIndex(texSize-resY+j-1);
			ci.SetNormalIndex(2);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+resC);
			ci.SetTextureCoordinateIndex(ind+resC);
			ci.SetNormalIndex(1+resC);
			index.push_back(ci);

			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			ci.SetNormalIndex(1+resC);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind-resC+1);
			ci.SetTextureCoordinateIndex(texSize-resY+j-2);
			ci.SetNormalIndex(2);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+1);
			ci.SetTextureCoordinateIndex(texSize-resY+j-1);
			ci.SetNormalIndex(2);
			index.push_back(ci);
			ind++;
		}

		// last ring
		for(int n = 0; n < resC-1; n++)
		{
			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			ci.SetNormalIndex(2+n);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+resC+1);
			ci.SetTextureCoordinateIndex(ind+resC+1);
			ci.SetNormalIndex(3+n);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+resC);
			ci.SetTextureCoordinateIndex(ind+resC);
			ci.SetNormalIndex(2+n);
			index.push_back(ci);

			ci.SetCoordinateIndex(ind+1);
			ci.SetTextureCoordinateIndex(ind+1);
			ci.SetNormalIndex(3+n);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+resC+1);
			ci.SetTextureCoordinateIndex(ind+resC+1);
			ci.SetNormalIndex(3+n);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			ci.SetNormalIndex(2+n);
			index.push_back(ci);
			ind++;
		}

		ci.SetCoordinateIndex(ind+resC);
		ci.SetTextureCoordinateIndex(ind+resC);
		ci.SetNormalIndex(1+resC);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind);
		ci.SetTextureCoordinateIndex(ind);
		ci.SetNormalIndex(1+resC);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind+1);
		ci.SetTextureCoordinateIndex(texSize-2);
		ci.SetNormalIndex(2);
		index.push_back(ci);


		ci.SetCoordinateIndex(ind);
		ci.SetTextureCoordinateIndex(ind);
		ci.SetNormalIndex(1+resC);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind-resC+1);
		ci.SetTextureCoordinateIndex(texSize-3);
		ci.SetNormalIndex(2);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind+1);
		ci.SetTextureCoordinateIndex(texSize-2);
		ci.SetNormalIndex(2);
		index.push_back(ci);
		ind++;
	}

	// top circle
	if(bTop)
	{
		ind = (int)coords.size() / 3 - 1 - resC * resD;
		ci.SetNormalIndex(2);
		for(int i = 1; i < resC; i++)
		{
			ci.SetCoordinateIndex( (int)coords.size() / 3 - 1 );
			ci.SetTextureCoordinateIndex( (int)coords.size() / 3 - 1 );
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind+1);
			ci.SetTextureCoordinateIndex(ind+1);
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ind++;
		}

		ci.SetCoordinateIndex( (int)coords.size() /3 - 1 );
		ci.SetTextureCoordinateIndex( (int)coords.size() / 3 - 1 );
		ci.SetNormalIndex(1);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind);
		ci.SetTextureCoordinateIndex(ind);
		ci.SetNormalIndex(1);
		index.push_back(ci);
		ci.SetCoordinateIndex(ind-resC+1);
		ci.SetTextureCoordinateIndex(ind-resC+1);
		ci.SetNormalIndex(1);
		index.push_back(ci);

		for(int j = 1; j < resD; j++)
		{
			int stop  = ind + resC;
			for(ind = ind+1; ind < stop; ind++)
			{

				ci.SetCoordinateIndex(ind);
				ci.SetTextureCoordinateIndex(ind);
				ci.SetNormalIndex(1);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+1);
				ci.SetTextureCoordinateIndex(ind+1);
				ci.SetNormalIndex(1);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind-resC);
				ci.SetTextureCoordinateIndex(ind-resC);
				ci.SetNormalIndex(1);
				index.push_back(ci);

				ci.SetCoordinateIndex(ind-resC);
				ci.SetTextureCoordinateIndex(ind-resC);
				ci.SetNormalIndex(1);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind+1);
				ci.SetTextureCoordinateIndex(ind+1);
				ci.SetNormalIndex(1);
				index.push_back(ci);
				ci.SetCoordinateIndex(ind-resC+1);
				ci.SetTextureCoordinateIndex(ind-resC+1);
				ci.SetNormalIndex(1);
				index.push_back(ci);
			}
			ci.SetCoordinateIndex(ind-resC);
			ci.SetTextureCoordinateIndex(ind-resC);
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind);
			ci.SetTextureCoordinateIndex(ind);
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind-resC+1);
			ci.SetTextureCoordinateIndex(ind-resC+1);
			ci.SetNormalIndex(1);
			index.push_back(ci);

			ci.SetCoordinateIndex(ind-resC+1);
			ci.SetTextureCoordinateIndex(ind-resC+1);
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind-2*resC+1);
			ci.SetTextureCoordinateIndex(ind-2*resC+1);
			ci.SetNormalIndex(1);
			index.push_back(ci);
			ci.SetCoordinateIndex(ind-resC);
			ci.SetTextureCoordinateIndex(ind-resC);
			ci.SetNormalIndex(1);
			index.push_back(ci);
		}
	}

	VistaGeometry* ret = GetSG()->NewIndexedGeometry
		(index,coords,textureCoords,normals,colors,vertexFormat,VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
	ret->SetColor(color);
	return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateTorus                                                 */
/*                                                                            */
/*==================================================================q==========*/
VistaGeometry* VistaGeometryFactory::CreateTorus(
				float innerRad, float outerRad,
				int resSides, int resRings,
				VistaColor color
				)
{

	VistaVertexFormat vertexFormat;
	vertexFormat.coordinate = VistaVertexFormat::COORDINATE;
	vertexFormat.color = VistaVertexFormat::COLOR_NONE;
	vertexFormat.normal = VistaVertexFormat::NORMAL;
	vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;


	vector<VistaIndexedVertex> index;
	vector<float> coords;
	vector<float> textureCoords;
	vector<float> normals;
	vector<VistaColor> colors;

	VistaIndexedVertex ci;
	ci.SetColorIndex(0);
	int a, b;
	float theta, phi;
	float cosTheta, sinTheta;
	float ringDelta, sideDelta;
	ringDelta = 2.0f * Vista::Pi / resRings;
	sideDelta = 2.0f * Vista::Pi / resSides;

	for(a = 0, theta = 0.0; a <= resRings; a++, theta += ringDelta)
	{
		cosTheta = std::cos(theta);
		sinTheta = std::sin(theta);

		for(b = 0, phi = 0.0; b <= resSides; b++, phi += sideDelta)
		{
				float cosPhi, sinPhi, dist;

				cosPhi = std::cos(phi);
				sinPhi = std::sin(phi);
				dist   = outerRad + innerRad * cosPhi;

				normals.push_back(cosTheta * cosPhi);
				normals.push_back(-sinTheta * cosPhi);
				normals.push_back(sinPhi);

				coords.push_back(cosTheta * dist);
				coords.push_back(-sinTheta * dist);
				coords.push_back(innerRad * sinPhi);

				textureCoords.push_back(1 - (float)a / resRings);
				textureCoords.push_back((float)b / resSides);
		 }
	}

	for(a = 0; a < resSides; a++)
	{
		for(b = 0; b < resRings; b++)
		{
			ci.SetCoordinateIndex(b * (resSides+1) + a);
			ci.SetTextureCoordinateIndex(b * (resSides+1) + a);
			ci.SetNormalIndex(b * (resSides+1) + a);
			index.push_back(ci);

			ci.SetCoordinateIndex(b * (resSides+1) + a + 1);
			ci.SetTextureCoordinateIndex(b * (resSides+1) + a + 1);
			ci.SetNormalIndex(b * (resSides+1) + a + 1);
			index.push_back(ci);

			ci.SetCoordinateIndex(b * (resSides+1) + a + resSides + 1);
			ci.SetTextureCoordinateIndex(b * (resSides+1) + a + resSides + 1);
			ci.SetNormalIndex(b * (resSides+1) + a + resSides + 1);
			index.push_back(ci);

			// NEXT TRIANGLE

			ci.SetCoordinateIndex(b * (resSides+1) + a + resSides + 1);
			ci.SetTextureCoordinateIndex(b * (resSides+1) + a + resSides + 1);
			ci.SetNormalIndex(b * (resSides+1) + a + resSides + 1);
			index.push_back(ci);

			ci.SetCoordinateIndex(b * (resSides+1) + a + 1);
			ci.SetTextureCoordinateIndex(b * (resSides+1) + a + 1);
			ci.SetNormalIndex(b * (resSides+1) + a + 1);
			index.push_back(ci);

			ci.SetCoordinateIndex(b * (resSides+1) + a + resSides + 2);
			ci.SetTextureCoordinateIndex(b * (resSides+1) + a + resSides + 2);
			ci.SetNormalIndex(b * (resSides+1) + a + resSides + 2);
			index.push_back(ci);

		}
	}

	VistaGeometry* ret = GetSG()->NewIndexedGeometry
		(index,coords,textureCoords,normals,colors,vertexFormat,VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
	ret->SetColor(color);
	return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateSphereGeo                                             */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateSphere(float radius, int resolution, VistaColor color)
{
	return CreateEllipsoid(radius,radius,radius,resolution,resolution,color);
}

VistaGeometry* VistaGeometryFactory::CreateEllipsoid(
		float radius_a,
		float radius_b,
		float radius_c,
		int thetaPrecision,
		int phiPrecision,
		const VistaColor & color
	)
{

	int	i, j, idx, tidx;
	float	phi  , deltaPhi;
	float	theta, deltaTheta;
	int	phiResolution;


	// determine increments

	deltaPhi      = Vista::Pi / (phiPrecision - 1);
	deltaTheta    = 2.0f * Vista::Pi / thetaPrecision;
	phiResolution = phiPrecision - 2;

	VistaIndexedVertex     vertex;
	vector<VistaIndexedVertex>            listVertices;

	vector<VistaVector3D>   listCoords;
	vector<VistaVector3D>   listTextureCoords2D;
	vector<VistaVector3D>   listNormals;
	vector<VistaColor>   listColors;

	listVertices.resize(phiResolution*6*thetaPrecision );
	
	listCoords.resize(thetaPrecision*(phiPrecision-2) + 2);
	listNormals.resize(thetaPrecision*(phiPrecision-2) + 2);
	listTextureCoords2D.resize((thetaPrecision+1)*(phiPrecision-2) + 2*(thetaPrecision));

	VistaVertexFormat  vFormat;

	vFormat.coordinate = VistaVertexFormat::COORDINATE;
	vFormat.normal = VistaVertexFormat::NORMAL;
	vFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;
	vFormat.color = VistaVertexFormat::COLOR_NONE;

	// Point and normal creation
	// intermediate points

	idx = 0;
	for (i=0; i < thetaPrecision; i++)
	{
		theta = i * deltaTheta;
		
		for (j= 1; j<phiPrecision-1; j++)
		{
			phi = j * deltaPhi;

			listCoords[idx] = VistaVector3D(-radius_a * sinf(phi) * cosf(theta),
										-radius_b * cosf(phi),
										-radius_c * sinf(phi) * sinf(theta));

			listNormals[idx] = VistaVector3D(-sinf(phi) * cosf(theta)/radius_a,
										-cosf(phi)/radius_b,
										-sinf(phi) * sinf(theta)/radius_c);
			listNormals[idx].Normalize();
			idx++;
		}
	}

	// north and south poles
	listCoords[idx] = VistaVector3D(0.0, -radius_b, 0.0);
	listCoords[idx+1] = VistaVector3D(0.0,radius_b, 0.0);

	listNormals[idx] = VistaVector3D(0.0, -1.0,       0.0);
	listNormals[idx+1] = VistaVector3D(0.0,1.0,       0.0);

	// Texture coordinates creation
	tidx = 0;
	for(i=0; i<=thetaPrecision; i++)
		for(j=1; j<phiPrecision-1; j++)
			listTextureCoords2D[tidx++] = VistaVector3D((float)i/(float)thetaPrecision,
										(float)j/((float)phiPrecision-1),0.0);

	for(j=0; j<2; j++)
		for(i=0; i<thetaPrecision; i++)
			listTextureCoords2D[tidx++] = VistaVector3D((2.0f*(float)i+1)/(2.0f*(float)thetaPrecision),
										j?1.0f:0.0f, 0.0f);


	// Generate vertices and faces

	idx = 0;
	for (i=0; i < thetaPrecision; i++)
	{
		// around north pole
		vertex = VistaIndexedVertex(phiResolution*thetaPrecision);
		vertex.SetNormalIndex(phiResolution*thetaPrecision);
		vertex.SetTextureCoordinateIndex(phiResolution*(thetaPrecision+1)+i);
		listVertices[idx++] = vertex;

		vertex = VistaIndexedVertex(phiResolution*i);
		vertex.SetNormalIndex(phiResolution*i);
		vertex.SetTextureCoordinateIndex(phiResolution*i);
		listVertices[idx++] = vertex;

		vertex = VistaIndexedVertex(phiResolution*((i+1)%thetaPrecision));
		vertex.SetNormalIndex(phiResolution*((i+1)%thetaPrecision));
		vertex.SetTextureCoordinateIndex(phiResolution*(i+1));
		listVertices[idx++] = vertex;

		// around south pole
		vertex = VistaIndexedVertex(phiResolution*thetaPrecision+1);
		vertex.SetNormalIndex(phiResolution*thetaPrecision+1);
		vertex.SetTextureCoordinateIndex((phiResolution+1)*(thetaPrecision+1)-1+i);
		listVertices[idx++] = vertex;

		vertex = VistaIndexedVertex(phiResolution*(((i+1)%thetaPrecision)+1)-1);
		vertex.SetNormalIndex(phiResolution*(((i+1)%thetaPrecision)+1)-1);
		vertex.SetTextureCoordinateIndex(phiResolution*(i+2)-1);
		listVertices[idx++] = vertex;

		vertex = VistaIndexedVertex(phiResolution*(i+1)-1);
		vertex.SetNormalIndex(phiResolution*(i+1)-1);
		vertex.SetTextureCoordinateIndex(phiResolution*(i+1)-1);
		listVertices[idx++] = vertex;
	}

	// bands in-between poles

	for (i=0; i < phiResolution-1; i++)
	{
		for (j=0; j < thetaPrecision; j++)
		{
			vertex = VistaIndexedVertex(i + phiResolution*j);
			vertex.SetNormalIndex(i + phiResolution*j);
			vertex.SetTextureCoordinateIndex(i + phiResolution*j);
			listVertices[idx++] = vertex;

			vertex = VistaIndexedVertex(i + phiResolution*j + 1);
			vertex.SetNormalIndex(i + phiResolution*j + 1);
			vertex.SetTextureCoordinateIndex(i + phiResolution*j + 1);
			listVertices[idx++] = vertex;

			vertex = VistaIndexedVertex(i+(phiResolution*((j+1)%thetaPrecision)) + 1);
			vertex.SetNormalIndex(i+(phiResolution*((j+1)%thetaPrecision)) + 1);
			vertex.SetTextureCoordinateIndex(i+(phiResolution*(j+1)) + 1);
			listVertices[idx++] = vertex;

			vertex = VistaIndexedVertex(i + phiResolution*j);
			vertex.SetNormalIndex(i + phiResolution*j);
			vertex.SetTextureCoordinateIndex(i + phiResolution*j);
			listVertices[idx++] = vertex;

			vertex = VistaIndexedVertex(i+(phiResolution*((j+1)%thetaPrecision)) + 1);
			vertex.SetNormalIndex(i+(phiResolution*((j+1)%thetaPrecision)) + 1);
			vertex.SetTextureCoordinateIndex(i+(phiResolution*(j+1)) + 1);
			listVertices[idx++] = vertex;

			vertex = VistaIndexedVertex(i+(phiResolution*((j+1)%thetaPrecision)));
			vertex.SetNormalIndex(i+(phiResolution*((j+1)%thetaPrecision)));
			vertex.SetTextureCoordinateIndex(i+(phiResolution*(j+1)));
			listVertices[idx++] = vertex;
		}
	}


	VistaGeometry *ret = GetSG()->NewIndexedGeometry(listVertices, listCoords,
					listTextureCoords2D, listNormals,
					listColors, vFormat, 
					VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
	ret->SetColor(color);
	return ret;
	
}

VistaGeometry* VistaGeometryFactory::CreateTriangle(
					const VistaVector3D & a,
					const VistaVector3D & b,
					const VistaVector3D & c,
					int resolution,
					const VistaColor & color
)
{
	// set up list for indexed geometries
	std::vector<VistaIndexedVertex> listIndex;
	std::vector<VistaVector3D> listCoords;
	std::vector<VistaVector3D> textureCoords;
	std::vector<VistaVector3D> listNormals;
	std::vector<VistaColor> listColors;
	
	// set up vertex format
	VistaVertexFormat vertexFormat;
	vertexFormat.coordinate = VistaVertexFormat::COORDINATE;
	vertexFormat.color = VistaVertexFormat::COLOR_RGB;
	vertexFormat.normal = VistaVertexFormat::NORMAL;
	vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;
	
	// calculate vectors relative to a
	VistaVector3D v1     = c - a;
	VistaVector3D v1Part = v1 / (float)resolution;
	VistaVector3D v2     = b - a;
	VistaVector3D v2Part = v2 / (float)resolution;

	// create vertices
	for( int i = 0; i < resolution + 1; ++i )
	{
		VistaVector3D offset = a + (float)i * v1;
		for( int j = 0; j < resolution + 1 - i; ++j )
		{
			listCoords.push_back( offset +  v2 * (float)j );
		}
	}

	// create normals
   listNormals.push_back( v1.Cross( v2 ) );
   
   // create colors
   listColors.push_back( color );

   // create texture coordinates
   // ToDo: calculate texture-coordinates for triangle
   textureCoords.push_back( VistaVector3D( 0, 0, 0 ) );
   
   // create indices
	int offset = 0;
	int step = resolution;
	for( int i = 0; i < resolution; ++i )
	{
		for( int j = 0; j < resolution - i; ++j )
		{

			VistaIndexedVertex vertex(offset + j);
			vertex.SetColorIndex(0);
			vertex.SetNormalIndex(0);
			vertex.SetTextureCoordinateIndex(0);
			listIndex.push_back(vertex);

			vertex = VistaIndexedVertex(offset + j + 1);
			vertex.SetColorIndex(0);
			vertex.SetNormalIndex(0);
			vertex.SetTextureCoordinateIndex(0);
			listIndex.push_back(vertex);

			vertex = VistaIndexedVertex(offset + j + 1 + step);
			vertex.SetColorIndex(0);
			vertex.SetNormalIndex(0);
			vertex.SetTextureCoordinateIndex(0);
			listIndex.push_back(vertex);

			if( i > 0 )
			{
				VistaIndexedVertex vertex(offset + j);
				vertex.SetColorIndex(0);
				vertex.SetNormalIndex(0);
				vertex.SetTextureCoordinateIndex(0);
				listIndex.push_back(vertex);

				vertex = VistaIndexedVertex(offset + j + 1);
				vertex.SetColorIndex(0);
				vertex.SetNormalIndex(0);
				vertex.SetTextureCoordinateIndex(0);
				listIndex.push_back(vertex);

				vertex = VistaIndexedVertex(offset + j - ( 1 + step));
				vertex.SetColorIndex(0);
				vertex.SetNormalIndex(0);
				vertex.SetTextureCoordinateIndex(0);
				listIndex.push_back(vertex);
			}

		}
		offset += resolution - i + 1;
		--step;
	}

	VistaGeometry* ret = GetSG()->NewIndexedGeometry
		(listIndex,listCoords,textureCoords,listNormals,
		 listColors,vertexFormat,VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);

	return ret;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateGeoFromPropertyList                                   */
/*                                                                            */
/*============================================================================*/

VistaGeometry* VistaGeometryFactory::CreateFromPropertyList( const VistaPropertyList &oPropList )
{
	std::string sType;
	if( oPropList.GetValue( "TYPE", sType ) == false )
	{
		vstr::warnp() << "[VistaGeometryFactory]: No type specified!" << std::endl;
		return NULL;
	}

	VistaAspectsComparisonStuff::StringCompareObject oCompare( false );

	if( oCompare( sType, "PLANE" ) )
	{
		float fSizeX = oPropList.GetValueOrDefault<float>( "SIZEX", fSizeX, 1.0f );
		float fSizeY = oPropList.GetValueOrDefault<float>( "SIZEZ", fSizeY, 1.0f );
		int iResolutionX = oPropList.GetValueOrDefault<int>( "RESOLUTIONX", iResolutionX, 1 );
		int iResolutionZ = oPropList.GetValueOrDefault<int>( "RESOLUTIONZ", iResolutionZ, 1 );
		int nFacing = oPropList.GetValueOrDefault<int>( "FACEING", nFacing, Vista::Y );
		
		VistaColor oColor = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", oColor );
		return CreatePlane( fSizeX, fSizeY, iResolutionX, iResolutionZ, nFacing, oColor );
	}
	else if( oCompare( sType, "BOX" ) )
	{
		float fSizeX = oPropList.GetValueOrDefault<float>( "SIZEX", 1.0f );
		float fSizeY = oPropList.GetValueOrDefault<float>( "SIZEY", 1.0f );
		float fSizeZ = oPropList.GetValueOrDefault<float>( "SIZEZ", 1.0f );
		int iResolutionX = oPropList.GetValueOrDefault<int>( "RESOLUTIONX", 1 );
		int iResolutionY = oPropList.GetValueOrDefault<int>( "RESOLUTIONY", 1 );
		int iResolutionZ = oPropList.GetValueOrDefault<int>( "RESOLUTIONZ", 1 );

		VistaColor oColor = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", oColor );
		return CreateBox (
				fSizeX, fSizeY, fSizeZ,
				iResolutionX, iResolutionY, iResolutionZ,
				oColor );
	}
	else if( oCompare( sType, "DISK" ) )
	{
		float fRadius = oPropList.GetValueOrDefault<float>( "RADIUS", 0.5f );
		int iResolutionC = oPropList.GetValueOrDefault<int>( "RESOLUTIONC", 16 );
		int iResolutionD = oPropList.GetValueOrDefault<int>( "RESOLUTIOND", 16 );
		int nNormal = oPropList.GetValueOrDefault<int>( "NORMAL", Vista::Y );
		
		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateDisk( fRadius,
							iResolutionC, iResolutionD,
							nNormal,color );
	}
	else if( oCompare( sType, "CONE" ) )
	{
		float fRadiusBottom = oPropList.GetValueOrDefault<float>( "RADIUSBOTTOM", 0.5f );
		float fRadiusTop = oPropList.GetValueOrDefault<float>( "RADIUSTOP", 0.5f );
		float fHeight = oPropList.GetValueOrDefault<float>( "HEIGHT", 1.0f );
		int fResolutionC = oPropList.GetValueOrDefault<int>( "RESOLUTIONC", 16 );
		int fResolutionD = oPropList.GetValueOrDefault<int>( "RESOLUTIOND", 1 );
		int fResolutionY = oPropList.GetValueOrDefault<int>( "RESOLUTIONY", 1 );
		bool fBottom = oPropList.GetValueOrDefault<bool>( "BOTTOM", true );
		bool fTop = oPropList.GetValueOrDefault<bool>( "TOP", true );
		bool fSsides = oPropList.GetValueOrDefault<bool>( "SIDES", true );

		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateCone(
				fRadiusBottom, fRadiusTop, fHeight,
				fResolutionC, fResolutionD, fResolutionY,
				color, fBottom, fTop, fSsides
			);
	}
	else if( oCompare( sType, "CYLINDER" ) )
	{
		float fRadius = oPropList.GetValueOrDefault<float>( "RADIUS", 0.5f );
		float fHeight = oPropList.GetValueOrDefault<float>( "HEIGHT", 1.0f );
		int fResolutionC = oPropList.GetValueOrDefault<int>( "RESOLUTIONC", 16 );
		int fResolutionD = oPropList.GetValueOrDefault<int>( "RESOLUTIOND", 1 );
		int fResolutionY = oPropList.GetValueOrDefault<int>( "RESOLUTIONY", 1 );
		bool fBottom = oPropList.GetValueOrDefault<bool>( "BOTTOM", true );
		bool fTop = oPropList.GetValueOrDefault<bool>( "TOP", true );
		bool fSsides = oPropList.GetValueOrDefault<bool>( "SIDES", true );

		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateCone(
				fRadius, fRadius, fHeight,
				fResolutionC, fResolutionD, fResolutionY,
				color, fBottom, fTop, fSsides
			);
	}
	else if( oCompare( sType, "TORUS" ) )
	{

		float ringRadius = oPropList.GetValueOrDefault<float>( "RINGRADIUS", 0.1f );
		float outerRadius = oPropList.GetValueOrDefault<float>( "OUTERRADIUS", 0.4f );
		int resolutionSides = oPropList.GetValueOrDefault<int>( "RESOLUTIONSIDES", 30 );
		int resolutionRing = oPropList.GetValueOrDefault<int>( "RESOLUTIONRING", 30 );

		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateTorus(
			ringRadius, outerRadius,
			resolutionSides, resolutionRing,
			color
		);

	}
	else if( oCompare( sType, "SPHERE" ) )
	{

		float radius = oPropList.GetValueOrDefault<float>( "RADIUS", 0.5f );
		int resolution = oPropList.GetValueOrDefault<int>( "RESOLUTION", 32 );
		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateSphere( radius, resolution, color );
	}
	else if( oCompare( sType, "ELLIPSOID" ) )
	{
		float radius_a = oPropList.GetValueOrDefault<float>( "RADIUS_A", 0.5f );
		float radius_b = oPropList.GetValueOrDefault<float>( "RADIUS_B", 0.5f );
		float radius_c = oPropList.GetValueOrDefault<float>( "RADIUS_C", 0.5f );
		int thetaPrecision = oPropList.GetValueOrDefault<int>( "THETARESOLUTION", 32 );
		int phiPrecision = oPropList.GetValueOrDefault<int>( "PHIRESOLUTION", 32 );

		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateEllipsoid( radius_a, radius_b, radius_c, thetaPrecision, phiPrecision, color );
	}
	else if( oCompare( sType, "TRIANGLE" ) )
	{
		VistaVector3D v3PointA = oPropList.GetValueOrDefault<VistaVector3D>(
											"POINT_A", VistaVector3D( 0.5f, 0.5f,0 ) );
		VistaVector3D v3PointB = oPropList.GetValueOrDefault<VistaVector3D>(	
											"POINT_B", VistaVector3D( 0.5f, -0.5f,0 ) );
		VistaVector3D v3PointC = oPropList.GetValueOrDefault<VistaVector3D>(
											"POINT_C", VistaVector3D( 0, 0.5f, 0 ) );
		int iResolution = oPropList.GetValueOrDefault<int>( "RESOLUTION", 3 );
		VistaColor color = VistaColor::WHITE;
		ReadColorFromProplist( oPropList, "COLOR", color );

		return CreateTriangle( v3PointA, v3PointB, v3PointC, iResolution, color );
	}
	else
	{
		vstr::warnp() << "[VistaGeometryFactory]: Given type ["
					<< sType << "] not valid!" << std::endl;
		return NULL;
	}
}
