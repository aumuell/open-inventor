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

#include <Inventor/SbDict.h>

class GraphIcon;
class SoCoordinate3;
class SoIndexedLineSet;
class SoNode;
class SoPath;
class SoSwitch;

////////////////////////////////////////////////////////////////////////
//
// Class: DisplayGraph
//
// Holds display graph. The graph is stored as an array of GraphIcon
// instances.
//
////////////////////////////////////////////////////////////////////////

class DisplayGraph {

  public:

    // Constructor - passed scene to view graph of, so it can count nodes
    DisplayGraph(SoNode *sceneGraph);

    // Destructor
    ~DisplayGraph();

    // Sets up horizontal and vertical spacing values
    void		setIconSpacing(const SbVec2f spacing)
	{ iconSpacing = spacing; }

    // Returns horizontal and vertical spacing values
    SbVec2f		getIconSpacing() const	{ return iconSpacing; }

    // Builds display graph. Returns root node. May be passed another
    // DisplayGraph from which to copy info about icons.
    SoNode *		build(DisplayGraph *oldDisplayGraph = NULL);

    // Returns root node of display graph after it has been built
    SoNode *		getRoot() const		{ return root; }

    // Updates display graph when something changed in scene graph
    void		update();

    // Returns icon corresponding to given path to icon geometry
    // within the display graph
    GraphIcon *		find(const SoPath *path);

    // Returns the icon that corresponds to the given path that starts
    // at the root of the scene graph this is an iconic version of
    GraphIcon *		findFromScenePath(const SoPath *path);

    // Shows/hides what node an instance icon is an instance of.
    // Returns TRUE if graph needs to be rebuilt
    SbBool		toggleInstance(GraphIcon *icon);

    // Swaps instance icon with icon it is an instance of. This allows
    // selection under an instance of a group
    void		swapInstance(GraphIcon *icon);

    // Returns a path from the given root of the graph to given graph
    // icon's root. The path is ref'ed.
    SoPath *		findPathToIcon(SoNode *root, GraphIcon *icon);

    // Used by selection pasting to determine where to draw the
    // feedback icon and where in the graph to insert the selection
    SbBool		getPasteLocation(float x, float y,
					 GraphIcon *&parentIcon,
					 int &childIndex,
					 float &xFeedback, float &yFeedback);

    // Returns icon subgraphs for closed groups and instances
    static SoNode *	getInstanceIcon()	{ return instanceIcon; }
    static SoNode *	getClosedIcon()		{ return closedIcon; }

  private:
    static SbBool	initialized;	// TRUE if class initialized
    GraphIcon		*icons;		// Array of icons
    int			numIcons;	// Number of icons in array
    SoNode		*root;		// Root of display graph
    SoIndexedLineSet	*lineSet;	// Lines connecting icons
    SoCoordinate3	*coords;	// Coordinates of lines
    SoSwitch		*instSwitch;	// Switch to turn on/off instance line
    SoCoordinate3	*instCoords;	// Coordinates of line to show instance
    GraphIcon		*instShown;	// Which instance has line shown
    SbVec2f		iconSpacing;	// Spacing between icons
    static SbDict	*iconDict;	// Holds icons for each node type

    // Scene graphs used for icons
    static SoNode	*instanceIcon;	// Graph for instances
    static SoNode	*closedIcon;	// Graph for closed groups
    static SoNode	*otherIcon;	// Graph for all other icons

    // Used to detect instances when counting or setting up node
    // correspondences
    SbDict		nodeDict;

    // This is used to find a GraphIcon from a picked shape in the
    // display graph
    SbDict		shapeDict;

    // Initializes class
    static void		init();

    // Recursively sets up correspondences between icons and nodes
    void		setNode(SoNode *node, GraphIcon *parentIcon,
				int iconIndex, int &firstChildIndex,
				int childIndex);

    // Counts nodes in subgraph rooted by node
    int			countNodes(const SoNode *root);
    
    // Makes sure state of icons rooted by given icon is consistent
    static void		checkIconState(GraphIcon *root,
				       SbBool shouldBeHidden);

    // Sets up the coordinate3 and lineset nodes to display connecting lines
    void		buildLines();

    // Makes sure the given icon is visible
    void		makeIconVisible(GraphIcon *icon);

    // Sets up coords to draw line connecting instance
    void		setInstanceCoords();

    // Finds the icon graph to use for the given node
    static SoNode *	findIconGraph(SoNode *node);
};
