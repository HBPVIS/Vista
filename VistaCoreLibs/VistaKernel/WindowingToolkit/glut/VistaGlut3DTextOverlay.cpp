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

#include "VistaGlut3DTextOverlay.h"
#include "VistaGlutTextEntity.h"

#include "../VistaTextEntity.h"

#include "../../DisplayManager/VistaDisplayManager.h"
#include "../../DisplayManager/VistaDisplaySystem.h"
#include "../../DisplayManager/VistaVirtualPlatform.h"

// Include Windows header for OpenGL
#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN) // we use the mac os GLUT framework on darwin
  #include <GLUT/glut.h>
#else
  #if defined(USE_NATIVE_GLUT)
    #include <GL/glut.h>
  #else
    #include <GL/freeglut.h>
  #endif
#endif

#if defined(DARWIN)
  #include <OpenGL/OpenGL.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlut3DTextOverlay::VistaGlut3DTextOverlay(
		VistaDisplayManager *pMgr )
	: m_bEnabled(true),
	  m_pDisplayMgr(pMgr)
{
	m_pDisplayMgr->AddSceneOverlay(this);
}
VistaGlut3DTextOverlay::~VistaGlut3DTextOverlay()
{
	m_pDisplayMgr->RemSceneOverlay(this);

	/**
	 * Do not clean up the TextEntitys here.
	 * The user has reserved the memory them selve,
	 * so he / she should cleanup it in the application code.
	 */
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaGlut3DTextOverlay::AddText(IVistaTextEntity *text)
{
	m_liTexts.push_back(text);
	m_liTexts.sort();
	return true;
}

bool VistaGlut3DTextOverlay::RemText(IVistaTextEntity *text)
{
	m_liTexts.remove(text);
	m_liTexts.sort();
	return true;
}
bool VistaGlut3DTextOverlay::GetIsEnabled() const
{
	return m_bEnabled;
}

void VistaGlut3DTextOverlay::SetIsEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

bool VistaGlut3DTextOverlay::Do ()
{
	if(!m_bEnabled)
		return true; // everything is allright, we are not visible

	if(m_liTexts.empty())
		return true; // no text to draw, save some state changes

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable(GL_LIGHTING);


	// as we are an overlay, we may not assume that the
	// current modelview matrix co-responds to the scene camera
	// so we get the matrix from the display system as the
	// inverse
	VistaTransformMatrix m;
	m_pDisplayMgr->GetDisplaySystem()->GetReferenceFrame()->GetMatrixInverse(m);

	// make it floats
	float af4x4[16];

	// transpose for GL
	m.GetTransposedValues(af4x4);

	// we assume the projection matrix to be "correct", though
	/** @todo maybe check the correct projection here? */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf( af4x4 ); // load cam
	glDisable(GL_DEPTH_TEST);

	for(std::list<IVistaTextEntity*>::const_iterator cit = m_liTexts.begin();
		cit != m_liTexts.end(); ++cit)
	{
		if((*cit)->GetEnabled() && !(*cit)->GetText().empty())
		{

			glColor3f((*cit)->GetColor().GetRed(),
						(*cit)->GetColor().GetGreen(),
						(*cit)->GetColor().GetBlue());

			glRasterPos3f((*cit)->GetXPos(),
						(*cit)->GetYPos(),
						(*cit)->GetZPos());

			(*cit)->DrawCharacters();
		}
	}

	glPopMatrix();
	glPopAttrib();

	return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


