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

#ifndef _VISTAKERNELDFNNODECREATORSS_H
#define _VISTAKERNELDFNNODECREATORSS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnTextOverlayNode.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaDisplayManager;
class VistaEventManager;
class VistaInteractionManager;
class VistaClusterAux;
class IVistaWindowingToolkit;
class VistaKeyboardSystemControl;

/*============================================================================*/
/* UTILITY                                                                    */
/*============================================================================*/

namespace VistaKernelDfnNodeCreators
{
	bool VISTAKERNELAPI RegisterNodeCreates( VistaSystem* pVistaSystem );
}

/*============================================================================*/
/* NODE CREATES                                                               */
/*============================================================================*/

class VISTAKERNELAPI Vista3DMouseTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	Vista3DMouseTransformNodeCreate(VistaDisplayManager *pSys);

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

	VistaDisplayManager *m_pMgr;
};


class VISTAKERNELAPI VdfnClusterNodeInfoNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VdfnClusterNodeInfoNodeCreate( VistaClusterAux *pDm );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
	VistaClusterAux *m_pAux;
};

class VISTAKERNELAPI VdfnDeviceDebugNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VdfnDeviceDebugNodeCreate(IVistaWindowingToolkit *wta);
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
	IVistaWindowingToolkit *m_pWta;
};

class VISTAKERNELAPI VdfnDumpHistoryNodeClusterCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VdfnDumpHistoryNodeClusterCreate( VistaClusterAux *pAux );
	IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
	VistaClusterAux *m_pAux;
};

class VISTAKERNELAPI VistaDfnEventSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnEventSourceNodeCreate( VistaEventManager *pEvMgr,
								VistaInteractionManager *pInMa );

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
private:
	VistaEventManager          *m_pEvMgr;
	VistaInteractionManager *m_pInMa;
};

class VISTAKERNELAPI VistaDfnNavigationNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
private:
};

class VISTAKERNELAPI VistaDfnWindowSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnWindowSourceNodeCreate( VistaEventManager *pEgMgr,
							   VistaDisplayManager *pMgr );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

private:
	VistaDisplayManager *m_pMgr;
	VistaEventManager *m_pEvMgr;
};

class VISTAKERNELAPI VistaDfnViewportSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnViewportSourceNodeCreate( VistaDisplayManager *pMgr );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

private:
	VistaDisplayManager *m_pMgr;
};


class VISTAKERNELAPI VistaDfnViewerSinkNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnViewerSinkNodeCreate( VistaDisplayManager *pMgr );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

private:
	VistaDisplayManager *m_pMgr;
};

class VISTAKERNELAPI VistaDfnTrackballNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnTrackballNodeCreate(VistaDisplayManager *pDispMgr);
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
private:
	VistaDisplayManager *m_pDispMgr;
};


template<class T>
class VISTAKERNELAPI VistaDfnTextOverlayNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{

public:
	typedef typename VistaDfnTextOverlayNode<T>::ToStringFct StringFct;
	VistaDfnTextOverlayNodeCreate( VistaDisplayManager *pDm,
					 StringFct fct )
		: m_CFct(fct),
		  m_pDm(pDm)
	{
	}

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		try
		{
			const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

			float nX = float(oSubs.GetDoubleValue("x_pos"));
			float nY = float(oSubs.GetDoubleValue("y_pos"));

			int nR = int(oSubs.GetIntValue("red"));
			int nG = int(oSubs.GetIntValue("green"));
			int nB = int(oSubs.GetIntValue("blue"));

			int nSize = 20; // is default from InitText()
			if(oSubs.HasProperty("size"))
				nSize = int(oSubs.GetIntValue("size"));

			std::string sIniText = oSubs.GetStringValue("value");


			Vista2DText::Vista2DTextFontFamily eFamily = Vista2DText::SANS;

			if(oSubs.HasProperty("font"))
			{
				std::string strFont = oSubs.GetStringValue("font");
				if(strFont.compare("TYPEWRITER") == 0)
					eFamily = Vista2DText::TYPEWRITER;
				else if(strFont.compare("SERIF") == 0)
					eFamily = Vista2DText::SERIF;
			}

			Vista2DText *pText = m_pDm->Add2DText(oSubs.GetStringValue("viewport"));
			if(pText)
			{
				pText->Init( sIniText, nX, nY,  nR, nG, nB, nSize, eFamily );

				VistaDfnTextOverlayNode<T> *pSink
					= new VistaDfnTextOverlayNode<T>( pText, m_CFct );

				std::string sPostfix = oSubs.GetStringValue("postfix");
				std::string sPrefix = oSubs.GetStringValue("prefix");

				pSink->SetPostfix(sPostfix);
				pSink->SetPrefix(sPrefix);

				return pSink;
			}
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
		}

		return NULL;
	}
private:
	StringFct m_CFct;
	VistaDisplayManager *m_pDm;
};

class VISTAKERNELAPI VistaDfnSystemTriggerControlNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnSystemTriggerControlNodeCreate( VistaKeyboardSystemControl *pCtrl );

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
private:
	VistaKeyboardSystemControl *m_pCtrl;
};

class VISTAKERNELAPI VistaDfnReferenceFrameTransformNodeCreate
								: public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnReferenceFrameTransformNodeCreate( VistaDisplayManager* pDisplayManager );

	virtual IVdfnNode* CreateNode( const VistaPropertyList &oParams ) const;

private:
	VistaDisplayManager* m_pDisplayManager;
};


class VISTAKERNELAPI VistaDfnProjectionSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnProjectionSourceNodeCreate( VistaDisplayManager *pMgr );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

private:
	VistaDisplayManager *m_pMgr;
};

class VistaFrameclockNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	class TimerNodeFrameclockGet;

	VistaFrameclockNodeCreate( VistaClusterAux* pClusterAux );

	virtual IVdfnNode* CreateNode( const VistaPropertyList &oParams ) const;
	
private:
	VistaClusterAux* m_pClusterAux;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAKERNELDFNNODECREATORSS_H
