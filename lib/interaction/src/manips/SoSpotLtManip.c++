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

////////////////////////////////////////////////////////////////////////
//
// Description:
//
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <Inventor/SoDB.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/manips/SoSpotLightManip.h>
#include <Inventor/draggers/SoSpotLightDragger.h>

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>

SO_NODE_SOURCE(SoSpotLightManip);

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Constructors. Each constructor calls constructorSub(), which
//  does work that is common to each.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

// Default constructor.
SoSpotLightManip::SoSpotLightManip()
{
    children = new SoChildList(this);

    SO_NODE_CONSTRUCTOR(SoSpotLightManip);

    isBuiltIn = TRUE;

    // Create the field sensors
    locationFieldSensor 
	= new SoFieldSensor(&SoSpotLightManip::fieldSensorCB, this);
    locationFieldSensor->setPriority(0);
    locationFieldSensor->attach(&location);

    directionFieldSensor 
	= new SoFieldSensor(&SoSpotLightManip::fieldSensorCB, this);
    directionFieldSensor->setPriority(0);
    directionFieldSensor->attach(&direction);

    angleFieldSensor 
	= new SoFieldSensor(&SoSpotLightManip::fieldSensorCB, this);
    angleFieldSensor->setPriority(0);
    angleFieldSensor->attach(&cutOffAngle);

    colorFieldSensor 
	= new SoFieldSensor(&SoSpotLightManip::fieldSensorCB, this);
    colorFieldSensor->setPriority(0);
    colorFieldSensor->attach(&color);

    setDragger( new SoSpotLightDragger );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoSpotLightManip::~SoSpotLightManip()
//
////////////////////////////////////////////////////////////////////////
{
    // Important to do this because dragger may have callbacks
    // to this node.
    setDragger(NULL);

    if (locationFieldSensor )
        delete locationFieldSensor;
    if (directionFieldSensor )
	delete directionFieldSensor;
    if (angleFieldSensor )
	delete angleFieldSensor;
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
SoSpotLightManip::setDragger( SoDragger *newDragger )
//
////////////////////////////////////////////////////////////////////////
{
    SoDragger *oldDragger = getDragger();
    if ( oldDragger ) {
	oldDragger->removeValueChangedCallback(
				&SoSpotLightManip::valueChangedCB,this);
	children->remove(0);
    }
	
    if (newDragger) {
	if (children->getLength() > 0)
	    children->set(0, newDragger );
	else 
	    children->append( newDragger );
	// Call the fieldSensorCB to transfer our values into the
	// new dragger.
	SoSpotLightManip::fieldSensorCB( this, NULL );

	newDragger->addValueChangedCallback(
		&SoSpotLightManip::valueChangedCB, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the dragger to be the given node...
//
SoDragger *
SoSpotLightManip::getDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if ( children->getLength() > 0 ) {
	SoNode *n = (*children)[0];
	if (n->isOfType( SoDragger::getClassTypeId() ))
	    return ( (SoDragger *) n );
#ifdef DEBUG
	else {
	    SoDebugError::post("SoSpotLightManip::getDragger",
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
SoSpotLightManip::replaceNode( SoPath *p )
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fullP = (SoFullPath *) p;

    SoNode     *fullPTail = fullP->getTail();
    if (fullPTail->isOfType(SoSpotLight::getClassTypeId()) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoSpotLightManip::replaceNode", 
	    "End of path is not an SoSpotLight");
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
	    SoSpotLight *oldPart 
		= (SoSpotLight *) lastKit->getPart(partName, TRUE); 
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
	SoDebugError::post("SoSpotLightManip::replaceNode",
	"Path is too short!");
#endif
	return FALSE;
    }

    SoNode      *parent = fullP->getNodeFromTail( 1 );
    if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoSpotLightManip::replaceNode",
	"Parent node is not a group.!");
#endif
	return FALSE;
    }

    ref();

    transferFieldValues( ((SoSpotLight *)fullPTail), this );

    ((SoGroup *)parent)->replaceChild( fullPTail, this );

    unrefNoDelete();
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces the tail of the path (which should be this manipulator)
//    with the given SoSpotLight node.
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
SoSpotLightManip::replaceManip( SoPath *p, 
					SoSpotLight *newOne ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fullP = (SoFullPath *) p;

    SoNode     *fullPTail = fullP->getTail();
    if ((SoSpotLightManip *)fullPTail != this ) {
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
		newOne = new SoSpotLight;
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
	SoDebugError::post("SoSpotLightManip::replaceManip",
	"Path is too short!");
#endif
	return FALSE;
    }

    SoNode      *parent = fullP->getNodeFromTail( 1 );
    if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoSpotLightManip::replaceManip",
	"Parent node is not a group.!");
#endif
	return FALSE;
    }

    if (newOne == NULL)
	newOne = new SoSpotLight;
    newOne->ref();

    transferFieldValues( this, newOne );

    ((SoGroup *)parent)->replaceChild((SoSpotLightManip *)this, newOne );

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
SoSpotLightManip::copyContents(const SoFieldContainer *fromFC,
			       SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Do the usual stuff
    SoSpotLight::copyContents(fromFC, copyConnections);

    // Copy the dragger
    SoSpotLightManip *origManip = (SoSpotLightManip *) fromFC;
    setDragger((SoDragger *) origManip->getDragger()->copy(copyConnections));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the child list...
//
SoChildList *
SoSpotLightManip::getChildren() const
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
SoSpotLightManip::transferFieldValues( const SoSpotLight *from,
					SoSpotLight *to )
//
////////////////////////////////////////////////////////////////////////
{
    SoSpotLightManip *m = NULL;
    if (to->isOfType( SoSpotLightManip::getClassTypeId() ) )
	m = (SoSpotLightManip *) to;
    if (m) {
	// detach the field sensors.
	m->locationFieldSensor->detach();
	m->directionFieldSensor->detach();
	m->angleFieldSensor->detach();
	m->colorFieldSensor->detach();
    }
    if (to->on.getValue()          != from->on.getValue())
	to->on                      = from->on.getValue();
    if (to->intensity.getValue()   != from->intensity.getValue())
	to->intensity               = from->intensity.getValue();
    if (to->color.getValue()       != from->color.getValue())
	to->color                   = from->color.getValue();
    if (to->location.getValue()    != from->location.getValue())
	to->location                = from->location.getValue();
    if (to->direction.getValue()   != from->direction.getValue())
	to->direction               = from->direction.getValue();
    if (to->dropOffRate.getValue() != from->dropOffRate.getValue())
	to->dropOffRate             = from->dropOffRate.getValue();
    if (to->cutOffAngle.getValue() != from->cutOffAngle.getValue())
	to->cutOffAngle             = from->cutOffAngle.getValue();
    if (m) {
	// call the callback, then reattach the field sensors.
	SoSpotLightManip::fieldSensorCB( m, NULL );
	m->locationFieldSensor->attach(&m->location);
	m->directionFieldSensor->attach(&m->direction);
	m->angleFieldSensor->attach(&m->cutOffAngle);
	m->colorFieldSensor->attach(&m->color);
    }
}


void 
SoSpotLightManip::doAction( SoAction *action )
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
SoSpotLightManip::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int         numIndices;
    const int   *indices;

    switch (action->getPathCode(numIndices, indices)) {

	case SoAction::NO_PATH:
	    break;

	case SoAction::IN_PATH:
	    children->traverse(action, 0, indices[numIndices - 1]);
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
SoSpotLightManip::callback( SoCallbackAction *action )
{ 
    SoSpotLight::callback(action);
    SoSpotLightManip::doAction( action );
}

void 
SoSpotLightManip::getBoundingBox( SoGetBoundingBoxAction *action )
{ 
    SbVec3f     totalCenter(0,0,0);
    int         numCenters = 0;
    int         numIndices;
    const int   *indices;
    int         lastChild;

    // Traverse this as a spot light
    SoSpotLight::getBoundingBox(action);

    // Now, as a separator
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
SoSpotLightManip::GLRender( SoGLRenderAction *action )
{
  SoSpotLight::GLRender(action);
  SoSpotLightManip::doAction( action ); 
}

void 
SoSpotLightManip::handleEvent( SoHandleEventAction *action )
{ 
  SoSpotLight::handleEvent(action);
  SoSpotLightManip::doAction( action ); 
}

void 
SoSpotLightManip::pick( SoPickAction *action )
{ 
  SoSpotLight::pick(action);
  SoSpotLightManip::doAction( action ); 
}

void 
SoSpotLightManip::search( SoSearchAction *action )
{ 
    // First see if the caller is searching for this
    SoSpotLight::search(action);
//MANIPS DONT TRAVERSE CHILDREN DURING SEARCH    SoSpotLightManip::doAction( action );
}

void
SoSpotLightManip::valueChangedCB( void *inManip, SoDragger *inDragger )
{
    if (inDragger == NULL)
	return;

    SoSpotLightManip   *manip       = (SoSpotLightManip *) inManip;

    SbMatrix motMat = inDragger->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    manip->directionFieldSensor->detach();
    manip->locationFieldSensor->detach();
    manip->angleFieldSensor->detach();

    SbVec3f newDir(0,0,-1);
    SbMatrix rotMat;
    rotMat.setRotate(rot);
    rotMat.multDirMatrix(newDir,newDir);

    if ( manip->direction.getValue() != newDir )
	manip->direction = newDir;
    if ( manip->location.getValue() != trans )
	manip->location = trans;

    // See if inDragger has an angle field...
	SoField *f;
	SoType  fType = SoSFFloat::getClassTypeId();
	if ( (f = inDragger->getField("angle")) && f->isOfType( fType ) ) {
	    float draggerAngle = ((SoSFFloat *)f)->getValue();
	    if ( manip->cutOffAngle.getValue() != draggerAngle )
		manip->cutOffAngle = draggerAngle;
	}

    // Reconnect the field sensors
    manip->directionFieldSensor->attach( &(manip->direction));
    manip->locationFieldSensor->attach( &(manip->location));
    manip->angleFieldSensor->attach( &(manip->cutOffAngle));
}

void
SoSpotLightManip::fieldSensorCB( void *inManip, SoSensor *inSensor)
{
    SoSpotLightManip *manip   = (SoSpotLightManip *) inManip;
    SoDragger        *dragger = manip->getDragger();

    if (dragger == NULL)
	return;

    SoField *trigF = NULL;
    if (inSensor)
	trigF = ((SoDataSensor *)inSensor)->getTriggerField();

    // If inSensor is NULL or the trigger field is NULL, do the color stuff and
    // matrix stuff, since we don't know what changed.
    SbBool doColor  = TRUE;
    SbBool doMatrix = TRUE;
    SbBool doAngle  = TRUE;

    // But if this is triggered by a field, figure out what we need to do.
    if (trigF ) {
	if ( trigF == &manip->location || trigF == &manip->direction ) {
	    doColor = FALSE;
	    doAngle = FALSE;
	}
	if ( trigF == &manip->cutOffAngle ) {
	    doColor  = FALSE;
	    doMatrix = FALSE;
	}
	if ( trigF == &manip->color ) {
	    doMatrix = FALSE;
	    doAngle  = FALSE;
	}
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

    // Save the cut off angle, since it can get screwed up when the
    // matrix on the dragger is set:
    float myCutOff = manip->cutOffAngle.getValue();

    if ( doMatrix ) {
	SbVec3f trans = manip->location.getValue();
	SbVec3f scale(1,1,1);
	SbRotation rot( SbVec3f(0,0,-1), manip->direction.getValue());
	SbMatrix newMat;
	newMat.setTransform( trans, rot, scale );

	dragger->setMotionMatrix(newMat);
    }

    // Restore the cut off angle:
    if (manip->cutOffAngle.getValue() != myCutOff)
        manip->cutOffAngle = myCutOff;

    // If the angle changed, copy our angle into the dragger.
    if ( doAngle ) {
	// See if dragger has an angle field...
	SoField *f;
	SoType  fType = SoSFFloat::getClassTypeId();
	if ( (f = dragger->getField( "angle" )) && f->isOfType( fType ) )
	    ((SoSFFloat *)f)->setValue(  manip->cutOffAngle.getValue() );
    }
}
