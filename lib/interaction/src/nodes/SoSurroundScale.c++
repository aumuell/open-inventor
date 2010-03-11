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
 |      SoSurroundScale
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSurroundScale.h>

SO_NODE_SOURCE(SoSurroundScale);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSurroundScale::SoSurroundScale()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSurroundScale);

    isBuiltIn = TRUE;

    SO_NODE_ADD_FIELD(numNodesUpToContainer,     (0) );
    SO_NODE_ADD_FIELD(numNodesUpToReset,     (0) );

    ignoreInBbox = FALSE;
    doTranslations = TRUE;

    // These variables are used to assist in making the node more efficient
    // by caching the values given back from updateMySurroundParams.
    cachedScale.setValue(1,1,1);
    cachedInvScale.setValue(1,1,1);
    cachedTranslation.setValue(0,0,0);
    cacheOK = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoSurroundScale::~SoSurroundScale()
//
////////////////////////////////////////////////////////////////////////
{
}


// Snaps numbers within epsilong of zero to zero
#define FUDGE(x,epsilon) \
if ((x >= 0.0) && (x < epsilon)) \
    x = epsilon; \
else if ((x < 0.0) && (x > -epsilon)) \
    x = -epsilon;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns transformation matrix.
//
// Use: protected

void
SoSurroundScale::updateMySurroundParams(SoAction *action,
					const SbMatrix &myInv )
//
////////////////////////////////////////////////////////////////////////
{
    const SoFullPath *curPath = (const SoFullPath *) action->getCurPath();
    int curPathLength = curPath->getLength();

    // If the container node is out of range, just return.
	int numUpCon = (int) numNodesUpToContainer.getValue();
	if (   (numUpCon <= 0)    || (numUpCon > (curPathLength - 1))  ){
	    cachedScale.setValue(1,1,1);
	    cachedInvScale.setValue(1,1,1);
	    cachedTranslation.setValue(0,0,0);
	    cacheOK = FALSE;
	    return;
	}

    // CHECK TO SEE IF OUR CACHED VALUES ARE OKAY
    // IF SO, JUST RETURN 
	if (   cacheOK  ) 
	    return;

    // Find the path to apply the bounding box action to. It should end
    // 'numUpCon' above this one.
	SoPath *applyPath = curPath->copy(0, (curPathLength - numUpCon));
	applyPath->ref();

    // See if there is a node to do a reset at. If so, build a resetPath
	SoPath *resetPath = NULL;
	int numUpReset = (int) numNodesUpToReset.getValue();

	if (numUpReset >= 0 && (numUpReset < numUpCon) ) {
	    // Build a path ending at the reset node.
	    resetPath = curPath->copy(0, curPathLength - numUpReset );
	    resetPath->ref();
	}
	SoFullPath *fullResetPath = (SoFullPath *) resetPath;

    // Create a getBoundingBox action
    // Set the reset path if we have one.
    // Apply the bounding box action and find out how big the box was.
    // Temporarily set the ignoreInBbox flag TRUE, so we don't infinite loop!

	SbViewportRegion vpRegion(0,0);
	SoState	*state = action->getState();
	vpRegion = SoViewportRegionElement::get(state);

	static SoGetBoundingBoxAction *boundingBoxAction = NULL;
	if (boundingBoxAction == NULL)
	    boundingBoxAction = new SoGetBoundingBoxAction(vpRegion);
	else
	    boundingBoxAction->setViewportRegion(vpRegion);

	if (fullResetPath)
	    boundingBoxAction->setResetPath( fullResetPath, FALSE,
						SoGetBoundingBoxAction::BBOX);

	SbBool oldFlag = isIgnoreInBbox();
	setIgnoreInBbox( TRUE );
	boundingBoxAction->apply( applyPath );
	setIgnoreInBbox( oldFlag );
	SbXfBox3f &myXfBox = boundingBoxAction->getXfBoundingBox();

	// Transform the box into our local space, then project it.
	myXfBox.transform( myInv );
	SbBox3f myBox = myXfBox.project();

    // Get the scale for this node to add to the ctm.
	if (myBox.isEmpty()) {
	    cachedScale.setValue(1,1,1);
	    cachedInvScale.setValue(1,1,1);
	    cachedTranslation.setValue(0,0,0);
	    cacheOK = TRUE;
	    return;
	}
	else {
	    float x, y, z;
	    myBox.getSize(x,y,z);
	    cachedScale.setValue(  .5*x, .5*y, .5*z );

	    float minLength = .01 * cachedScale.length();
	    // Macro defined just before beginning of this method.
	    FUDGE(cachedScale[0],minLength);
	    FUDGE(cachedScale[1],minLength);
	    FUDGE(cachedScale[2],minLength);

	    // Find the inverse values
	    for (int j = 0; j < 3; j++ )
		cachedInvScale[j] = 1.0 / cachedScale[j];
	}

    // Get the translation for this node to add to the ctm.
	// This will get the cube centered about the bbox center.
	// If the bounding box is not centered at the origin, we have to
	// move the cube to the correct place. 
	if (doTranslations)
	    cachedTranslation = 0.5 * (   myBox.getMin() + myBox.getMax() );
	else
	    cachedTranslation.setValue(0,0,0);

    // Establish the cached values to save us some time later...
	cacheOK = TRUE;

	if (resetPath)
	    resetPath->unref();
	if (applyPath)
	    applyPath->unref();
}

#undef FUDGE

void
SoSurroundScale::callback( SoCallbackAction *action )
{
    SoSurroundScale::doAction( action );
}

void
SoSurroundScale::GLRender( SoGLRenderAction *action )
{
    SoSurroundScale::doAction( action );
}

void
SoSurroundScale::getBoundingBox( SoGetBoundingBoxAction *action )
{
    if ( ignoreInBbox == FALSE )
	doAction( action );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns transformation matrix.
//
// Use: protected

void
SoSurroundScale::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
	SbMatrix	&ctm = action->getMatrix();
	SbMatrix	&inv = action->getInverse();
	SbMatrix m;

    // Get my scale and its inverse
	if (cacheOK == FALSE)
	    updateMySurroundParams( action, inv );

    // Append the translation.
	if (doTranslations) {
	    m.setTranslate( cachedTranslation );
	    ctm.multLeft( m );
	    m.setTranslate( -cachedTranslation );
	    inv.multRight( m );
	}
	else
	    cachedTranslation.setValue(0,0,0);

    // Append the scale.
	m.setScale( cachedScale );
	ctm.multLeft( m );
	m.setScale( cachedInvScale );
	inv.multRight( m );
}

void
SoSurroundScale::pick( SoPickAction *action )
{
    SoSurroundScale::doAction( action );
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles any action derived from SoAction.
//
// Use: private

void
SoSurroundScale::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState	*state = action->getState();

    SbMatrix theCtm = SoModelMatrixElement::get(state);

    if (cacheOK == FALSE )
	updateMySurroundParams( action, theCtm.inverse() );

    if (doTranslations)
	SoModelMatrixElement::translateBy(state, this, cachedTranslation );
    else
	cachedTranslation.setValue(0,0,0);

    SoModelMatrixElement::scaleBy(state, this, cachedScale );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the cacheOK flag to FALSE
//   This means that the next action coming through this node will cause the
//   scale and translation to be re-calculated.
//
// Use: public

void
SoSurroundScale::invalidate()
//
////////////////////////////////////////////////////////////////////////
{
    cacheOK = FALSE;
    touch();
}
