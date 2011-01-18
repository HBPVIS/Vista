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

#ifndef _VISTANODEBRIDGE_H
#define _VISTANODEBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaKernel/GraphicsManager/VistaLightNode.h" // needed for lighttype

#include <map>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaVector3D;
class VistaQuaternion;
class IVistaOpenGLDraw;
class VistaOpenGLNode;
class VistaTraversalCallbackNode;

class IVista3DText;
class VistaTextNode;


class VistaSG;
class IVistaGeometryData;
class IVistaNodeData;
class VistaTransformNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Helper class to provide a proper context for toolkit specific method
 * calls. It only declares a virtual destructor, as it's meaning is
 * completely toolkit specific.
 */
class VISTAKERNELAPI IVistaNodeData
{
public:
	virtual ~IVistaNodeData();
};

class VISTAKERNELAPI IVistaNodeBridge
{
public:
	virtual ~IVistaNodeBridge();

	//###################################################
	//bridge construction and query
	//###################################################
	virtual bool Init(VistaSG* pSG);
	VistaSG    *GetVistaSG() const;


	//###################################################
	//Node construction
	//###################################################
	virtual IVistaNodeData*		NewRootNodeData() = 0;
	virtual IVistaNodeData*		NewGroupNodeData() = 0;
	virtual IVistaNodeData*		NewSwitchNodeData() = 0;
	virtual IVistaNodeData*		NewLODNodeData() = 0;
	virtual IVistaNodeData*		NewGeomNodeData(IVistaGeometryData*) = 0;

	virtual IVistaNodeData*		NewLightNodeData(VISTA_LIGHTTYPE t) = 0;

	virtual IVistaNodeData*		NewOpenGLNodeData()    = 0;
	virtual IVistaNodeData*     NewExtensionNodeData() = 0;
	virtual IVistaNodeData*     NewTransformNodeData() = 0;

	// TEXT INTERFACES
	virtual IVistaNodeData*     NewTextNodeData(IVista3DText *) = 0;

	//###################################################


	//###################################################
	//General purpose functions
	//###################################################
	virtual bool	SetName			(const std::string &, IVistaNodeData*) =0;
	virtual bool    GetName         (std::string &sName, IVistaNodeData*) const = 0 ;
	virtual bool    GetIsEnabled    (IVistaNodeData*) const =0;
	virtual void    SetIsEnabled    (bool, IVistaNodeData*) =0;

	//###################################################
	// deprecated
	virtual bool	Enable			(IVistaNodeData*) =0;
	virtual bool	Disable			(IVistaNodeData*) =0;
	virtual bool	IsEnabled		(IVistaNodeData*) const =0;
	//###################################################

	virtual bool	GetBoundingBox	(VistaVector3D& pMin, VistaVector3D& pMax, IVistaNodeData*) const =0 ;

	//###################################################

	//###################################################
	//Transformation functions
	//###################################################
	virtual bool	GetTranslation		(VistaVector3D &, IVistaNodeData*) const =0;
	virtual bool	SetTranslation		(const VistaVector3D &, IVistaNodeData*) =0;
	virtual bool	GetRotation			(VistaTransformMatrix &, IVistaNodeData*) const =0;
	virtual bool	SetRotation			(const VistaTransformMatrix &, IVistaNodeData*) =0;
	virtual bool	GetRotation			(VistaQuaternion &, IVistaNodeData*) const =0;
	virtual bool	SetRotation			(const VistaQuaternion &, IVistaNodeData*)=0;
	virtual bool	SetTransform		(const VistaTransformMatrix & trans, IVistaNodeData*)=0;
	virtual bool	GetTransform		(VistaTransformMatrix &, IVistaNodeData*) const =0;

	virtual bool	Rotate				(float x, float y, float z, IVistaNodeData*)=0;
	virtual bool	Rotate				(const VistaQuaternion &, IVistaNodeData*)=0;
	virtual bool	Translate			(const VistaVector3D &, IVistaNodeData*)=0;

	virtual bool	GetWorldTransform	(VistaTransformMatrix &, IVistaNodeData*)const =0;
	virtual bool  	GetWorldPosition	(VistaVector3D &, IVistaNodeData*) const =0;
	virtual bool  	GetWorldOrientation	(VistaQuaternion &, IVistaNodeData*) const =0;
	//###################################################

	//###################################################
	// SPECIAL NODE FUNCTIONALITY
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	 //GroupNode functions
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual bool	CanAddChild			(IVistaNodeData* pChildData, IVistaNodeData*) const =0;
	virtual bool	AddChild			(IVistaNodeData* pChildData, IVistaNodeData*) = 0;
	virtual bool	InsertChild			(IVistaNodeData* pChildData, int index, IVistaNodeData*) = 0;
	virtual bool	DisconnectChild		(int nChildindex, IVistaNodeData*) = 0;


	//++++++++++++++++++++++++++++++++++++++++++++++++++
	 //SwitchNode functions
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual bool	SetActiveChild	(int i, IVistaNodeData*) = 0;
	virtual int		GetActiveChild	(IVistaNodeData*) const  = 0;

	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	 // LOD Node functions (Level Of Detail)
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual bool SetRange(const std::vector<float> &rangeList,IVistaNodeData*) = 0;
	virtual bool GetRange(std::vector<float> &rangeList,IVistaNodeData*) const = 0;
	virtual bool SetCenter(const VistaVector3D &center,IVistaNodeData*) = 0;
	virtual bool GetCenter(VistaVector3D& center,IVistaNodeData*) const = 0;

	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	 // LightNode functions
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual bool    SetLightColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*)     = 0;
	virtual bool	SetLightAmbientColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*)=0;
	virtual bool	SetLightDiffuseColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*)=0;
	virtual bool	SetLightSpecularColor(VISTA_LIGHTTYPE t, float, float, float, IVistaNodeData*)=0;

	virtual bool	SetLightAttenuation(VISTA_LIGHTTYPE t, const VistaVector3D&, IVistaNodeData*)=0;

	virtual bool	SetLightPosition(VISTA_LIGHTTYPE t, const VistaVector3D&, IVistaNodeData*)=0;
	virtual bool	SetLightDirection(VISTA_LIGHTTYPE t, const VistaVector3D&, IVistaNodeData*)=0;


	virtual bool	GetLightAmbientColor(VISTA_LIGHTTYPE t, float&, float&, float&, IVistaNodeData*)=0;
	virtual bool	GetLightDiffuseColor(VISTA_LIGHTTYPE t, float&, float&, float&, IVistaNodeData*)=0;
	virtual bool	GetLightSpecularColor(VISTA_LIGHTTYPE t, float&, float&, float&, IVistaNodeData*)=0;

	virtual bool	GetLightAttenuation	(VISTA_LIGHTTYPE t, VistaVector3D&, IVistaNodeData*)=0;

	virtual bool	GetLightPosition(VISTA_LIGHTTYPE t, VistaVector3D&, IVistaNodeData*) const = 0;
	virtual bool	GetLightDirection(VISTA_LIGHTTYPE t, VistaVector3D&, IVistaNodeData*)=0;

	virtual float	GetLightIntensity		(IVistaNodeData*) const  = 0;
	virtual bool	SetLightIntensity		(float, IVistaNodeData*) = 0;

	virtual bool    SetSpotCharacter(float eCharacter, IVistaNodeData *) = 0;
	virtual float   GetSpotCharacter(IVistaNodeData *) const = 0;

	virtual bool    SetSpotDistribution(int nExponent, IVistaNodeData *) = 0;
	virtual int     GetSpotDistribution(IVistaNodeData *) = 0;

	virtual bool    GetLightIsEnabled(IVistaNodeData*) const = 0;
	virtual void    SetLightIsEnabled(bool bEnabled, IVistaNodeData*) = 0;

	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	 // 3D-TEXT NODES
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual IVista3DText *CreateFontRepresentation(const std::string &sFontName)= 0;
	virtual bool          DestroyFontRepresentation(IVista3DText *) = 0;

	// ##################################################################################

	//little helpers
	//constructor functions -> circumvent <friend class Vista(Implementation)NodeBridge> in VistaNodes
	VistaNode*			NewNode			(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaGroupNode*	NewGroupNode	(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);


	VistaSwitchNode*	NewSwitchNode	(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaLODNode*	    NewLODNode	    (VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaGeomNode*		NewGeomNode		(VistaGroupNode* pParent,
										 VistaGeometry* pGeom,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaAmbientLight*	NewAmbientLight	(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaDirectionalLight*	NewDirectionalLight	(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaPointLight*	NewPointLight	(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaSpotLight*	NewSpotLight	(VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &strName);

	VistaTextNode *    NewTextNode     (VistaGroupNode* pParent,
										 IVista3DText *,
										 IVistaNodeData* pData,
										 const std::string &sName);

	VistaTransformNode* NewTransformNode     (VistaGroupNode* pParent,
										 IVistaNodeData* pData,
										 const std::string &sName);

	virtual void	     DeleteNode		(IVistaNode* pNode);


	// General purpose "callback" node
	VistaExtensionNode *NewExtensionNode(VistaGroupNode *pParent,
										  IVistaExplicitCallbackInterface *pExtension,
										  IVistaNodeData *pData,
										  const std::string &sName);


	//OpenGLNode functions
	VistaOpenGLNode*	NewOpenGLNode	(VistaGroupNode* pParent,
										 IVistaOpenGLDraw* pDi,
										 IVistaNodeData* pData,
										 const std::string &strName);

	// ##################################################################################

	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	 // OPENGL CALLBACK NODES
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual bool    InitExtensionNode(VistaExtensionNode *pNode, IVistaNodeData*) = 0;

	virtual bool	InitOpenGLNode	(IVistaNodeData*, VistaOpenGLNode* )=0;

	virtual void    ClearGeomNodeData(VistaGeomNode *) const = 0;

	//###################################################

	//###################################################
	// Storage functions
	//###################################################
	enum eOptFlags
	{
		OPT_NONE = 0,

		OPT_GEOMETRY_LOW = 1,
		OPT_GEOMETRY_MID = 2,
		OPT_GEOMETRY_HIGH = 4,

		OPT_MEMORY_LOW = 8,
		OPT_MEMORY_HIGH = 16,

		OPT_CULLING = 32,

		OPT_BEST_EFFORT = 256
	};

	virtual IVistaNode* LoadNode(const std::string &strFileName,
								 float fScale = 1.0f,
								 eOptFlags = OPT_NONE)=0;

	virtual bool		SaveNode(std::string strFileName, VistaNode* pNode)=0;

	virtual bool        ApplyOptimizationToNode(IVistaNode *pNode,
											 eOptFlags eFlags ) = 0;


	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	 // GEOMETRY
	 //++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual VistaGeometry* NewGeometry(IVistaGeometryData* pData);
	virtual bool	SetGeometry(IVistaGeometryData*, IVistaNodeData*)=0;


	// #################################################################################
	// NODE BRIDGE INTERNAL STATE FUNCTIONS
	// #################################################################################
	/**
	 *
	 */
	/** @todo handle this as an observer? -> think about this! */
	virtual int  RegisterGeometry(VistaGeometry *pGeom);

	/** unregister geometry  - gets deleted if refcount goes down to zero
	 * returns true if pGeom has been deleted, false, if it still is alive
	 */
	virtual bool UnregisterGeometry(VistaGeometry *pGeom);

protected:
	IVistaNodeBridge();
private:
	typedef std::map<VistaGeometry*, int> REFMAP;
	REFMAP m_mpGeomRefs;
	VistaSG* m_pSG;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
// ============================================================================
// ============================================================================

#endif

