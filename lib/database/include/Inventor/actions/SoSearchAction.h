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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Defines the SoSearchAction class
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SEARCH_ACTION_
#define  _SO_SEARCH_ACTION_

#include <Inventor/actions/SoSubAction.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSearchAction
//
//  For searching for specific nodes in scene graphs. Nodes can be
//  searched for by pointer, by type (exact or derived), by name, or
//  by a combination of these. You can also specify whether you are
//  interested in only the first match, only the last match, or all
//  matches. You can also control whether normal traversal rules are
//  followed (switching, separators, etc.) or whether every single
//  node is to be searched.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSearchAct
class SoSearchAction : public SoAction {

    SO_ACTION_HEADER(SoSearchAction);

  public:

    // Enum that defines the search criterion:
    enum LookFor {
	NODE = 0x01,	// Looking for a particular node
	TYPE = 0x02,	// Looking for a particualr type of node
	NAME = 0x04	// Looking for a node with a particular name
    };

    // Enum that defines which paths to return:
    enum Interest {
	FIRST,		// Return only the first path found
	LAST,		// Return only the last path found
	ALL		// Return all paths found
    };

    // Constructor (default action searches for first node of or
    // derived from given type, using regular traversal rules)
    SoSearchAction();

    // Destructor
    virtual ~SoSearchAction();

    // Reset options back to default values, clears list of returned paths
    void		reset();

    // Set what to look for; this is a bitmask of the LookFor enum
    // values. Default is no flags at all.
    void		setFind(int what)	{ lookingFor = what; }
    int			getFind()		{ return lookingFor; }

    // Returns/sets the node to search for
    SoNode *   		getNode() const		{ return node; }
    void		setNode(SoNode *n);

    // Returns/sets the node type searching for, and whether the type
    // must match exactly
    SoType		getType(SbBool &derivedIsOk) const
	{ derivedIsOk = derivedOk; return type; }	
    void		setType(SoType t, SbBool derivedIsOk = TRUE);

    // Returns/sets the name of the node to search for
    const SbName &	getName() const { return name; }
    void		setName(const SbName &n);

    // Returns/sets which paths to return. Default is FIRST.
    Interest		getInterest() const	{ return interest; }
    void		setInterest(Interest i)	{ interest = i;    }

    // Returns/sets whether searching uses normal traversal (switches, etc.)
    // or whether it traverses every single node. Default is FALSE;
    // C-api: name=isSearchAll
    SbBool		isSearchingAll() const	{ return searchingAll; }
    // C-api: name=setSearchAll
    void		setSearchingAll(SbBool flag) { searchingAll = flag; }

    // Returns resulting path (if interest is not ALL)
    SoPath *		getPath() const		{ return retPath; }

    // Returns resulting path list (if interest is ALL)
    SoPathList &	getPaths() 		{ return retPaths; }

  SoEXTENDER public:
    // Sets/returns whether action has found all desired nodes
    void		setFound()		{ setTerminated(TRUE); }
    SbBool		isFound() const		{ return hasTerminated(); }

    // Sets found path or adds to list of found paths (depending on interest)
    void		addPath(SoPath *path);

  SoINTERNAL public:
    static void		initClass();

    // This flag is used by the SoSwitch node, which must return a
    // different result from its 'affectsState' method when called
    // during a SearchAction that is searching all children.
    static SbBool	duringSearchAll;

  protected:
    // Initiates action on graph
    virtual void	beginTraversal(SoNode *node);

  private:

    SoType		type;		// Node type id to search for
    SbBool		derivedOk;	// Exact match or derived ok
    SoNode		*node;		// Node to search for
    SbName		name;		// Name of node to search for	
    int			lookingFor;     // What to search for
    Interest		interest;	// Which paths to return
    SbBool		searchingAll;	// Search all children or follow 
					// normal traversal rules.
    SoPath		*retPath;	// Found node (if interest != ALL)
    SoPathList		retPaths;	// Found nodes (if interest == ALL)
};

#endif /* _SO_SEARCH_ACTION_ */
