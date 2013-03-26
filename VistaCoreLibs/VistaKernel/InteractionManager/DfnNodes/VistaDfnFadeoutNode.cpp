/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#include "VistaDfnFadeoutNode.h" 

#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

enum
{
	MODE_CONST_OPACITY,
	MODE_OFF,
	MODE_FADEIN,
	MODE_FADEOUT,
};

class VistaDfnFadeoutNode::FadeoutOverlay : public IVistaSceneOverlay
{
public:
	FadeoutOverlay( VistaViewport* pViewport, VistaDfnFadeoutNode* pParent )
	: IVistaSceneOverlay( pViewport )
	, m_bEnabled( true )
	, m_pParent( pParent )
	{
	}

	virtual bool GetIsEnabled() const
	{
		return m_bEnabled;
	}

	virtual void SetIsEnabled( bool bEnabled ) 
	{
		m_bEnabled = bEnabled;;
	}

	virtual void UpdateOnViewportChange( int iWidth, int iHeight, int iPosX, int iPosY ) 
	{		
	}

	virtual bool Do()
	{
		if( m_bEnabled == false )
			return false;
		
		float nOpacity = m_pParent->GetModeOpacity();

		if( nOpacity <= 0 )
			return true;

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glDisable( GL_CULL_FACE );

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		// Note: y is inverted, so we start with (0,0) at the top left
		gluOrtho2D( 0, 1, 0, 1 );

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		float nAlpha = nOpacity * m_pParent->m_oColor[3];
		glColor4f( m_pParent->m_oColor[0], m_pParent->m_oColor[1], m_pParent->m_oColor[2], nAlpha );
		glBegin( GL_QUADS );
			glVertex3f( 0, 0, 0 );
			glVertex3f( 1, 0, 0 );
			glVertex3f( 1, 1, 0 );
			glVertex3f( 0, 1, 0 );
		glEnd();

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();

		return true;
	}
private:
	VistaDfnFadeoutNode* m_pParent;
	bool m_bEnabled;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaDfnFadeoutNode::VistaDfnFadeoutNode( VistaClusterMode* pClusterMode )
: IVdfnNode()
, m_oColor( VistaColor::GRAY )
, m_nFadeinTime( 1.0f )
, m_nFadeoutTime( 1.0f )
, m_pStatePort( NULL )
, m_pOpacityPort( NULL )
, m_pClusterMode( pClusterMode )
, m_nOpacity( 0 )
, m_nState( MODE_OFF )
{
	RegisterInPortPrototype( "opacity", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "state", new TVdfnPortTypeCompare<TVdfnPort<bool> > );
}


VistaDfnFadeoutNode::~VistaDfnFadeoutNode()
{
	for( std::vector<FadeoutOverlay*>::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		delete (*itOverlay);
	}
}

bool VistaDfnFadeoutNode::GetIsValid() const
{
	return ( m_pStatePort || m_pOpacityPort );
}

bool VistaDfnFadeoutNode::PrepareEvaluationRun()
{
	m_pStatePort = VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>( "state", this );
	m_pOpacityPort = VdfnUtil::GetInPortTyped<TVdfnPort<float>*>( "opacity", this );
	return GetIsValid();
}

void VistaDfnFadeoutNode::AddToViewport( VistaViewport* pViewport )
{
	m_vecOverlays.push_back( new FadeoutOverlay( pViewport, this ) );
}

void VistaDfnFadeoutNode::AddToAllViewports( VistaDisplayManager* pManager )
{
	for( std::map<std::string, VistaViewport*>::const_iterator itViewport = pManager->GetViewportsConstRef().begin();
			itViewport != pManager->GetViewportsConstRef().end(); ++itViewport )
	{
		m_vecOverlays.push_back( new FadeoutOverlay( (*itViewport).second, this ) );
	}
}

VistaColor VistaDfnFadeoutNode::GetColor() const
{
	return m_oColor;
}

void VistaDfnFadeoutNode::SetColor( const VistaColor& oValue )
{
	m_oColor = oValue;
}

VistaType::microtime VistaDfnFadeoutNode::GetFadeinTime() const
{
	return m_nFadeinTime;
}

void VistaDfnFadeoutNode::SetFadeinTime( const VistaType::microtime oValue )
{
	m_nFadeinTime = oValue;
}

VistaType::microtime VistaDfnFadeoutNode::GetFadeoutTime() const
{
	return m_nFadeoutTime;
}

void VistaDfnFadeoutNode::SetFadeoutTime( const VistaType::microtime oValue )
{
	m_nFadeoutTime = oValue;
}

float VistaDfnFadeoutNode::GetModeOpacity()
{
	switch( m_nState )
	{
		case MODE_CONST_OPACITY:
			return m_nOpacity;
		case MODE_FADEOUT:
		{
			VistaType::microtime nDiff = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nDiff /= m_nFadeoutTime;
			if( nDiff > 1 )
			{
				m_nState = MODE_CONST_OPACITY;
				m_nOpacity = 1;
				return m_nOpacity;
			}
			return nDiff;
		}
		case MODE_FADEIN:
		{
			VistaType::microtime nDiff = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nDiff /= m_nFadeinTime;
			if( nDiff > 1 )
			{
				m_nState = MODE_OFF;
				m_nOpacity = 0;
				return m_nOpacity;
			}
			return 1 - nDiff;
		}
		case MODE_OFF:
		default:
			return 0;
	}
}

bool VistaDfnFadeoutNode::DoEvalNode()
{
	if( m_pStatePort )
	{
		bool bState = m_pStatePort->GetValue();
		if( bState == true && ( m_nState == MODE_FADEIN || m_nState == MODE_CONST_OPACITY ) )
			return true; // already correct state
		if( bState == false && ( m_nState == MODE_FADEOUT || m_nState == MODE_OFF ) )
			return true; // already correct state

		// check if we aort a fade-in-progress
		VistaType::microtime nOffset = 0;
		if( m_nState == MODE_FADEIN )
		{
			VistaType::microtime nDelta = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nOffset = m_nFadeinTime - nDelta;
		}
		else if( m_nState == MODE_FADEOUT )
		{
			VistaType::microtime nDelta = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nOffset = m_nFadeinTime - nDelta;
		}
		m_nStartTime = m_pClusterMode->GetFrameClock() - nOffset;
		if( bState )
			m_nState = MODE_FADEOUT;
		else
			m_nState = MODE_FADEIN;
	}
	else if( m_pOpacityPort )
	{
		m_nOpacity = m_pOpacityPort->GetValue();
		m_nState = MODE_CONST_OPACITY;
	}
	return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


