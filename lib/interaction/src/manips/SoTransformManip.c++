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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoTransformManip
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <SoDebug.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/manips/SoTransformManip.h>

SO_NODE_SOURCE(SoTransformManip);

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Constructor.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

// Default constructor.
SoTransformManip::SoTransformManip()
{
    children = new SoChildList(this);

    SO_NODE_CONSTRUCTOR(SoTransformManip);

    isBuiltIn = TRUE;

    // Create the field sensors
    rotateFieldSensor = new SoFieldSensor(&SoTransformManip::fieldSensorCB, 
					    this);
    translFieldSensor = new SoFieldSensor(&SoTransformManip::fieldSensorCB,
					   this);
    scaleFieldSensor = new SoFieldSensor(&SoTransformManip::fieldSensorCB,
					   this);
    centerFieldSensor = new SoFieldSensor(&SoTransformManip::fieldSensorCB,
					   this);
    scaleOrientFieldSensor = new SoFieldSensor(&SoTransformManip::fieldSensorCB,
					   this);

    rotateFieldSensor->setPriority(0);
    translFieldSensor->setPriority(0);
    scaleFieldSensor->setPriority(0);
    centerFieldSensor->setPriority(0);
    scaleOrientFieldSensor->setPriority(0);

    rotateFieldSensor->attach(&rotation);
    translFieldSensor->attach(&translation);
    scaleFieldSensor->attach(&scaleFactor);
    centerFieldSensor->attach(&center);
    scaleOrientFieldSensor->attach(&scaleOrientation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoTransformManip::~SoTransformManip()
//
////////////////////////////////////////////////////////////////////////
{
    // Important to do this because dragger may have callbacks
    // to this node.
    setDragger(NULL);

    if (rotateFieldSensor)
        delete rotateFieldSensor;
    if (translFieldSensor)
        delete translFieldSensor;
    if (scaleFieldSensor)
        delete scaleFieldSensor;
    if (centerFieldSensor)
        delete centerFieldSensor;
    if (scaleOrientFieldSensor)
        delete scaleOrientFieldSensor;
    delete children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the dragger to be the given node...
//
void
SoTransformManip::setDragger( SoDragger *newDragger )
//
////////////////////////////////////////////////////////////////////////
{
    SoDragger *oldDragger = getDragger();
    if ( oldDragger ) {
	oldDragger->removeValueChangedCallback(
				&SoTransformManip::valueChangedCB,this);
	children->remove(0);
    }
	
    if (newDragger) {
	if (children->getLength() > 0)
	    children->set(0, newDragger );
	else 
	    children->append( newDragger );
	// Call the fieldSensorCB to transfer our values into the
	// new dragger.
	SoTransformManip::fieldSensorCB( this, NULL );
	newDragger->addValueChangedCallback(
				&SoTransformManip::valueChangedCB,this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the dragger to be the given node...
//
SoDragger *
SoTransformManip::getDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if ( children->getLength() > 0 ) {
	SoNode *n = (*children)[0];
	if (n->isOfType( SoDragger::getClassTypeId() ))
	    return ( (SoDragger *) n );
#ifdef DEBUG
	else {
	    SoDebugError::post("SoTransformManip::getDragger",
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
SoTransformManip::replaceNode( SoPath *p )
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fullP = (SoFullPath *) p;

    SoNode     *fullPTail = fullP->getTail();
    if (fullPTail->isOfType(SoTransform::getClassTypeId()) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoTransformManip::replaceNode", 
	    "End of path is not an SoTransform");
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
	    SoTransform *oldPart 
		= (SoTransform *) lastKit->getPart(partName, TRUE); 
	    if (oldPart != NULL) {
		oldPart->ref();
		lastKit->setPart(partName, this);

		// If the surroundScale exists, we must update it now.
		SoSurroundScale *ss = SO_CHECK_PART( this->getDragger(), 
					     "surroundScale", SoSurroundScale );
		if ( ss != NULL ) {
		    // If we have a surround scale node, invalidate it
		    // and force it to calculate its new matrix 
		    // by running a getMatrix action.  This must be done
		    // or 'transferFieldValues' will incorrectly set any
		    // internal matrices that depend on surroundScale.
		    // (SoCenterballManip is an example of a manip that
		    // requires this).
		    ss->invalidate();
		    static SoGetBoundingBoxAction *kitBba = NULL;
		    if (kitBba == NULL)
			kitBba = new SoGetBoundingBoxAction(SbViewportRegion());
		    kitBba->apply(p);
		}

		transferFieldValues( oldPart, this );
		oldPart->unref();

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
	SoDebugError::post("SoTransformManip::replaceNode",
	"Path is too short!");
#endif
	return FALSE;
    }

    SoNode      *parent = fullP->getNodeFromTail( 1 );
    if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoTransformManip::replaceNode",
	"Parent node is not a group.!");
#endif
	return FALSE;
    }

    ref();

    // We need to ref fullPTail because it can't disappear here.
    // We've got to transfer its field values later on.
    fullPTail->ref();

    ((SoGroup *)parent)->replaceChild( fullPTail, this );

    // If the surroundScale exists, we must update it now.
    SoSurroundScale *ss = SO_CHECK_PART( this->getDragger(), 
					 "surroundScale", SoSurroundScale );
    if ( ss != NULL ) {
	// If we have a surround scale node, invalidate it
	// and force it to calculate its new matrix 
	// by running a getMatrix action.  This must be done
	// or 'transferFieldValues' will incorrectly set any
	// internal matrices that depend on surroundScale.
	// (SoCenterballManip is an example of a manip that
	// requires this).
	ss->invalidate();
	static SoGetBoundingBoxAction *bba = NULL;
	if (bba == NULL)
	    bba = new SoGetBoundingBoxAction(SbViewportRegion());
	bba->apply(p);
    }

    transferFieldValues( ((SoTransform *)fullPTail), this );

    // Now that we've extracted the values from the fullPTail we
    // can unref it.
    fullPTail->unref();

    unrefNoDelete();
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces the tail of the path (which should be this manipulator)
//    with the given SoTransform node.
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
SoTransformManip::replaceManip( SoPath *p, SoTransform *newOne ) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fullP = (SoFullPath *) p;

    SoNode     *fullPTail = fullP->getTail();
    if ((SoTransformManip *)fullPTail != this ) {
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
		newOne = new SoTransform;
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
	SoDebugError::post("SoTransformManip::replaceManip",
	"Path is too short!");
#endif
	return FALSE;
    }

    SoNode      *parent = fullP->getNodeFromTail( 1 );
    if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
	SoDebugError::post("SoTransformManip::replaceManip",
	"Parent node is not a group.!");
#endif
	return FALSE;
    }

    if (newOne == NULL)
	newOne = new SoTransform;
    newOne->ref();

    transferFieldValues( this, newOne );

    ((SoGroup *)parent)->replaceChild((SoTransformManip *)this, newOne );

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
SoTransformManip::copyContents(const SoFieldContainer *fromFC,
			       SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Do the usual stuff
    SoTransform::copyContents(fromFC, copyConnections);

    // Copy the dragger
    SoTransformManip *origManip = (SoTransformManip *) fromFC;
    setDragger((SoDragger *) origManip->getDragger()->copy(copyConnections));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the child list...
//
SoChildList *
SoTransformManip::getChildren() const
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
SoTransformManip::transferFieldValues( const SoTransform *from,
					SoTransform *to )
//
////////////////////////////////////////////////////////////////////////
{
    SoTransformManip *m = NULL;
    if (to->isOfType( SoTransformManip::getClassTypeId() ) )
	m = (SoTransformManip *) to;
    if (m) {
	// detach the field sensors.
	m->rotateFieldSensor->detach();
	m->translFieldSensor->detach();
	m->scaleFieldSensor->detach();
	m->centerFieldSensor->detach();
	m->scaleOrientFieldSensor->detach();
    }
    if (to->rotation.getValue()           != from->rotation.getValue())
	to->rotation                       = from->rotation.getValue();
    if (to->translation.getValue()        != from->translation.getValue())
	to->translation                    = from->translation.getValue();
    if (to->scaleFactor.getValue()        != from->scaleFactor.getValue())
	to->scaleFactor                    = from->scaleFactor.getValue();
    if (to->scaleOrientation.getValue()   != from->scaleOrientation.getValue())
	to->scaleOrientation               = from->scaleOrientation.getValue();
    if (to->center.getValue()             != from->center.getValue())
	to->center                         = from->center.getValue();
    if (m) {
	// call the callback, then reattach the field sensors.
	if (SoDebug::GetEnv("IV_DEBUG_TRANSFORM_MANIP_FIELDS")) {
	    fprintf(stderr,"before:\n");
	    SbVec3f t = m->translation.getValue();
	    SbVec3f s = m->scaleFactor.getValue();
	    SbVec3f c = m->center.getValue();
	    fprintf(stderr,"translation = %f %f %f\n", t[0], t[1], t[2]);
	    fprintf(stderr,"scale = %f %f %f\n", s[0], s[1], s[2]);
	    fprintf(stderr,"center = %f %f %f\n", c[0], c[1], c[2]);
	}
	SoTransformManip::fieldSensorCB( m, NULL );
	if (SoDebug::GetEnv("IV_DEBUG_TRANSFORM_MANIP_FIELDS")) {
	    fprintf(stderr,"after:\n");
	    SbVec3f t = m->translation.getValue();
	    SbVec3f s = m->scaleFactor.getValue();
	    SbVec3f c = m->center.getValue();
	    fprintf(stderr,"translation = %f %f %f\n", t[0], t[1], t[2]);
	    fprintf(stderr,"scale = %f %f %f\n", s[0], s[1], s[2]);
	    fprintf(stderr,"center = %f %f %f\n", c[0], c[1], c[2]);
	}
	m->rotateFieldSensor->attach( &m->rotation);
	m->translFieldSensor->attach( &m->translation);
	m->scaleFieldSensor->attach( &m->scaleFactor );
	m->centerFieldSensor->attach(&m->center);
	m->scaleOrientFieldSensor->attach( &m->scaleOrientation );

    }
}

void 
SoTransformManip::doAction( SoAction *action )
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
SoTransformManip::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int         numIndices;
    const int   *indices;

    // Note: this is most unusual behavior!
    // 
    // What we have is a transform node that may sometimes find itself 
    // on a path. In the cases of NO_PATH and BELOW_PATH, we just return
    // the SoTransform matrix without traversing the kids. This is a cross
    // between SoGroup, which doesn't traverse, and SoTransform, which 
    // affects the matrix.
    // In the case of OFF_PATH, traverse the children first, like SoGroup,
    // then add the matrix, like SoTransform
    //
    // For IN_PATH, we do not want to affect the matrix, since its affect
    // occurs after the children are traversed, (at the 'back half' of 
    // traversal) and does not affect the nodes under the path.
    //
    switch (action->getPathCode(numIndices, indices)) {

	case SoAction::NO_PATH:
	    SoTransform::getMatrix(action);
	    break;

	case SoAction::IN_PATH:
	    children->traverse(action, 0, indices[numIndices - 1]);
	    break;

	case SoAction::BELOW_PATH:
	    SoTransform::getMatrix(action);
	    break;

	case SoAction::OFF_PATH:
	    children->traverse(action);
	    SoTransform::getMatrix(action);
	    break;

    }
}


// These functions implement all actions for nodekits.
void 
SoTransformManip::callback( SoCallbackAction *action )
{ 
    SoTransformManip::doAction( action );
    SoTransform::callback(action);
}

void 
SoTransformManip::getBoundingBox( SoGetBoundingBoxAction *action )
{ 
    SbVec3f     totalCenter(0,0,0);
    int         numCenters = 0;
    int         numIndices;
    const int   *indices;
    int         lastChild;

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

    // Traverse this as a directional light
    SoTransform::getBoundingBox(action);
    if (action->isCenterSet()) {
	totalCenter += action->getCenter();
	numCenters++;
	action->resetCenter();
    }

    // Now, set the center to be the average:
    if (numCenters != 0)
	action->setCenter(totalCenter / numCenters, FALSE);
}

void 
SoTransformManip::GLRender( SoGLRenderAction *action )
{
  SoTransformManip::doAction( action ); 
  SoTransform::GLRender(action);
}

void 
SoTransformManip::handleEvent( SoHandleEventAction *action )
{ 
  SoTransformManip::doAction( action ); 
  SoTransform::handleEvent(action);
}

void 
SoTransformManip::pick( SoPickAction *action )
{ 
  SoTransformManip::doAction( action ); 
  SoTransform::pick(action);
}

void 
SoTransformManip::search( SoSearchAction *action )
{ 
    // First see if the caller is searching for this
    SoTransform::search(action);
//MANIPS DONT TRAVERSE CHILDREN DURING SEARCH    SoTransformManip::doAction( action );
}

void
SoTransformManip::valueChangedCB( void *inManip, SoDragger *inDragger )
{
    if (inDragger == NULL)
	return;

    SoTransformManip *manip = (SoTransformManip *) inManip;

    SbMatrix motMat = inDragger->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    SbVec3f center = manip->center.getValue();

    // See if inDragger has a center field...
    SoField *f;
    SoType fType = SoSFVec3f::getClassTypeId();
    if ( (f = inDragger->getField("center")) && f->isOfType( fType ) )
	center = ((SoSFVec3f *)f)->getValue();

    motMat.getTransform( trans, rot, scale, scaleOrient, center );

    // Disconnect the field sensors
    manip->rotateFieldSensor->detach();
    manip->translFieldSensor->detach();
    manip->scaleFieldSensor->detach();
    manip->centerFieldSensor->detach();
    manip->scaleOrientFieldSensor->detach();

    if ( manip->rotation.getValue() != rot )
	manip->rotation = rot;
    if ( manip->translation.getValue() != trans )
	manip->translation = trans;
    if ( manip->scaleFactor.getValue() != scale )
	manip->scaleFactor = scale;
    if ( manip->center.getValue() != center )
	manip->center = center;
    if ( manip->scaleOrientation.getValue() != scaleOrient )
	manip->scaleOrientation = scaleOrient;

    // Reconnect the field sensors
    manip->rotateFieldSensor->attach( &(manip->rotation));
    manip->translFieldSensor->attach( &(manip->translation));
    manip->scaleFieldSensor->attach( &(manip->scaleFactor));
    manip->centerFieldSensor->attach( &(manip->center));
    manip->scaleOrientFieldSensor->attach( &(manip->scaleOrientation));
}

void
SoTransformManip::fieldSensorCB( void *inManip, SoSensor *)
{
    SoTransformManip *manip = (SoTransformManip *) inManip;
    SoDragger        *dragger = manip->getDragger();

    if (dragger == NULL)
	return;

    SbVec3f    trans       = manip->translation.getValue(); 
    SbVec3f    scale       = manip->scaleFactor.getValue(); 
    SbRotation rot         = manip->rotation.getValue(); 
    SbRotation scaleOrient = manip->scaleOrientation.getValue(); 
    SbVec3f    center      = manip->center.getValue(); 

    SbMatrix newMat;
    newMat.setTransform( trans, rot, scale, scaleOrient, center );

    // We may be setting two different things at once-- a matrix and 
    // a center field. Temporarily disable valueChangedCBs on dragger, 
    // and call them when we are all done.
    SbBool saveEnabled = dragger->enableValueChangedCallbacks(FALSE);

	// If dragger has a center field, set it.
	SoField *f;
	SoType  fType = SoSFVec3f::getClassTypeId();
	if ( (f = dragger->getField( "center" )) && f->isOfType( fType ) )
	    ((SoSFVec3f *)f)->setValue(  manip->center.getValue() );

	// Set the motionMatrix.
	dragger->setMotionMatrix(newMat);

    // Re-enable value changed callbacks and call.
    dragger->enableValueChangedCallbacks( saveEnabled );
    dragger->valueChanged();
}
