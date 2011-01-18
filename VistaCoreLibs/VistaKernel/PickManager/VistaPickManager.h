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

#ifndef _VISTAPICKMANAGER_H
#define _VISTAPICKMANAGER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <iostream>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <VistaBase/VistaVectorMath.h>
#include <VistaMath/VistaGeometries.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
class VistaTransformNode;
class VistaGroupNode;

class VistaPickable;
class VistaPickEvent;

class VistaEventManager;
class VistaOldInteractionManager;


// prototypes
class   VistaPickManager;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaPickManager & );

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class implements a pick manager, which allows registration and unregistration
 * of scene graph nodes for being touched and picked. Touching and picking can be
 * done with multiple input devices, which are to be registered explicitly as well.
 *
 * To use it several steps have to be taken:
 * 1.: Register an input device. Or several, if you like...
 * 1.a.: (Optionally) Retrieve this devices pick manager data with GetDeviceData()
 *       and change some settings.
 * 2.: Register scene graph nodes to be picked. If desired, provide a VistaPickable
 *     object for direct notifications.
 * 2.a.: (Optionally) Modify their behavior after retrieving it with GetPickBehavior().
 *       Important flags are e.g. CPickBehavior::m_bBlockTargetRay,
 *       CPickBehavior::m_bEventOnTouch, CPickBehavior::m_bEventOnPick.
 * 3.: React on
 *     a: touch and pick events or
 *     b: touch and pick callbacks
 *
 */
class VISTAKERNELAPI VistaPickManager
{
public:
	/**
	 * This class defines the behavior of a single objects to be touched and/or picked.
	 *
	 */
	class VISTAKERNELAPI CPickBehavior
	{
	public:
		CPickBehavior();

		/**
		 * This is a pointer to a VistaPickable object, which
		 * (if present) will be called in case of the respective object being picked/touched.
		 * The default value is NULL.
		 */
		VistaPickable	*m_pPickable;

		/**
		 * This flag defines, whether the respective object blocks
		 * the target ray or not. For group nodes this should obviously be set to false,
		 * to allow for this node's children to be picked/touched. The default value is true.
		 */
		bool			m_bBlockTargetRay;

		/**
		 * Should the objects bounding box be used for contact determination (default), or
		 * should the object be queried through VistaPickable::QueryForContact()?
		 */
		bool			m_bQueryForContact;

		/**
		 * Should an event be fired, if the object is picked? Default is false.
		 */
		bool			m_bEventOnPick;

		/**
		 * Should events be fired, if the contacts of
		 * this node change? Default is false.
		 */
		bool			m_bEventOnTouch;
	};

	class VISTAKERNELAPI CPickQuery
	{
	public:
		CPickQuery();
		~CPickQuery();

		VistaRay m_oQueryRay;
	};

	/**
	 * This class manages data for a single input device being registerd for touching
	 * and picking.
	 */
	class VISTAKERNELAPI CPickData
	{
	public:
		CPickData();

		/**
		 * Control device status, i.e. whether the respective device can be used for
		 * picking and/or touching.
		 */
		void	Activate();
		void	Deactivate();
		bool	IsActive() const;

		/**
		 * Control picking status, i.e. whether the respective device will be used
		 * for picking during the next pick manager update (which is scheduled
		 * between input device update and pre-draw event). After picking, it
		 * will be deactivated again.
		 */
		void	ActivatePicking();
		void	DeactivatePicking();
		bool	IsPicking() const;

		/**
		 * Control the range of the input device. This not used, yet
		 */
		float	GetRange() const;
		void	SetRange(float fRange);

	protected:
		float              m_fRange;
//		float              m_fLastUpdate;
		bool               m_bPick;
		bool               m_bActive;

		friend class VistaPickManager;
	};

	/**
	 * A simple container for contact information.
	 */
	struct VISTAKERNELAPI SContactData
	{
	public:
		SContactData();

		/**
		 * A pointer to the query, which has contact.
		 */
		CPickQuery *pPickQuery;

		/**
		 * The contact position in world space.
		 */
		VistaVector3D v3Position;

		/**
		 * The distance from the input device to
		 * the contact position.
		 * A value of -1 indicates that there is
		 * no contact. This happens whenever a
		 * touch event is sent for an object,
		 * which is not touched anymore (i.e.
		 * a kind of "untouch" event).
		 */
		float fDistance;
	};

public:
	VistaPickManager(VistaEventManager *pEvMa);
	virtual ~VistaPickManager();

	bool Init(VistaGroupNode *pRoot);

	/**
	 * Activate and deactivate the pick manager.
	 */
	void Activate();
	void Deactivate();
	bool IsActive() const;

	/**
	 * Manage input devices to be used for picking.
	 * Note, that for repeated activating and deactivating
	 * of a given input device its CPickData object should
	 * be used.
	 */
	bool RegisterQuery(CPickQuery *pPickQuery);
	bool UnregisterQuery(CPickQuery *pPickQuery);

	/**
	 * Retrieve the CPickData class for a given input device,
	 * which can then be used for further control, like activating
	 * and deactivating the device and kick off picking.
	 */
	CPickData *GetQueryData(CPickQuery *pDevice);
//	bool Pick(VistaInputDevice *pDevice);

	/**
	 * Register a scene graph node for touching/picking. An optional
	 * VistaPickable object can be given, whose callbacks for
	 * touching, picking, and (optionally) querying will be used.
	 */
	bool RegisterNode(IVistaNode *pNode, VistaPickable *pPickable = NULL);
	bool UnregisterNode(IVistaNode *pNode);

	/**
	 * Control a node's activation and deactivation. If a group node is deactivated
	 * all its children won't be regarded for contacts as well. Note, that the nodes
	 * given here don't necessarily have to be registered before, if they have been
	 * inserted into the internal pick tree before. E.g. when adding two nodes with
	 * a common parent, this parent is likely to be automatically inserted into
	 * the pick tree as well. Of course, you could (or better: should) insert the
	 * common parent manually, if you depend on having it in the pick tree. Note,
	 * that in this case you will have to set this node's CPickBehavior::m_bBlockTargetRay
	 * flag to false!!!
	 */
	bool ActivateNode(IVistaNode *pNode);
	bool DeactivateNode(IVistaNode *pNode);
	bool IsNodeActive(IVistaNode *pNode);

	/**
	 * Retrieve contact information for a given node.
	 */
	bool GetContacts(IVistaNode *pNode, std::list<SContactData> &liContacts);

	/**
	 * Retrieve the pick behavior for a given node. This allows for finer control
	 * of the node's behavior and possible event creation.
	 */
	CPickBehavior *GetPickBehavior(IVistaNode *pNode);

	/**
	 * Dump information about the pick managers internal state
	 * (and the internal pick tree).
	 */
	void Debug ( std::ostream & out ) const;
	void DumpPickTree ( std::ostream & out) const;

	/**
	 * Rebuild the pick tree. This method should be called after the removal of
	 * single nodes from the pick manager to get rid of unnecessary nodes in
	 * the internal pick tree.
	 */
	void RebuildPickTree();

	/**
	 * This method is called by the system on reception of a UPDATE_PICKING event
	 * and determines contacts between interaction devices (and their target rays)
	 * and scene graph nodes. If desired, the objects will be notified of the
	 * respective events and the application and the rest of the system are
	 * notified through VistaPickEvents.
	 */
	void UpdatePicking();

//protected:
	/**
	 * This is an interal data structure to provide an efficient means of
	 * determining hierarchical contact information.
	 */
	struct VISTAKERNELAPI SPickTreeNode
	{
		SPickTreeNode *pParent;
		std::list<SPickTreeNode *> liChildren;
		IVistaNode *pNode;
		CPickBehavior *pBehavior;
		std::list<SContactData *> *pliContacts;
		std::list<SContactData *> *pliPrevContacts;
		bool bHasTouchedChildren;
		bool bActive;

		SPickTreeNode(IVistaNode *pVistaNode);
		~SPickTreeNode();
	};

	/**
	 * This is an internal data structure to queue information about picked
	 * objects.
	 */
	struct VISTAKERNELAPI SPickData
	{
		SPickData()
		{}

		SPickData(SPickTreeNode *pNewNode, SContactData *pNewContact)
			: pNode(pNewNode), pContact(pNewContact)
		{}

		SPickTreeNode *pNode;
		SContactData *pContact;
	};

	static bool RegisterEventTypes(VistaEventManager *);
protected:
	void BuildStackOfParents(IVistaNode *pNode,
							 IVistaNode *pParent,
							 std::stack<IVistaNode *> &stParents);
	void DumpNode(std::ostream & out, SPickTreeNode *pNode, int iLevel) const;
	void ClearTreeNode(SPickTreeNode *pNode);
	float DistanceToBoundingBox(IVistaNode *pNode,
								VistaVector3D &v3Pos,
								VistaVector3D &v3Dir);

	SPickTreeNode *FindNodeInPickTree(IVistaNode *pNode);
	bool InsertNewPredecessor(IVistaNode *pPredecessor,
							  SPickTreeNode *pNewNode,
							  SPickTreeNode *pOldNode);
	bool InsertPickTreeNode(SPickTreeNode *pNewNode,
							SPickTreeNode *pSubtreeParent);

	void RemoveContactInformation(SPickTreeNode *pNode);
	void AgeContactInformation(SPickTreeNode *pNode);
	bool UpdatePickTreeNode(SPickTreeNode *pTo, SPickTreeNode *pFrom);
	void UpdateContacts(CPickQuery *pPickQuery, CPickData *pData);

	std::map<CPickQuery *, CPickData *> m_mapQueries;
	std::queue<SPickData *>                      m_qPicks;

	SPickTreeNode			    *m_pPickTreeRoot;
	VistaGroupNode			*m_pVistaRoot;

	VistaPickEvent             *m_pPickEvent;
	VistaEventManager          *m_pEventManager;

	bool	m_bActive;
	bool	m_bDirty;	// do we have to update the pick tree for a last time?
};

/*============================================================================*/
/* INLINE METHODS                                                             */
/*============================================================================*/

inline bool	VistaPickManager::CPickData::IsActive() const
{
	return m_bActive;
}

inline bool	VistaPickManager::CPickData::IsPicking() const
{
	return m_bPick;
}

inline float VistaPickManager::CPickData::GetRange() const
{
	return m_fRange;
}

inline bool VistaPickManager::IsActive() const
{
	return m_bActive;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAPICKMANAGER_H
