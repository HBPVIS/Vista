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

#ifndef _VISTANNEWGRAPHICSMANAGER_H
#define _VISTANNEWGRAPHICSMANAGER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsBridge.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSG;
class IVistaNodeBridge;
class VistaGroupNode;
class VistaLightNode;
class VistaVector3D;
class VistaQuaternion;
class VistaTransformMatrix;
class VistaEventObserver;

class VistaEventManager;
class VistaDisplayManager;


// prototypes
class   VistaGraphicsManager;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaGraphicsManager & );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaGraphicsManager : public VistaEventHandler
{
public:
	VistaGraphicsManager(VistaEventManager *pEvMgr);
	virtual ~VistaGraphicsManager();


	bool InitScene(IVistaNodeBridge* pNodeBridge,
	               IVistaGraphicsBridge* pGraphicsBridge,
	                const std::string &strIniFile);

	VistaDisplayManager *GetDisplayManager() const;
	void                  SetDisplayManager(VistaDisplayManager *pDispManager);

	//** access functions
	VistaSG* GetSceneGraph() const;

	float GetFrameRate() const;
	unsigned int GetFrameCount() const;

#if 0
	//** transformation functions
	/** Move the whole scene by pTrans. This call has the same effect as
	* GetSceneGraph()->GetRoot()->Translate(pTrans)
	* @todo fix const-cast here
	*/
	bool MoveScene(const VistaVector3D & pTrans);


	/** Rotate the whole scene by pRot. This call has the same effect as
	*	GetSceneGraph()->GetRoot()->Rotate(pRot)
	*/
	bool RotateScene(const VistaQuaternion &pRot);


	/** Transform the whole scene by pMat. This call has the same effect as
	*	GetSceneGraph()->GetRoot()->SetTransform(pMat)
	* @todo fix const cast here
	*/
	bool TransformScene(const VistaTransformMatrix &pMat);

	/** Get the RCS -> WCS Transformation
	*/
	VistaTransformMatrix GetRealToWorldTransform() const;

	/** Get the WCS -> RCS Transformation
	*/
	VistaTransformMatrix GetWorldToRealTransform() const;

	/** Transform the given position and orientation from the real coordinate system to
	*   the world coordinate system
	*/
	void TransformToWorld(const VistaVector3D &posIn, const VistaQuaternion &orientIn,
	                      VistaVector3D &posOut, VistaQuaternion &orientOut) const;

	/** Transform the given position and orientation from the world coordinate system to
	*   the real coordinate system
	*/
	void TransformToReal(const VistaVector3D &posIn, const VistaQuaternion &orientIn,
	                     VistaVector3D &posOut, VistaQuaternion &orientOut) const;

#endif

	//** misc
	/** Reset default scene state (yet to be specified)
	* @todo specify default scene-state
	*/
	bool            ResetScene();
	void            Debug ( std::ostream & out ) const;

	bool            GetShowInfo() const;
	void            SetShowInfo(bool bInfo);
	/**
	 * Get/set background color
	 */
	VistaColorRGB  GetBackgroundColor() const;
	void            SetBackgroundColor(const VistaColorRGB & color);

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


	IVistaGraphicsBridge*   GetGraphicsBridge() const;
	IVistaNodeBridge*       GetNodeBridge() const;

	/**
	 * Handle graphics events, which haven't been handled by the application.
	 *
	 * @param    VistaEvent *pEvent
	 *
	 */
	virtual void HandleEvent(VistaEvent *pEvent);

	static bool RegisterEventTypes(VistaEventManager*);

	void CreateDefaultLights();
protected:

	VistaSG*               m_pSceneGraph;
	VistaGroupNode*        m_pModelRoot;
	IVistaGraphicsBridge*   m_pGraphicsBridge;
	IVistaNodeBridge*       m_pNodeBridge;
private:
	bool                    m_bShowInfo;
	VistaEventObserver    *m_pFrameRateText;
	std::string             m_strIniFile;
	VistaDisplayManager   *m_pDispMgr;
	VistaEventManager     *m_pEvMgr;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
inline float VistaGraphicsManager::GetFrameRate() const
{
	return m_pGraphicsBridge->GetFrameRate();
}
inline unsigned int VistaGraphicsManager::GetFrameCount() const
{
	return m_pGraphicsBridge->GetFrameCount();
}
#endif //_VISTANNEWGRAPHICSMANAGER_H

