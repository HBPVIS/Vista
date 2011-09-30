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
/*
 * $Id: main.cpp 5209 2010-03-24 19:35:42Z tbeer $
 */

// include header here

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>


#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>


#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaTextNode.h>

#include <VistaBase/VistaVectorMath.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

class Translate : public IVistaExplicitCallbackInterface
{
public:
	Translate( VistaVirtualPlatform *pPlatform,
				const VistaVector3D &v3Trans )
				: m_pPlatform(pPlatform),
				  m_v3Trans(v3Trans)
	{
	}

	bool Do()
	{
		// move the camera, but keep the movement in 
		// the view alignments, so get the current rotation
		// of the vp
		VistaQuaternion q = m_pPlatform->GetRotation();

		// rotate the given world vector along the view direction
		VistaVector3D v3Trans = q.Rotate( m_v3Trans );

		// translate to that direction
		return m_pPlatform->SetTranslation( m_pPlatform->GetTranslation() + v3Trans );
	}

private:
	VistaVirtualPlatform *m_pPlatform;
	VistaVector3D         m_v3Trans;
};

class Rotate : public IVistaExplicitCallbackInterface
{
public:
	Rotate( VistaVirtualPlatform *pPlatform,
			 float nDeg )
			 : m_pPlatform(pPlatform),
			   m_nRot(nDeg)
	{
	}

	bool Do()
	{
		// rotation is simple: apply an incremental roation 
		// the AxisAndAngle constructs a quat that defines a rotation
		// around the global given axis, keep in mind that the rotation
		// is given in radians.
		VistaAxisAndAngle aaa( VistaVector3D(0,1,0), Vista::DegToRad(m_nRot) );
		VistaQuaternion q(aaa);

		// due to numerical reasons, a quat might degenerate during the
		// manipulation, so it might be a good idea to normalize it once and then
		// here, we do it on every call, but that might be too much
		VistaQuaternion qRot = m_pPlatform->GetRotation();
		qRot.Normalize();

		// set the rotation as an incremental rotation
		return m_pPlatform->SetRotation( qRot * q );

		// the rotation is around the center of the virtual platform.
		// iff you decide to have the viewpoint not in 0,0,0, this might
		// look akward. In that case, you have to apply a transform
		// that includes a normalization around the view point position.
		// for this demo, we chose the viewpoint to be 0,0,0 and the projection
		// plane midpoint in 0,0,-2. So the rotation look ''natural'' around the
		// view point (as it is in 0,0,0).
	}

private:
	VistaVirtualPlatform *m_pPlatform;
	float m_nRot;
};
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int main(int argc, char **argv)
{
	VistaSystem vistaSystem;

	if(vistaSystem.Init(argc, argv))
	{
		VistaGraphicsManager *pMgr = vistaSystem.GetGraphicsManager();
		VistaSG *pSG = pMgr->GetSceneGraph();

        
        VistaGeometryFactory geometryFactory( pSG );
        
		VistaQuaternion q;
		VistaVector3D v3TransOld;
		VistaVector3D v3TransNew;
		
        /*
         * Create a large box that goes from just behind the screen
         * way behind the screen. If the stereo projection is flipped,
         * the box will come out of the screen.
         */
        VistaGeometry *pBoxBack = geometryFactory.CreateBox ( 0.2f, 100.0f, 0.2f );
        pBoxBack->SetColor( VistaColorRGB( VistaColorRGB::GREEN ) );
        VistaTransformNode *pBoxBackTrans = pSG->NewTransformNode( pSG->GetRoot() );
		pBoxBackTrans->GetRotation(q);
		v3TransNew = q.Rotate( VistaVector3D(-0.0f, 50.0f, -0.2f) );
		pBoxBackTrans->GetTranslation(v3TransOld);
		pBoxBackTrans->SetTranslation(  v3TransOld + v3TransNew );
		pSG->NewGeomNode( pBoxBackTrans, pBoxBack );
        
        // Create the info text
		Vista2DText *pText2D = vistaSystem.GetDisplayManager()->Add2DText( "MAIN_VIEWPORT" );
        if( pText2D )
            pText2D->Init( "If you see a triangle coming out of the screen, your stereo projection is flipped!", 0, 0 );
        
		// now determine the reference frame from a given display system
		// we query that from the display manager here
		VistaDisplayManager *pDispMgr = vistaSystem.GetDisplayManager();

		// for this demo, we simply take the first display system (index = 0)
		VistaDisplaySystem *pDispSys  = pDispMgr->GetDisplaySystem(0);

		// each display system has a reference frame, which represents a virtual
		// camera, so we claim that.
		VistaVirtualPlatform *pVp     = pDispSys->GetReferenceFrame();


        // TODO Move camera to zero
        
        
        
        
        
        
		// register some callbacks for some keyboard strokes.
		// these have to be registered with the keyboard system control
		VistaKeyboardSystemControl *pCtrl = vistaSystem.GetKeyboardSystemControl();

		// self descriptive.
		pCtrl->BindAction( 'a', new Translate( pVp, VistaVector3D(-0.25f, 0,  0 ) ),
												   "translate to -x");
		pCtrl->BindAction( 'd', new Translate( pVp, VistaVector3D( 0.25f, 0,  0 ) ),
												   "translate to  x");
		pCtrl->BindAction( 'w', new Translate( pVp, VistaVector3D(     0, 0, -0.25f ) ),
												   "translate to -z");
		pCtrl->BindAction( 's', new Translate( pVp, VistaVector3D(     0, 0,  0.25f ) ),
												   "translate to  z");
		
		pCtrl->BindAction( VISTA_KEY_UPARROW, new Translate( pVp, VistaVector3D( 0, 0.25f, 0 ) ),
													  "translate to y");
		pCtrl->BindAction( VISTA_KEY_DOWNARROW, new Translate( pVp, VistaVector3D( 0, -0.25f, 0 ) ),
													   "translate to -y");

		pCtrl->BindAction( VISTA_KEY_RIGHTARROW, new Rotate( pVp, -0.5f ),
													   "rotate -0.5 deg about the y-axis");
		pCtrl->BindAction( VISTA_KEY_LEFTARROW,  new Rotate( pVp,   0.5f ),
													   "rotate 0.5 deg about the y-axis");

		vistaSystem.GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle(argv[0]);

		// start the simulation.
		vistaSystem.Run();
	}


}

