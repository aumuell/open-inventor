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

//  -*- C++ -*-

#include <Inventor/SbLinear.h>

class SbDict;
class SoGetBoundingBoxAction;
class SoNode;
class SoSwitch;
class SoTranslation;

class FieldEditor;

////////////////////////////////////////////////////////////////////////
//
// Class: GraphIcon
//
// Represents one node of a scene graph in the display graph.
//
////////////////////////////////////////////////////////////////////////

class GraphIcon {

  public:
    // One-time initialization
    static void		init();

    // Constructor and destructor
    GraphIcon();
    ~GraphIcon();

    // Sets scene graph node this corresponds to
    void		setNode(SoNode *node)		{ sceneNode = node; }

    // Sets parent icon
    void		setParent(GraphIcon *parentIcon){ parent = parentIcon;}

    // Sets index of child node in parent group
    void		setChildIndex(int index)	{ childIndex = index; }

    // Sets display subgraph for icon
    void		setGraph(SoNode *graph)		{ icon = graph; }

    // Sets node this icon is an instance of
    void		setInstance(GraphIcon *inst)	{ instanceOf = inst; }

    // Sets this icon to be a group
    void		setGroup(int nChildren, GraphIcon *firstChild)
	{ numChildren = nChildren; children = firstChild; }

    // Constructs and returns a display subgraph for the icon
    SoNode *		buildGraph();

    // Computes size of sub-display graph rooted by this
    void		computeSize(const SbVec2f &iconSpacing);

    // Computes position of children given position of this
    void		computePosition(const SbVec2f thisPosition,
					const SbVec2f &iconSpacing);

    // Returns scene graph node this corresponds to
    SoNode *		getNode() const		{ return sceneNode; }

    // Returns root node of icon subgraph for icon
    SoNode *		getIconRoot() const	{ return iconGraph; }

    // Returns TRUE if icon is a group
    SbBool		isGroup() const		{ return (numChildren >= 0); }

    // Returns TRUE if icon is an instance
    SbBool		isInstance() const	{ return (instanceOf != NULL);}

    // Returns icon this is an instance of
    GraphIcon *		getInstance() const	{ return instanceOf; }

    // Returns TRUE if icon is visible
    SbBool		isVisible() const	{ return (state != HIDDEN); }

    // Returns TRUE if group icon is open
    SbBool		isOpen() const		{ return (state == NORMAL); }

    // Returns TRUE if group icon is closed
    SbBool		isClosed() const	{ return (state == CLOSED); }

    // Returns parent icon
    GraphIcon *		getParent() const	{ return parent; }

    // Returns index of child node in parent group
    int			getChildIndex() const	{ return childIndex; }

    // Returns number of children in group
    SbBool		getNumChildren() const	{ return numChildren; }

    // Returns ith child of group
    GraphIcon *		getChild(int i) const	{ return &children[i]; }

    // Returns position of icon
    const SbVec2f &	getPosition() const	{ return position; }
    void		getPosition(float &x, float &y) const
	{ x = position[0]; y = position[1]; }

    // Returns size of icon
    const SbVec2f &	getSize() const		{ return iconSize; }
    void		getSize(float &x, float &y) const
	{ x = iconSize[0]; y = iconSize[1]; }

    // Returns size of icon subgraph
    const SbVec2f &	getSubgraphSize() const		{ return subgraphSize; }
    void		getSubgraphSize(float &x, float &y) const
	{ x = subgraphSize[0]; y = subgraphSize[1]; }

    // The following methods all operate on group icons. They should
    // not be called on non-group icons. Each returns TRUE if the icon
    // graph needs to be rebuilt as a result of the action.

    // Shows icon if it is HIDDEN. Returns TRUE if graph needs to be rebuilt.
    SbBool		show();

    // Hides icon if it is not HIDDEN. Returns TRUE if graph needs to
    // be rebuilt.
    SbBool		hide();

    // Opens group only if it is CLOSED. If openAll is TRUE, opens all
    // descendent groups. Returns TRUE if graph needs to be rebuilt.
    SbBool		openGroup(SbBool openAll);
    SbBool		closeGroup();	// Closes group only if it is NORMAL
    SbBool		toggleGroup();	// Toggles between CLOSED and NORMAL

    // The following method operates on an instance icon. It should
    // not be called on non-instance icons.

    // Swaps instance icon with icon it is an instance of. This allows
    // selection under an instance of a group
    void		swapInstance();

    // Brings up window to show field data for node represented by icon.
    void		showFields();

  private:

    // Possible states for an icon to be in
    enum IconState {
	NORMAL,			// Icon and subgraph are displayed
	CLOSED,			// Subgraph is closed
	HIDDEN 			// Icon is below a closed icon
    };

    // Used to compute sizes of graphs
    static SoGetBoundingBoxAction	*bboxAct;

    // Dictionary that maps graph nodes to GraphIcon structures
    static SbDict	*nodeDict;

    SoNode	*sceneNode;	// Corresponding node in scene graph
    SoNode	*icon;		// Subgraph representing node in display graph
    GraphIcon	*parent;	// Parent in display graph
    int		childIndex;	// Index of child in parent's list of children
    int		numChildren;	// Number of children of node (0 if nongroup)
    GraphIcon	*children;	// Pointer to array of children
    GraphIcon	*instanceOf;	// Icon this is instance of (NULL if not inst)
    IconState	state;		// State this icon is in
    SbBool	fieldsShown;	// TRUE if field values are shown

    // These nodes are used in the display graph
    SoNode		*iconGraph;	// Root of icon subgraph
    SoSwitch		*iconSwitch;	// Switches between reg/closed icons
    SoTranslation	*translation;	// Translates into position

    // Display graph icon size and position info:
    SbVec2f	iconSize;	// Size of icon
    SbVec2f	subgraphSize;	// Size including all children's icons
    SbVec2f	position;	// Position of top center of icon
    float	heightOffset;	// Distance from top of bbox to icon center

    // Changes state of icon, recursively
    void	setState(IconState newState);

    // Sets the icon switch based on the icon's state
    void	setIconSwitch();

    // Callback for when field editing is finished
    static void	finishShowFieldsCB(void *userData, FieldEditor *editor);
};
