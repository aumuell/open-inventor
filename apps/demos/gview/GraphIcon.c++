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

#include <X11/Intrinsic.h>

#include <Inventor/SbDict.h>
#include <Inventor/SoPath.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>

#include "DisplayGraph.h"
#include "FieldEditor.h"
#include "GraphIcon.h"

SoGetBoundingBoxAction	*GraphIcon::bboxAct;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    One-time initialization
//

void
GraphIcon::init()
//
////////////////////////////////////////////////////////////////////////
{
    // Use default viewport region for now - we really should change	???
    // it later when we know the real size, but that would require	???
    // passing the vp info around.					???
    bboxAct = new SoGetBoundingBoxAction(SbViewportRegion());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

GraphIcon::GraphIcon()
//
////////////////////////////////////////////////////////////////////////
{
    sceneNode	= NULL;
    icon	= NULL;
    iconGraph	= NULL;
    numChildren	= -1;		// Not a group
    children	= NULL;
    instanceOf  = NULL;		// Not an instance
    state	= HIDDEN;	// Not visible
    fieldsShown	= FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

GraphIcon::~GraphIcon()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructs and returns a display subgraph for the given icon.
//    The subgraph depends on the current state of the icon.
//

SoNode *
GraphIcon::buildGraph()
//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator	*sep;

    sep		= new SoSeparator;
    iconSwitch	= new SoSwitch;
    translation	= new SoTranslation;

    // Translation value will be set when positions are computed

    sep->addChild(translation);
    sep->addChild(iconSwitch);
    iconSwitch->addChild(icon);
    iconSwitch->addChild(DisplayGraph::getClosedIcon());
    iconSwitch->addChild(DisplayGraph::getInstanceIcon());

    // Turn switch on, depending on state of icon
    setIconSwitch();

    // Save root of graph
    iconGraph = sep;

    return iconGraph;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes size of sub-display graph rooted by this.
//

void
GraphIcon::computeSize(const SbVec2f &iconSpacing)
//
////////////////////////////////////////////////////////////////////////
{
    float	x, y, z;

    // Reset translation to keep bounding box at origin
    translation->translation.setValue(0.0, 0.0, 0.0);

    // Compute size of the icon
    bboxAct->apply(iconGraph);
    bboxAct->getBoundingBox().getSize(x, y, z);
    iconSize.setValue(x, y);
    heightOffset = bboxAct->getBoundingBox().getMax()[1];

    subgraphSize = iconSize;

    // If the node is closed or has no children, the subgraph size is
    // the same. Otherwise, compute the children's sizes and add these
    // to the icon size to get the subgraph size. Use the appropriate
    // spacing values.
    if (numChildren > 0 && state == NORMAL) {
	int		i;
	GraphIcon	*child;
	SbVec2f		childrenSize(0.0, 0.0);

	for (i = 0; i < numChildren; i++) {

	    child = &children[i];

	    child->computeSize(iconSpacing);

	    // Keep track of maximum Y size
	    if (child->subgraphSize[1] > childrenSize[1])
		 childrenSize[1] = child->subgraphSize[1];

	    // Keep track of total X size
	    childrenSize[0] += child->subgraphSize[0];
	    if (i > 0)
		childrenSize[0] += iconSpacing[0];
	}

	// The size in y is the y size of this icon + the maximum
	// y size of the children's icons plus the vertical spacing.
	// The size in x is the size of the children's icons, if
	// bigger than this node's icon.

	subgraphSize[1] += childrenSize[1] + iconSpacing[1];
	if (childrenSize[0] > subgraphSize[0])
	    subgraphSize[0] = childrenSize[0];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes position of visible children given position of this.
//    Also sets translation node in icon graph.
//

void
GraphIcon::computePosition(const SbVec2f thisPosition,
			   const SbVec2f &iconSpacing)
//
////////////////////////////////////////////////////////////////////////
{
    position = thisPosition;

    // Set translation to center of icon
    translation->translation.setValue(position[0],
				      position[1] - heightOffset,
				      0.0);

    // Compute visible children's positions
    if (numChildren > 0 && state == NORMAL) {
	SbVec2f	childPosition;

	// Height of children is below this one, figuring in the spacing
	childPosition[1] = position[1] - (iconSize[1] + iconSpacing[1]);

	// Just one child is easy
	if (numChildren == 1) {
	    childPosition[0] = position[0];
	    children[0].computePosition(childPosition, iconSpacing);
	}

	// More than 1 child is a little harder
	else {
	    int	i;

	    // Position of first child
	    childPosition[0] = position[0] - 0.5 *
		(subgraphSize[0] - children[0].subgraphSize[0]);

	    for (i = 0; i < numChildren; i++) {
		children[i].computePosition(childPosition, iconSpacing);
		childPosition[0] += (0.5 * children[i].subgraphSize[0] +
				     iconSpacing[0]);
		if (i < numChildren - 1)
		    childPosition[0] += 0.5 * children[i+1].subgraphSize[0];
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Shows icon if it is HIDDEN. Returns TRUE if graph needs to be
//    rebuilt.
//

SbBool
GraphIcon::show()
//
////////////////////////////////////////////////////////////////////////
{
    // Change state if necessary
    if (state == HIDDEN) {
	setState(NORMAL);
	return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Hides icon if it is not HIDDEN. Returns TRUE if graph needs to be
//    rebuilt.
//

SbBool
GraphIcon::hide()
//
////////////////////////////////////////////////////////////////////////
{
    // Change state if necessary
    if (state != HIDDEN) {
	setState(HIDDEN);
	return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Opens the icon if it is a CLOSED group. If openAll is TRUE,
//    opens all descendent groups. Returns TRUE if the graph needs to
//    be rebuilt as a result.
//

SbBool
GraphIcon::openGroup(SbBool openAll)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	rebuild = FALSE;

    // Change state
    if (state != NORMAL) {
	setState(NORMAL);
	rebuild = TRUE;
    }

    // Open children if necessary
    if (openAll) {
	int	i;

	for (i = 0; i < numChildren; i++)
	    if (children[i].openGroup(openAll))
		rebuild = TRUE;
    }

    return rebuild;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Closes the icon if it is a NORMAL group. Returns TRUE if the
//    graph needs to be rebuilt as a result.
//

SbBool
GraphIcon::closeGroup()
//
////////////////////////////////////////////////////////////////////////
{
    // Can't close it if it is already closed or it has no children
    if (state != CLOSED && numChildren > 0) {
	setState(CLOSED);
	return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Toggles group between NORMAL (open) state and CLOSED. Should
//    only be called on a group icon. Returns TRUE if the graph needs
//    to be rebuilt as a result.
//

SbBool
GraphIcon::toggleGroup()
//
////////////////////////////////////////////////////////////////////////
{
    SbBool rebuild = FALSE;

    switch (state) {

      case NORMAL:
	setState(CLOSED);
	rebuild = TRUE;
	break;

      case CLOSED:
	// If it is currently closed, open it and display children
	setState(NORMAL);
	rebuild = TRUE;
	break;

      case HIDDEN:
	// Do nothing
	break;
    }

    return rebuild;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Swaps instance icon with icon it is an instance of. This allows
//    selection under an instance of a group.
//

void
GraphIcon::swapInstance()
//
////////////////////////////////////////////////////////////////////////
{
    GraphIcon	*otherIcon = instanceOf;

    // Set up instance connections in other direction
    otherIcon->setInstance(this);
    setInstance(NULL);

    // Make sure they are both displayed correctly
    otherIcon->setIconSwitch();
    setIconSwitch();

    setState(otherIcon->state);
    otherIcon->setState(NORMAL);

    if (otherIcon->isGroup()) {
	setGroup(otherIcon->getNumChildren(), otherIcon->getChild(0));
	otherIcon->setGroup(-1, NULL);

	// Reparent the children
	for (int i = 0; i < numChildren; i++)
	    children[i].setParent(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Brings up window to show field data for node represented by icon.
//

void
GraphIcon::showFields()
//
////////////////////////////////////////////////////////////////////////
{
    // Don't do anything if fields are already displayed or there are
    // no fields in the node
    if (fieldsShown || FieldEditor::getNumFields(sceneNode) == 0)
	return;

    // Create and display an instance of the FieldEditor
    FieldEditor	*editor = new FieldEditor(sceneNode);

    editor->setFinishCallback(&GraphIcon::finishShowFieldsCB, this);
    editor->show();

    fieldsShown = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes the state of the icon to the given state. Recurses to
//    change children to an appropriate state. Changing the state may
//    also require a change in icon representation.
//

void
GraphIcon::setState(IconState newState)
//
////////////////////////////////////////////////////////////////////////
{
    // If this is not a group or it is a group with no children, never
    // set it to CLOSED (use NORMAL)
    if (numChildren <= 0) {
	if (newState == CLOSED)
	    state = NORMAL;
	else
	    state = newState;
    }

    // Set the group's state
    else {
	int		i;
	IconState	childState;

	state = newState;

	// Set all children to appropriate state

	switch (newState) {
	  case NORMAL:
	    childState = CLOSED;
	    break;
	  case CLOSED:
	  case HIDDEN:
	    childState = HIDDEN;
	    break;
	}

	for (i = 0; i < numChildren; i++)
	    children[i].setState(childState);
    }

    setIconSwitch();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the icon switch based on the icon's state.
//

void
GraphIcon::setIconSwitch()
//
////////////////////////////////////////////////////////////////////////
{
    switch (state) {

      case NORMAL:
	if (isInstance())
	    iconSwitch->whichChild = 2; // Use instance icon
	else
	    iconSwitch->whichChild = 0; // Use regular icon
	break;

      case CLOSED:
	iconSwitch->whichChild = 1; // Use closed icon
	break;

      case HIDDEN:
	iconSwitch->whichChild = SO_SWITCH_NONE; // No icon!
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for when field editing is finished.
//

void
GraphIcon::finishShowFieldsCB(void *userData, FieldEditor *)
//
////////////////////////////////////////////////////////////////////////
{
    ((GraphIcon *) userData)->fieldsShown = FALSE;
}
