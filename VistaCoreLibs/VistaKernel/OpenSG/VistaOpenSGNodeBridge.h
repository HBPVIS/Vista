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

#ifndef _VISTAOPENSGNODEBRIDGE_H
#define _VISTAOPENSGNODEBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaMath/VistaBoundingBox.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGDrawActionBase.h>

#ifdef WIN32
#pragma warning(pop)
#endif

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGNodeData : public IVistaNodeData
{
	friend class VistaOpenSGSG;
	friend class VistaOpenSGNodeBridge;
public:

	osg::NodePtr        GetNode() const ;
	bool                SetNode( const osg::NodePtr &node );

	virtual osg::NodeCorePtr    GetCore() const;
	virtual bool                SetCore( const osg::NodeCorePtr & core );

protected:
	VistaOpenSGNodeData();
	virtual ~VistaOpenSGNodeData();

	osg::NodeRefPtr        m_ptrNode;
};


class VISTAKERNELAPI VistaOpenSGGeomNodeData : public VistaOpenSGNodeData
{
	friend class VistaOpenSGNodeBridge;
public:
	virtual bool             SetCore( const osg::NodeCorePtr &core );
	virtual osg::NodeCorePtr GetCore() const;
protected:
	VistaOpenSGGeomNodeData();
	virtual ~VistaOpenSGGeomNodeData();
};

class VISTAKERNELAPI VistaOpenSGExtensionNodeData : public VistaOpenSGNodeData
{
	friend class VistaOpenSGNodeBridge; // for destructor access
public:
protected:
	VistaOpenSGExtensionNodeData();
	virtual ~VistaOpenSGExtensionNodeData();
};

/**
 * This class is needed for the intensity functionality.
 * for each light node data, we will store the original
 * values as given by the user when creating or manipulating
 * the light node's properties.
 * These values will be scaled by the factor given in
 * SetIntensity() for a given light node.
 * In case you want to query the original value, use this
 * structure, in case you want to query the "current"
 * value use the methods of the node bridge, as these
 * will ask the OSG content for their values.
 */
class VISTAKERNELAPI VistaOpenSGLightNodeData : public VistaOpenSGNodeData
{
	// lights are split into light node and beacon in OpenSG
	// the VistaLightNode represents the beacon part of it.
	// The VistaOpenSGLightNodeData actually handles two OpenSG nodes!

	friend class VistaOpenSGNodeBridge;

public:
	osg::NodePtr GetLightNode() const
	{
		return m_ptrLightNode;
	}

	osg::NodeCorePtr GetLightCore() const
	{
		if(m_ptrLightNode != osg::NullFC)
			return m_ptrLightNode->getCore();
		else
			return osg::NullFC;
	}

private:

	osg::Color4f    m_cAmbient, m_cDiffuse, m_cSpecular;
	osg::Real32     m_fIntensity;
	osg::Real32     m_fShadowIntensity;
	osg::NodeRefPtr m_ptrLightNode;
	int             m_nLightType;

	VistaOpenSGLightNodeData()
		: m_fIntensity(1.0f)
		, m_fShadowIntensity(1.0f)
		, m_ptrLightNode(osg::NullFC)
		, m_nLightType(-1)
	{
	}

	virtual ~VistaOpenSGLightNodeData()
	{

	}
};


class VISTAKERNELAPI VistaOpenSGOpenGLNodeData : public VistaOpenSGNodeData
{
	friend class VistaOpenSGSG;
	friend class VistaOpenSGNodeBridge;

public:
	virtual ~VistaOpenSGOpenGLNodeData() {}


private:
	VistaOpenSGOpenGLNodeData()
	{}
};

//class VISTAKERNELAPI VistaOpenSGTextNodeDraw : public IVistaOpenGLDraw
//{
//	friend class VistaOpenSGSG;
//	friend class VistaOpenSGNodeBridge;
//public:
//	VistaOpenSGTextNodeDraw();
//	virtual ~VistaOpenSGTextNodeDraw();
//
//	virtual bool Do ();
//	virtual bool GetBoundingBox( VistaBoundingBox &bb ) const;
//
//private:
//	IVista3DText *m_pText;
//};

class VISTAKERNELAPI VistaOpenSGTextNodeData : public VistaOpenSGNodeData
{
public:
	VistaOpenSGTextNodeData(IVista3DText *pText)
	: m_pText(pText)
	{
	}

	virtual ~VistaOpenSGTextNodeData()
	{
	}

	IVista3DText *GetVistaText() const
	{
		return m_pText;
	}
protected:
	IVista3DText *m_pText;
private:
};


class VISTAKERNELAPI VistaOpenSGNodeBridge : public IVistaNodeBridge
{
public:
	//Constructor/Destructor
	VistaOpenSGNodeBridge();
	virtual ~VistaOpenSGNodeBridge();

	//###################################################
	//Node construction
	//###################################################
	IVistaNodeData *       NewRootNodeData();
	IVistaNodeData *       NewGroupNodeData();
	IVistaNodeData *       NewSwitchNodeData();
	IVistaNodeData *       NewLODNodeData();
	IVistaNodeData *       NewGeomNodeData( IVistaGeometryData * );
	IVistaNodeData *       NewLightNodeData( VISTA_LIGHTTYPE t = VISTA_AMBIENT_LIGHT );
	IVistaNodeData *       NewOpenGLNodeData();
	IVistaNodeData *       NewExtensionNodeData();
	IVistaNodeData *       NewTextNodeData(IVista3DText *);
	IVistaNodeData *       NewTransformNodeData();
	void	               DeleteNode(IVistaNode* pNode);
	//###################################################

	//###################################################
	//General purpose functions
	//###################################################
	bool    SetName        (const std::string &, IVistaNodeData*);
	bool    GetName        (std::string & strName, IVistaNodeData* pData) const;
	bool    GetIsEnabled    (IVistaNodeData*) const;
	void    SetIsEnabled    (bool, IVistaNodeData*);


	//###################################################
	// deprecated
	bool    Enable         (IVistaNodeData*);
	bool    Disable        (IVistaNodeData*);
	bool    IsEnabled      (IVistaNodeData*) const;
	//###################################################

	bool    GetBoundingBox (VistaVector3D& pMin, VistaVector3D& pMax, IVistaNodeData*) const;
	//###################################################

	//###################################################
	//Transformation functions
	//###################################################
	bool    SetTranslation      (const VistaVector3D& t,               IVistaNodeData*);
	bool    GetTranslation      (VistaVector3D&,                       IVistaNodeData*) const;
	bool    Translate           (const VistaVector3D& pos,             IVistaNodeData*);	
	bool    SetRotation         (const VistaTransformMatrix& matrix,   IVistaNodeData*);
	bool    SetRotation         (const VistaQuaternion& q,             IVistaNodeData*);
	bool    GetRotation         (VistaQuaternion&,                     IVistaNodeData*) const;
	bool    Rotate              (float x, float y, float z,            IVistaNodeData*);
	bool    Rotate              (const VistaQuaternion& q,             IVistaNodeData*);
	bool    GetRotation         (VistaTransformMatrix&,                IVistaNodeData*) const;
	bool    SetTransform        (const VistaTransformMatrix& trans,    IVistaNodeData*);
	bool    GetTransform        (VistaTransformMatrix&,                IVistaNodeData*) const;
	bool    GetWorldPosition	(VistaVector3D&,                       IVistaNodeData*) const;
	bool    GetWorldOrientation (VistaQuaternion&,                     IVistaNodeData*) const;
	bool    GetWorldTransform	(VistaTransformMatrix&,			       IVistaNodeData*) const;	
	
	//###################################################

	//###################################################
	// SPECIAL NODE FUNCTIONALITY
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	//GroupNode functions
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	bool    CanAddChild         (IVistaNodeData* pChildData,            IVistaNodeData*) const;
	bool    AddChild            (IVistaNodeData* pChildData,            IVistaNodeData*);
	bool    InsertChild         (IVistaNodeData* pChildData, int index, IVistaNodeData*);
	bool    DisconnectChild     (int nChildindex,                       IVistaNodeData*);
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	//SwitchNode functions
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	bool    SetActiveChild    (int i, IVistaNodeData*);
	int     GetActiveChild    (       IVistaNodeData*) const;

	//++++++++++++++++++++++++++++++++++++++++++++++++++
	// LOD Node functions (Level Of Detail)
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	bool SetCenter(const VistaVector3D& center,IVistaNodeData*);
	bool GetCenter(VistaVector3D& center,IVistaNodeData*) const;
	bool SetRange(const std::vector<float>& rangeList,IVistaNodeData*);
	bool GetRange(std::vector<float>& rangeList,IVistaNodeData*) const;

	//++++++++++++++++++++++++++++++++++++++++++++++++++
	// LightNode functions
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual bool    SetLightColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*)     ;
	virtual bool	SetLightAmbientColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*);
	virtual bool	SetLightDiffuseColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*);
	virtual bool	SetLightSpecularColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*);

	virtual bool	SetLightAttenuation(VISTA_LIGHTTYPE t, const VistaVector3D&, IVistaNodeData*);

	virtual bool	SetLightPosition(VISTA_LIGHTTYPE t, const VistaVector3D&, IVistaNodeData*);
	virtual bool	SetLightDirection(VISTA_LIGHTTYPE t, const VistaVector3D&, IVistaNodeData*);

	virtual bool	GetLightAmbientColor(VISTA_LIGHTTYPE t, float&, float&, float&, IVistaNodeData*);
	virtual bool	GetLightDiffuseColor(VISTA_LIGHTTYPE t, float&, float&, float&, IVistaNodeData*);
	virtual bool	GetLightSpecularColor(VISTA_LIGHTTYPE t, float&, float&, float&, IVistaNodeData*);

	virtual bool	GetLightAttenuation	(VISTA_LIGHTTYPE t, VistaVector3D&, IVistaNodeData*);

	virtual bool	GetLightPosition(VISTA_LIGHTTYPE t, VistaVector3D&, IVistaNodeData*) const ;
	virtual bool	GetLightDirection(VISTA_LIGHTTYPE t, VistaVector3D&, IVistaNodeData*);

	float   GetLightIntensity   (       IVistaNodeData*) const;
	bool    SetLightIntensity   (float, IVistaNodeData*);

	//float   GetLightShadowIntensity   (       IVistaNodeData*) const;
	//bool    SetLightShadowIntensity   (float, IVistaNodeData*);

	bool    SetSpotCharacter(float eCharacter, IVistaNodeData *);
	float   GetSpotCharacter(IVistaNodeData *) const;

	bool    SetSpotDistribution(int nExponent, IVistaNodeData *);
	int     GetSpotDistribution(IVistaNodeData *);

	bool    GetLightIsEnabled(IVistaNodeData*) const;
	void    SetLightIsEnabled(bool bEnabled, IVistaNodeData*);

	//++++++++++++++++++++++++++++++++++++++++++++++++++
	// GEOMETRY
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	bool    SetGeometry     (IVistaGeometryData*, IVistaNodeData*);


	//++++++++++++++++++++++++++++++++++++++++++++++++++
	// OPENGL CALLBACK NODES
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	bool    InitOpenGLNode  (IVistaNodeData*, VistaOpenGLNode*);

	//++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3D-TEXT NODES
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	IVista3DText *CreateFontRepresentation(const std::string &sFontName);
	bool          DestroyFontRepresentation(IVista3DText *);

	bool InitExtensionNode(VistaExtensionNode *pNode, IVistaNodeData*);


	//###################################################
	// Storage functions
	//###################################################
	IVistaNode *    LoadNode(const std::string &strFileName,
				 float fScale = 1.0f,
				 IVistaNodeBridge::eOptFlags = OPT_NONE);
	bool            SaveNode(std::string strFileName, VistaNode* pNode);

	virtual bool        ApplyOptimizationToNode(IVistaNode *pNode,
			eOptFlags eFlags );
	//###################################################

	/** clears the geometry node's core */
	void ClearGeomNodeData(VistaGeomNode *pNode) const;


	//###################################################
	// utility methods
	//###################################################
	osg::TransformPtr GetTransformCore(IVistaNodeData*) const;

	bool GetAmbientLightState() const;
private:
	osg::Color4f m_afAmbientLight;
	bool         m_bAmbientLightState;
};

/*============================================================================*/
/* INLINES		                                                              */
/*============================================================================*/

#endif
