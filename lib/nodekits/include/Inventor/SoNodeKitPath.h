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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file contains the definition of the SoNodeKitPath class
 |
 |   Author(s)		: Paul Isaacs, Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NODE_KIT_PATH_
#define  _SO_NODE_KIT_PATH_

#include <Inventor/SoPath.h>

class SoBaseKit;
class SoSearchAction;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNodeKitPath
//
//  A SoNodeKitPath represents a scene graph or subgraph. It contains
//  pointers to a chain of nodeKitss, each of which is a child of the
//  previous. 
//  
//  Intermediary nodes between nodeKits are not included in the nodeKit path.
//
//  
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoNkitPath
class SoNodeKitPath : public SoPath {

  public:

    // Returns length of path chain (number of nodes)
    // C-api: name=getLen
    int			getLength() const;

    // Returns the last nodeKit in a path chain.
    SoNode *		getTail() const;

    // Returns pointer to ith nodeKit in chain
    SoNode *		getNode(int i) const;

    // Returns pointer to ith node from the tail in chain
    // i.e. index 0 == tail, index 1 == 1 before tail, 2 == 2 before tail
    SoNode *		getNodeFromTail(int i) const;

    // Removes all nodes from indexed nodeKit on
    void		truncate(int start);

    // Allows path to be treated as a stack: pop the last nodeKit
    void		pop();

    // Adds nodeKit to end of chain; uses first occurrance of nodeKit as
    // part of current last nodekit. If path is empty, this is 
    // equivalent to setHead().
    void		append(SoBaseKit *childKit);

    // Adds all nodeKits in path to end of chain; head node of fromPath must
    // be a part of current last node
    // C-api: name=AppendPath
    void		append(const SoNodeKitPath *fromPath);

    // Returns TRUE if the passed nodeKit is in the path chain
    SbBool		containsNode(SoBaseKit *node) const;
    
    // If the paths have different head nodes, this returns -1.
    // Otherwise, it returns the index into the chain of the last nodeKit
    // (starting at the head) that is the same for both paths.
    int			findFork(const SoNodeKitPath *path) const;

    // Comparison operator: returns TRUE if all nodes on the nodekit path
    // are equal
    friend int operator ==(const SoNodeKitPath &p1, const SoNodeKitPath &p2);

  protected:
    // Hide the constructor
    SoNodeKitPath(int approxLength) : SoPath(approxLength)	{}
    // Destructor
    virtual ~SoNodeKitPath();

  private:
    // This action is used to search for nodes.
    static SoSearchAction *searchAction;

    // Hide these methods. They make no sense for a SoNodeKitPath.

    // The methods that take an index or a node are replaced by
    // methods that take a nodekit argument.
    void		append(int childIndex);
    void		append(SoNode *childNode);
    void		append(const SoPath *fromPath);
    void		push(int childIndex);
    int			getIndex(int i) const;
    int			getIndexFromTail(int i) const;
    void                insertIndex(SoNode *parent,int newIndex);
    void                removeIndex(SoNode *parent,int oldIndex);
    void                replaceIndex(SoNode *parent,int index,SoNode *newChild);
};

#endif /* _SO_NODE_KIT_PATH_ */
