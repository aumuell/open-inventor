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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoBaseList
 |
 |   Author(s)		: Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/misc/SoBase.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor.
//
// Use: public

SoBaseList::SoBaseList() : SbPList()
//
////////////////////////////////////////////////////////////////////////
{
    // By default, this adds references to things in the list when
    // they are added
    addRefs = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes initial approximate size (number of items
//    in list).
//
// Use: public

SoBaseList::SoBaseList(int size) : SbPList(size)
//
////////////////////////////////////////////////////////////////////////
{
    // By default, this adds references to things in the list when
    // they are added
    addRefs = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes another list to copy values from.
//
// Use: public

SoBaseList::SoBaseList(const SoBaseList &l)
//
////////////////////////////////////////////////////////////////////////
{
    // By default, this adds references to things in the list when
    // they are added
    addRefs = TRUE;

    copy(l);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a pointer to the end of the list
//
// Use: public

void
SoBaseList::append(SoBase * ptr)	// pointer to append
//
////////////////////////////////////////////////////////////////////////
{
    SbPList::append((void *) ptr);
    if (addRefs && ptr)
	ptr->ref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Insert given pointer into list before pointer with given index
//
// Use: public

void
SoBaseList::insert(SoBase *ptr,		// pointer to insert
		   int addBefore)	// index to add before
//
////////////////////////////////////////////////////////////////////////
{
    if (addRefs && ptr)
	ptr->ref();
    SbPList::insert((void *) ptr, addBefore);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove pointer with given index
//
// Use: public

void
SoBaseList::remove(int which)		// index of pointer to remove
//
////////////////////////////////////////////////////////////////////////
{
    if (addRefs && (*this)[which])
	(*this)[which]->unref();
    SbPList::remove(which);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove all pointers from one with given index on, inclusive
//
// Use: public

void
SoBaseList::truncate(int start)		// first index to remove
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    if (addRefs) {
	for ( i = start; i < getLength(); i++) {
	    if ( (*this)[i] ) {
		(*this)[i]->unref();
	    }
	}
    }

    SbPList::truncate(start);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy a list, keeping all reference counts correct
//
// Use: public

void
SoBaseList::copy(const SoBaseList &bList)	// list to copy from
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    truncate(0);

    if (addRefs) {
	for (i = 0; i < bList.getLength(); i++) {
	    if ( bList[i] ) {
		bList[i]->ref();
	    }
	}
    }

    SbPList::copy(bList);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set an element of a list
//
// Use: public

void
SoBaseList::set(int i,			// index to set
		SoBase *ptr)		// new pointer value
//
////////////////////////////////////////////////////////////////////////
{
    if (addRefs) {
	if ( ptr ) {
	    ptr->ref() ;
	}
	if ( (*this)[i] ) {
	    (*this)[i]->unref();
	}
    }
    (*(const SbPList *) this) [i] = (void *) ptr;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks in the SoPathList for a path that matches a given one, using the
// == operator.  If it is found, return that index, otherwise retun -1
//
// Use: public

int
SoPathList::findPath(const SoPath &path)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;
    SoPath	*testPath;

    for(i = 0; i < getLength(); i++) {
	testPath = (*this)[i];
	if (*testPath == path)
	    return i;
    }
    return -1;	// not found
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sorts path list in place based on (1) increasing address of head
//    node, then (2) increasing indices of children.
//
// Use: public

void
SoPathList::sort()
//
////////////////////////////////////////////////////////////////////////
{
    SoPath	**paths = new SoPath *[getLength()];
    int		i;

    // Use qsort to do the work
    for (i = 0; i < getLength(); i++) {
	paths[i] = (*this)[i];
	paths[i]->ref();
    }

    qsort(paths, getLength(), sizeof(SoPath *), comparePaths);

    // Move the paths back into this list
    for (i = 0; i < getLength(); i++)
	set(i, paths[i]);

    // Get rid of the array
    for (i = 0; i < getLength(); i++)
	paths[i]->unref();
    delete [] paths;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a sorted list, removes any path that (1) is a duplicate,
//    or (2) goes through a node that is the tail of another path.
//
// Use: public

void
SoPathList::uniquify()
//
////////////////////////////////////////////////////////////////////////
{
    int			i, lastSame;
    const SoPath	*p1;
    const SoPath	*p2;

    // Remove duplicates from the end to minimize array shuffling
    for (i = getLength() - 2; i >= 0; i--) {

	// Use the SoPath::findFork() method to determine the last
	// node that is on a common chain for both paths. Since the
	// paths are sorted, we can just check if this node is at the
	// end of the first path. If it is, the second one is a
	// duplicate and can be removed.

	p1 = (*this)[i];
	p2 = (*this)[i + 1];

	lastSame = p1->findFork(p2);

	if (lastSame == p1->getLength() - 1)
	    remove(i + 1);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Comparison method for path list sorting.
//
// Use: private

int
SoPathList::comparePaths(const void *p1Ptr, const void *p2Ptr)
//
////////////////////////////////////////////////////////////////////////
{
    const SoPath	*p1, *p2;

    p1 = * (const SoPath * const *) p1Ptr;
    p2 = * (const SoPath * const *) p2Ptr;

    // Most likely, the head nodes will be the same, so test this first
    if (p1->getHead() == p2->getHead()) {

	// Test indices in order. A missing child comes before an
	// existing child

	int	depth;
	for (depth = 1; depth < p1->getLength(); depth++) {
	    if (depth >= p2->getLength())
		return 1;
	    if (p1->getIndex(depth) < p2->getIndex(depth))
		return -1;
	    if (p1->getIndex(depth) > p2->getIndex(depth))
		return 1;
	}

	// If we get here, then the paths are the same up to the end
	// of path 1. If path2 is longer, then it comes after
	if (p2->getLength() > p1->getLength())
	    return -1;

	// Exact same paths
	return 0;
    }

    else if (p1->getHead() < p2->getHead())
	return -1;
    else
	return  1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a typeId to the end of the list
//
// Use: public

void
SoTypeList::append(SoType typeId)	// typeId to append
//
////////////////////////////////////////////////////////////////////////
{
    // we have to do some hackage to cast an SoType into a void *...
    void *hackage = (void*)(unsigned long)(*(int32_t *)&typeId);
    SbPList::append(hackage);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns index of given typeId in list, or -1 if not found.
//
// Use: public

int
SoTypeList::find(SoType typeId) const
//
////////////////////////////////////////////////////////////////////////
{
    // we have to do some hackage to cast an SoType into a void *...
    void *hackage = (void*)(unsigned long)(*(int32_t *)&typeId);
    return SbPList::find(hackage);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Insert given typeId into list before typeId with given index
//
// Use: public

void
SoTypeList::insert(SoType typeId,	// typeId to insert
		   int addBefore)	// index to add before
//
////////////////////////////////////////////////////////////////////////
{
    // we have to do some hackage to cast an SoType into a void *...
    void *hackage = (void*)(unsigned long)(*(int32_t *)&typeId);
    SbPList::insert(hackage, addBefore);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Access an element of a list
//
// Use: public

SoType
SoTypeList::operator [](int i) const
//
////////////////////////////////////////////////////////////////////////
{ 
    // we have to do some hackage to cast our void * back to an SoType...
    int32_t hackage = (long)(* (const SbPList *) this)[i];
    return *(SoType*)&hackage;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set an element of a list
//
// Use: public

void
SoTypeList::set(int i,			// index to set
		SoType typeId)		// new typeId value
//
////////////////////////////////////////////////////////////////////////
{
    // we have to do some hackage to cast an SoType into a void *...
    void *hackage = (void*)(unsigned long)(*(int32_t *)&typeId);
    (*(const SbPList *) this) [i] = hackage;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy constructor.
//
// Use: public

SoDetailList::SoDetailList(const SoDetailList &l) : SbPList(l)
{
    // We need to copy the details, since we delete them when we
    // truncate the list
    for (int i = 0; i < getLength(); i++)
	(* (const SbPList *) this)[i] = (void *) (*this)[i]->copy();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Delete all details on the list from one with given index on,
//    inclusive.
//
// Use: public

void
SoDetailList::truncate(int start)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    for ( i = start; i < getLength(); i++) {
        if ( (*this)[i] ) {
	    delete (*this)[i];
        }
    }

    SbPList::truncate(start);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies list, making copy instances of each detail in list.
//
// Use: public

void
SoDetailList::copy(const SoDetailList &l)
//
////////////////////////////////////////////////////////////////////////
{
    truncate(0);

    int num = l.getLength();
    for (int i = 0; i < num; i ++) {
	SoDetail* detail = l[i];
	if (detail)
	    append(detail->copy());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets an element of a list, deleting old entry.
//
// Use: public

void
SoDetailList::set(int i, SoDetail *detail)
//
////////////////////////////////////////////////////////////////////////
{
    if ((*this)[i] != NULL)
	delete (*this)[i];

    (* (const SbPList *) this)[i] = (void *) detail;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy constructor.
//
// Use: public

SoPickedPointList::SoPickedPointList(const SoPickedPointList &l) : SbPList(l)
{
    // We need to copy the pickedPoints, since we delete them when we
    // truncate the list
    for (int i = 0; i < getLength(); i++)
	(* (const SbPList *) this)[i] = (void *) (*this)[i]->copy();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Delete all pickedPoints on the list from one with given index on,
//    inclusive.
//
// Use: public

void
SoPickedPointList::truncate(int start)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    for (i = start; i < getLength(); i++)
        if ((*this)[i] != NULL)
	    delete (*this)[i];

    SbPList::truncate(start);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets an element of a list, deleting old entry.
//
// Use: public

void
SoPickedPointList::set(int i, SoPickedPoint *pickedPoint)
//
////////////////////////////////////////////////////////////////////////
{
    if ((*this)[i] != NULL)
	delete (*this)[i];

    (* (const SbPList *) this)[i] = (void *) pickedPoint;
}

