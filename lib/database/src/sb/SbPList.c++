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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SbPList
 |
 |   Author(s)		: Nick Thompson, Paul S. Strauss
 |
 |   Notes:
 |	SbPLists are stored as dynamically-allocated arrays.
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SbPList.h>
#include <Inventor/errors/SoDebugError.h>

#define DEFAULT_INITIAL_SIZE	4

//
// Constructor
//

SbPList::SbPList()
{
    ptrs  = NULL;
    nPtrs = ptrsSize = 0;

    setSize(0);
}

//
// Constructor allocates enough room for the given number of pointers.
//

SbPList::SbPList(int initSize)
{
    ptrs  = NULL;
    nPtrs = ptrsSize = 0;

    setSize(initSize);		// Makes enough room for initSize entries
    setSize(0);			// Sets nPtrs back to 0
}

//
// Initialize one PList from another
//

SbPList::SbPList(const SbPList &pl)
{
    int i;

    nPtrs = pl.nPtrs;
    ptrsSize = pl.ptrsSize;
    ptrs = new void *[ptrsSize];

    for (i = 0; i < nPtrs; i++)
	ptrs[i] = pl.ptrs[i];
}

//
// Destructor
//

SbPList::~SbPList()
{
    if (ptrs != NULL)
	delete [] ptrs;
}

//
// Returns index of given pointer in list, or -1 if not found
//

int
SbPList::find(const void *ptr) const
{
    int i;

    for (i = 0; i < nPtrs; i++)
	if (ptrs[i] == ptr)
	    return(i);

    return -1;	// Not found
}

//
// Inserts given pointer in list before pointer with given index
//

void
SbPList::insert(void *ptr, int addBefore)
{
    int	i;

    // If addBefore is off the end of the list, grow the list (and
    // initialize any new elements to NULL)
    if (addBefore > nPtrs) grow(addBefore);

    // Make room for one more
    setSize(nPtrs + 1);

    // Move pointers forward to make room
    for (i = nPtrs - 1; i > addBefore; --i)
	ptrs[i] = ptrs[i - 1];

    // Insert the new one
    ptrs[addBefore] = ptr;
}

//
// Removes pointer with given index
//

void
SbPList::remove(int which)
{
    int i;

    // Move all pointers after the ith one backward
    for (i = which; i < nPtrs - 1; i++)
	ptrs[i] = ptrs[i + 1];

    // Shrink the list
    setSize(nPtrs - 1);
}

//
// Copies a pointer list
//

void
SbPList::copy(const SbPList &pl)
{
    int i;

    setSize(pl.nPtrs);

    for (i = 0; i < nPtrs; i++)
	ptrs[i] = pl.ptrs[i];
}

//
// Makes the list big enough for "newSize" elements, and initializes
// any newly created elements to NULL.
//

void
SbPList::grow(int max) const
{
    int newSize = max+1;
    int oldSize = nPtrs;

#ifdef DEBUG
    if (newSize <= oldSize)
	SoDebugError::post("(internal) SbPList::grow", "newSize <= oldSize!");
#endif /* DEBUG */

    // Get around the const thing:
    SbPList *me = (SbPList *)this;

    me->setSize(newSize);
    
    for (int i = oldSize; i < newSize; i++) {
	me->ptrs[i] = NULL;
    }
}


//
// Changes size of list to be new size. If the new size is larger than
// the old one, it allocates more space.
//

void
SbPList::expand(int size)
{
    void	**newPtrs;
    int	i;

    if (ptrsSize == 0)
	ptrsSize = DEFAULT_INITIAL_SIZE;

    while (size > ptrsSize) {
#ifdef DEBUG
	// check for overflow
	int	oldPtrsSize = ptrsSize;
	ptrsSize *= 2;
	if (ptrsSize < oldPtrsSize)
	    SoDebugError::post("SbPList::expand",
			       "Attempt to expand list beyond capacity;\n"
			       "A core dump is likely");
#else
	ptrsSize *= 2;
#endif
    }

    newPtrs = new (void *[ptrsSize]);

    if (ptrs != NULL) {
	for (i = 0; i < nPtrs; i++)
	    newPtrs[i] = ptrs[i];
	delete [] ptrs;
    }

    ptrs = newPtrs;
}

// NOTE: this must be called only if the number of elements in the two
// lists is the same, otherwise badness could result

int
SbPList::compare(const SbPList &pl) const
{
    int i;
    for (i = 0; i < nPtrs; i++)
	if ((*this)[i] != pl[i])
	    return FALSE;

    return TRUE;
}

SbVec3fList::~SbVec3fList()
{
    int		i;

    for(i = 0; i < getLength(); i++)
	delete (*this)[i];
}

