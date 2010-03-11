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
 |	SoCenterballManip
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/draggers/SoCenterballDragger.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/events/SoEvent.h>

SO_NODE_SOURCE(SoCenterballManip);

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
SoCenterballManip::SoCenterballManip()
{
    SO_NODE_CONSTRUCTOR(SoCenterballManip);
    isBuiltIn = TRUE;

    SoCenterballDragger *d = new SoCenterballDragger;
    setDragger(d);
    SoSurroundScale *ss = (SoSurroundScale *) d->getPart("surroundScale",TRUE);
    ss->numNodesUpToContainer = 4;
    ss->numNodesUpToReset = 3;

    // Assign our own personal fieldSensorCB instead of the old one.
    rotateFieldSensor->setFunction(&SoCenterballManip::fieldSensorCB );
    translFieldSensor->setFunction(&SoCenterballManip::fieldSensorCB );
    scaleFieldSensor->setFunction(&SoCenterballManip::fieldSensorCB );
    centerFieldSensor->setFunction(&SoCenterballManip::fieldSensorCB );
    scaleOrientFieldSensor->setFunction(&SoCenterballManip::fieldSensorCB );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoCenterballManip::~SoCenterballManip()
//
////////////////////////////////////////////////////////////////////////
{
    // Important to do this because dragger may have callbacks
    // to this node.
    setDragger(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the dragger to be the given node...
//
void
SoCenterballManip::setDragger( SoDragger *newDragger )
//
////////////////////////////////////////////////////////////////////////
{
    SoDragger *oldDragger = getDragger();
    if ( oldDragger ) {
	oldDragger->removeValueChangedCallback(
				    &SoCenterballManip::valueChangedCB,this);
	children->remove(0);
    }
	
    if (newDragger) {
	if (children->getLength() > 0)
	    children->set(0, newDragger );
	else 
	    children->append( newDragger );
	// Call the fieldSensorCB to transfer our values into the
	// new dragger.
	SoCenterballManip::fieldSensorCB( this, NULL );
	newDragger->addValueChangedCallback(
				    &SoCenterballManip::valueChangedCB, this);
    }
}

void
SoCenterballManip::valueChangedCB( void *inManip, SoDragger *inDragger )
{
    if (inDragger == NULL)
	return;

    SoCenterballManip   *manip   = (SoCenterballManip *) inManip;

    SbMatrix motMat = inDragger->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    SbVec3f center = manip->center.getValue();

    // See if inDragger has a center field...
    SoField *f;
    SoType  fType = SoSFVec3f::getClassTypeId();
    if ( (f = inDragger->getField("center")) && f->isOfType( fType ) )
	center = ((SoSFVec3f *)f)->getValue();

    motMat.getTransform( trans, rot, scale, scaleOrient, center );

    // Disconnect the field sensors
    manip->rotateFieldSensor->detach();
    manip->translFieldSensor->detach();
    manip->scaleFieldSensor->detach();
    manip->scaleOrientFieldSensor->detach();
    manip->centerFieldSensor->detach();

    if ( manip->rotation.getValue() != rot )
	manip->rotation = rot;
    if ( manip->translation.getValue() != trans )
	manip->translation = trans;
    if ( manip->scaleFactor.getValue() != scale )
	manip->scaleFactor = scale;
    if ( manip->scaleOrientation.getValue() != scaleOrient )
	manip->scaleOrientation = scaleOrient;
    if ( manip->center.getValue() != center )
	manip->center = center;

    // Reconnect the field sensors
    manip->rotateFieldSensor->attach( &(manip->rotation));
    manip->translFieldSensor->attach( &(manip->translation));
    manip->scaleFieldSensor->attach( &(manip->scaleFactor));
    manip->scaleOrientFieldSensor->attach( &(manip->scaleOrientation));
    manip->centerFieldSensor->attach( &(manip->center));
}

void
SoCenterballManip::fieldSensorCB( void *inManip, SoSensor * )
{
    SoCenterballManip *manip    = (SoCenterballManip *) inManip;
    SoDragger         *dragger  = manip->getDragger();

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

//XXX////////////////////////////////////////////////////////////////////////
//XXX//
//XXX// Description:
//XXX//    Returns transformation matrix.
//XXX//
//XXX// Use: protected
//XXX
//XXXSbBox3f
//XXXSoCenterballManip::getSurroundBox( const SoPath *pickPath )
//XXX//
//XXX////////////////////////////////////////////////////////////////////////
//XXX{
//XXX    SbXfBox3f xfBox;
//XXX    SbBox3f   nonXfBox;
//XXX
//XXX    if (pickPath == NULL )
//XXX	return nonXfBox;
//XXX
//XXX    // Get 'pathToThis' a full path copy of the pickPath with extra nodes
//XXX    // popped off back up to the manip.
//XXX    SoPath *pathCopy = pickPath->copy();
//XXX    pathCopy->ref();
//XXX    SoFullPath *pathToThis = (SoFullPath *) pathCopy;
//XXX    pathToThis->ref();
//XXX    while ( pathToThis->getTail() != this )
//XXX	pathToThis->pop();
//XXX
//XXX    if ( pathToThis->getLength() <= 1 ) {
//XXX	pathCopy->unref();
//XXX	pathToThis->unref();
//XXX	return nonXfBox;
//XXX    }
//XXX
//XXX    // Make 'pathToParent' -- a copy of 'pathToThis' with one node removed
//XXX    SoPath *pathCopy2 = pathToThis->copy();
//XXX    pathCopy2->ref();
//XXX    SoFullPath *pathToParent = (SoFullPath *) pathCopy2;
//XXX    pathToParent->pop();
//XXX
//XXX    // Create a getBoundingBox action
//XXX    // Set the reset path to be 'pathToThis'
//XXX    // Apply the bounding box action to 'pathToParent' and find out the bbox
//XXX    SoGetBoundingBoxAction ba;
//XXX    ba.setResetPath( pathToThis, FALSE, SoGetBoundingBoxAction::ALL);
//XXX
//XXX    ba.apply( pathToParent );
//XXX    xfBox = ba.getXfBoundingBox();
//XXX    nonXfBox = xfBox.project();
//XXX
//XXX    pathCopy->unref();
//XXX    pathCopy2->unref();
//XXX    pathToThis->unref();
//XXX    pathToParent->unref();
//XXX
//XXX    return nonXfBox;
//XXX}
