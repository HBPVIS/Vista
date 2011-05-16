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

#include "VistaPickManager.h"
#include "VistaPickable.h"

#include "../GraphicsManager/VistaTransformNode.h"

#include "../EventManager/VistaPickEvent.h"
#include "../EventManager/VistaEventManager.h"

#include <VistaKernel/VistaKernelOut.h>

#include <cassert>
#include <queue>

#ifdef DEBUG
#define TALKATIVE
#endif

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
typedef std::map<VistaPickManager::CPickQuery *, VistaPickManager::CPickData *> PICK_DATA_MAP;

#ifdef LINUX
const float LOCAL_INFINITY = 1337e42;
#else
const float LOCAL_INFINITY = numeric_limits<float>::infinity();
#endif

bool VistaPickManager::RegisterEventTypes(VistaEventManager *pEventMgr)
{
	
	VistaEventManager::EVENTTYPE eTp = pEventMgr->RegisterEventType("VET_PICK");
	//pEventMgr->MapEventType(eTp, VistaEvent::VET_PICK);
	VistaPickEvent::SetTypeId(eTp);

	for(int n = VistaPickEvent::VPE_PICKED; n < VistaPickEvent::VPE_UPPER_BOUND; ++n)
		pEventMgr->RegisterEventId(eTp, VistaPickEvent::GetIdString(n));

	return true;
}
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaPickManager::VistaPickManager(VistaEventManager *pEvMa)
: m_pEventManager(pEvMa)
, m_pVistaRoot(NULL)
, m_bActive(false)
, m_bDirty(true)
, m_pPickTreeRoot(NULL)
, m_pPickEvent(NULL)
{
#ifdef TALKATIVE
	vkernout << "*** [VistaPickManager] >> CONSTRUCTOR <<" << endl;
#endif
}

VistaPickManager::~VistaPickManager()
{
#ifdef TALKATIVE
	vkernout << " [VistaPickManager] >> DESTRUCTOR <<" << endl;
#endif

	// first of all: destroy all pick data
	PICK_DATA_MAP::iterator itPickData = m_mapQueries.begin();
	while (itPickData != m_mapQueries.end())
	{
		delete itPickData->second;
		itPickData->second = NULL;
		++itPickData;
	}

	// go for pick tree data
	// (a): remove all contact data
	if (m_pPickTreeRoot)
		RemoveContactInformation(m_pPickTreeRoot);

	// (b): remove the pick tree itself
	if (m_pPickTreeRoot)
		ClearTreeNode(m_pPickTreeRoot);

	delete m_pPickEvent;
	m_pPickEvent = NULL;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   BuildStackOfParents                                         */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::BuildStackOfParents(IVistaNode *pNode, 
											IVistaNode *pParent, 
											std::stack<IVistaNode *> &stParents)
{
	// we assert that pNode is valid and has the given parent node as a predecessor
	IVistaNode *pPos = pNode;
	while (pPos != pParent)
	{
		stParents.push(pPos);
		pPos = pPos->GetParent();
	}

	stParents.push(pPos);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ClearTreeNode                                               */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::ClearTreeNode(VistaPickManager::SPickTreeNode *pNode)
{
	// first of all: recursively clean up all children
	list<SPickTreeNode *>::iterator itChild = pNode->liChildren.begin();
	while (itChild != pNode->liChildren.end())
	{
		ClearTreeNode(*itChild);
		++itChild;
	}
	pNode->liChildren.clear();

	delete pNode->pBehavior;
	pNode->pBehavior = NULL;

	delete pNode;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DistanceToBoundingBox                                       */
/*                                                                            */
/*============================================================================*/
float VistaPickManager::DistanceToBoundingBox(IVistaNode *pNode, 
											   VistaVector3D &v3Pos, 
											   VistaVector3D &v3Dir)
{
	VistaTransformMatrix m4Xform, m4XformInv;
	
	pNode->GetWorldTransform(m4Xform);
	m4Xform.GetInverted(m4XformInv);

	// transform the pick ray into the nodes local frame
	VistaVector3D v3PosLocal = m4XformInv * v3Pos;
	VistaVector3D v3DirLocal = m4XformInv * v3Dir;

	VistaVector3D v3Min, v3Max;

	// get local bounding box
	pNode->GetBoundingBox(v3Min, v3Max);

	float fLambdaMin = -LOCAL_INFINITY;
	float fLambdaMax = LOCAL_INFINITY;
	float fTempMin = fLambdaMin;
	float fTempMax = fLambdaMax;
	bool bOutside = false;

	if (v3DirLocal[0] > numeric_limits<float>::epsilon())
	{
		fLambdaMin = (v3Min[0] - v3PosLocal[0]) / v3DirLocal[0];
		fLambdaMax = (v3Max[0] - v3PosLocal[0]) / v3DirLocal[0];
	}
	else if (v3DirLocal[0] < -numeric_limits<float>::epsilon())
	{
		fTempMin = fLambdaMin = (v3Max[0] - v3PosLocal[0]) / v3DirLocal[0];
		fTempMax = fLambdaMax = (v3Min[0] - v3PosLocal[0]) / v3DirLocal[0];
	}
	else // v3DirLocal[0] == 0
	{
		if (v3PosLocal[0] < v3Min[0] 
			|| v3PosLocal[0] > v3Max[0])
			bOutside = true;
	}

	if (!bOutside)
	{
		if (v3DirLocal[1] > numeric_limits<float>::epsilon())
		{
			fTempMin = (v3Min[1] - v3PosLocal[1]) / v3DirLocal[1];
			fTempMax = (v3Max[1] - v3PosLocal[1]) / v3DirLocal[1];
		}
		else if (v3DirLocal[1] < -numeric_limits<float>::epsilon())
		{
			fTempMin = (v3Max[1] - v3PosLocal[1]) / v3DirLocal[1];
			fTempMax = (v3Min[1] - v3PosLocal[1]) / v3DirLocal[1];
		}
		else // v3DirLocal[1] == 0
		{
			if (v3PosLocal[1] < v3Min[1] 
				|| v3PosLocal[1] > v3Max[1])
				bOutside = true;
		}
	}

	if (!bOutside)
	{
		if (fLambdaMin < fTempMin)
			fLambdaMin = fTempMin;
		if (fLambdaMax > fTempMax)
			fLambdaMax = fTempMax;

		if (v3DirLocal[2] > numeric_limits<float>::epsilon())
		{
			fTempMin = (v3Min[2] - v3PosLocal[2]) / v3DirLocal[2];
			fTempMax = (v3Max[2] - v3PosLocal[2]) / v3DirLocal[2];
		}
		else if (v3DirLocal[2] < -numeric_limits<float>::epsilon())
		{
			fTempMin = (v3Max[2] - v3PosLocal[2]) / v3DirLocal[2];
			fTempMax = (v3Min[2] - v3PosLocal[2]) / v3DirLocal[2];
		}
		else // v3DirLocal[2] == 0
		{
			if (v3PosLocal[2] < v3Min[2] 
				|| v3PosLocal[2] > v3Max[2])
				bOutside = true;
		}
	}

	if (!bOutside)
	{
		if (fLambdaMin < fTempMin)
			fLambdaMin = fTempMin;
		if (fLambdaMax > fTempMax)
			fLambdaMax = fTempMax;

		bOutside = (fLambdaMax < 0) || (fLambdaMin > fLambdaMax);
	}

	if (bOutside)
		return -1.0f;
	else
		return fLambdaMin<0 ? 0 : fLambdaMin;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DumpNode                                                    */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::DumpNode(std::ostream &out, 
								 VistaPickManager::SPickTreeNode *pNode, 
								 int iLevel) const
{
	out << " [ViPickMan] - ";
	
	int i;
	for (i=0; i<iLevel; ++i)
	{
		out << "    ";
	}
	out << "|--";
	out << "Level " << iLevel << ":" ;

	out << pNode->pNode->GetName() << " [" << pNode->pNode << "]:";
	if (pNode->bActive)
		out << "active";
	else
		out << "inactive";
	out << ":";
	if (pNode->pBehavior)
		out << "registered";
	else
		out << "internal";
	out << ":" << pNode->pliContacts->size() << " contacts" << endl;

	list<SPickTreeNode *>::const_iterator itNode = pNode->liChildren.begin();
	while (itNode != pNode->liChildren.end())
	{
		SPickTreeNode *pChild = *itNode;
		DumpNode(out, pChild, iLevel+1);
		++itNode;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   FindNodeInPickTree                                          */
/*                                                                            */
/*============================================================================*/
VistaPickManager::SPickTreeNode *VistaPickManager::FindNodeInPickTree(IVistaNode *pNode)
{
#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] trying to find node in pick tree..." << endl;
#endif

	if (!pNode)
		return NULL;

	SPickTreeNode *pCurrentNode = m_pPickTreeRoot;
	stack<IVistaNode *> stNodeParents, stRootParents;
	BuildStackOfParents(pNode, m_pVistaRoot, stNodeParents);
	BuildStackOfParents(pCurrentNode->pNode, m_pVistaRoot, stRootParents);

	while (!stNodeParents.empty() && !stRootParents.empty()
		&& stNodeParents.top() == stRootParents.top())
	{
		stNodeParents.pop();
		stRootParents.pop();
	}

	while (!stNodeParents.empty())
	{
		IVistaNode *pTop = stNodeParents.top();
		list<SPickTreeNode *>::iterator itChild = pCurrentNode->liChildren.begin();

		while (itChild != pCurrentNode->liChildren.end()
			&& pTop != (*itChild)->pNode)
		{
			++itChild;
		}

		if (itChild != pCurrentNode->liChildren.end())
			pCurrentNode = *itChild;

		stNodeParents.pop();
	}

	if (pCurrentNode->pNode == pNode)
		return pCurrentNode;
	else
		return NULL;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   InsertNewPredecessor                                        */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::InsertNewPredecessor(IVistaNode *pPredecessor,
											 VistaPickManager::SPickTreeNode *pNewNode,
											 VistaPickManager::SPickTreeNode *pOldNode)
{
	SPickTreeNode *pCommon = new SPickTreeNode(pPredecessor);
	pCommon->pParent = pOldNode->pParent;
	pCommon->liChildren.push_back(pOldNode);
	pCommon->liChildren.push_back(pNewNode);
	pOldNode->pParent = pCommon;
	pNewNode->pParent = pCommon;

	if (pCommon->pParent)
	{
		pCommon->pParent->liChildren.remove(pOldNode);
		pCommon->pParent->liChildren.push_back(pCommon);
	}
	else
	{
		// the common predecessor is the new pick tree root
		m_pPickTreeRoot = pCommon;
	}

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   InsertPickTreeNode                                          */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::InsertPickTreeNode(VistaPickManager::SPickTreeNode *pNewNode,
										   VistaPickManager::SPickTreeNode *pSubtree)
{
//	SPickTreeNode *pCurrentParent = NULL;
	stack<IVistaNode *> stNodeParents;
	stack<IVistaNode *> stCurrentParents;
	BuildStackOfParents(pNewNode->pNode, m_pVistaRoot, stNodeParents);
	BuildStackOfParents(pSubtree->pNode, m_pVistaRoot, stCurrentParents);
	IVistaNode *pCommonVistaParent = NULL;

	// check, whether we're a parent of the pick tree root
	while (!stNodeParents.empty() && !stCurrentParents.empty()
		&& stNodeParents.top()==stCurrentParents.top())
	{
		pCommonVistaParent = stNodeParents.top();
		stNodeParents.pop();
		stCurrentParents.pop();
	}

	assert(pCommonVistaParent && "no common parent for vista nodes to be found...");

	if (stNodeParents.empty())
	{
		// well, we could be a parent of the pick tree root...
		if (stCurrentParents.empty())
		{
			// oh, we ARE the pick tree root
#ifdef TALKATIVE
			vkernout << "*** [ViPickMan] node to be registered is already in pick tree" << endl;
			vkernout << "                replacing corresponding data..." << endl;
#endif
			return UpdatePickTreeNode(pSubtree, pNewNode);
		}

		// we are a parent of the pick tree root
		if (pSubtree->pParent)
		{
			pSubtree->pParent->liChildren.remove(pSubtree);
			pSubtree->pParent->liChildren.push_back(pNewNode);
		}
		else
		{
			m_pPickTreeRoot = pNewNode;
		}
		pSubtree->pParent = pNewNode;
		pNewNode->liChildren.push_back(pSubtree);


#ifdef TALKATIVE
		vkernout << "*** [ViPickMan] registering new node as parent of subtree..." << endl;
#endif
		return true;
	}

	if (stCurrentParents.empty())
	{
		// The pick tree root is our parent,
		// so start checking the children.
		// For the top of the new node's parents stack,
		// we're traversing the current node's children 
		// list until we run out of predecessors for the current node
		// (so we're a sibling of these children) or until
		// we find a matching child, in which case we continue with
		// its children.
		list<SPickTreeNode *> *pCurrentList = &(pSubtree->liChildren);
		list<SPickTreeNode *>::iterator itChild = pCurrentList->begin();
		SPickTreeNode *pCurrentParent = pSubtree;

		while (!stNodeParents.empty())
		{
			while (!stNodeParents.empty() && itChild != pCurrentList->end())
			{
				BuildStackOfParents((*itChild)->pNode, pCurrentParent->pNode, stCurrentParents);
				stCurrentParents.pop();

				while (!stNodeParents.empty()
					&& !stCurrentParents.empty()
					&& stCurrentParents.top()==stNodeParents.top())
				{
					// this child and the new node have common predecessors
					return InsertPickTreeNode(pNewNode, (*itChild));
				}

				++itChild;
			}

			// add new node as child of current parent
#ifdef TALKATIVE
			vkernout << "*** [ViPickMan] adding new node as child of existing node" << endl;
#endif
			pCurrentParent->liChildren.push_back(pNewNode);
			pNewNode->pParent = pCurrentParent;

			return true;
		}
	}

	// the new node and the current node have a common predecessor, 
	// which is not in the pick tree (yet ;-)
#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] creating new pick tree root and registering new node" << endl;
	vkernout << "                and former pick tree root as its children..." << endl;
#endif
	return InsertNewPredecessor(pCommonVistaParent, pNewNode, pSubtree);
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   RemoveContactInformation                                    */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::RemoveContactInformation(VistaPickManager::SPickTreeNode *pNode)
{
	if (!pNode)
		return;

	SPickTreeNode *pCurrent;
	stack <SPickTreeNode *> stNodes;
	stNodes.push(pNode);
	list<SContactData *>::iterator itContact;
	list<SPickTreeNode *>::iterator itChild;
	while (!stNodes.empty())
	{
		pCurrent = stNodes.top();
		stNodes.pop();

		// clear own contacts
		itContact = pCurrent->pliContacts->begin();
		while (itContact != pCurrent->pliContacts->end())
		{
			delete (*itContact);
			++itContact;
		}
		pCurrent->pliContacts->clear();

		// clear prev contacts
		itContact = pCurrent->pliPrevContacts->begin();
		while (itContact != pCurrent->pliPrevContacts->end())
		{
			delete (*itContact);
			++itContact;
		}
		pCurrent->pliContacts->clear();

		// schedule children for contact information removal
		if (pCurrent->bHasTouchedChildren)
		{
			itChild = pCurrent->liChildren.begin();
			while (itChild != pCurrent->liChildren.end())
			{
				stNodes.push(*itChild);
				++itChild;
			}
			pCurrent->bHasTouchedChildren = false;
		}
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   AgeContactInformation                                       */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::AgeContactInformation(VistaPickManager::SPickTreeNode *pNode)
{
	if (!pNode)
		return;

	SPickTreeNode *pCurrent;
	stack <SPickTreeNode *> stNodes;
	stNodes.push(pNode);
	list<SContactData *>::iterator itContact;
	list<SPickTreeNode *>::iterator itChild;
	list<SContactData *> *pSwapHelp;
	while (!stNodes.empty())
	{
		pCurrent = stNodes.top();
		stNodes.pop();

		// clear old contacts
		itContact = pCurrent->pliPrevContacts->begin();
		while (itContact != pCurrent->pliPrevContacts->end())
		{
			delete (*itContact);
			++itContact;
		}
		pCurrent->pliPrevContacts->clear();

		// swap contact lists
		pSwapHelp = pCurrent->pliContacts;
		pCurrent->pliContacts = pCurrent->pliPrevContacts;
		pCurrent->pliPrevContacts = pSwapHelp;

		// schedule children for contact information removal
		if (pCurrent->bHasTouchedChildren)
		{
			itChild = pCurrent->liChildren.begin();
			while (itChild != pCurrent->liChildren.end())
			{
				stNodes.push(*itChild);
				++itChild;
			}
			pCurrent->bHasTouchedChildren = false;
		}
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UpdateContacts                                              */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::UpdateContacts(CPickQuery *pPickQuery, 
									   VistaPickManager::CPickData *pData)
{
	if (!pData->IsActive())
	{
		pData->DeactivatePicking();  // just to make sure...
		return;
	}

	// we know, that the pick tree root is valid!!!
	stack<SPickTreeNode *> stNodes;
	stNodes.push(m_pPickTreeRoot);
	SPickTreeNode *pNode;

	VistaVector3D v3Pos(pPickQuery->m_oQueryRay.GetOrigin());

	//VistaQuaternion qDir();

	VistaVector3D v3Dir(pPickQuery->m_oQueryRay.GetDir());

	// we're talking about a "vector" here (as opposed to a "point")
	v3Dir[3] = 0;
	v3Dir.Normalize();

	assert(v3Dir[3] == 0 && "WARNING! Direction with w!=0!!!");

	// transform pos and dir into vista coord system
	VistaTransformMatrix m4Xform, m4XformInv;
/*
	m_pVistaRoot->GetTransform(m4Xform);
	m4Xform.GetInverted(m4XformInv);
	v3Pos = m4XformInv * v3Pos;
	v3Dir = m4XformInv * v3Dir;
*/
	// avoid expensive allocation of temporary variables
	list<SPickTreeNode *>::iterator itChild;
	SContactData *pContact=NULL;
	bool bContact=false;
	float fDistance, fClosestContact = LOCAL_INFINITY;

	while (!stNodes.empty())
	{
		pNode = stNodes.top();
		stNodes.pop();
		fDistance = -1;

		// check whether object has been touched
		if (pNode->pBehavior && pNode->pBehavior->m_bQueryForContact)
		{
			if (pNode->pBehavior->m_pPickable)
			{
				fDistance = pNode->pBehavior->m_pPickable->QueryForContact(v3Pos, v3Dir);
			}
		}
		else
		{
			fDistance = DistanceToBoundingBox(pNode->pNode, v3Pos, v3Dir);
		}

		if (fDistance >= 0 && fDistance <= fClosestContact)
		{
			if (pNode->pBehavior && pNode->pBehavior->m_bBlockTargetRay)
				fClosestContact = fDistance;

			pContact = new SContactData;
			pContact->fDistance = fDistance;
			pContact->pPickQuery = pPickQuery;
			pContact->v3Position = v3Pos + fDistance * v3Dir;
			pNode->pliContacts->push_back(pContact);
			bContact = true;
		}

		if (bContact)
		{
			if (pNode->pParent)
				pNode->pParent->bHasTouchedChildren = true;

			if (!pNode->liChildren.empty())
			{
				// check the children
				itChild = pNode->liChildren.begin();
				while (itChild != pNode->liChildren.end())
				{
					if ((*itChild)->bActive)
						stNodes.push(*itChild);
					++itChild;
				}
			}
		}
	}

	// remove contact information for nodes, which are too far away
	stNodes.push(m_pPickTreeRoot);
	list<SContactData *>::iterator itContact;
	SPickData *pPickData=NULL;
	while (!stNodes.empty())
	{
		pNode = stNodes.top();
		stNodes.pop();

		// scan through list of contacts,...
		itContact = pNode->pliContacts->begin();
		while (itContact != pNode->pliContacts->end())
		{
			pContact = *itContact;

			if (pContact->pPickQuery == pPickQuery)
			{
				if (pContact->fDistance > fClosestContact)
				{
					// ...remove those, which are too far away,...
					delete pContact;
					pNode->pliContacts->erase(itContact);
					break;
				}
				else
				{
					// ...and if we're picking, push appropriate information into the respective queue
					if (pNode->pBehavior && pData->IsPicking())
					{
						pPickData = new SPickData(pNode, *itContact);
						this->m_qPicks.push(pPickData);
					}
				}
			}
			++itContact;
		}

		if (pNode->bHasTouchedChildren)
		{
			itChild = pNode->liChildren.begin();
			while (itChild != pNode->liChildren.end())
			{
				stNodes.push(*itChild);
				++itChild;
			}
		}
	}

	// avoid further picking with this pick data...
	pData->DeactivatePicking();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UpdatePickTreeNode                                          */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::UpdatePickTreeNode(VistaPickManager::SPickTreeNode *pTo,
										   VistaPickManager::SPickTreeNode *pFrom)
{
	// check whether the node to be updated is a registered or internal node
	if (pTo->pBehavior)
	{
		// it's a registered one -> don't overwrite the old information
#ifdef TALKATIVE
		vkernout << "*** [ViPickMan] Warning! Tried to register an already present node..." << endl;
#endif
		return false;
	}

	// copy the appropriate data
	pTo->pBehavior = pFrom->pBehavior;
	pTo->bActive = pFrom->bActive;

	// free memory from "from" node
	delete pFrom;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::Init(VistaGroupNode *pRoot)
{
#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] starting initialization..." << endl;
#endif
	// do we already have the vista root node?
	// if so, we're already initialized
	if (m_pVistaRoot)
	{
		vkernout << " [VistaPickManager] WARNING! Already initialized..." << endl;
		return false;
	}

	m_pVistaRoot = pRoot;
	if (!m_pVistaRoot)
	{
		vkernout << " [VistaPickManager] unable to find ViSTA root node..." << endl;
		return false;
	}

	// create pick event
	m_pPickEvent = new VistaPickEvent;

	// make sure, we're working...
	m_bActive = true;
	m_bDirty = true;

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] initialization complete - alive and kicking..." << endl;
#endif

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Activate                                                    */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::Activate()
{
	m_bActive = true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Deactivate                                                  */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::Deactivate()
{
	m_bActive = false;
	m_bDirty = true;	// make sure everything gets deactivated
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   RegisterInputDevice                                         */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::RegisterQuery(CPickQuery *pPickQuery)
{
	if (!pPickQuery)
		return false;

	PICK_DATA_MAP::iterator itPickData = m_mapQueries.find(pPickQuery);

	if (itPickData != m_mapQueries.end())
		return false;

	CPickData *pData = new CPickData;
	m_mapQueries.insert(PICK_DATA_MAP::value_type(pPickQuery, pData));

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] registration of pick query successful..." << endl;
#endif

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UnregisterInputDevice                                       */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::UnregisterQuery(CPickQuery *pPickQuery)
{
	if (!pPickQuery)
		return false;

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] unregistering pick query..." << endl;
#endif

	PICK_DATA_MAP::iterator itPickData = m_mapQueries.find(pPickQuery);

	if (itPickData != m_mapQueries.end())
	{
		// remove device data
		delete itPickData->second;
		m_mapQueries.erase(itPickData);

		return true;
	}

	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDeviceData                                               */
/*                                                                            */
/*============================================================================*/
VistaPickManager::CPickData *VistaPickManager::GetQueryData(CPickQuery *pDevice)
{
	PICK_DATA_MAP::iterator itPickData = m_mapQueries.find(pDevice);

	if (itPickData != m_mapQueries.end())
	{
		return itPickData->second;
	}

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   RegisterNode                                                */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::RegisterNode(IVistaNode *pNode, VistaPickable *pPickable)
{
	if (!pNode)
		return false;

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] trying to register node '" << pNode->GetName() << "'" << endl;
#endif

	// create data for pick tree node
	SPickTreeNode *pTreeNode = new SPickTreeNode(pNode);
	pTreeNode->pBehavior = new CPickBehavior;
	pTreeNode->pBehavior->m_pPickable = pPickable;

	if (!m_pPickTreeRoot)
	{
		// this is the first node to be inserted,
		// so make it the pick tree root
		m_pPickTreeRoot = pTreeNode;

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] registration of '" << pNode->GetName() << "' successful..." << endl;
#endif
		return true;
	}

	// otherwise, insert node into existing pick tree
	return InsertPickTreeNode(pTreeNode, m_pPickTreeRoot);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UnregisterNode                                              */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::UnregisterNode(IVistaNode *pNode)
{
	if (!pNode)
	{
#ifdef TALKATIVE
		vkernout << "*** [ViPickMan] unregistration of NULL failed (for obvious reasons)..." << endl;
#endif
		return false;
	}

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] trying to unregister node '" << pNode->GetName() << "'" << endl;
#endif


	SPickTreeNode *pTreeNode = FindNodeInPickTree(pNode);

	if (!pTreeNode)
	{
#ifdef TALKATIVE
		vkernout << "*** [ViPickMan] unregistration failed..." << endl;
#endif
		return false;
	}

	// if we don't have a corresponding behavior, this node hasn't been
	// properly registered but is merely an internal node, so flag 
	// unregistration as unsuccessful...
	if (!pTreeNode->pBehavior)
	{
#ifdef TALKATIVE
		vkernout << "*** [ViPickMan] unregistration of '" << pNode->GetName() << "' failed..." << endl;
		vkernout << "               '" << pNode->GetName() << "' hasn't been registered before..." << endl;
#endif
		return false;
	}

	// well, as we need single nodes for hierarchy information,
	// we don't remove them. We merely remove their behavior object
	// to avoid contact callbacks and events.
	delete pTreeNode->pBehavior;
	pTreeNode->pBehavior = NULL;

#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] unregistration of '" << pNode->GetName() << "' successful..." << endl;
#endif

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ActivateNode                                                */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::ActivateNode(IVistaNode *pNode)
{
	SPickTreeNode *pTreeNode = FindNodeInPickTree(pNode);

	if (!pTreeNode)
		return false;

	pTreeNode->bActive = true;
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DeactivateNode                                              */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::DeactivateNode(IVistaNode *pNode)
{
	SPickTreeNode *pTreeNode = FindNodeInPickTree(pNode);

	if (!pTreeNode)
		return false;

	pTreeNode->bActive = false;
	AgeContactInformation(pTreeNode);
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsNodeActive                                                */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::IsNodeActive(IVistaNode *pNode)
{
	SPickTreeNode *pTreeNode = FindNodeInPickTree(pNode);

	if (!pTreeNode)
		return false;

	return pTreeNode->bActive;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetContacts                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaPickManager::GetContacts(IVistaNode *pNode, 
									std::list<SContactData> &liContacts)
{
	SPickTreeNode *pTreeNode = FindNodeInPickTree(pNode);

	if (!pTreeNode)
		return false;

	liContacts.clear();

	list<SContactData *>::iterator itContact = pTreeNode->pliContacts->begin();
	while (itContact != pTreeNode->pliContacts->end())
	{
		liContacts.push_back(*(*itContact));
		++itContact;
	}

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetPickBehavior                                             */
/*                                                                            */
/*============================================================================*/
VistaPickManager::CPickBehavior *VistaPickManager::GetPickBehavior(IVistaNode *pNode)
{
	SPickTreeNode *pTreeNode = FindNodeInPickTree(pNode);

	if (!pTreeNode)
		return NULL;

	return pTreeNode->pBehavior;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::Debug ( std::ostream & out ) const
{
	out << " [ViPickMan] active: " << (m_bActive ? "true" : "false") << endl;
	out << " [ViPickMan] dirty: " << (m_bDirty ? "true" : "false") << endl;
	out << " [ViPickMan] registered pick queries: " << m_mapQueries.size() << endl;

#ifdef TALKATIVE
	DumpPickTree(out);
#endif

}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DumpPickTree                                                */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::DumpPickTree(std::ostream &out) const
{
	if (m_pPickTreeRoot)
	{
		DumpNode(out, m_pPickTreeRoot, 0);
	}
	else
	{
		vkernout << " [ViPickMan] pick tree is empty" << endl;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UpdatePicking                                               */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::UpdatePicking()
{
#ifdef TALKATIVE
//	cout << "*** [ViPickMan] updating pick information..." << endl;
#endif
	if (!m_pPickTreeRoot || !m_pPickTreeRoot->bActive)
		return;

	// do some quick checks
	if (!m_bDirty && 
		(!m_bActive || m_mapQueries.empty()))
		return;


	// swap old contact information into appropriate place
	AgeContactInformation(m_pPickTreeRoot);

	// update contact and pick information, if we're active.
	// if not, proceed with object notifications.
	if (m_bActive)
	{
		PICK_DATA_MAP::iterator itQuery = m_mapQueries.begin();
		while (itQuery != m_mapQueries.end())
		{
			UpdateContacts(itQuery->first, itQuery->second);
			++itQuery;
		}
	}

	// traverse tree to notify objects and call events
	stack<SPickTreeNode *> stNodes;
	stNodes.push(m_pPickTreeRoot);
	SPickTreeNode *pNode;

	list<SPickTreeNode *>::iterator itChild;

	list<SContactData *>::iterator itContact, itPrevContact;

	bool bContactChange = false;

	while (!stNodes.empty())
	{
		bContactChange = false;
		pNode = stNodes.top();
		stNodes.pop();

		// check, whether the contacts have changed
		// first pass: for all valid contacts, check, whether we had this contact before...
		itContact = pNode->pliContacts->begin();
		int iNumberOfContacts = pNode->pliContacts->size();
		bool bContactFound = false;
		while (itContact != pNode->pliContacts->end())
		{
			bContactFound = false;
			itPrevContact = pNode->pliPrevContacts->begin();
			while (itPrevContact != pNode->pliPrevContacts->end())
			{
				if ((*itPrevContact)->pPickQuery == (*itContact)->pPickQuery)
				{
					bContactFound = true;
					pNode->pliPrevContacts->erase(itPrevContact);
					break;
				}
				++itPrevContact;
			}

			if (!bContactFound)
			{
				// allright, we didn't have this contact before, so take appropriate measures...
				if (pNode->pBehavior)
				{
					// this is a user-registered node -> react!
					if (pNode->pBehavior->m_pPickable)
					{
						pNode->pBehavior->m_pPickable->Touch(pNode->pNode, iNumberOfContacts, *itContact);
					}

					if (pNode->pBehavior->m_bEventOnTouch)
					{
						m_pPickEvent->SetTouchedEvent(pNode->pNode, iNumberOfContacts, *itContact, 
							pNode->pBehavior->m_pPickable);
						m_pEventManager->ProcessEvent(m_pPickEvent);
					}
				}
				bContactChange = true;
			}

			++itContact;
		}

		// second pass: if we still have old contacts, they are not valid anymore,
		// so send appropriate events / notifications...
		itContact = pNode->pliPrevContacts->begin();
		while (itContact != pNode->pliPrevContacts->end())
		{
			if (pNode->pBehavior)
			{
				// reset contact information, i.e. mark it as being invalid
				(*itContact)->fDistance = -1;

				// this is a user-registered node -> react!
				if (pNode->pBehavior->m_pPickable)
				{
					pNode->pBehavior->m_pPickable->Touch(pNode->pNode, iNumberOfContacts, *itContact);
				}

				if (pNode->pBehavior->m_bEventOnTouch)
				{
					m_pPickEvent->SetTouchedEvent(pNode->pNode, iNumberOfContacts, *itContact, 
						pNode->pBehavior->m_pPickable);
					m_pEventManager->ProcessEvent(m_pPickEvent);
				}
			}
			bContactChange = true;
			++itContact;
		}
		// - if there should have been any 'untouch'-events, they were sent by now
		// - the 'PrevContacts' list should be cleared, so that in following updates 
		//   new contacts are not mistakenly considered as old contacts
		// - this clering usually happens in 'AgeContactInformation', 
		//   but it does not work properly if child nodes are involved
		// -> clear list now
		itContact = pNode->pliPrevContacts->begin();
		for(;itContact != pNode->pliPrevContacts->end();++itContact)
		{
			delete (*itContact);
		}
		pNode->pliPrevContacts->clear();

		// avoid traversal of children, if this node didn't change 
		// and no child has been touched before...
		if (!bContactChange && pNode->pliContacts->empty())
		{
				continue;
		}

		// traverse children
		itChild = pNode->liChildren.begin();
		while (itChild != pNode->liChildren.end())
		{
			stNodes.push(*itChild);
			++itChild;
		}
	}

	// traverse list of picked nodes, notifying them if desired
	SPickData *pPickData;
	while (!m_qPicks.empty())
	{
		pPickData = m_qPicks.front();
		m_qPicks.pop();

		if (pPickData->pNode->pBehavior->m_pPickable)
		{
			pPickData->pNode->pBehavior->m_pPickable->Pick(pPickData->pNode->pNode, 
														   pPickData->pContact);
		}

		if (pPickData->pNode->pBehavior->m_bEventOnPick)
		{
			m_pPickEvent->SetPickedEvent(pPickData->pNode->pNode, 
										 pPickData->pContact, 
										 pPickData->pNode->pBehavior->m_pPickable);
			m_pEventManager->ProcessEvent(m_pPickEvent);
		}

		delete pPickData;
	}

	// well, everything's got processed...
	m_bDirty = false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   RebuildPickTree                                             */
/*                                                                            */
/*============================================================================*/
void VistaPickManager::RebuildPickTree()
{
#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] rebuilding pick tree..." << endl;
#endif
	if (!m_pPickTreeRoot)
		return;

	RemoveContactInformation(m_pPickTreeRoot);

	// traverse pick tree and push all user-registered nodes into a queue
	queue<SPickTreeNode *> qNodes;
	stack<SPickTreeNode *> stNodes;

	stNodes.push(m_pPickTreeRoot);

	while (!stNodes.empty())
	{
		SPickTreeNode *pNode = stNodes.top();
		stNodes.pop();
		
		if (pNode->pBehavior)
			qNodes.push(pNode);

		list<SPickTreeNode *>::iterator itChild = pNode->liChildren.begin();
		while (itChild != pNode->liChildren.end())
		{
			stNodes.push(*itChild);
			++itChild;
		}
	}

	if (qNodes.empty())
	{
		// there's obviously no active node -> dump the whole pick tree...
		ClearTreeNode(m_pPickTreeRoot);
		m_pPickTreeRoot = NULL;

		return;
	}

	// now re-register all those nodes again
	m_pPickTreeRoot = qNodes.front();
	qNodes.pop();
#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] using '" 
		 << m_pPickTreeRoot->pNode->GetName() 
		 << "'as pick tree root..." << endl;
#endif
	m_pPickTreeRoot->pParent = NULL;
	m_pPickTreeRoot->liChildren.clear();

	while (!qNodes.empty())
	{
		SPickTreeNode *pNode = qNodes.front();
		pNode->pParent = NULL;
		pNode->liChildren.clear();
#ifdef TALKATIVE
		vkernout << "*** [ViPickMan] re-registering '" 
			 << pNode->pNode->GetName() 
			 << "'..." << endl;
#endif
		InsertPickTreeNode(pNode, m_pPickTreeRoot);
		qNodes.pop();
	}
#ifdef TALKATIVE
	vkernout << "*** [ViPickMan] pick tree rebuild complete..." << endl;
#endif

}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( std::ostream & out, const VistaPickManager & device )
{
	device.Debug ( out );
	return out;
}


/*============================================================================*/
/*  Methods for VistaPickManager::CPickBehavior                              */
/*============================================================================*/
VistaPickManager::CPickBehavior::CPickBehavior()
: m_pPickable(NULL), m_bBlockTargetRay(true), m_bQueryForContact(false),
  m_bEventOnPick(false), m_bEventOnTouch(false) 
{
}

/*============================================================================*/
/*  Methods for VistaPickManager::CPickData                                */
/*============================================================================*/
VistaPickManager::CPickData::CPickData()
: m_fRange(-1.0f), m_bPick(false), m_bActive(true)
{
}

void VistaPickManager::CPickData::Activate()
{
	m_bActive = true;
}

void VistaPickManager::CPickData::Deactivate()
{
	m_bActive = false;
}

void VistaPickManager::CPickData::ActivatePicking()
{
	m_bPick = true;
}

void VistaPickManager::CPickData::DeactivatePicking()
{
	m_bPick = false;
}

void VistaPickManager::CPickData::SetRange(float fRange)
{
	m_fRange = fRange;
}

/*============================================================================*/
/*  Methods for VistaPickManager::SContactData                               */
/*============================================================================*/
VistaPickManager::SContactData::SContactData()
: pPickQuery(NULL), fDistance(0)
{
}

/*============================================================================*/
/*  Methods for VistaPickManager::SPickTreeNode                              */
/*============================================================================*/
VistaPickManager::SPickTreeNode::SPickTreeNode(IVistaNode *pVistaNode)
: pParent(NULL), pNode(pVistaNode), pBehavior(NULL),
  bHasTouchedChildren(false), bActive(true)
{
	pliContacts = new std::list<SContactData *>;
	pliPrevContacts = new std::list<SContactData *>;
}

VistaPickManager::SPickTreeNode::~SPickTreeNode()
{
	delete pliContacts;
	delete pliPrevContacts;
}


VistaPickManager::CPickQuery::CPickQuery()
{
}

VistaPickManager::CPickQuery::~CPickQuery()
{
}



