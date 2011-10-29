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
 * $Id$
 */

// include header here

#include <VistaKernel/VistaSystem.h>

// needed for interaction
#include <VistaKernel/InteractionManager/VistaIntentionSelect.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

// neeeded for the scene
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

// needed for the trackball camera movement
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>

// implementation of mouse interaction
//#include <VistaKernel/Stuff/VistaPlatformTrackball.h>

// skeleton implementation of an interaction handler,
// reacting on VistaInteractionEvents.
#include <VistaKernel/Stuff/VistaInteractionHandlerBase.h>

#include <VistaBase/VistaExceptionBase.h>


// needed for the VistaEventCone
#include <VistaMath/VistaGeometries.h>

// simple methods for AABB handling
#include <VistaMath/VistaBoundingBox.h>

#include <VistaDataFlowNet/VdfnObjectRegistry.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * The interaction component that implements the selection test upon
 * change of the spatial input data change, indicated by an attached context.
 * it creates an instance of the IntentionSelection that can be used to
 * implement the scoring on the elements.
 */

class SelectionHandler : public IVistaInteractionHandlerBase
{
public:
    SelectionHandler( VistaEventManager *pEvMgr,
                       unsigned int nRoleId,
                       VistaSceneGraph *pSG)
                       : IVistaInteractionHandlerBase(pEvMgr, nRoleId, false),
                       m_pSelection(new VistaIntentionSelect),
					   m_pSG(pSG)
    {

    	// create new geometryFactors
    	VistaGeometryFactory geometryFactory( pSG );

        // for this demo, we simply collect all child geom nodes
        // of the scene as candidates for the selection.
        // in a real world application, this should be done smarter.
		CollectChildren( dynamic_cast<VistaGroupNode*>(pSG->GetNode( "Boxes" )) );

        // we adapt the selection volume to be of a specific size
        // in this case: we want a scoring up to 3.0f units away in
        // a cone with radius of 0.25f units, which is half the size of the
        // size of the boxes we created in this demo. This gives a nice
        // ''fit'' feel for the selection of the objects.
        // larger values, at least for mouse based selection
        // might not feel well, you should experiment on these parameters
        VistaEvenCone selectionVolume( 3.0f, 0.25f );

        // set this selection volume with the intention select algorithm
        m_pSelection->SetSelectionVolume(selectionVolume);

        // in this demo, we want a rather ''sluggish'' change of focus,
        // we we set sticky rather high and snappy rather low
        // you should experiment on your needs in a real world application
        m_pSelection->SetStickyness( 0.9f );
        m_pSelection->SetSnappiness( 0.1f );

        // for the visual feedback on the selection, we create a box
        // slightly larger than the visual boxes to select (0.6f)
        VistaGeometry *pHighlightGeometry = geometryFactory.CreateBox ( 0.6f, 0.6f, 0.6f );
        //new VistaBox( pSG, pSG->GetRoot(), 0.6f,0.6f,0.6f, false );

		// create a transformnode to move the box
        m_pHighlightTransform = pSG->NewTransformNode( pSG->GetRoot() );

        // Put the geometry into a node
        pSG->NewGeomNode( m_pHighlightTransform, pHighlightGeometry );

        // but turn this off, nothing is selected in the beginning
    	m_pHighlightTransform->SetIsEnabled(false);


        // we set the rendering style to wireframe, as we want it to look
        // as if this is a kind of an outline.
        VistaRenderingAttributes att;
        att.style = VistaRenderingAttributes::STYLE_WIREFRAME;
        pHighlightGeometry->SetRenderingAttributes(att);
    }

    ~SelectionHandler()
    {
        // clean up
        delete m_pSelection;
        delete m_pHighlightTransform;
    }


    /**
     * From construction, this routine is called on a change of the spatial
     * data given by the input device driver.
     * In this demo, we assume that the context is setup to deliver
     * world position and world orientation (!!). If setup otherwise, the
     * selection part will not work correctly.
     */
    virtual bool HandleContextChange( VistaInteractionEvent *pEvent )
    {
        // retrieve the context from the event.
        const VistaInteractionContext *pCtx = pEvent->GetInteractionContext();

        // claim pos and ori -> NOTE: this is better be in WORLD COORDINATES!
		VistaVector3D   v3Position;
		VistaQuaternion qOrientation;
		VistaTransformNode* intentionNode = dynamic_cast<VistaTransformNode*>(m_pSG->GetNode("INTENTION_NODE"));

		intentionNode->GetTranslation(v3Position);
		intentionNode->GetRotation(qOrientation);

        // set this as the current WORLD transform of the selection cone
        m_pSelection->SetConeTransform( v3Position, qOrientation );

        // while being here, we start doing the scoring in order to determine
        // if anything is selected
        std::vector<IVistaIntentionSelectAdapter*> vIntenSelectAdapter;

        m_pSelection->Update(vIntenSelectAdapter);

        if(!vIntenSelectAdapter.empty())
        {
			// yes! we have at least a score on one element of the selection set.

            // turn on the selection box as visual feedback of the current focus element
        	m_pHighlightTransform->SetIsEnabled(true);

            // get queue top (i.e.: our selection focus!)
			IVistaIntentionSelectAdapter *pNode = vIntenSelectAdapter[0];

            // identify the hightlight's box and pos
            // with the one from the node
            VistaVector3D v3WorldCoordinatesPos;
            pNode->GetPosition( v3WorldCoordinatesPos );

            // set as the higlight box is defined in world
            // space
            m_pHighlightTransform->SetTranslation( v3WorldCoordinatesPos );
        }
        else
        {
            // no! no selection, simply turn off the visual feedback
        	m_pHighlightTransform->SetIsEnabled( false );
        }
        return true;
    }

    virtual bool HandleGraphUpdate( VistaInteractionEvent *pEvent )
    {
		return HandleContextChange( pEvent );
    }

	virtual bool HandleTimeUpdate( double dTs, double dLastTs )
	{
		return true;
	}

private:
    VistaIntentionSelect *m_pSelection;
    VistaTransformNode *m_pHighlightTransform;
	VistaSceneGraph *m_pSG;

    /**
     * A helper routine to collect all geom nodes of the scene
     * recursively. This is the part where usually the application
     * decides how to find the nodes that should be considered
     * during the selection update phase.
     */
    void CollectChildren( VistaGroupNode *pParent )
    {
        // iterate over all children of this group node
        for( unsigned int n=0; n < pParent->GetNumChildren(); ++n)
        {
			IVistaNode *pNode = pParent->GetChild(n);

            // dirty: use the type from rtti to get a branch decision
            VistaGroupNode *pGroup = dynamic_cast<VistaGroupNode*>(pNode);

            if(pGroup) // was at least a group node
            {
                // recurse over children
                CollectChildren( pGroup );
            }
            else if(pNode->GetType() == VISTA_GEOMNODE) // use the ViSTA type test for geometry nodes
            {
                // register this node as being part of the selection set
                m_pSelection->RegisterNode( new VistaNodeAdapter(pNode) );
            }
        }
    }
};

void CreateScene(VistaSystem &vistaSystem)
{
    // we need access to the scene, which
    // we get from the graphics manager
    VistaGraphicsManager *pGraphicsManager = vistaSystem.GetGraphicsManager();

    // claim sg
    VistaSceneGraph *pSG = pGraphicsManager->GetSceneGraph();

    // claim root, we want to attach then in world space
    VistaGroupNode *pRoot = pSG->GetRoot();
	VistaGroupNode *pBoxes = pSG->NewGroupNode(pRoot);
	pBoxes->SetName("Boxes");

    // the user is standing in 0,0,2, looking dir 0,0,-1
    // so we start with the scene a bit away at 0,0,-1
    VistaVector3D v3Pos(0,0,-1);

	// create new geometryFactors
	VistaGeometryFactory oGeometryFactory( pSG );

    // create 10 boxes in a loop
    for(unsigned int n=0; n < 10; ++n)
    {
        // note the size is 0.5f, this is important for a good feel with
        // the intention select cone size
        VistaGeometry *pBox = oGeometryFactory.CreateBox ( 0.5f, 0.5f, 0.5f );

        // give a fancy color
        pBox->SetColor( VistaColorRGB( VistaColorRGB::DARK_ORCHID ) );

        // shift boxes away from the user with
        // a slight tendency to the left
        v3Pos += VistaVector3D(-0.15f, 0.0f, -1.0f);

        // Get a new transform node
        VistaTransformNode *pBoxTrans = pSG->NewTransformNode( pBoxes );

        // set as translation, being childs of world root
        // this equals their world position
        pBoxTrans->SetTranslation( v3Pos );

        // Put the geometry into a node
        pSG->NewGeomNode( pBoxTrans, pBox );

    }
}

VistaTransformNode* m_pIntentionNode;
VistaIntentionSelect* m_pIntentionSelect;

void CreateInteraction(VistaSystem &vistaSystem)
{
	// Getting the GraphicsManager
	VistaGraphicsManager *pMgr = vistaSystem.GetGraphicsManager();
	// Get the Vista Scenegraph from the GraphicsManager
	VistaSceneGraph *pSG = pMgr->GetSceneGraph();
	// Get the root as GroupNode
	VistaGroupNode *pRoot = pSG->GetRoot();
	// No Name is a bad name, so give a name to the root
	pRoot->SetName( "VISTA-ROOT" );
	// The transformNode to store the 3DMouse data
	m_pIntentionNode = pSG->NewTransformNode( pRoot );
	// A name for the poor node
	m_pIntentionNode->SetName("INTENTION_NODE");
	// Register the node in the DFN
	vistaSystem.GetDfnObjectRegistry()->SetObject("INTENTION_NODE",NULL,m_pIntentionNode);
	// TransformNode pointer for the pointing ray
	VistaTransformNode* pTransNode;
	// Create the transform node
	pTransNode = pSG->NewTransformNode( m_pIntentionNode );
	// Perhaps do some tunig here
	// Set the rotation
	pTransNode->SetRotation( VistaQuaternion( VistaEulerAngles( -0.5f*Vista::Pi, 0, 0 ) ) );
	// Set the translation
	//pTransNode->SetTranslation(0.0f, 0.0f, 32.0f );
	// create the cone
	VistaGeometryFactory oGeometryFactory(pSG);
	VistaGeometry* cone = oGeometryFactory.CreateCone(0.01f, 0.001f, 64.0f);
	pSG->NewGeomNode(pTransNode,cone);
	//pSG->NewGeomNode(pTransNode,cone);

	// Create the SelectionHandler
	new SelectionHandler( vistaSystem.GetEventManager(),
		vistaSystem.GetInteractionManager()->GetRoleId( "WORLD_POINTER" ),
		pSG );
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int main(int argc, char **argv)
{
	try
	{
		VistaSystem oVistaSystem;

		std::list<std::string> liSearchPaths;
		liSearchPaths.push_back( "configfiles/" );
		liSearchPaths.push_back( "../12IntentionSelect/configfiles/" ); // for bin folder
		oVistaSystem.SetIniSearchPaths(liSearchPaths);

		if(oVistaSystem.Init(argc, argv))
		{

			if ( oVistaSystem.GetDisplayManager()->GetDisplaySystem(0)==0 )
				VISTA_THROW("No DisplaySystem found",1);

			// create the scene, a number of boxes
			CreateScene( oVistaSystem );

			// when we have the scene, we setup the
			// interaction
			CreateInteraction( oVistaSystem );

			oVistaSystem.GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle(argv[0]);

			// done, run
			oVistaSystem.Run();
		}
	}
	catch ( VistaExceptionBase &eException )
	{
		eException.PrintException();
	}
}


