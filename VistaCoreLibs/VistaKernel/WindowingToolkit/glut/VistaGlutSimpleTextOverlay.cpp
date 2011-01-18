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

#include "VistaGlutSimpleTextOverlay.h"
#include "VistaGlutTextEntity.h"

#include "../VistaTextEntity.h"

#include "../../DisplayManager/VistaDisplayManager.h"

// Include Windows header for OpenGL
#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

bool SortByYPos( const IVistaTextEntity* pText1, const IVistaTextEntity* pText2 )
{
	return pText1->GetYPos() < pText2->GetYPos();
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutSimpleTextOverlay::VistaGlutSimpleTextOverlay(VistaDisplayManager *pMgr,
												 VistaEventManager *pEvMgr,
												 unsigned int nWidth,
												 unsigned int nHeight)
 : m_pMgr(pMgr),
   m_nWidth(nWidth),
   m_nHeight(nHeight),
   m_bEnabled(true),
   m_pObs( NULL )
{
    m_pObs = new CRatioChangeHandler(pEvMgr, this );
	m_pMgr->AddSceneOverlay(this);

}

VistaGlutSimpleTextOverlay::~VistaGlutSimpleTextOverlay()
{
	m_pMgr->RemSceneOverlay(this);
	if( m_pObs )
		delete m_pObs;

	/**
	 * Do not clean up the TextEntitys here.
	 * The user has reserved the memory them selve,
	 * so he / she should cleanup it in the application code.
	 */
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaGlutSimpleTextOverlay::AddText(IVistaTextEntity *text)
{
	m_liTexts.push_back(text);
	m_liTexts.sort( SortByYPos );
	return true;
}

bool VistaGlutSimpleTextOverlay::RemText(IVistaTextEntity *text)
{
	m_liTexts.remove(text);
	m_liTexts.sort( SortByYPos );
	return true;
}

bool VistaGlutSimpleTextOverlay::GetIsEnabled() const
{
	return m_bEnabled;
}

void VistaGlutSimpleTextOverlay::SetIsEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

unsigned int VistaGlutSimpleTextOverlay::GetWidth() const
{
	return m_nWidth;
}

unsigned int VistaGlutSimpleTextOverlay::GetHeight() const
{
	return m_nHeight;
}

void VistaGlutSimpleTextOverlay::SetWidth(unsigned int nWidth)
{
	m_nWidth = nWidth;
}

void VistaGlutSimpleTextOverlay::SetHeight(unsigned int nHeight)
{
	m_nHeight = nHeight;
}

bool VistaGlutSimpleTextOverlay::Do ()
{
	if( !m_bEnabled || m_liTexts.empty() )
		return true; // everything is all right, we are not visible or have no text

	int iLastTextSize = -1;
	bool bSortList = false;

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	float fLastYPos = 0.0f;
	float fAccumRasterPos = 0;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// Note: y is inverted, so we start with (0,0) at the top left
	gluOrtho2D( 0, m_nWidth, m_nHeight, 0 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	for( std::list<IVistaTextEntity*>::const_iterator cit = m_liTexts.begin();
		cit != m_liTexts.end(); ++cit )
	{
		if( (*cit)->GetEnabled() == false )
			continue;

		if( fLastYPos > (*cit)->GetYPos() )
			bSortList = true;

		int iTextSize =(*cit)->GetFontSize();

		float fLineAdvance = (*cit)->GetYPos() - fLastYPos;
		fAccumRasterPos += (float)iTextSize * fLineAdvance;

		glColor3f( (*cit)->GetColor().GetRed(),
					(*cit)->GetColor().GetGreen(),
					(*cit)->GetColor().GetBlue() );

		 /// @todo Translate OpenGL coords to window coords
		glRasterPos2f( (*cit)->GetXPos() * iTextSize,
						fAccumRasterPos );

		fLastYPos = (*cit)->GetYPos();

		(*cit)->DrawCharacters();			
	}


	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();

	if( bSortList )
		m_liTexts.sort( SortByYPos );

	return true;
}

VistaGlutSimpleTextOverlay::CRatioChangeHandler::CRatioChangeHandler( VistaEventManager *pEvMgr,
		VistaGlutSimpleTextOverlay *pOverlay)
:
VistaEventHandler(),
m_pEvMgr(pEvMgr),
m_pOverlay(pOverlay)
{
	m_pEvMgr->AddEventHandler(this, VistaDisplayEvent::GetTypeId(), VistaEventManager::NVET_ALL);
}

VistaGlutSimpleTextOverlay::CRatioChangeHandler::~CRatioChangeHandler()
{
	m_pEvMgr->RemEventHandler(this, VistaDisplayEvent::GetTypeId(),
			VistaEventManager::NVET_ALL);
}

void VistaGlutSimpleTextOverlay::CRatioChangeHandler::HandleEvent(VistaEvent *pEvent)
{
	VistaDisplayEvent *pDispEv = dynamic_cast<VistaDisplayEvent*> (pEvent);
	switch (pEvent->GetId())
	{
	case VistaDisplayEvent::VDE_RATIOCHANGE:
	{
		VistaWindow *pWindow = pDispEv->GetViewport()->GetWindow();

		int nWidth, nHeight;
		if (!pWindow->GetWindowProperties()->GetSize(nWidth, nHeight))
			return;
		m_pOverlay->SetWidth(nWidth);
		m_pOverlay->SetHeight(nHeight);

		break;
	}
	default:
		break;
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


