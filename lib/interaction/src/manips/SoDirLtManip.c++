/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoDirectionalLightManip
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SoDB.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/draggers/SoDirectionalLightDragger.h>

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>

SO_NODE_SOURCE(SoDirectionalLightManip);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoDirectionalLightManip::SoDirectionalLightManip()
//
////////////////////////////////////////////////////////////////////////
{
    children = new SoChildList(this);

    SO_NODE_CONSTRUCTOR(SoDirectionalLightManip);

    isBuiltIn = TRUE;

    // Create the field sensors
    directionFieldSensor 
	= new SoFieldSensor(&SoDirectionalLightManip::fieldSensorCB, this);
    directionFieldSensor->setPriority(0);
    directionFieldSensor->attach(&direction);

    colorFieldSensor 
	= new SoFieldSensor(&SoDirectionalLightManip::fieldSensorCB, this);
    colorFieldSensor->setPriority(0);
    colorFieldSensor->attach(&color);

    setDragger( new SoDirectionalLightDragger );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoDirectionalLightManip::~SoDirectionalLightManip()
//
////////////////////////////////////////////////////////////////////////
{
    // Important to do this because dragger may have callbacks
    // to this node.
    setDragger(NULL);

    if (directionFieldSensor )
        delete directionFieldSensor;
    if (colorFieldSensor )
        delete colorFieldSensor;
    delete children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the dragger to be the given node...
//
void
SoDirectionalLightManip::setDragger( SoDragger *newDragger )
//
////////////////////////////////////////////////////////////////////////
{
    SoDragger *oldDragger = getDragger();
    if ( oldDragger ) {
	oldDragger->removeValueChangedCallback(
				&SoDirectionalLightManip::valueChangedCB,this);
	children->remove(0);
    }
	
    if (newDragger) {
	if (children->getLength() > 0)
	    children->set(0, newDragger );
	else 
	    children->append( newDragger );
	// Call the fieldSensorCB to transfer our values into the
	// new dragger.
	SoDirectionalLightManip::fieldSensorCB( this, NULL );
	newDragger->addValueChangedCallback(
		&SoDirectionalLightManip::valueChangedCB, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the dragger to be the given node...
//
SoDragger *
SoDirectionalLightManip::getDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if ( children->getLength() > 0 ) {
	SoNode *n = (*children)[0];
	if (n->isOfType( SoDragger::getClassTypeId() ))
	    return ( (SoDragger *) n );
#ifdef DEBUG
	else {
	    SoDebugError::post("SoDirectionalLightManip::getDragger",
	    "Child is not a dragger!");
	}
#endif
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces the tail of the path with this manipulator.
//
//    [1] Tail of fullpath must be correct type, or we return.
//    [2] If path has a nodekit, we try to use setPart() to insert manip.
//    otherwise:
//    [3] Path must be long enough, or we return without replacing.
//    [4] Second to last node must be a group, or we return without replacing.
//    [5] Copy values from node we are replacing into this manip
//    [6] Replace this manip as the child.
//    [7] Do not ref or unref anything. Assume that the user knows what 
//        they're doing.
//    [8] Do not fiddle with either node's field connections. Assume that the
//        user knows what they're doing.
//
SbBool
SoDirectionalLightManip::replaceNode( SoPath *p )
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fullP = (SoFullPath *) p;

    SoNode     *fullPTail = fullP->getTail();
    if (fullPTail->isOfType(SoDirectionalLight::getClassTypeId()) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoDirectionalLightManip::replaceNode", 
	    "End of path is not an SoDirectionalLight");
#endif
	return FALSE;
    }

    SoNode *pTail = p->getTail();
    if ( pTail->isOfType(SoBaseKit::getClassTypeId())) {

	// Okay, we've got a nodekit here! Let's do this right...
	// If fullPTail is a part in the kit, then we've got to follow
	// protocol and let the kit set the part itself.
	SoBaseKit *lastKit = (SoBaseKit *) ((SoNodeKitPath *)p)->getTail();
	SbString partName = lastKit->getPartString(p);
	if ( partName != "" ) {
	    SoDirectionalLight *oldPart 
		= (SoDirectionalLight *) lastKit->getPart(partName, TRUE); 
	    if (oldPart != NULL) {
		transferFieldValues( oldPart, this );
		lastKit->setPart(partName, this);
		return TRUE;
	    }
	    else {
		// Although the part's there, we couldn't get at it.
		// Some kind of problem going on
		return FALSE;
	    }
	}
	// If it's not a part, that means it's contained within a subgraph
	// underneath a part. For example, it's within the 'contents'
	// separator of an SoWrapperKit. In that case, the nodekit doesn't
	// care and we just continue into the rest of the method...
    }

    if ( fullP->getLength() < 2 ) {
#ifdef DEBUG
	SoDebugError::post("SoDirectionalLightManip::replaceNode",
	"Path is too short!");
#endif
	return FALSE;
    }

    SoNode      *parent = fullP->getNodeFromTail( 1 );
    if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoDirectionalLightManip::replaceNode",
	"Parent node is not a group.!");
#endif
	return FALSE;
    }

    ref();

    transferFieldValues( ((SoDirectionalLight *)fullPTail), this );

    ((SoGroup *)parent)->replaceChild( fullPTail, this );

    unrefNoDelete();
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces the tail of the path (which should be this manipulator)
//    with the given SoDirectionalLight node.
//
//    [1] Tail of fullpath must be this node, or we return.
//    [2] If path has a nodekit, we try to use setPart() to insert new node.
//    otherwise:
//    [3] Path must be long enough, or we return without replacing.
//    [4] Second to last node must be a group, or we return without replacing.
//    [5] Copy values from node we are replacing into this manip
//    [6] Replace this manip as the child.
//    [7] Do not ref or unref anything. Assume that the user knows what 
//        they're doing.
//    [8] Do not fiddle with either node's field connections. Assume that the
//        user knows what they're doing.
//
SbBool
SoDirectionalLightManip::replaceManip( SoPath *p, 
					SoDirectionalLight *newOne ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fullP = (SoFullPath *) p;

    SoNode     *fullPTail = fullP->getTail();
    if ((SoDirectionalLightManip *)fullPTail != this ) {
#ifdef DEBUG
	SoDebugError::post("SoTransformManip::replaceManip",
	"Child to replace is not this manip!");
#endif
	return FALSE;
    }

    SoNode *pTail = p->getTail();
    if ( pTail->isOfType(SoBaseKit::getClassTypeId())) {

	// Okay, we've got a nodekit here! Let's do this right...
	// If fullPTail is a part in the kit, then we've got to follow
	// protocol and let the kit set the part itself.
	SoBaseKit *lastKit = (SoBaseKit *) ((SoNodeKitPath *)p)->getTail();
	SbString partName = lastKit->getPartString(p);
	if ( partName != "" ) {

	    if (newOne == NULL)
		newOne = new SoDirectionalLight;
	    newOne->ref();

	    transferFieldValues( this, newOne );

	    lastKit->setPart(partName, newOne);
	    newOne->unrefNoDelete();
	    return TRUE;
	}
	// If it's not a part, that means it's contained within a subgraph
	// underneath a part. For example, it's within the 'contents'
	// separator of an SoWrapperKit. In that case, the nodekit doesn't
	// care and we just continue on through...
    }

    if ( fullP->getLength() < 2 ) {
#ifdef DEBUG
	SoDebugError::post("SoDirectionalLightManip::replaceManip",
	"Path is too short!");
#endif
	return FALSE;
    }

    SoNode      *parent = fullP->getNodeFromTail( 1 );
    if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoDirectionalLightManip::replaceManip",
	"Parent node is not a group.!");
#endif
	return FALSE;
    }

    if (newOne == NULL)
	newOne = new SoDirectionalLight;
    newOne->ref();

    transferFieldValues( this, newOne );

    ((SoGroup *)parent)->replaceChild((SoDirectionalLightManip *)this, newOne );

    newOne->unrefNoDelete();

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to also copy the dragger.
//
// Use: protected, virtual

void
SoDirectionalLightManip::copyContents(const SoFieldContainer *fromFC,
				      SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Do the usual stuff
    SoDirectionalLight::copyContents(fromFC, copyConnections);

    // Copy the dragger
    SoDirectionalLightManip *origManip = (SoDirectionalLightManip *) fromFC;
    setDragger((SoDragger *) origManip->getDragger()->copy(copyConnections));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the child list...
//
SoChildList *
SoDirectionalLightManip::getChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    return children;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Transfers field values from one node to another...
//
void
SoDirectionalLightManip::transferFieldValues( const SoDirectionalLight *from,
					SoDirectionalLight *to )
//
////////////////////////////////////////////////////////////////////////
{
    SoDirectionalLightManip *m = NULL;
    if (to->isOfType( SoDirectionalLightManip::getClassTypeId() ) )
	m = (SoDirectionalLightManip *) to;
    
    if (m) {
	// detach the field sensors.
	m->directionFieldSensor->detach();
	m->colorFieldSensor->detach();
    }
    if (to->on.getValue()        != from->on.getValue())
	to->on                    = from->on.getValue();
    if (to->intensity.getValue() != from->intensity.getValue())
	to->intensity             = from->intensity.getValue();
    if (to->color.getValue()     != from->color.getValue())
	to->color                 = from->color.getValue();
    if (to->direction.getValue() != from->direction.getValue())
	to->direction             = from->direction.getValue();

    if (m) {
	// call the callback, then reattach the field sensors.
	SoDirectionalLightManip::fieldSensorCB( m, NULL );
	m->directionFieldSensor->attach(&m->direction);
	m->colorFieldSensor->attach(&m->color);
    }
}


void 
SoDirectionalLightManip::doAction( SoAction *action )
{
    int         numIndices;
    const int   *indices;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);
    else
	children->traverse(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements get matrix action.
//
// Use: protected

void
SoDirectionalLightManip::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int         numIndices;
    const int   *indices;

    switch (action->getPathCode(numIndices, indices)) {

	case SoAction::NO_PATH:
	    break;

	case SoAction::IN_PATH:
	    children->traverse(action, 0, indices[numIndices-1]);
	    break;

	case SoAction::BELOW_PATH:
	    break;

	case SoAction::OFF_PATH:
	    children->traverse(action);
	    break;
    }
}

// These functions implement all actions for nodekits.
void 
SoDirectionalLightManip::callback( SoCallbackAction *action )
{ 
    SoDirectionalLight::callback(action);
    SoDirectionalLightManip::doAction( action );
}

void 
SoDirectionalLightManip::getBoundingBox( SoGetBoundingBoxAction *action )
{ 
    SbVec3f     totalCenter(0,0,0);
    int         numCenters = 0;
    int         numIndices;
    const int   *indices;
    int         lastChild;

    // Traverse this as a directional light
    SoDirectionalLight::getBoundingBox(action);

    // Now, as a separator...
    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	lastChild = indices[numIndices - 1];
    else
	lastChild = getNumChildren() - 1;

    // Traverse the children.
    for (int i = 0; i <= lastChild; i++) {
	children->traverse(action, i, i);
	if (action->isCenterSet()) {
	    totalCenter += action->getCenter();
	    numCenters++;
	    action->resetCenter();
	}
    }

    // Now, set the center to be the average:
    if (numCenters != 0)
	action->setCenter(totalCenter / numCenters, FALSE);
}

void 
SoDirectionalLightManip::GLRender( SoGLRenderAction *action )
{
  SoDirectionalLight::GLRender(action);
  SoDirectionalLightManip::doAction( action ); 
}

void 
SoDirectionalLightManip::handleEvent( SoHandleEventAction *action )
{ 
  SoDirectionalLight::handleEvent(action);
  SoDirectionalLightManip::doAction( action ); 
}

void 
SoDirectionalLightManip::pick( SoPickAction *action )
{ 
  SoDirectionalLight::pick(action);
  SoDirectionalLightManip::doAction( action ); 
}

void 
SoDirectionalLightManip::search( SoSearchAction *action )
{ 
    // First see if the caller is searching for this
    SoDirectionalLight::search(action);
//MANIPS DONT TRAVERSE CHILDREN DURING SEARCH    SoDirectionalLightManip::doAction( action );
}

void
SoDirectionalLightManip::valueChangedCB( void *inManip, SoDragger *inDragger )
{
    if (inDragger == NULL)
	return;

    SoDirectionalLightManip *manip = (SoDirectionalLightManip *) inManip;

    SbMatrix motMat = inDragger->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    manip->directionFieldSensor->detach();

    SbVec3f newDir(0,0,-1);
    SbMatrix rotMat;
    rotMat.setRotate(rot);
    rotMat.multDirMatrix(newDir,newDir);

    if ( manip->direction.getValue() != newDir )
	manip->direction = newDir;

    // Reconnect the field sensors
    manip->directionFieldSensor->attach( &(manip->direction));
}

void
SoDirectionalLightManip::fieldSensorCB( void *inManip, SoSensor *inSensor)
{
    SoDirectionalLightManip *manip   = (SoDirectionalLightManip *) inManip;
    SoDragger               *dragger = manip->getDragger();

    if (dragger == NULL)
	return;

    SoField *trigF = NULL;
    if (inSensor)
	trigF = ((SoDataSensor *)inSensor)->getTriggerField();

    // If inSensor is NULL or the trigger field is NULL, do the color stuff and
    // matrix stuff, since we don't know what changed.
    SbBool doColor = TRUE;
    SbBool doMatrix = TRUE;

    // But if this is triggered by a field, figure out what we need to do.
    if (trigF ) {
	if ( trigF == &manip->direction )
	    doColor = FALSE;
	if ( trigF == &manip->color )
	    doMatrix = FALSE;
    }

    // If the light color changed, then copy the color into the 
    // material part of the dragger.
    // Cut the color in half, and give half each to diffuse and emissive.
    if ( doColor ) {
	SoMaterial *mtl = SO_GET_PART(dragger, "material", SoMaterial);
	if (mtl != NULL) {
	    SbColor halfColor = 0.5 * manip->color.getValue();
	    mtl->diffuseColor = halfColor;
	    mtl->emissiveColor = halfColor;
	}
    }

    if ( doMatrix ) {
	SbVec3f trans(0,0,0);
	SbVec3f scale(1,1,1);
	SbRotation rot( SbVec3f(0,0,-1), manip->direction.getValue());

	// See if dragger has a translation field to use instead of (0,0,0)...
	SoField *f;
	SoType  fType = SoSFVec3f::getClassTypeId();
	if ( (f = dragger->getField("translation")) && f->isOfType( fType ) )
	    trans = ((SoSFVec3f *)f)->getValue();

	SbMatrix newMat;
	newMat.setTransform( trans, rot, scale );

	dragger->setMotionMatrix(newMat);
    }
}

