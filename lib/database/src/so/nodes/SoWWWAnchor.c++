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
 |   SoWWWAnchor implementation.
 |
 |   Author(s)	: David Mott, Jim Kent, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/nodes/SoWWWAnchor.h>
#include <Inventor/SoPickedPoint.h>

SoWWWAnchorCB *		SoWWWAnchor::fetchURLcb = NULL;
void *			SoWWWAnchor::fetchURLdata = NULL;
SoWWWAnchorCB *		SoWWWAnchor::highlightURLcb = NULL;
void *			SoWWWAnchor::highlightURLdata = NULL;

SO_NODE_SOURCE(SoWWWAnchor);

SbString		SoWWWAnchor::emptyString;

////////////////////////////////////////////////////////////////////////
//
//  Constructor
//  
SoWWWAnchor::SoWWWAnchor()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoWWWAnchor);
    SO_NODE_ADD_FIELD(name, ("<Undefined URL>"));
    SO_NODE_ADD_FIELD(description, (""));
    SO_NODE_ADD_FIELD(map, (NONE));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Mapping, NONE);
    SO_NODE_DEFINE_ENUM_VALUE(Mapping, POINT);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(map, Mapping);

    isBuiltIn = TRUE;
    fullURL.makeEmpty();
}

////////////////////////////////////////////////////////////////////////
//
//  Destructor
//  
SoWWWAnchor::~SoWWWAnchor()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the full URL, or if not set, the URL as specified in the name field.
//
const SbString &
SoWWWAnchor::getFullURLName()
//
////////////////////////////////////////////////////////////////////////
{
    if (fullURL.getLength() != 0)
	 return fullURL;
    else return name.getValue();
}

////////////////////////////////////////////////////////////////////////
//
//  Handle event action - report back to the application any activation 
//  (left mouse clicks) over our children if the app has registered interest
//  so the app can fetch the URL.
//  
void
SoWWWAnchor::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    const SoEvent *event = action->getEvent();
    if (fetchURLcb != NULL && SO_MOUSE_RELEASE_EVENT(event, BUTTON1)) {
    
	// Left mouse button and app is interested. Do a pick.
	SbBool isPicked = FALSE;
	const SoPickedPoint *pp = action->getPickedPoint();
	SoFullPath *pPath = (pp != NULL) ? (SoFullPath *) pp->getPath() : NULL;
	if (pPath && pPath->containsPath(action->getCurPath())) {
	    // Make sure this is the lowest anchor in the pick path!
	    isPicked = TRUE;
	    for (int i = 0; i < pPath->getLength(); i++) {
		SoNode *node = pPath->getNodeFromTail(i);
		if (node->isOfType(SoWWWAnchor::getClassTypeId())) {
		    if (node != this)
			isPicked = FALSE;
		    break; // found the lowest anchor - look no further
		}
	    }
	}
	
	// If something was picked, and the picked path contains this
	// node, we're in business.
	if (isPicked) {
	    
	    // If mapping is on, append the ?x,y,z syntax
	    SbString url = getFullURLName();
	    
	    if (map.getValue() == POINT) {
		SbVec3f point = pp->getObjectPoint();
		char str[60];
		
		sprintf(str, "?%g%%2c%g%%2c%g", point[0], point[1], point[2]);
		url += str;
	    }
	    
	    // Tell the app to do that fetch!
	    (*fetchURLcb)(url, fetchURLdata, this);    
	} else {
	    // pass the event on down the tree
	    SoLocateHighlight::handleEvent(action);	    
	}
    }
    else {
	// Let SoLocateHighlight handle the event.
	SoLocateHighlight::handleEvent(action);
    }
}

////////////////////////////////////////////////////////////////////////
//
// The app will do all URL fetching through this callback.
//  
// static
//
void
SoWWWAnchor::setFetchURLCallBack(SoWWWAnchorCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    fetchURLcb = f;
    fetchURLdata = userData;
}

////////////////////////////////////////////////////////////////////////
//
// The app can provide feedback, like changing the cursor, in this callback.
//  
// static
//
void
SoWWWAnchor::setHighlightURLCallBack(SoWWWAnchorCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    highlightURLcb = f;
    highlightURLdata = userData;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Redefine this to also invoke the app callbacks...
//  This is called when the locate highlight status of this node changes.
//
// Usage: virtual protected
//
void
SoWWWAnchor::redrawHighlighted(SoAction *action, SbBool isNowHighlighting)
//
////////////////////////////////////////////////////////////////////////
{
    // call the parent class to do the rendering
    SoLocateHighlight::redrawHighlighted(action, isNowHighlighting);
    
    if (highlightURLcb == NULL) return;
    
    // Tell the app our new status
    if (isNowHighlighting)
	 (*highlightURLcb)(getFullURLName(), highlightURLdata, this);  
    else (*highlightURLcb)(emptyString, highlightURLdata, this);  
}

