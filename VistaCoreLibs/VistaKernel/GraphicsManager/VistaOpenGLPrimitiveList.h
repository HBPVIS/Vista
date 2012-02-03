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
// $Id: VistaOpenGLPolyLine.h 23044 2011-08-23 14:22:37Z dr165799 $

#ifndef _VISTAOPENGLPRIMITIVELIST_H
#define _VISTAOPENGLPRIMITIVELIST_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaMath/VistaBoundingBox.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
class VistaGroupNode;
class VistaSceneGraph;
class VistaOpenGLNode;
class IVistaNode;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenGLPrimitiveList
{
public:
	class VISTAKERNELAPI COpenGLPrimitiveList : public IVistaOpenGLDraw
	{
	public:
		COpenGLPrimitiveList();
		virtual ~COpenGLPrimitiveList();

		// IVistaOpenGLDraw interface
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		void SetVertices(const std::vector<float> &vecPoints);
		bool GetVertices(std::vector<float> &vecPoints) const;

		unsigned int GetPrimitiveType() const;
		void  SetPrimitiveType(unsigned int eMd);

		VistaMaterial GetMaterial() const;
		void           SetMaterial(const VistaMaterial &mat);

	protected:
		bool UpdateDisplayList();
	private:
		int m_iDispId;
		bool m_bDlistDirty;

		std::vector<float> m_vecPoints;
		VistaBoundingBox   m_oBBox;
		unsigned int m_eGLPrimitiveType;
		VistaMaterial     m_oMat;
	};
public:

	enum ePrimitiveType
	{
		POINTS=0,
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		QUADS,
		QUAD_STRIP,
		POLYGON
	};

	VistaOpenGLPrimitiveList(VistaSceneGraph* pVistaSceneGraph, VistaGroupNode* pParent);

	virtual ~VistaOpenGLPrimitiveList();

	IVistaNode* GetVistaNode() const;

	void SetVertices(const std::vector<float> &vecPoints);
	bool GetVertices(std::vector<float> &vecPoints) const;

	bool GetRemoveFromSGOnDelete() const;
	void SetRemoveFromSGOnDelete(bool bRemove);

	bool SetMaterial(const VistaMaterial &oMat) const;
	bool SetColor (const VistaColor  & color);
	bool SetPrimitiveType(const ePrimitiveType & primitiveType);

private:
	VistaOpenGLNode		*m_pOglNode;
	COpenGLPrimitiveList		*m_pDrawInterface;
	/** default is true */
	bool					m_bRemoveFromSGOnDelete;
	ePrimitiveType			m_ePrimitiveType;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALINE_H