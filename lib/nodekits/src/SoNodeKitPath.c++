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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoNodeKitPath
 |
 |   Author(s)		: Paul Isaacs, Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/actions/SoSearchAction.h>

SoSearchAction *SoNodeKitPath::searchAction = NULL;

////////////////////////////////////////////////////////////////////////
//
// SoNodeKitPath class
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gives the length of the nodeKitPath. This is 1 for the head
//    plus one more for any other nodekit in the path.
//
// Use: public

int
SoNodeKitPath::getLength() const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fp = (SoFullPath *) this;

    if (fp->getLength() == 0)
	return 0;

    int count = 1;

    for ( int i = 1; i < fp->getLength(); i++ )
	if (  fp->getNode(i)->isOfType( SoBaseKit::getClassTypeId() ) )
	    count++;

    return count;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the last nodekit along the path, or the head if none are found.
//
// Use: public

SoNode *
SoNodeKitPath::getTail() const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fp = (SoFullPath *) this;

    if (fp->getLength() == 0)
	return NULL;

    for ( int i = fp->getLength() - 1; i >= 0; i-- )
	if (  fp->getNode(i)->isOfType( SoBaseKit::getClassTypeId() ) )
	    return fp->getNode(i);

    return fp->getHead();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the i'th nodekit in the path. The head always counts as node 0
//   whether it is a nodekit or not.
//
// Use: public

SoNode *
SoNodeKitPath::getNode(int i) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fp = (SoFullPath *) this;

    if (fp->getLength() == 0 || i < 0 )
	return NULL;

    SoNode *answer;
    int curKitCount = -1;

    for ( int j = 0; j < fp->getLength(); j++ ) {
	if (j == 0 || fp->getNode(j)->isOfType( SoBaseKit::getClassTypeId() )) {
	    answer = fp->getNode(j);
	    curKitCount++;

	    if (curKitCount == i )
		return answer;
	}
    }
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the i'th nodekit from the tail in the path.
//   i.e. index 0 == tail, index 1 == 1 before tail, etc.
//   whether it is a nodekit or not.
//
// Use: public

SoNode *
SoNodeKitPath::getNodeFromTail(int i) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *fp = (SoFullPath *) this;

    if (fp->getLength() == 0 )
	return NULL;

    if ( i < 0 )
	// return last nodekit. Use 'this' not 'fp'
	return getTail();

    SoNode *answer;
    int curKitCount = -1;

    for ( int j = fp->getLength() - 1; j >= 0; j-- ) {
	if (j == 0 || fp->getNode(j)->isOfType( SoBaseKit::getClassTypeId() )) {
	    answer = fp->getNode(j);
	    curKitCount++;

	    if (curKitCount == i )
		return answer;
	}
    }
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes all nodes from the indexed nodeKit on
//
// Use: public

void
SoNodeKitPath::truncate(int start)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the start'th nodekit
    SoNode *n = getNode(start);

    int fullStart = 0;
    SoFullPath *fp = (SoFullPath *) this;
    for ( int i = 0; i < fp->getLength(); i++ )
	if ( fp->getNode(i) == n )
	    fullStart = i;

    fp->truncate( fullStart);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pop all nodes off the path up 'til the nodeKit before the last 
//    one.
//
// Use: public

void
SoNodeKitPath::pop()
//
////////////////////////////////////////////////////////////////////////
{
    truncate( getLength() - 1 );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a node to the end of the current chain of nodes in a path.
//    The node is passed by pointer, so it must be a descendant of the
//    current last nodeKit in the chain.
//    If it is used more than once as a descendant, the first occurrance 
//    is appended. If the path is empty, this is equivalent to setHead(node).
//
// Use: public

void
SoNodeKitPath::append(SoBaseKit *childKit)
//
////////////////////////////////////////////////////////////////////////
{
    // If the path is empty, this is a setHead() operation
    if (getLength() == 0) {
	setHead(childKit);
	return;
    }

    // Get tail kit in path.
    SoNode	*tail;
    tail = getTail();

#ifdef DEBUG
    // Make sure tail node can have children.
    if (tail->getChildren() == NULL ) {
	SoDebugError::post("SoNodeKitPath::append",
	                   "tail node can not have children");
	return;
    }
#endif

    // Turn ON searching below nodekits...
    SbBool wasSearching = SoBaseKit::isSearchingChildren();
    SoBaseKit::setSearchingChildren(TRUE);

	// Search for childKit under tail

	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setNode( childKit );
	searchAction->apply( tail );
	SoFullPath *kidPath = (SoFullPath *) searchAction->getPath();

    // Restore searching under nodekits...
    SoBaseKit::setSearchingChildren(wasSearching);

#ifdef DEBUG
    // Make sure we found it...
    if (kidPath == NULL) {
	SoDebugError::post("SoNodeKitPath::append",
			   "node to append is not a descendent of path tail");
	return;
    }
#endif

    SoFullPath *fp = (SoFullPath *) this;

    // truncate full path to end at tail (if the tail is a nodekit but
    // the fullPath tail is not, this is necessary).
    int i;
    for ( i = fp->getLength() - 1; i >= 0; i-- ) {
	if ( fp->getNode(i) != tail )
	    fp->pop();
	else
	    break;
    }

    // append all nodes from the kidPath (except its head, which is tail of fp)
    for ( i = 1; i < kidPath->getLength(); i++ )
	fp->append( kidPath->getIndex(i) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds all nodes in path to end of chain; head node of fromPath
//    must be a descendant of current last nodeKit.
//
// Use: public

void
SoNodeKitPath::append(const SoNodeKitPath *fromPath)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Make sure there is already a node in the path
    if (getLength() == 0) {
	SoDebugError::post("SoNodeKitPath::append",
			   "no head node to append to");
	return;
    }
#endif

    // Get tail of path.
    SoNode	*tail;
    tail = getTail();

#ifdef DEBUG
    // Make sure tail node can have children.
    if (tail->getChildren() == NULL ) {
	SoDebugError::post("SoNodeKitPath::append",
			   "tail node can not have children.");
	return;
    }
#endif

    // Turn ON searching below nodekits...
    SbBool wasSearching = SoBaseKit::isSearchingChildren();
    SoBaseKit::setSearchingChildren(TRUE);

	// Search for head of second path under tail
	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setNode( fromPath->getHead() );
	searchAction->apply( tail );

    // Restore searching under nodekits...
    SoBaseKit::setSearchingChildren(wasSearching);

    SoFullPath *kidPath = (SoFullPath *) searchAction->getPath();

#ifdef DEBUG
    // Make sure we found it...
    if (kidPath == NULL) {
	SoDebugError::post("SoNodeKitPath::append",
			"head of append path is not descendent of path tail");
	return;
    }
#endif

    SoFullPath *fp = (SoFullPath *) this;

    // truncate full path to end at tail (if the tail is a nodekit but
    // the fullPath tail is not, this is necessary).
    int i;
    for ( i = fp->getLength() - 1; i >= 0; i-- ) {
	if ( fp->getNode(i) != tail )
	    fp->pop();
	else
	    break;
    }

    // append all nodes from the kidPath (except the head, which is tail of fp)
    for ( i = 1; i < kidPath->getLength(); i++ )
	fp->append( kidPath->getIndex(i) );

    // Append each node in fromPath (except head, which we just appended)
    // to end of path
    SoFullPath *fullFrom = (SoFullPath *) fromPath;
    for (i = 1; i < fullFrom->getLength(); i++)
	fp->append( fullFrom->getIndex(i) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the passed node is in the path chain.
//
// Use: public

SbBool
SoNodeKitPath::containsNode(SoBaseKit *node) const
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < getLength(); i++)
    	if (getNode(i) == node)
	    return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    If the paths have different head nodes, this returns -1.
//    Otherwise, it returns the index into the chain of the last node
//    (starting at the head) that is the same for both paths.
//
// Use: public

int
SoNodeKitPath::findFork(const SoNodeKitPath *path) const
//
////////////////////////////////////////////////////////////////////////
{
    int shorterLength, i;

    // Return -1 if heads are different nodes
    if (path->getHead() != getHead())
	return -1;

    // Find length of shorter path
    shorterLength = path->getLength();
    if (getLength() < shorterLength )
	shorterLength = getLength();

    // Return the index of the last pair of nodes that match
    for (i = 1; i < shorterLength; i++)
	if (getNode(i) != path->getNode(i))
	    return i - 1;

    // All the nodes matched - return the index of the tail
    return shorterLength - 1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Comparison operator tests path equality. 
//    Two paths are equal if their chains of nodekits
//    are the same length, the heads are the same, and all the nodekits
//    on them are the same.
//
int
operator ==(const SoNodeKitPath &p1, const SoNodeKitPath &p2)
{
    int i;

    if (p1.getLength() != p2.getLength())
	return FALSE;

    // Compare path nodes from bottom up since there is more
    // likelihood that they differ at the bottom. That means that
    // unequal paths will exit this loop sooner.

    for (i = p1.getLength() -1; i >= 0; --i )
	if ( p1.getNode(i) != p2.getNode(i) )
	    return FALSE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoNodeKitPath::~SoNodeKitPath()
//
////////////////////////////////////////////////////////////////////////
{
}

