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

#include <stdlib.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbDict.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoType.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSwitch.h>

#include "GraphIcon.h"
#include "DisplayGraph.h"

SbBool	 DisplayGraph::initialized = FALSE;
SoNode	*DisplayGraph::instanceIcon;
SoNode	*DisplayGraph::closedIcon;
SoNode	*DisplayGraph::otherIcon;
SbDict	*DisplayGraph::iconDict;

#define ICON_FILE	"gviewIcons.iv"
#define ICON_INST_DIR	IVPREFIX "/demos/data/Inventor"
#define ICON_ENV_VAR	"IV_GRAPH_DIR"

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - passed scene to view graph of, so it can count
//    nodes and set up correspondences. Also takes another
//    DisplayGraph (may be NULL) from which to copy info about icons.
//

DisplayGraph::DisplayGraph(SoNode *sceneGraph)
//
////////////////////////////////////////////////////////////////////////
{
    int	firstChildIndex;

    // Make sure icon stuff is initialized
    if (! initialized)
	init();

    // Count nodes in scene. nodeDict is used to detect instances
    numIcons = countNodes(sceneGraph);

    // Allocate array of GraphIcon instances to represent the nodes
    icons = new GraphIcon[numIcons];

    // Clear nodeDict, which will now be used to hold correspondences
    // to scene graph nodes to detect instances
    nodeDict.clear();

    // First node goes in index 0. First child of it goes in index 1.
    // The childIndex of the root is -1 to distinguish it from children.
    firstChildIndex = 1;
    setNode(sceneGraph, NULL, 0, firstChildIndex, -1);

    // Set up default spacing
    iconSpacing.setValue(1.0, 1.0);

    // Not showing any instance line
    instShown = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

DisplayGraph::~DisplayGraph()
//
////////////////////////////////////////////////////////////////////////
{
    if (numIcons > 0)
	delete [] icons;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//

void
DisplayGraph::init()
//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator		*sep   = new SoSeparator;
    SoBaseColor		*color = new SoBaseColor;
    SoInput		in;
    SoNode		*inRoot, *parent;
    SoSearchAction	sa;
    const SoLabel	*label;
    SoType		type;
    int			i;

    // Build default closed icon graph
    sep   = new SoSeparator;
    color = new SoBaseColor;
    color->rgb.setValue(0.7, 0.2, 0.3);
    sep->addChild(color);
    sep->addChild(new SoCone);
    closedIcon = sep;

    // Build default instance icon graph
    sep   = new SoSeparator;
    color = new SoBaseColor;
    color->rgb.setValue(0.6, 0.6, 0.1);
    sep->addChild(color);
    sep->addChild(new SoSphere);
    instanceIcon = sep;

    // Build default unspecified node icon graph
    otherIcon = new SoCube;

    instanceIcon->ref();
    closedIcon->ref();
    otherIcon->ref();

    // Set up icon dictionary
    iconDict = new SbDict;

    // Insert icon for unspecific nodes. The casting is because C++
    // thinks the SoType is more than just an integer
    type = SoNode::getClassTypeId();
    iconDict->enter((unsigned long) * (int *) &type, (void *) otherIcon);

#if 0
    // Tell input to look for icon file based on environment variable
    // (before looking in current directory)
    in.addEnvDirectoriesFirst(ICON_ENV_VAR);

    // If it's not found in any of those, look in the standard
    // installed place
    in.addDirectoryLast(ICON_INST_DIR);

    // Open file containing icon graphs
    if (! in.openFile(ICON_FILE)) {
	fprintf(stderr, "Can't open %s\n", ICON_FILE);
	exit(1);
    }
#endif

#include "gviewIcons.iv.h"

    // Set to read from included char array
    in.setBuffer((void *) gviewIcons, sizeof(gviewIcons));

    // Read graph from file
    if (! SoDB::read(&in, inRoot)) {
	fprintf(stderr, "Error reading %s\n", ICON_FILE);
	exit(1);
    }
    inRoot->ref();

    // Search for all labels
    sa.setType(SoLabel::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(inRoot);

    // For each label, store its parent node as the root of the icon
    // graph for the named node class
    for (i = 0; i < sa.getPaths().getLength(); i++) {
	label  = (const SoLabel *) sa.getPaths()[i]->getTail();
	parent = sa.getPaths()[i]->getNodeFromTail(1);

	// Find the type id for the named class
	type = SoType::fromName(label->label.getValue());

	// If such a type exists, add the parent to the dictionary
	if (! type.isBad()) {
	    iconDict->enter((unsigned long) * (int *) &type, (void *) parent);
	    parent->ref();
	}

	// Otherwise, it might be the icon for instances or closed groups
	else if (label->label.getValue() == "Instance") {
	    instanceIcon->unref();
	    instanceIcon = parent;
	    instanceIcon->ref();
	}
	else if (label->label.getValue() == "Closed") {
	    closedIcon->unref();
	    closedIcon = parent;
	    closedIcon->ref();
	}
    }

    inRoot->unref();

    GraphIcon::init();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up correspondence between display graph icon and scene
//    graph node. Recurses to handle children. The second parameter is
//    the index of the graph icon to set. The third parameter is the
//    index into the "icons" array in which to put the first child of
//    the current icon. The fourth parameter is the index of this
//    child in its parent's list of children.
//

void
DisplayGraph::setNode(SoNode *node, GraphIcon *parentIcon,
		      int iconIndex, int &firstChildIndex, int childIndex)
//
////////////////////////////////////////////////////////////////////////
{
    GraphIcon	*icon = &icons[iconIndex];
    void	*oldIconPtr;

    icon->setNode(node);
    icon->setParent(parentIcon);
    icon->setChildIndex(childIndex);
    icon->setGraph(findIconGraph(node));

    // See if node was already counted - if so, it's an instance
    if (nodeDict.find((unsigned long) node, oldIconPtr))
	icon->setInstance((GraphIcon *) oldIconPtr);

    else {
	nodeDict.enter((unsigned long) node, (void *) icon);

	// If it's a group, recurse on children
	if (node->isOfType(SoGroup::getClassTypeId())) {
	    const SoGroup	*g = (const SoGroup *) node;
	    int			childIndex = firstChildIndex, i;

	    icon->setGroup(g->getNumChildren(), &icons[firstChildIndex]);

	    firstChildIndex += g->getNumChildren();

	    for (i = 0; i < g->getNumChildren(); i++)
		setNode(g->getChild(i), icon, childIndex + i,
			firstChildIndex, i);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds display graph. Returns root node. May be passed another
//    DisplayGraph from which to copy info about icons. (It is NULL
//    otherwise.)
//

SoNode *
DisplayGraph::build(DisplayGraph *oldDisplayGraph)
//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator		*rootSep;	// Root for the display graph
    SoPickStyle		*pickStyle;	// Makes lines unpickable
    SoLightModel	*lightModel;	// To turn off lighting for lines
    SoDrawStyle		*drawStyle;	// To make lines wider
    SoLineSet		*instLine;	// Line to show instance connection
    SoBaseColor		*instColor;	// Color of instance connection line
    SoSeparator		*instSep;	// Sub-root for instance stuff
    SoNode		*iconGraph;
    int			i;

    rootSep = new SoSeparator(3 + numIcons);
    rootSep->ref();

    // Build subgraphs for all icons; add them to root. Also store
    // correspondence between GraphIcon and top node in icon graph in
    // dictionary. This lets us find the GraphIcon from a picked
    // geometry.
    for (i = 0; i < numIcons; i++) {
	iconGraph = icons[i].buildGraph();
	rootSep->addChild(iconGraph);
	shapeDict.enter((unsigned long) iconGraph, (void *) &icons[i]);
    }

    // Start out with just the first level of children visible
    if (icons[0].isGroup())
	icons[0].openGroup(FALSE);
    else
	icons[0].show();

    // If we are to copy visibility info from an old display graph, do so
    if (oldDisplayGraph != NULL) {
	int		i;
	GraphIcon	*newIcon, *oldIcon;
	void		*iconPtr;

	// Run through all icons in new display graph
	for (i = 0, newIcon = icons; i < numIcons; i++, newIcon++) {

	    // If there was an old icon for the node that this icon represents
	    if (oldDisplayGraph->nodeDict.find((unsigned long) newIcon->getNode(),
					       iconPtr)) {

		oldIcon = (GraphIcon *) iconPtr;

		// Change state of icon if necessary
		if (oldIcon->isGroup()) {
		    if (oldIcon->isOpen() && ! newIcon->isOpen())
			newIcon->openGroup(FALSE);
		    else if (oldIcon->isClosed() && ! newIcon->isClosed())
			newIcon->closeGroup();
		}
	    }
	}

	// If nodes had been deleted from the old display graph, the
	// state of the graph can be inconsistent. Clean it up.
	checkIconState(&icons[0], FALSE);
    }

    // Compute sizes and positions of icons
    icons[0].computeSize(iconSpacing);
    icons[0].computePosition(SbVec2f(0.0, 0.0), iconSpacing);

    // Lines are unpickable, unlit, and wide
    pickStyle = new SoPickStyle;
    pickStyle->style = SoPickStyle::UNPICKABLE;
    drawStyle = new SoDrawStyle;
    drawStyle->lineWidth = 2;
    lightModel = new SoLightModel;
    lightModel->model = SoLightModel::BASE_COLOR;
    rootSep->addChild(pickStyle);
    rootSep->addChild(drawStyle);
    rootSep->addChild(lightModel);

    // Build the stuff to represent the lines connecting icons
    coords  = new SoCoordinate3;
    lineSet = new SoIndexedLineSet;
    buildLines();
    rootSep->addChild(coords);
    rootSep->addChild(lineSet);

    // Build a line set to display instance connection
    instSwitch	= new SoSwitch;
    instSep	= new SoSeparator;
    instColor	= new SoBaseColor;
    instCoords	= new SoCoordinate3;
    instLine	= new SoLineSet;
    instColor->rgb.setValue(0.2, 0.2, 0.9);
    instSep->addChild(instColor);
    instSep->addChild(instCoords);
    instSep->addChild(instLine);
    instSwitch->addChild(instSep);
    rootSep->addChild(instSwitch);

    rootSep->unrefNoDelete();

    root = rootSep;

    return root;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates display graph when something changed in scene graph.
//

void
DisplayGraph::update()
//
////////////////////////////////////////////////////////////////////////
{
    // Compute sizes and positions of icons
    icons[0].computeSize(iconSpacing);
    icons[0].computePosition(SbVec2f(0.0, 0.0), iconSpacing);

    // Just rebuild the stuff to represent the connecting lines
    buildLines();

    // Make sure the instance line (if any) is ok
    if (instShown != NULL) {

	// Turn off line if either end is not visible
	if (! instShown->isVisible() ||
	    ! instShown->getInstance()->isVisible())
	    toggleInstance(instShown);

	// Otherwise, make sure coords are still correct
	else
	    setInstanceCoords();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns icon corresponding to given selected path. We search up
//    the path from the bottom until we find a node that was entered
//    in the dictionary. If we find one, we return the corresponding
//    GraphIcon.
//

GraphIcon *
DisplayGraph::find(const SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    void	*iconPtr;
    int		i;

    if (path == NULL)
	return NULL;

    // Check nodes from tail of path toward head
    for (i = path->getLength() - 1; i >= 0; --i)
	if (shapeDict.find((unsigned long) path->getNode(i), iconPtr))
	    return (GraphIcon *) iconPtr;

    // Not found
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the icon that corresponds to the given path that starts
//    at the root of the scene graph this is an iconic version of.
//

GraphIcon *
DisplayGraph::findFromScenePath(const SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    GraphIcon	*icon;
    int		i, index;
    SbBool	needUpdate = FALSE;

    // Trace the icons down to the correct one. On the way, make sure
    // the icon is visible: no closed groups or instances.

    icon = icons;

    if (icon->isClosed()) {
	icon->openGroup(FALSE);
	needUpdate = TRUE;
    }

    for (i = 1; i < path->getLength(); i++) {

	index = path->getIndex(i);

	if (! icon->isGroup() || index >= icon->getNumChildren()) {
	    fprintf(stderr, "Yipes! bad path in findFromScenePath()\n");
	    return icon;
	}

	icon = icon->getChild(index);

	// Last icon can be closed or an instance
	if (i < path->getLength() - 1) {
	    if (icon->isClosed()) {
		icon->openGroup(FALSE);
		needUpdate = TRUE;
	    }

	    else if (icon->isInstance()) {
		swapInstance(icon);
		needUpdate = TRUE;
	    }
	}
    }

    if (needUpdate)
	update();

    return icon;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Shows/hides what node an instance icon is an instance of.
//    Returns TRUE if graph needs to be rebuilt.
//

SbBool
DisplayGraph::toggleInstance(GraphIcon *icon)
//
////////////////////////////////////////////////////////////////////////
{
    // If there already is an instance showing and it is this icon,
    // just turn it off
    if (instShown == icon) {
	instSwitch->whichChild = SO_SWITCH_NONE;
	instShown = NULL;
	return TRUE;
    }

    // Find node this icon is an instance of
    GraphIcon	*instanceOf = icon->getInstance();

    // Make sure that node is visible
    if (! instanceOf->isVisible()) {
	makeIconVisible(instanceOf);
	// Recompute positions and all
	update();
    }

    // Save the pointer for later
    instShown = icon;

    // Set instance line to join the center of the two icons
    setInstanceCoords();

    // Turn the line on by setting the switch
    instSwitch->whichChild = 0;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Swaps instance icon with icon it is an instance of. This allows
//    selection under an instance of a group.
//

void
DisplayGraph::swapInstance(GraphIcon *instanceIcon)
//
////////////////////////////////////////////////////////////////////////
{
    GraphIcon	*instanceOf, *icon;
    int		i;

    instanceOf = instanceIcon->getInstance();

    // Make sure the instance line is still valid, if present
    if (instShown == instanceIcon)
	instShown = instanceIcon->getInstance();

    // Swap the icon stuff
    instanceIcon->swapInstance();

    // If any other icons were instances of the other icon, make them
    // instances of the given icon
    for (i = 0, icon = icons; i < numIcons; i++, icon++)
	if (icon->getInstance() == instanceOf)
	    icon->setInstance(instanceIcon);

    // Recompute positions and all
    update();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a path from the given root of the graph to the given
//    graph icon's root. The path is ref'ed.
//

SoPath *
DisplayGraph::findPathToIcon(SoNode *root, GraphIcon *icon)
//
////////////////////////////////////////////////////////////////////////
{
    SoSearchAction	sa;
    SoPath		*path;

    // This may be called with a NULL icon pointer.
    if (icon == NULL)
	return NULL;

    sa.setNode(icon->getIconRoot());
    sa.setInterest(SoSearchAction::FIRST);
    sa.apply(root);

    // Ref the path so it won't go away
    path = sa.getPath();

    if (path == NULL) {
	fprintf(stderr, "Yow! Can't find path to node!\n");
	return NULL;
    }

    path->ref();

    return path;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is used by selection pasting to determine where to draw the
//    feedback icon and where in the graph to insert the selection. It
//    returns TRUE if it was able to find a good place to paste. The
//    parentIcon parameter returns the parent under which to paste,
//    and the childIndex parameter returns the index of the child that
//    the pasted selection will become. The xFeedback and yFeedback
//    parameters indicate where the feedback should be centered.
//

SbBool
DisplayGraph::getPasteLocation(float x, float y,
			       GraphIcon *&parentIcon, int &childIndex, 
			       float &xFeedback, float &yFeedback)
//
////////////////////////////////////////////////////////////////////////
{
// X coordinate of feedback to right of given icon
#define NEXT_X_RIGHT(rightIcon)						      \
	(rightIcon->getPosition()[0] +					      \
	 .5 * (rightIcon->getSubgraphSize()[0] + iconSpacing[0]))

// X coordinate of feedback to left of given icon
#define NEXT_X_LEFT(leftIcon)						      \
	(leftIcon->getPosition()[0] -					      \
	 .5 * (leftIcon->getSubgraphSize()[0] + iconSpacing[0]))

// X coordinate of feedback between given icons
#define X_BETWEEN(leftIcon, rightIcon)					      \
	(.5 * (leftIcon->getPosition()[0] + rightIcon->getPosition()[0]))

// Y coordinate of feedback to left or right of given icon
#define NEXT_Y(icon)	(icon->getPosition()[1] - .5 * icon->getSize()[1])

    GraphIcon	*icon, *nextChild, *lastChild;
    SbVec2f	point(x, y);
    float	xPos, yPos, xSize, ySize;
    float	xl, xr, xFudge, yb, yt;
    SbBox2f	iconBox;
    int		index, i;

    for (i = 0, icon = icons; i < numIcons; i++, icon++) {

	if (! icon->isVisible())
	    continue;

	// Find the extent of the icon bounding box
	icon->getPosition(xPos, yPos);
	icon->getSize(xSize, ySize);
	xl = xPos - .5 * xSize;
	xr = xPos + .5 * xSize;
	yb = yPos - ySize;
	yt = yPos;
	iconBox.setBounds(xl, yb, xr, yt);

	//------------------------------------------------------------------
	// The point is on the icon
	//------------------------------------------------------------------

	if (iconBox.intersect(point)) {

	    // See if it's a group
	    if (icon->isGroup()) {

		parentIcon = icon;

		// If group has no children, draw feedback below icon
		if (icon->getNumChildren() == 0) {
		    childIndex = 0;
		    xFeedback = xPos;
		    yFeedback = yb - iconSpacing[1];
		    return TRUE;
		}

		// If open group, draw feedback to right of last child
		else if (icon->isOpen()) {
		    childIndex = icon->getNumChildren();
		    lastChild = icon->getChild(childIndex - 1);
		    xFeedback = NEXT_X_RIGHT(lastChild);
		    yFeedback = NEXT_Y(lastChild);
		}

		// If closed group, draw feedback below icon
		else {
		    childIndex = icon->getNumChildren();
		    xFeedback = xPos;
		    yFeedback = yb - iconSpacing[1];
		}

		return TRUE;
	    }

	    // If non-group, draw feedback halfway to next sibling (if
	    // any) to right or left, depending on whether the cursor
	    // is to the right or left of the center of the icon
	    else {
		parentIcon = icon->getParent();
		index = icon->getChildIndex();

		// Cursor is on left side
		if (x < xPos) {
		    childIndex = index;

		    if (index == 0)
			xFeedback = NEXT_X_LEFT(icon);
		    else {
			nextChild = parentIcon->getChild(index - 1);
			xFeedback = X_BETWEEN(nextChild, icon);
		    }
		}

		// Cursor is on right side
		else {
		    childIndex = index + 1;

		    if (index == parentIcon->getNumChildren() - 1)
			xFeedback = NEXT_X_RIGHT(icon);
		    else {
			nextChild = parentIcon->getChild(index + 1);
			xFeedback = X_BETWEEN(icon, nextChild);
		    }
		}

		yFeedback = yPos - .5 * ySize;

		return TRUE;
	    }
	}

	//------------------------------------------------------------------
	// The point is NOT on the icon, but it may be close enuf to one side
	//------------------------------------------------------------------

	if ((parentIcon = icon->getParent()) != NULL) {
	    index = icon->getChildIndex();

	    // If this is the first child of a group, see if the point
	    // is close enough to its left
	    if (index == 0)
		xFudge = 0.5 * icon->getSubgraphSize()[0] + iconSpacing[0];

	    // Otherwise, see if the point is between it and the next
	    // icon to the left
	    else {
		nextChild = parentIcon->getChild(index - 1);
		xFudge = xPos - nextChild->getPosition()[0];
	    }

	    iconBox.setBounds(xPos - xFudge, yb, xl, yt);

	    if (iconBox.intersect(point)) {
		childIndex = index;
		if (index == 0)
		    xFeedback = NEXT_X_LEFT(icon);
		else
		    xFeedback = X_BETWEEN(icon, nextChild);
		yFeedback = yPos - .5 * ySize;
		return TRUE;
	    }

	    // If this is the last child of a group, see if the point
	    // is close enough to its right
	    if (index == parentIcon->getNumChildren() - 1) {
		xFudge = 0.5 * (icon->getSubgraphSize()[0] + iconSpacing[0]);

		iconBox.setBounds(xr, yb, xPos + xFudge, yt);

		if (iconBox.intersect(point)) {
		    childIndex = index + 1;
		    xFeedback = xPos + xFudge;
		    yFeedback = yPos - .5 * ySize;
		    return TRUE;
		}
	    }
	}

	//------------------------------------------------------------------
	// Now see if it is below a closed or childless group icon
	//------------------------------------------------------------------

	// See if the point is close below a closed or childless group
	if (icon->isGroup() &&
	    (icon->isClosed() || icon->getNumChildren() == 0)) {

	    iconBox.setBounds(xl, yb - iconSpacing[1], xr, yb);

	    if (iconBox.intersect(point)) {
		parentIcon = icon;
		childIndex = icon->getNumChildren();
		xFeedback = xPos;
		yFeedback = yb - iconSpacing[1];

		return TRUE;
	    }
	}
    }

    //------------------------------------------------------------------
    // None of the above...
    //------------------------------------------------------------------

    return FALSE;
}

#undef NEXT_X_RIGHT
#undef NEXT_X_LEFT
#undef X_BETWEEN
#undef NEXT_Y

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Counts nodes in subgraph rooted by node
//

int
DisplayGraph::countNodes(const SoNode *root)
//
////////////////////////////////////////////////////////////////////////
{
    int	num = 1;

    // See if node was already counted - if so, it's an instance
    if (! nodeDict.enter((unsigned long) root, (void *) 1))
	;

    // Count children if this node is a group
    else if (root->isOfType(SoGroup::getClassTypeId())) {
	const SoGroup	*g = (const SoGroup *) root;
	int		i;

	for (i = 0; i < g->getNumChildren(); i++)
	    num += countNodes(g->getChild(i));
    }

    return num;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Makes sure state of icons rooted by given icon is consistent.
//

void
DisplayGraph::checkIconState(GraphIcon *root, SbBool shouldBeHidden)
//
////////////////////////////////////////////////////////////////////////
{
    if (shouldBeHidden && root->isVisible())
	root->hide();

    // Hide children if necessary
    if (root->isGroup()) {

	if (! shouldBeHidden && root->isClosed())
	    shouldBeHidden = TRUE;

	for (int i = 0; i < root->getNumChildren(); i++)
	    checkIconState(root->getChild(i), shouldBeHidden);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up the coordinate3 and lineset nodes to display connecting
//    lines between icons.
//

void
DisplayGraph::buildLines()
//
////////////////////////////////////////////////////////////////////////
{
    int			numVisible, numLines, i, j, c;
    SbVec3f		*pts;
    int32_t		*inds;
    GraphIcon		*icon;
    float		hy, yBar, x, y, sx, sy, cx1, cy1, cx2, cy2;

    // Count non-hidden icons
    numVisible = 0;
    for (i = 0; i < numIcons; i++)
	if (icons[i].isVisible())
	    numVisible++;

    // All nodes except the root have 1 line above them
    numLines = numVisible - 1;

    // All open groups with > 1 children have 2 lines below them
    for (i = 0; i < numIcons; i++)
	if (icons[i].isGroup() &&
	    icons[i].getNumChildren() > 1 &&
	    icons[i].isOpen())
	    numLines += 2;

    // No groups? We're done!
    if (numLines <= 0) {
	lineSet->coordIndex.deleteValues(0);
	return;
    }

    hy   = iconSpacing[1] / 2.0;
    pts  = new SbVec3f[numLines * 2];
    inds = new int32_t[numLines * 3];

    // Fill in coordinates
    for (i = c = 0, icon = icons; i < numIcons; i++, icon++) {

	icon->getPosition(x, y);
	icon->getSize(sx, sy);

	if (icons[i].isGroup() &&
	    icons[i].getNumChildren() > 1 &&
	    icons[i].isOpen()) {

	    yBar = y - sy - hy;

	    // Line from top icon down
	    pts[c+0].setValue(x, yBar + hy, 0.0);
	    pts[c+1].setValue(x, yBar, 0.0);

	    // Horizontal bar
	    icon->getChild(0)->getPosition(cx1, cy1);
	    icon->getChild(icon->getNumChildren() - 1)->getPosition(cx2, cy2);
	    pts[c+2].setValue(cx1, yBar, 0.0);
	    pts[c+3].setValue(cx2, yBar, 0.0);
	    c += 4;

	    for (j = 0; j < icons[i].getNumChildren(); j++) {
		icon->getChild(j)->getPosition(cx1, cy1);
		pts[c+0].setValue(cx1, yBar, 0.0);
		pts[c+1].setValue(cx1, cy1, 0.0);
		c += 2;
	    }
	}
	else if (icons[i].isGroup() &&
		 icons[i].getNumChildren() == 1 &&
		 icons[i].isOpen()) {
	    // Line from top icon down to child
	    icon->getChild(0)->getPosition(cx1, cy1);
	    pts[c+0].setValue(x, y - sy, 0.0);
	    pts[c+1].setValue(cx1, cy1, 0.0);
	    c += 2;
	}
    }

    // Fill in indices
    c = 0;
    for (i = 0; i < numLines; i++) {
	inds[c+0] = i * 2;
	inds[c+1] = i * 2 + 1;
	inds[c+2] = SO_END_LINE_INDEX;
	c += 3;
    }

    coords->point.setValues(0, numLines * 2, pts);
    lineSet->coordIndex.setValues(0, numLines * 3, inds);

    // Delete old values if not needed any more
    if (lineSet->coordIndex.getNum() > numLines * 3)
	lineSet->coordIndex.deleteValues(numLines * 3);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Makes sure the given icon is visible.
//

void
DisplayGraph::makeIconVisible(GraphIcon *icon)
//
////////////////////////////////////////////////////////////////////////
{
    // Could this be called on the root? I think not! It's always visible!
    if (icon->getParent() == NULL)
	return;

    // Make sure parent is visible
    if (! icon->getParent()->isVisible())
	makeIconVisible(icon->getParent());

    // Make sure parent is open
    icon->getParent()->openGroup(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets instCoords so that line between current instance and what
//    is an instance of will be drawn correctly.
//

void
DisplayGraph::setInstanceCoords()
//
////////////////////////////////////////////////////////////////////////
{
    // Find node the shown instance icon is an instance of
    GraphIcon	*instanceOf = instShown->getInstance();

    // Set instance line to join the center of the two icons
    float	x1, y1, x2, y2;
    float	sx1, sy1, sx2, sy2;
    instShown->getPosition(x1, y1);
    instShown->getSize(sx1, sy1);
    instanceOf->getPosition(x2, y2);
    instanceOf->getSize(sx2, sy2);
    // Line connects to center of icons
    y1 -= sy1 / 2.0;
    y2 -= sy2 / 2.0;
    instCoords->point.set1Value(0, x1, y1, 0.0);
    instCoords->point.set1Value(1, x1 + .2 * (x2-x1), y1 + .2 * (y2-y1), 3.0);
    instCoords->point.set1Value(2, x1 + .8 * (x2-x1), y1 + .8 * (y2-y1), 3.0);
    instCoords->point.set1Value(3, x2, y2, 0.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Finds the icon graph to use for the given node.
//

SoNode *
DisplayGraph::findIconGraph(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // Look in dictionary for icon for this node class. If not found,
    // check parent class. Repeat until an icon is found. Since SoNode
    // always has a valid icon, this loop will always terminate
    // successfully.

    SoType	type;
    void	*graphPtr;

    type = node->getTypeId();

    while (TRUE) {
	if (iconDict->find((unsigned long) * (int *) &type, graphPtr))
	    return (SoNode *) graphPtr;

	type = type.getParent();
    }
}
