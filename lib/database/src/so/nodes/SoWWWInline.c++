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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   SoWWWInline implementation.
 |
 |   Author(s)	: Gavin Bell, David Mott, Jim Kent
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoWWWInline.h>

#include <Inventor/SoLists.h>

#include <Inventor/sensors/SoFieldSensor.h>


// static members
SoWWWInlineFetchURLCB *	SoWWWInline::fetchURLcb = NULL;
void *			SoWWWInline::fetchURLdata = NULL;

SoWWWInline::BboxVisibility     SoWWWInline::bboxVisibility = UNTIL_LOADED;
SbColor                         SoWWWInline::bboxColor(0.8, 0.8, 0.8); 

SO_NODE_SOURCE(SoWWWInline);

////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//  
SoWWWInline::SoWWWInline()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoWWWInline);
    SO_NODE_ADD_FIELD(name, ("<Undefined file>"));
    SO_NODE_ADD_FIELD(bboxCenter, (SbVec3f(0,0,0)));
    SO_NODE_ADD_FIELD(bboxSize, (SbVec3f(0,0,0)));
    SO_NODE_ADD_FIELD(alternateRep, (NULL));

    isBuiltIn = TRUE;
    children = NULL;
    kidsAreHere = FALSE;
    kidsRequested = FALSE;
    fullURL.makeEmpty();
}

////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//  
SoWWWInline::~SoWWWInline()
//
////////////////////////////////////////////////////////////////////////
{
    delete children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the full URL, or if not set, the URL as specified in the name field.
//
const SbString &
SoWWWInline::getFullURLName()
//
////////////////////////////////////////////////////////////////////////
{
    if (fullURL.getLength() != 0)
	 return fullURL;
    else return name.getValue();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return a copy of our hidden children.
//
SoGroup *
SoWWWInline::copyChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    // Do we have any children?
    if (! kidsAreHere)
	return NULL;
	
    // Create a new SoGroup with our children, and return a copy of
    // it. This will ensure that connections are copied properly.
    SoGroup *holder = new SoGroup;
    holder->ref();

    // child 0 is the bounding box
    for (int i = 1; i < children->getLength(); i++)
	holder->addChild((*children)[i]);

    SoGroup *result = (SoGroup *) holder->copy(TRUE);

    holder->unref();

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Fetch children from a URL. This is asynchronous. We make the
// request here, and at some later point, we'll be called with the
// children.
//
void
SoWWWInline::requestChildrenFromURL()
//
////////////////////////////////////////////////////////////////////////
{
    // Callback to fetch the URL. We let the viewer do this
    // so that it can be involved in the fetching process, and
    // this node can be browser independent.
    if (fetchURLcb == NULL)
	return;
    
    // We have just requested the children
    kidsRequested = TRUE;  

    (*fetchURLcb)(getFullURLName(), fetchURLdata, this);  
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    The children have arrived from a URL - add them here.
//
void
SoWWWInline::setChildData(SoNode *URLdata)
//
////////////////////////////////////////////////////////////////////////
{
    if (URLdata != NULL) {
	// Prepare the child list for data
	if (children == NULL) {
	    // Create the list
	    children = new SoChildList(this);
	} else if (children->getLength() > 0) {
	    children->truncate(1);
	}

	// the bounding box has been removed at this point
	if (bboxSize.isDefault()) {
	    SoGetBoundingBoxAction bba =
	      SoGetBoundingBoxAction(SbViewportRegion());
	    bba.apply(URLdata);
	    SbBox3f inlineBbox = bba.getBoundingBox();
	    
	    SbVec3f bbsize;
	    inlineBbox.getSize(bbsize[0], bbsize[1], bbsize[2]);
	    SbVec3f bbcenter = inlineBbox.getCenter();
	    addBoundingBoxChild(bbcenter, bbsize);
	} else {
	    addBoundingBoxChild(bboxCenter.getValue(), bboxSize.getValue());
	}

	// Add the fetched URL data to our child list
	children->append(URLdata);
	kidsAreHere = TRUE;
    }
    
    //??? what if the URLdata is NULL? remove the children?
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the child data put here by the application.
//
SoNode *
SoWWWInline::getChildData() const
//
////////////////////////////////////////////////////////////////////////
{
    // child 0 is the bounding box, child 1 is the data
    SoNode *kids = kidsAreHere ? (*children)[1] : NULL;
    return kids;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal.
//
void
SoWWWInline::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (kidsAreHere) {
	int		numIndices;
	const int	*indices;

	// start at 1, child 0 is the bounding box
	if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	     children->traverse(action, 1, indices[numIndices - 1]);
	else children->traverse(action, 1, children->getLength() - 1);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the search action, but only if our children are here!
//
void
SoWWWInline::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the caller is searching for this node
    SoNode::search(action);

    // Then recurse on children
    if (! action->isFound())
	doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the callback action
//
void
SoWWWInline::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoWWWInline::doAction(action);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs the action on the kids if here, or on the bounding
// box if kids are not here and box is specified.
//
void
SoWWWInline::doActionOnKidsOrBox(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If the children are not here, and the author told us the bounding
    // box size, then we should render a wireframe bounding box until the 
    // URL data arrives. (It will have been requested in doAction).
    if (((children == NULL) || (children->getLength() == 0)) &&
	! kidsAreHere && ! bboxSize.isDefault()) {
	// Make sure our bounding box scene graph is in the child list
	addBoundingBoxChild(bboxCenter.getValue(), bboxSize.getValue());
    }
    // Render the children if they are here, or the bounding box if they are not.
    // Either way, children must be set for us to render.
    if (children != NULL) {
	int		numIndices;
	const int	*indices;
	
	if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH) {
	    if (kidsAreHere) {
		if ((bboxVisibility == ALWAYS) &&
		    (action->isOfType(SoGLRenderAction::getClassTypeId()))) {
		    
		    // traverse the bounding box (child 0)
		    children->traverse(action, 0, 0);
		} 
		
		// traverse the actual children
		children->traverse(action, 1, indices[numIndices - 1]);
	    } else {
		// kids aren't here, so traverse the bounding box if appropriate
		if (bboxVisibility != NEVER) {
		    children->traverse(action, 0, 0);
		}
	    }
	} else {
	    if (kidsAreHere) {
		if ((bboxVisibility == ALWAYS) &&
		    (action->isOfType(SoGLRenderAction::getClassTypeId()))) {
		    
		    // traverse the bounding box (child 0)
		    children->traverse(action, 0, 0);
		} 
		
		// traverse the actual children
		children->traverse(action, 1, children->getLength() - 1);
	    } else {
		// kids aren't here, so traverse the bounding box if appropriate
		if (bboxVisibility != NEVER) {
		    children->traverse(action, 0, 0);
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the GL render action
//
void
SoWWWInline::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If the child scene is not here, make sure that data has been requested!
    if (! kidsAreHere && ! kidsRequested) {
	requestChildrenFromURL(); 
    }
    
    // Render the children or a bbox for the children
    doActionOnKidsOrBox(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pick if the children are here. If not here, then pick the
// bounding box if it exists.
//
void
SoWWWInline::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Pick the children or a bbox for the children
    doActionOnKidsOrBox(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get bounding box action.  This takes care of averaging
//    the centers of all children to get a combined center.
//
void
SoWWWInline::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If kids are not here, use the bboxCenter and bboxSize fields
    if (! kidsAreHere) {
	// If the bbox is default and the kids are not here, then 
	// we have no choice but to request the children (assume we
	// want them).
	if (bboxSize.isDefault()) {
	    if (! kidsRequested)
		requestChildrenFromURL();
	}
	
	// If the bbox is set and the kids are not here, use the bbox!
	else {
	    SbVec3f min = bboxCenter.getValue() - bboxSize.getValue()/2;
	    SbVec3f max = bboxCenter.getValue() + bboxSize.getValue()/2;
	    action->extendBy(SbBox3f(min, max));
    
	    action->setCenter(bboxCenter.getValue(), FALSE);
	}
    }
    
    // Else kids are here. Use them!
    else {
	SbVec3f	totalCenter(0,0,0);
	int		numCenters = 0;
	int		numIndices;
	const int	*indices;
	int		lastChild;

	if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	    lastChild = indices[numIndices - 1];
	else
	    lastChild = children->getLength() - 1;

	// start at 1, child 0 is the bounding box 
	for (int i = 1; i <= lastChild; i++) {
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
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the handle event thing if the children are here.
//
void
SoWWWInline::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoWWWInline::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements get matrix action.
//
void
SoWWWInline::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (kidsAreHere) {	
	int		numIndices;
	const int	*indices;
    
	// Only need to compute matrix if group is a node in middle of
	// current path chain or is off path chain (since the only way
	// this could be called if it is off the chain is if the group is
	// under a group that affects the chain).
    
	switch (action->getPathCode(numIndices, indices)) {
    
	  case SoAction::NO_PATH:
	    break;
    
	  case SoAction::IN_PATH:
	    children->traverse(action, 1, indices[numIndices - 1]);
	    break;
    
	  case SoAction::BELOW_PATH:
	    break;
    
	  case SoAction::OFF_PATH:
	    children->traverse(action, 1, children->getLength() - 1);
	    break;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to children.
//
// Use: internal

SoChildList *
SoWWWInline::getChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    return children;
}


////////////////////////////////////////////////////////////////////////
//
// The app will do all URL fetching through this callback.
//  
// static
//
void
SoWWWInline::setFetchURLCallBack(SoWWWInlineFetchURLCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    fetchURLcb = f;
    fetchURLdata = userData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a bounding box to the child scene graph
//
void
SoWWWInline::addBoundingBoxChild(SbVec3f center, SbVec3f size)
//
////////////////////////////////////////////////////////////////////////
{
    if (children == NULL) {
	children = new SoChildList(this);
    }

    if (children->getLength() == 0) {

	SoSeparator *sep = new SoSeparator;

	// child 0
	SoLightModel *lm = new SoLightModel;
	lm->model = SoLightModel::BASE_COLOR;
	sep->addChild(lm);

	// child 1
	SoDrawStyle *ds   = new SoDrawStyle;
	ds->style = SoDrawStyle::LINES;
	sep->addChild(ds);

	// child 2
	SoBaseColor *bc = new SoBaseColor;
	bc->rgb.setValue(bboxColor);
	sep->addChild(bc);

	// child 3
	SoTranslation *tr = new SoTranslation;
	tr->translation = center;
	sep->addChild(tr);

	// child 4
	SoCube      *cube = new SoCube;
	cube->width  = size[0];
	cube->height = size[1];
	cube->depth  = size[2];
	sep->addChild(cube);

	children->append(sep);
	
    } else {
	SoNode *n = (*children)[0];

	if (!n->isOfType(SoSeparator::getClassTypeId())) {
	    return;
	}

	SoSeparator *sep = (SoSeparator *)n;

	// just update the center and size
	n = sep->getChild(3);

        if (!n->isOfType(SoTranslation::getClassTypeId())) {
	    return;
	}

	SoTranslation *tr = (SoTranslation *)n;

	tr->translation = center;

	n = sep->getChild(4);

	if (!n->isOfType(SoCube::getClassTypeId())) {
	    return;
	}

	SoCube *cube = (SoCube *)n;
	cube->width  = size[0];
	cube->height = size[1];
	cube->depth  = size[2];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads instance. If no fetch CB registered by the app,
// use the alternateRep as child data.
//
// Use: protected

SbBool
SoWWWInline::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool readOK = TRUE;
    
    readOK = SoNode::readInstance(in, flags);

    // If no fetch CB and there is an alternate rep, use it as child data
    if (readOK && fetchURLcb == NULL && alternateRep.getValue() != NULL) {
	SbBool saveNotify = enableNotify(FALSE);    // notification off
	setChildData(alternateRep.getValue());	    // set child data
	enableNotify(saveNotify);		    // notification reset
    }
    
    return readOK;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node as well as the hidden
//    child data (if there is any)
//
// Use: protected, virtual
void
SoWWWInline::copyContents(const SoFieldContainer *fromFC, SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Copy the usual stuff
    SoNode::copyContents(fromFC, copyConnections);

    // Copy the children if they're set
    if (fromFC->isOfType(SoWWWInline::getClassTypeId())) {
	SoWWWInline *fromInline = (SoWWWInline *)fromFC;

	SoGroup *kids = fromInline->copyChildren();

	if (kids) {
	    kids->ref();
	}

	// copyChildren puts a group over the children
	// -- remove it if it has only one child
	if (kids && (kids->getNumChildren() == 1)) {
	    this->setChildData(kids->getChild(0));
	} else {
	    this->setChildData(kids);
	}

	if (kids) {
	    kids->unref();
	}
    }
}

